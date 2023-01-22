#ifndef STANDARD_API_BINDINGS
#define STANDARD_API_BINDINGS

#include <future>
#include <functional>
#include "./type-lookup.hpp"
#include "./member-pointer.hpp"
#include "./member-function-pointer.hpp"
#include "./static-pointer.hpp"
#include "./static-function-pointer.hpp"
#include "./class-typing.hpp"

namespace APICore
{
    template <class T>
    struct TypeLookup<std::future<T>>
    {
        using registeredType = ClassTyping<
            "Promise",
            "An awaitable promise of an asynchronous execution.",
            std::future<T>,
            MemberFunction<
                "await",
                +[](std::future<T>& self){
                    self.wait();
                    return self.get();
                },
                "Block this thread and wait for the promise to resolve.",
                ParameterPack<>>>;
        static bool isDefined;
    };
    template <class T>
    bool APICore::TypeLookup<std::future<T>>::isDefined = true;


    // template <class T>
    // struct TypeLookup<std::function<T>>
    // {
    //     using registeredType = ClassTyping<
    //         "FunctionObject",
    //         "A lua binding for the std::function.",
    //         std::function<T>
    //         >;
    //     static bool isDefined;
    // };
    // template <class T>
    // bool APICore::TypeLookup<std::function<T>>::isDefined = true;

}

#endif