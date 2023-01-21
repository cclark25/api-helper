#ifndef PROMISE_LUA_BINDER
#define PROMISE_LUA_BINDER

#include <future>
#include <string>
#include <sol.hpp>
#include "./type-binder.hpp"
#include "../concepts.hpp"

namespace APICore
{
    template <FutureTypeConcept Future>
    struct LuaBinderGenerator<Future>
    {
        using PromiseReturnType = decltype(Future().get());
        static void generateType(sol::state &state, sol::usertype<Future> *newPromiseType)
        {
            // auto baseType = APICore::LuaBinder<PromiseReturnType>::declareType();
           
            (*newPromiseType)["await"] = (
                +[](Future &self)
                {
                    bool x = std::is_same_v<std::future<decltype(self.get())>, Future>;
                    bool valid = self.valid();
                    self.wait();
                    return self.get();
                });
        };
    };
}

#endif