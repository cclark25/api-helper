#ifndef CLASS_LUA_BINDER
#define CLASS_LUA_BINDER

#include "../type-lookup.hpp"
#include "./type-binder.hpp"
#include "../class-typing.hpp"
#include "../concepts.hpp"
#include <sol.hpp>
#include <string>

namespace APICore
{
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
                         if constexpr (MemberPtrSpec<Fields>)
                         {
                             std::string key = Fields::key;
                             auto ptr = Fields::ptr;
                             if constexpr (!std::is_function_v<typename Fields::type>)
                             {
                                 LuaBinder<typename Fields::type>::declareType(state);
                                 (*userType)[Fields::key] = ptr;
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
                             if (Fields::isConstructor)
                             {
                                 (*userType)["new"] = sol::var(std::ref(*Fields::ptr));
                             }

                             if constexpr (!std::is_function_v<typename Fields::type>)
                             {
                                 LuaBinder<typename Fields::type>::declareType(state);
                             }
                             std::string key = Fields::key;

                             (*userType)[Fields::key] = sol::var(std::ref(*Fields::ptr));
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

        static sol::usertype<FunctionType> *generateType(sol::state &state)
        {
            sol::usertype<FunctionType> *newClassType = new sol::usertype<FunctionType>;
            (*newClassType) = state.new_usertype<FunctionType>(
                TypeLookup<FunctionType>::registeredType::name,
                sol::constructors<FunctionType()>());

            FieldBinders<typename TypeLookup<FunctionType>::registeredType>::bindMembers(state, newClassType);
            FieldBinders<typename TypeLookup<FunctionType>::registeredType>::bindStaticFields(state, newClassType);

            return newClassType;
        };
    };
}

#endif