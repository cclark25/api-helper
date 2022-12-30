#ifndef LUA_MACROS
#define LUA_MACROS
#include "./macros.hpp"

#define DependOnType(state, Type)                                                            \
    {                                                                                        \
        if constexpr (std::is_class<Type>::value && !std::is_same<Type, std::string>::value) \
        {                                                                                    \
            LuaBinder<void, Type, "ANY">::bind(state, nullptr);                              \
        }                                                                                    \
    }

#endif