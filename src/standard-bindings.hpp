#pragma once

#include <future>
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
            void,
            MemberFunction<
                "await",
                +[](std::future<T> &self)
                {
                    self.wait();
                    return self.get();
                },
                "Block this thread and wait for the promise to resolve.",
                ParameterPack<>>>;

        static bool isDefined;
    };

    template <class T>
    bool APICore::TypeLookup<std::future<T>>::isDefined = true;

    template <class T>
    struct TypeLookup<std::shared_future<T>>
    {
        using registeredType = ClassTyping<
            "SharedPromise",
            "An awaitable promise of an asynchronous execution.",
            std::shared_future<T>,
            void,
            MemberFunction<
                "await",
                +[](std::shared_future<T> &self)
                {
                    self.wait();
                    return self.get();
                },
                "Block this thread and wait for the promise to resolve.",
                ParameterPack<>>,
            MemberFunction<
                "onResolve",
                +[](std::shared_future<T> &self, std::function<void(
                    std::conditional_t<std::is_void_v<T>, void*, T> &promiseResult
                )>
                                                     callback)
                {
                    auto r = std::shared_future<void> (std::async(
                        std::launch::async,
                        // std::launch::deferred,
                        [self, callback]()
                        {
                            self.wait();
                            if constexpr (std::is_void_v<T>)
                            {
                                void* n = nullptr;
                                callback(n);
                            }
                            else
                            {
                                T promiseResult = self.get();
                                callback(promiseResult);
                            }
                        }

                        ));

                    return r;
                },
                "Execute the passed callback function upon completion of the promise. Execution is deferred until the promise returned by onResolve is awaited.",
                ParameterPack<>>>;

        static bool isDefined;
    };

    template <class T>
    bool APICore::TypeLookup<std::shared_future<T>>::isDefined = true;

}
#pragma once