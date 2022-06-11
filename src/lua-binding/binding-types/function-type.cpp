
#include <sol/sol.hpp>
#include "../../data-wrappers/data-wrapper.hpp"
#include "./function-type.h"
#include "./lua-object-to-data-wrapper.h"
#include "./create-binding-object.h"
#include "../../macros.hpp"

#define LUA_SOURCE_FUNCTION_KEY "API_LUA"

namespace APILua
{
    using namespace APICore;

    template <>
    sol::table createBindingObject<DataPrimitive::function>(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::function>> wrapper)
    {
        sol::table table = sol::table(state, sol::new_table());
        if (wrapper->canGet())
        {
            table["get"] = [wrapper, &state]()
            {
                auto funcPtr = wrapper->get();
                auto functionDef = *CastSharedPtr(data_primitive_to_type<DataPrimitive::function>, funcPtr);
                auto func = functionDef.function;

                sol::object returnFunction;
                /**
                 * @brief 
                 * If the function was originally defined in lua to begin with, accessing through
                 * the regular binding will only add unnecessary overhead. 
                 * This is a difference of 11,000 function calls per second and 85,000 function calls
                 * per second. 
                 */
                if (functionDef.externalSource != nullptr && functionDef.externalSourceKeyName == LUA_SOURCE_FUNCTION_KEY)
                {
                    auto source = functionDef.externalSource;
                    auto fun = CastSharedPtr(data_primitive_to_lua_type<DataPrimitive::function>, source);
                    if (source != nullptr)
                    {
                        returnFunction = (*fun);
                    }
                }
                else
                {
                    // TODO: Make the function parameters type safe instead of using variadic_args.
                    sol::table tmp = sol::table(state, sol::new_table());
                    tmp["f"] = [func, wrapper, &state](sol::variadic_args arguments)
                    {
                    FunctionInternalType::FunctionParams wrappedParams;
                    for (auto param : arguments)
                    {
                        std::shared_ptr<DataWrapper> paramWrapper = luaObjectToDataWrapper(param.as<sol::object>());
                        wrappedParams.push_back(paramWrapper);
                        continue;
                    }
                    std::shared_ptr<APICore::DataWrapper> returnResult = func(wrappedParams);
                    auto wrappedResult = createBindingObject<DataPrimitive::unknown>(state, CastSharedPtr(DataWrapperSub<DataPrimitive::unknown>, returnResult));

                    return wrappedResult["get"](); };
                    returnFunction = tmp["f"];
                }

                return returnFunction;
            };
        }
        if (wrapper->canSet())
        {
            table["set"] = [wrapper, &state](sol::table self, data_primitive_to_lua_type<DataPrimitive::function> fun)
            {
                wrapper->set(
                    std::shared_ptr<data_primitive_to_type<DataPrimitive::function>>(
                        new data_primitive_to_type<DataPrimitive::function>(
                            [fun, &state](FunctionInternalType::FunctionParams params)
                             {
                                 std::vector<sol::object> luaParams;

                                 // luaParams.push(nullptr);
                                 for (auto param : params)
                                 {
                                     auto wrappedParam = createBindingObject<DataPrimitive::unknown>(state, CastSharedPtr(DataWrapperSub<DataPrimitive::unknown>, param));
                                     auto x = wrappedParam["get"]();
                                     auto y = x.get_type();

                                     luaParams.push_back(x);
                                 }
                                 auto result = fun(sol::as_args(luaParams));
                                 return luaObjectToDataWrapper(result);
                             },
                             LUA_SOURCE_FUNCTION_KEY,
                             std::shared_ptr<sol::function>(new sol::function(fun))
                        )
                    )
                );
            };
        }

        return table;
    }
}