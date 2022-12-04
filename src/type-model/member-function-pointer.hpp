#ifndef MEMBER_FUNCTION_POINTER
#define MEMBER_FUNCTION_POINTER

#include "./string-literal.hpp"
#include "./member-pointer.hpp"
#include "./type-lookup.hpp"
#include <type_traits>
#include <string>
#include <vector>

namespace APICore
{
    template <StringLiteral Name, StringLiteral Description>
    struct Parameter
    {
        static std::string name;
        static std::string description;
    };
    template <StringLiteral Name, StringLiteral Description>
    std::string Parameter<Name, Description>::name = std::string(Name.value);
    template <StringLiteral Name, StringLiteral Description>
    std::string Parameter<Name, Description>::description = std::string(Description.value);

    template <typename T>
    concept ParameterDescription = requires
    {
        {
            T::name
            } -> std::convertible_to<std::string>;
        {
            T::description
            } -> std::convertible_to<std::string>;
    };

    template <ParameterDescription... Parameters>
    struct ParameterPack
    {
        static std::vector<std::pair<std::string, std::string>> parameters;
    };
    template <ParameterDescription... Parameters>
    std::vector<std::pair<std::string, std::string>>
        ParameterPack<Parameters...>::parameters = ([]()
                                                    {
            std::vector<std::pair<std::string, std::string>> parameterList;
            (parameterList.push_back(std::pair<std::string, std::string>(Parameters::name, Parameters::description)), ...);
            return parameterList; })();

    template <typename T>
    concept ParameterPackDefinition = requires
    {
        {
            T::parameters
            } -> std::convertible_to<std::vector<std::pair<std::string, std::string>>>;
    };

    template <StringLiteral Key, auto Pointer, StringLiteral Description, ParameterPackDefinition Parameters>
    struct MemberFunction : public MemberPtr<Pointer>
    {
        using pointerTypeLookup = void;
        using parameterPack = Parameters;
        static std::string key;
        static std::string description;
    };
    template <StringLiteral Key, auto Pointer, StringLiteral Description, ParameterPackDefinition Parameters>
    std::string MemberFunction<Key, Pointer, Description, Parameters>::key = Key.value;
    template <StringLiteral Key, auto Pointer, StringLiteral Description, ParameterPackDefinition Parameters>
    std::string MemberFunction<Key, Pointer, Description, Parameters>::description = Description.value;

    template <typename PointerType>
    struct MemberFunctionTyping
    {
        using classType = void;
        using returnType = void;
    };

    template <class ClassType, typename ReturnType, typename... Parameters>
    struct MemberFunctionTyping<ReturnType ClassType::*(Parameters...)>
    {
        using classType = ClassType;
        using returnType = ReturnType;

      
    };

    template <typename PointerType, StringLiteral Key, PointerType Pointer, StringLiteral Description, ParameterPackDefinition Parameters>
    struct MemberFunction<Key, Pointer, Description, Parameters> : public MemberPtr<Pointer>
    {
        using functionTyping = MemberFunctionTyping<PointerType>;
        using parameterPack = Parameters;
        static std::vector<std::pair<std::string, std::string>> parameters;
        static std::string key;
        static std::string description;
        static bool lookupCreated;
    };

    template <typename PointerType, StringLiteral Key, PointerType Pointer, StringLiteral Description, ParameterPackDefinition Parameters>
    std::string MemberFunction<Key, Pointer, Description, Parameters>::key = Key.value;
    template <typename PointerType, StringLiteral Key, PointerType Pointer, StringLiteral Description, ParameterPackDefinition Parameters>
    std::string MemberFunction<Key, Pointer, Description, Parameters>::description = Description.value;

    template <typename T>
    concept MemberFunctionPtrSpec = requires()
    {
        requires MemberPtrSpec<T>;
        {T::functionTyping::classType};
        {T::functionTyping::returnType};
    };
};

#endif