#ifndef FUNCTION_PARAMETERS
#define FUNCTION_PARAMETERS

#include "./string-literal.hpp"
#include <type_traits>
#include <string>
#include <vector>

namespace APICore {
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
}

#endif