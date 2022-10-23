#ifndef TYPE_MODEL
#define TYPE_MODEL

#include <vector>
#include <string>
#include <memory>
#include <map>
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

    template <typename T, StringLiteral Name, StringLiteral Description = "">
    struct Named
    {
        using varType = T;
        constexpr static const char *name = Name.value;
        constexpr static const char *description = Description.value;
        T value;
    };

    template <typename R, typename... P>
    std::vector<std::string> getVariableNames(std::function<R(P...)> p)
    {
        std::vector<std::string> result;

        (result.push_back(P::name), ...);

        return result;
    }

#define Parameter(Type, Name) Named<Type, #Name> Name

    template <typename... Fields>
    class TypeModel;

    template <class T, template <class...> class Template>
    struct is_specialization : std::false_type
    {
    };

    template <template <class...> class Template, class... Args>
    struct is_specialization<Template<Args...>, Template> : std::true_type
    {
    };

    template <typename T>
    DataPrimitive getPrimitiveType()
    {
        if (is_specialization<T, TypeModel>{})
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

    template <typename... Fields>
    class TypeModel
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

            (result.insert_or_assign(Fields::name, TypeModel::getBasicTyping<typename Fields::varType>(Fields::description)), ...);

            return result;
        }

        static std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> generateTyping()
        {
            std::map<std::string, std::shared_ptr<TypeWrapper<unknown>>> fields = TypeModel::getFields();
            auto result = makeBasicType(DataPrimitive::object, TypeModel::description);
            auto objectWrapperCast = CastSharedPtr(ObjectTypeWrapper, result);
            objectWrapperCast->fields.merge(fields);

            return result;
        }
    };
    template<typename... T>
    std::string TypeModel<T...>::description;
}

#endif