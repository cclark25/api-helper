#ifndef TYPE_MODEL
#define TYPE_MODEL

#include <vector>
#include <array>
#include <string>
#include <memory>
#include <map>
#include <type_traits>
#include "./data-primitive.hpp"
#include "./lua-binding/binding-types/all.hpp"

namespace APICore
{

    template <size_t Length>
    struct StringLiteral
    {
        char value[Length]{};
        static constexpr std::size_t size = Length - 1;

        [[nodiscard]] std::string_view view() const
        {
            return {value, value + size};
        }

        consteval StringLiteral() {}
        consteval StringLiteral(const char (&str)[Length]) : StringLiteral()
        {
            std::copy_n(str, Length, value);
        }
    };

    template <typename T>
    DataPrimitive getPrimitiveType();

    template <typename T>
    struct NamedBasicType
    {
        T value;
        template <typename... Params>
        requires std::constructible_from<T, Params...>
        NamedBasicType(Params... params) : value(params...)
        {
        }
    };
    template <typename T>
    struct NamedClassType : public T
    {
        NamedClassType(const T &other) : T(other)
        {
        }
        template <typename... Params>
        requires std::constructible_from<T, Params...>
        NamedClassType(Params... params) : T(params...)
        {
        }
    };

    template <typename T, StringLiteral Name, StringLiteral Description = "">
    struct Named : public std::conditional<std::is_class<T>::value, NamedClassType<T>, NamedBasicType<T>>::type
    {
        using varType = T;
        constexpr static const char *name = Name.value;
        constexpr static const char *description = Description.value;
        static DataPrimitive getPrimitiveType()
        {
            return APICore::getPrimitiveType<T>();
        }

        // Data

        template <typename... Params>
        requires std::constructible_from<T, Params...>
        Named(Params... params) : std::conditional<std::is_class<T>::value, NamedClassType<T>, NamedBasicType<T>>::type(params...)
        {
        }

        operator T &()
        {
            return *((T *)this);
        }

        T &operator=(const T &other)
        {
            T &ref = *static_cast<T *>(this);
            ref = other;
            return ref;
        }
    };

    template <typename T, typename Arg>
    using ReplaceType = T;

    template <typename C, typename T>
    using MemberVariablePointer = T C::*;
    template <typename C, typename T, typename... P>
    struct MemberFunctionPointer
    {
        using Pointer = T (C::*)(P...);
        using ReturnType = T;
        // using Parameters = P...;
        using ClassType = C;
        Pointer v;
        std::array<std::string, sizeof...(P)> parameterNames;
        MemberFunctionPointer(const Pointer &o, std::array<std::string, sizeof...(P)> &parameterNames) : parameterNames(parameterNames), v(o)
        {
        }
    };

    template <typename T, StringLiteral Name, typename ClassType, MemberVariablePointer<ClassType, T> MemberPointer, StringLiteral Description = "">
    struct Member : public Named<T, Name, Description>
    {
        constexpr static MemberVariablePointer<ClassType, T> memberPointer = MemberPointer;
    };

    template <typename R, typename... P>
    std::vector<std::string> getVariableNames(std::function<R(P...)> p)
    {
        std::vector<std::string> result;

        (result.push_back(P::name), ...);

        return result;
    }

    template <class T, template <class...> class Template>
    struct is_specialization : std::false_type
    {
    };

    template <template <class...> class Template, class... Args>
    struct is_specialization<Template<Args...>, Template> : std::true_type
    {
    };

    template <typename Base, typename... Fields>
    class ObjectTypeModel;

    template <typename T>
    DataPrimitive getPrimitiveType()
    {
        if (is_specialization<T, ObjectTypeModel>{})
        {
            return DataPrimitive::object;
        }
        return DataPrimitive::unknown;
    }

    template <>
    DataPrimitive getPrimitiveType<std::string>()
    {
        return DataPrimitive::string;
    }
    template <>
    DataPrimitive getPrimitiveType<int>()
    {
        return DataPrimitive::int32;
    }
    template <>
    DataPrimitive getPrimitiveType<void>()
    {
        return DataPrimitive::null;
    }

    template <typename Base, typename... Fields>
    class ObjectTypeModel : Base
    {
    public:
        static std::string description;

        template <typename T>
        static std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> getBasicTyping(std::string description)
        {
            auto primitiveType = getPrimitiveType<T>();

            auto result = makeBasicType(primitiveType);
            result->description = description;

            return result;
        }

        static std::map<std::string, std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>> getFields()
        {
            std::map<std::string, std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>> result;

            (result.insert_or_assign(Fields::name, ObjectTypeModel::getBasicTyping<typename Fields::varType>(Fields::description)), ...);

            return result;
        }

        static std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> generateTyping()
        {
            std::map<std::string, std::shared_ptr<TypeWrapper<unknown>>> fields = ObjectTypeModel::getFields();
            auto result = makeBasicType(DataPrimitive::object, ObjectTypeModel::description);
            auto objectWrapperCast = CastSharedPtr(ObjectTypeWrapper, result);
            objectWrapperCast->fields.merge(fields);

            return result;
        }
    };
    template <typename Base, typename... T>
    std::string ObjectTypeModel<Base, T...>::description;

    template <typename T>
    struct TypeDefinition;

    template <typename ParentType>
    struct TypeFieldSetter
    {
        TypeDefinition<ParentType>::Type *typeParent;
        std::string key;
        TypeFieldSetter(TypeDefinition<ParentType>::Type *typeParent, const std::string &key) : typeParent(typeParent), key(key) {}

        template <typename FieldType>
        TypeFieldSetter &operator=(MemberVariablePointer<ParentType, FieldType> dataAccessor)
        {
            typeParent->setField(this->key, dataAccessor);
            return *this;
        }
    };

    class empty_type
    {
    };

    template <typename ClassType, typename FieldType>
    struct MemberFields : public std::map<
        std::string,
        MemberVariablePointer<ClassType, FieldType>>{};
    template <typename FieldType>
    struct MemberFields<int, FieldType> : public std::map<
        std::string,
        void*>{};

    template <typename ClassType>
    struct TypeDefinition
    {
        template <typename FieldType>
        using MemberFields = std::conditional<
            !(std::is_class<ClassType>::value),
            MemberFields<ClassType, empty_type>,
            MemberFields<ClassType, FieldType>>::type;

        template <typename FieldType>
        static TypeDefinition::MemberFields<FieldType> memberFields;

        // template <typename FieldType, typename... Parameters>
        // static std::map<std::string, MemberFunctionPointer<ClassType, FieldType, Parameters...>> memberFunctions;

        struct Type
        {
            TypeFieldSetter<ClassType> operator[](const std::string &key)
            {
                return TypeFieldSetter<ClassType>(this, key);
            }

            template <typename FieldType>
            void setField(std::string &key, MemberVariablePointer<ClassType, FieldType> dataAccessor)
            {
                TypeDefinition::memberFields<FieldType>.insert_or_assign(key, dataAccessor);
            }

            template <typename... TT>
            static std::map<std::string, std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>> getFieldTypings()
            {
                std::map<std::string, std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>> fields;

                (fields.merge(Type::getFieldTypingsForType<TT>()), ...);

                return fields;
            }

            template <typename T>
            static std::map<std::string, std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>> getFieldTypingsForType()
            {
                std::map<std::string, std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>> fields;

                for (auto f : TypeDefinition::memberFields<T>)
                {
                    auto fieldType = TypeDefinition<T>::type::generateTyping();
                    fields.insert_or_assign(fieldType);
                }

                return fields;
            }

            static std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> generateTyping()
            {
                std::map<std::string, std::shared_ptr<TypeWrapper<unknown>>> fields = Type::getFieldTypings<int, std::string>();
                auto result = makeBasicType(DataPrimitive::object, "");
                auto objectWrapperCast = CastSharedPtr(ObjectTypeWrapper, result);
                objectWrapperCast->fields.merge(fields);

                return result;
            }
        };

        static Type type;
    };

    template <typename ClassType>
    TypeDefinition<ClassType>::Type TypeDefinition<ClassType>::type = TypeDefinition<ClassType>::Type();
    template <typename ClassType>
    template <typename FieldType>
    TypeDefinition<ClassType>::MemberFields<FieldType> TypeDefinition<ClassType>::memberFields;
}

#endif