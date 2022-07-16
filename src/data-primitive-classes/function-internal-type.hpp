#ifndef DATA_WRAPPER_FUNCTION_INTERNAL_TYPE
#define DATA_WRAPPER_FUNCTION_INTERNAL_TYPE
#include <string>
#include <memory>
#include <vector>
#include <functional>

namespace APICore
{
    class DataWrapper;
    struct FunctionInternalType
    {
        using FunctionParams = std::vector<std::shared_ptr<DataWrapper>>;
        using APIFunction = std::function<std::shared_ptr<DataWrapper>(FunctionParams)>;
        APIFunction function;
        /**
         * @brief
         * Optional source of functions created by external APIs.
         * For example, this could be the original Lua function used to create this function.
         * This lua function can be passed back into Lua instead of adding the overhead
         * of translating lua paramaters to internal types and then the return value
         * to Lua.
         */
        std::shared_ptr<void> externalSource;
        std::string externalSourceKeyName;

        FunctionInternalType(APIFunction function, std::string externalSourceKeyName = "", std::shared_ptr<void> externalSource = nullptr) : function(function), externalSourceKeyName(externalSourceKeyName), externalSource(externalSource) {}
    };
}

#endif