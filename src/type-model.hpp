#ifndef TYPE_MODEL
#define TYPE_MODEL

#include <vector>
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

    template <typename C, typename T>
    using MemberVariablePointer = T C::*;

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
}

#endif