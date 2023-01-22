#ifndef CLASS_LUA_BINDER
#define CLASS_LUA_BINDER

#include "../type-lookup.hpp"
#include "./type-binder.hpp"
#include "../class-typing.hpp"
#include "../concepts.hpp"
#include <sol.hpp>
#include <string>
#include <functional>

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

    template <ClassTypeConcept FunctionType>
    struct LuaBinderGenerator<FunctionType>
    {
        template <typename T>
        struct FieldBinders
        {

            static void bindMembers(sol::state &state, sol::usertype<auto> *userType)
            {
            }

            static void bindStaticFields(sol::state &state, sol::usertype<auto> *userType)
            {
            }
        };

        template <StringLiteral Name, StringLiteral Description, ClassField<FunctionType>... Fields>
        struct FieldBinders<ClassTyping<Name, Description, FunctionType, Fields...>>
        {
            static void bindMembers(sol::state &state, sol::usertype<FunctionType> *userType)
            {
                ((
                     [&state, &userType]()
                     {
                         std::string key = Fields::key;
                         if constexpr (MemberPtrSpec<Fields>)
                         {
                             auto ptr = Fields::ptr;
                             if constexpr (!std::is_function_v<typename Fields::type>)
                             {
                                 LuaBinder<typename Fields::type>::declareType(state);

                                 if constexpr (IsFunctionObject<typename Fields::type>::value)
                                 {
                                     /*
                                         std::function objects should get bounded as properties returning
                                         shared_ptr's to the functions. That way, they get treated like userdata
                                         instead of sol functions.
                                         This allows the functions to be passed back and forth to lua without adding
                                         extra overhead with every sol wrap.
                                     */
                                     (*userType)[Fields::key] = sol::property(
                                         [ptr](FunctionType &self)
                                         {
                                             return std::shared_ptr<decltype(std::function(self.*ptr))>(new std::function(self.*ptr));
                                         },

                                         [ptr](FunctionType &self, sol::object newVal)
                                         {
                                             /*
                                                 Overload when setting an std::function using a shared_ptr.
                                                 This preserves an original C++ std::function instead of wrapping it in sol.
                                             */
                                             if (newVal.is<std::shared_ptr<decltype(std::function(self.*ptr))>>())
                                             {
                                                 self.*ptr = *(newVal.as<std::shared_ptr<decltype(std::function(self.*ptr))>>());
                                             }
                                             /*
                                                 Default overload is that the newVal value must be castable to a new std::function.
                                                 This results in sol functions getting wrapped into new std::function objects.
                                             */
                                             else
                                             {
                                                 self.*ptr = newVal.as<decltype(self.*ptr)>();
                                             }
                                         }

                                     );
                                 }
                                 else
                                 {
                                     (*userType)[Fields::key] = ptr;
                                 }
                             }
                             else
                             {
                                 (*userType)[Fields::key] = ptr;
                             }
                             return true;
                         }
                         else
                         {
                             return false;
                         }
                     }()),
                 ...);

                return;
            }
            static void bindStaticFields(sol::state &state, sol::usertype<FunctionType> *userType)
            {
                ((
                     [&state, &userType]()
                     {
                         if constexpr (StaticPtrSpec<Fields>)
                         {
                             std::string key = Fields::key;
                             if (Fields::isConstructor)
                             {
                                 (*userType)["new"] = sol::var(std::ref(*Fields::ptr));
                             }

                             if constexpr (!std::is_function_v<typename Fields::type>)
                             {
                                 LuaBinder<typename Fields::type>::declareType(state);
                             }
                             else
                             {
                                 LuaBinder<typename FunctionTyping<typename Fields::type>::ReturnType>::declareType(state);
                             }

                             if constexpr (IsFunctionObject<typename Fields::type>::value)
                             {
                                 // Static std::function's wrapped in shared_ptrs for the same reason as members
                                 (*userType)[Fields::key] = sol::property(
                                     []()
                                     {
                                         return std::shared_ptr<decltype(std::function(*(Fields::ptr)))>(new std::function(*(Fields::ptr)));
                                     },

                                     [](sol::object newVal)
                                     {
                                         if (newVal.is<std::shared_ptr<decltype(std::function(*(Fields::ptr)))>>())
                                         {
                                             *(Fields::ptr) = *(newVal.as<std::shared_ptr<decltype(std::function(*(Fields::ptr)))>>());
                                         }
                                         else
                                         {
                                             *(Fields::ptr) = newVal.as<decltype(*(Fields::ptr))>();
                                         }
                                     }

                                 );
                             }
                             else
                             {
                                 (*userType)[Fields::key] = sol::var(std::ref(*Fields::ptr));
                             }

                             return true;
                         }
                         else
                         {
                             return false;
                         }
                     }()),
                 ...);

                return;
            }
        };

        static void generateType(sol::state &state, sol::usertype<FunctionType> *newClassType)
        {
            FieldBinders<typename TypeLookup<FunctionType>::registeredType>::bindMembers(state, newClassType);
            FieldBinders<typename TypeLookup<FunctionType>::registeredType>::bindStaticFields(state, newClassType);
        };
    };
}

#endif