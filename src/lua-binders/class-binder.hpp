#pragma once

#include "../type-lookup.hpp"
#include "./type-binder.hpp"
#include "../class-typing.hpp"
#include "../concepts.hpp"
#include "./function-binder.hpp"
#include <sol.hpp>
#include <string>
#include <functional>
#include <set>
#include <vector>

namespace APICore
{

    template <ClassTypeConcept ClassType, class InheritedFrom>
    struct LuaBinderGenerator<ClassType, InheritedFrom>
    {
        template <typename T>
        struct FieldBinders
        {
            template <class C = ClassType>
            static void bindMembers(sol::state &state, sol::usertype<C> *userType)
            {
                using a = T::type;
                bool b = T::isClass;
                std::string c = T::name;
                std::string d = T::description;
                bool x = ClassTypingDef<T>;

                bool y = std::is_same_v<decltype(T::name), std::string>;
                bool z = std::is_same_v<decltype(T::description), std::string>;

                return;
            }

            template <class C = ClassType>
            static void bindStaticFields(sol::state &state, sol::usertype<C> *userType)
            {
            }
        };

        template <StringLiteral Name, StringLiteral Description, ClassField<ClassType>... Fields>
        struct FieldBinders<ClassTyping<Name, Description, ClassType, InheritedFrom, Fields...>>
        {

            template <class T = ClassType>
            static void bindMembers(sol::state &state, sol::usertype<T> *userType)
            {
                ((
                     [&state, &userType]()
                     {
                         if constexpr (FunctionBinder<Fields>::isMember || FunctionBinder<Fields>::isFunctional)
                         {
                             FunctionBinder<Fields>::template generateLuaBinding<T>(
                                 state,
                                 Fields::key,
                                 userType);
                             return true;
                         }
                         else if constexpr (MemberPtrSpec<Fields>)
                         {
                             std::string key = Fields::key;
                             auto ptr = Fields::ptr;

                             LuaBinder<typename Fields::type>::declareType(state);
                             (*userType)[Fields::key] = ptr;

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

            template <class T = ClassType>
            static void bindStaticFields(sol::state &state, sol::usertype<T> *userType)
            {
                ((
                     [&state, &userType]()
                     {
                         if constexpr (FunctionBinder<Fields>::isStaticField || FunctionBinder<Fields>::isFunctional)
                         {
                             FunctionBinder<Fields>::template generateLuaBinding<T>(
                                 state,
                                 Fields::key,
                                 userType);
                         }
                         else if constexpr (StaticPtrSpec<Fields>)
                         {
                             std::string key = Fields::key;
                             auto ptr = Fields::ptr;
                            
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

        template <ClassTypeConcept T, StringLiteral... TemplateParameters>
        struct FieldBinders<
            Templated<T, TemplateParameters...>> : public FieldBinders<T>
        {
        };

        template <class T, SpecializedTemplateDef... TemplateParameters>
        struct FieldBinders<
            TemplateSpecialization<T, TemplateParameters...>> : public FieldBinders<typename TypeLookup<T>::registeredType>
        {
        };

        template <class ChildType = ClassType>
        static void generateType(sol::state &state, sol::usertype<ChildType> *newClassType)
        {
            if constexpr (!std::is_void_v<InheritedFrom>)
            {
                using inherited_lookup = TypeLookup<InheritedFrom>;
                LuaBinder<InheritedFrom>::declareType(state);
                LuaBinderGenerator<
                    InheritedFrom,
                    typename inherited_lookup::registeredType::inheritedFrom>::generateType(state, newClassType);
            }
            std::string name = TypeLookup<ClassType>::registeredType::name;
            FieldBinders<typename TypeLookup<ClassType>::registeredType>::bindMembers(state, newClassType);
            FieldBinders<typename TypeLookup<ClassType>::registeredType>::bindStaticFields(state, newClassType);

            /*
                Add a universal getType member function to every class usertype.
                It returns the metatable for the type, which can be compared.
                Example:

                testObject:getType() == GlobalMetatable;

                The above will resolve to true if testObject is a usertype with bindings
                provided by the GlobalMetatable metatable.
                Note: It must be the exact type in order to match, and not a child class of the type.
            */
            (*newClassType)["getType"] = [newClassType](sol::variadic_args args)
            {
                return *newClassType;
            };
        };
    };
}
