#pragma once

#include "../member-function-pointer.hpp"
#include "../static-function-pointer.hpp"
#include "../member-pointer.hpp"
#include "../static-pointer.hpp"
#include "../concepts.hpp"
#include "../type-lookup.hpp"
#include "./type-binder.hpp"

#include <functional>
#include <string>
#include <type_traits>
#include <iostream>

namespace APICore
{
    template <class T>
    struct FunctionTyping
    {
        using ReturnType = void;
    };

    template <class ReturnTypeT, class... Parameters>
    struct FunctionTyping<ReturnTypeT(Parameters...)>
    {
        using ReturnType = ReturnTypeT;
    };
    template <class ReturnTypeT, class... Parameters>
    struct FunctionTyping<std::function<ReturnTypeT(Parameters...)>>
    {
        using ReturnType = ReturnTypeT;
    };

    template <class T>
    concept FunctionObjectSpec = requires {
                                     requires IsFunctionObject<typename T::type>::value;
                                 };

    template <class FunctionSpec>
    class FunctionBinder;

    template <class... Functions>
        requires(MemberFunctionPtrSpec<Functions> && ...) || (StaticFunctionPtrSpec<Functions> && ...)
    struct Overloader
    {
        static void print()
        {
            std::cout << "Overloader function count: " << sizeof...(Functions) << "\n";
        }

        template <class ClassType, class KeyType>
        static void bind(sol::state &state, KeyType key, sol::usertype<ClassType> *userType)
        {
            size_t count = sizeof...(Functions);
            auto x = sol::overload(
                ((typename Functions::functionTyping::childMemberCast<ClassType>)Functions::ptr)...);

            (*userType)[key] = x;
        }
    };

    template <class FunctionSpec>
    class FunctionBinder
    {
        template <class ClassType>
        static constexpr auto makeGetter(auto ptr)
        {
            if constexpr (MemberPtrSpec<FunctionSpec>)
            {
                return [ptr](ClassType &self)
                {
                    return std::shared_ptr<decltype(std::function(self.*ptr))>(new std::function(self.*ptr));
                };
            }
            else
            {
                return [ptr]()
                {
                    return std::shared_ptr<decltype(std::function(*ptr))>(new std::function(*ptr));
                };
            }
        }

        template <class ClassType>
        static constexpr auto set(auto *ptr, sol::object &newVal)
        {
            /*
                Overload when setting an std::function using a shared_ptr.
                This preserves an original C++ std::function instead of wrapping it in sol.
            */
            if (newVal.is<std::shared_ptr<decltype(std::function(*ptr))>>())
            {
                (*ptr) = *(newVal.as<std::shared_ptr<decltype(std::function(*ptr))>>());
            }
            /*
                Default overload is that the newVal value must be castable to a new std::function.
                This results in sol functions getting wrapped into new std::function objects.
            */
            else
            {
                (*ptr) = newVal.as<decltype(*ptr)>();
            }
        }

        template <class ClassType>
        static constexpr auto makeSetter(auto &ptr)
        {
            if constexpr (MemberPtrSpec<FunctionSpec>)
            {
                return [ptr](ClassType &self, sol::object newVal)
                {
                    FunctionBinder::set<ClassType>(&(self.*ptr), newVal);
                };
            }
            else
            {
                return [ptr](sol::object newVal)
                {
                    FunctionBinder::set<ClassType>(ptr, newVal);
                };
            }
        }

    public:
        static const bool isFunctional = FunctionObjectSpec<FunctionSpec> || StaticFunctionPtrSpec<FunctionSpec> || ConstructorSpec<FunctionSpec>;
        static const bool isMember = MemberFunctionPtrSpec<FunctionSpec> || MemberOverloadSpec<FunctionSpec>;
        static const bool isStaticField = StaticFunctionPtrSpec<FunctionSpec> || StaticOverloadSpec<FunctionSpec>;

        template <class ClassType>
        static constexpr void generateLuaBinding(sol::state &state, std::string key, sol::usertype<ClassType> *userType)
        {
            using NewKeyType = std::conditional_t<
                requires {
                    requires FunctionSpec::isConstructor || ConstructorSpec<FunctionSpec>;
                },
                sol::meta_method,
                std::string>;

            std::shared_ptr<NewKeyType> newKey = nullptr;
            std::vector<std::shared_ptr<NewKeyType>> keys;

            if constexpr (requires {
                              requires FunctionSpec::isConstructor || ConstructorSpec<FunctionSpec>;
                          })
            {
                keys.push_back(std::shared_ptr<NewKeyType>(new sol::meta_method(sol::meta_method::construct)));

                /*
                    call_construct SHOULD allow the the meta table to be called in order to construct,
                    like `local val = ClassName(12, "abc")`, but for now it seems this is not functional.
                */
                keys.push_back(std::shared_ptr<NewKeyType>(new sol::meta_method(sol::meta_method::call_construct)));
            }
            else
            {
                keys.push_back(std::shared_ptr<NewKeyType>(new std::string(key)));
            }

            for (std::shared_ptr<NewKeyType> newKey : keys)
            {

                if constexpr (FunctionObjectSpec<FunctionSpec>)
                {
                    auto ptr = FunctionSpec::ptr;
                    /*
                        std::function objects should get bounded as properties returning
                        shared_ptr's to the functions. That way, they get treated like userdata
                        instead of sol functions.
                        This allows the functions to be passed back and forth to lua without adding
                        extra overhead with every sol wrap.
                    */

                    using F = FunctionTyping<typename FunctionSpec::type>;
                    LuaBinder<typename F::ReturnType>::declareType(state);

                    (*userType)[*newKey] = sol::property(
                        FunctionBinder::makeGetter<ClassType>(ptr),
                        FunctionBinder::makeSetter<ClassType>(ptr));
                }
                else if constexpr (MemberFunctionPtrSpec<FunctionSpec> || StaticFunctionPtrSpec<FunctionSpec>)
                {
                    auto ptr = FunctionSpec::ptr;
                    using F = FunctionTyping<typename FunctionSpec::type>;
                    LuaBinder<typename F::ReturnType>::declareType(state);
                    (*userType)[*newKey] = ptr;
                }
                else if constexpr (MemberOverloadSpec<FunctionSpec> || StaticOverloadSpec<FunctionSpec>)
                {
                    using O = typename FunctionSpec::IterateOverloads<Overloader>;
                    size_t functionCount = FunctionSpec::functionCount;

                    O::print();

                    O::bind(state, *newKey, userType);
                }
            }
        }
    };
}