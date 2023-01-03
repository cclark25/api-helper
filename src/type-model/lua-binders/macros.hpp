// #ifndef LUA_MACROS
// #define LUA_MACROS
// #include "./macros.hpp"

// #define DependOnType(state, Type)                                                                      \
//     {                                                                                                  \
//         if constexpr (!requires(Type val) { {*val} ; })                                                         \
//         {                                                                                              \
//             if constexpr (std::is_class<Type>::value && !std::is_same<Type, std::string>::value)       \
//             {                                                                                          \
//                 LuaBinder<Type>::bind(state, nullptr);                                    \
//             }                                                                                          \
//         }                                                                                              \
//         else if constexpr (!std::is_function_v<Type>)                                                  \
//         {                                                                                              \
//             Type __val = nullptr;                                                                      \
//             LuaBinder<std::remove_reference_t<decltype(*(__val))>>::bind(state, nullptr); \
//         }                                                                                              \
//     }

// #endif