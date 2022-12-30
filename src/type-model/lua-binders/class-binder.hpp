#ifndef CLASS_LUA_BINDER
#define CLASS_LUA_BINDER

#include "../type-lookup.hpp"
#include "./type-binder.hpp"
#include "../class-typing.hpp"
#include <sol.hpp>
#include <string>

namespace APICore
{
    template <class ParentClassType, class ClassType, class... ExtraData>
    requires std::is_class<ClassType>::value
    struct LuaBinder<ParentClassType, ClassType, "ANY", ExtraData...>
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
        
        template <StringLiteral Name, StringLiteral Description, ClassField<ClassType>... Fields>
        struct FieldBinders<ClassTyping<Name, Description, ClassType, Fields...>>
        {
            static void bindMembers(sol::state &state, sol::usertype<ClassType> *userType)
            {
                ((
                     MemberPtrSpec<Fields> ? [&state, &userType]()
                         { 
                        LuaBinder<ClassType, typename Fields::type, "MEMBER", Fields>::bind(state, userType, Fields::ptr, std::string(Fields::key)); 
                        return true; }()
                                           : false),
                 ...);

                return;
            }
            static void bindStaticFields(sol::state &state, sol::usertype<ClassType> *userType)
            {
                 ((
                     StaticPtrSpec<Fields> ? [&state, &userType]()
                         { 
                        LuaBinder<ClassType, typename Fields::type, "MEMBER", Fields>::bind(state, userType, Fields::ptr, std::string(Fields::key)); 
                        return true; }()
                                           : false),
                 ...);

                return;
            }
        };

        using type_definition = TypeLookup<ClassType>;

        static void bind(sol::state &state, sol::usertype<ParentClassType> *userType)
        {
            sol::usertype<ClassType> newClassType = state.new_usertype<ClassType>(
                type_definition::registeredType::name,
                sol::constructors<ClassType()>());
            

            FieldBinders<typename type_definition::registeredType>::bindMembers(state, &newClassType);            
            FieldBinders<typename type_definition::registeredType>::bindStaticFields(state, &newClassType);

            // TODO: add support for defining constructors
        }
    };

}

#endif