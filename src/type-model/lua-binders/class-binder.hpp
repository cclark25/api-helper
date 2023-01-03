#ifndef CLASS_LUA_BINDER
#define CLASS_LUA_BINDER

#include "../type-lookup.hpp"
#include "./type-binder.hpp"
#include "../class-typing.hpp"
#include "./concepts.hpp"
#include <sol.hpp>
#include <string>

namespace APICore
{
    template <ClassForLua ClassType>
    struct LuaBinderGenerator<ClassType>
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
            static void bindStaticFields(sol::state &state, sol::usertype<ClassType> *userType)
            {
                ((
                     [&state, &userType]()
                     {
                         if constexpr (StaticPtrSpec<Fields>)
                         {
                             if constexpr (!std::is_function_v<typename Fields::type>)
                             {
                                 LuaBinder<typename Fields::type>::declareType(state);
                             }
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

        static sol::usertype<ClassType> generateType(sol::state &state)
        {
            sol::usertype<ClassType> newClassType = state.new_usertype<ClassType>(
                TypeLookup<ClassType>::registeredType::name,
                sol::constructors<ClassType()>());

            FieldBinders<typename TypeLookup<ClassType>::registeredType>::bindMembers(state, &newClassType);
            FieldBinders<typename TypeLookup<ClassType>::registeredType>::bindStaticFields(state, &newClassType);

            return newClassType;
        };
    };

    // template <class ClassType>
    //     requires requires(ClassType val) {
    //                  requires std::is_class_v<ClassType>;
    //                  requires !requires {
    //                                {
    //                                    *val
    //                                };
    //                            };
    //              }
    // struct LuaBinder<ClassType>
    // {
    //     template <typename T>
    //     struct FieldBinders
    //     {

    //         static void bindMembers(sol::state &state, sol::usertype<auto> *userType)
    //         {
    //         }

    //         static void bindStaticFields(sol::state &state, sol::usertype<auto> *userType)
    //         {
    //         }
    //     };

    //     template <StringLiteral Name, StringLiteral Description, ClassField<ClassType>... Fields>
    //     struct FieldBinders<ClassTyping<Name, Description, ClassType, Fields...>>
    //     {
    //         static void bindMembers(sol::state &state, sol::usertype<ClassType> *userType)
    //         {
    //             ((
    //                  MemberPtrSpec<Fields> ? [&state, &userType]()
    //                      {
    //                     LuaBinder<typename Fields::type>::bind(state, userType, Fields::ptr, std::string(Fields::key));
    //                     return true; }()
    //                                        : false),
    //              ...);

    //             return;
    //         }
    //         static void bindStaticFields(sol::state &state, sol::usertype<ClassType> *userType)
    //         {
    //             ((
    //                  StaticPtrSpec<Fields> ? [&state, &userType]()
    //                      {
    //                     LuaBinder<typename Fields::type>::bind(state, userType, Fields::ptr, std::string(Fields::key));
    //                     return true; }()
    //                                        : false),
    //              ...);

    //             return;
    //         }
    //     };

    //     using type_definition = TypeLookup<ClassType>;

    //     template <class ParentClassType>
    //     static void bind(sol::state &state, sol::usertype<ParentClassType> *userType)
    //     {
    //         sol::usertype<ClassType> newClassType = state.new_usertype<ClassType>(
    //             type_definition::registeredType::name,
    //             sol::constructors<ClassType()>());

    //         FieldBinders<typename type_definition::registeredType>::bindMembers(state, &newClassType);
    //         FieldBinders<typename type_definition::registeredType>::bindStaticFields(state, &newClassType);

    //         // TODO: add support for defining constructors
    //     }
    // };

}

#endif