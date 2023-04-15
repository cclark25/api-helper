#pragma once

#include <future>
#include "./type-lookup.hpp"
#include "./member-pointer.hpp"
#include "./member-function-pointer.hpp"
#include "./static-pointer.hpp"
#include "./static-function-pointer.hpp"
#include "./class-typing.hpp"
#include "./templated-types.hpp"

namespace APICore
{
    using PromisedType = TemplateParam<"PromisedType">;
    using CallbackReturnType = TemplateParam<"CallbackReturnType">;

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
    struct PromiseMembers
    {
        using onCallbackTyping = Templated<
            MemberFunction<
                "onResolve",
                +[](
                    std::shared_future<T> &self, 
                    std::function<CallbackReturnType(std::conditional_t<std::is_void_v<T>, void *, T> &promiseResult)>
                                                    callback)
                {
                    auto r = std::shared_future<CallbackReturnType>(std::async(
                        std::launch::async,
                        [self, callback]()
                        {
                            self.wait();

                            T promiseResult = self.get();
                            return callback(promiseResult);
                        }

                        ));

                    return r;
                },
                "Execute the passed callback function upon completion of the promise. Execution is deferred until the promise returned by onResolve is awaited.",
                ParameterPack<>
            >,
            "CallbackReturnType">;

        using awaiterTyping = MemberFunction<
            "await",
            +[](std::shared_future<T> &self)
            {
                self.wait();
                return self.get();
            },
            "Block this thread and wait for the promise to resolve.",
            ParameterPack<>>;
    };

    template <>
    struct TypeLookup<std::shared_future<PromisedType>>
    {

        using registeredType = Templated<
            ClassTyping<
                "SharedPromise",
                "An awaitable promise of an asynchronous execution.",
                std::shared_future<PromisedType>,
                void,
                PromiseMembers<PromisedType>::awaiterTyping,
                PromiseMembers<PromisedType>::onCallbackTyping>,
            "PromisedType">;

        static const bool isDefined = true;
    };

    using FutureTyping = TypeLookup<
        std::shared_future<PromisedType>>;

    template <class T>
    struct TypeLookup<std::shared_future<T>>
    {
        using registeredType = ClassTyping<
            "SharedPromise",
            "An awaitable promise of an asynchronous execution.",
            std::shared_future<T>,
            std::shared_future<PromisedType>,

            typename PromiseMembers<T>::awaiterTyping,
            typename PromiseMembers<T>::onCallbackTyping

            >;
        static const bool isDefined = true;
    };

    // template <class T>
    // struct TypeLookup<std::shared_future<T>>
    // {
    //     using registeredType = ClassTyping<
    //         "TemplateSpecialization",
    //         "An awaitable promise of an asynchronous execution.",
    //         std::shared_future<T>,

    //         std::shared_future<PromisedType>

    //         >;

    //     static const bool isDefined = true;
    // };
}
#pragma once