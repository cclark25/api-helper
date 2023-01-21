#ifndef ASYNC_WRAPPER
#define ASYNC_WRAPPER

#include <type_traits>
#include <future>

namespace APICore
{

    template <auto Fun, class Type>
    struct af
    {
        static auto constexpr ff(){
            // return Fun();
        };
    };

    template <auto Fun, class ReturnType, class... Parameters>
    struct af<Fun, ReturnType (*)(Parameters...)>
    {
        static auto constexpr ff(Parameters... params)
        {
            return std::async(std::launch::async, [params...]()
                              {
    				return Fun(params...);
            });
            
        };
    };

    template <auto FunctionPointer>
    static auto constexpr Async = af<FunctionPointer, decltype(FunctionPointer)>::ff;
    // struct Async
    // {
    //     using FunctionType = decltype(FunctionPointer);
    //     static auto constexpr value = ff<FunctionPointer, FunctionType>;
    // };

    // template <auto FunctionPointer, class ReturnType, class... Parameters>
    // // requires std::is_same_v<decltype(FunctionPointer), ReturnType(Parameters...)>
    // struct Async<( FunctionPointer )>
    // {
    //     static auto constexpr value = +[]()
    //     {
    //         return std::async(std::launch::async, [](Parameters... params)
    //                           {
    // 				std::this_thread::sleep_for(std::chrono::seconds(5));
    // 				return FunctionPointer(params...); });
    //     };
    // };
}

#endif