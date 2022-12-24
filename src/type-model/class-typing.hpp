#ifndef CLASS_TYPING
#define CLASS_TYPING

#include "./member-pointer.hpp"
#include "./member-function-pointer.hpp"
#include "./static-pointer.hpp"
#include <string>

namespace APICore
{
    template <typename T, class ClassType>
    concept ClassMember = requires
    {
        requires MemberPtrSpec<T> || MemberFunctionPtrSpec<T>;
        requires std::is_same<typename T::classType, ClassType>::value;
    };

    template <typename T, class ClassType>
    concept ClassField = StaticPtrSpec<T> || ClassMember<T, ClassType>;

    template <StringLiteral ClassName, StringLiteral ClassDescription, class ClassType, ClassField<ClassType>... Fields>
    struct ClassTyping
    {
        using type = ClassType;
        using isClass = void;
        static std::string name;
        static std::string description;

        template <template <ClassField<ClassType>> class Callback>
        static void memberCallback()
        {
            ((ClassMember<Fields, ClassType> ? []()
                  { Callback<Fields>::processMemberField(); return true; }()
                                             : false),
             ...);
        }

        template <template <ClassField<ClassType>> class Callback>
        static void staticFieldCallback()
        {
            ((StaticPtrSpec<Fields> ? []()
                  { Callback<Fields>::processStaticField(); return true; }()
                                             : false),
             ...);
        }
    };
    template <StringLiteral ClassName, StringLiteral ClassDescription, class ClassType, ClassField<ClassType>... Fields>
    std::string ClassTyping<ClassName, ClassDescription, ClassType, Fields...>::name = ClassName.value;
    template <StringLiteral ClassName, StringLiteral ClassDescription, class ClassType, ClassField<ClassType>... Fields>
    std::string ClassTyping<ClassName, ClassDescription, ClassType, Fields...>::description = ClassDescription.value;

    template <typename T>
    concept ClassTypingDef = requires {
        { T::type };
        { T::isClass };
        { T::name } -> std::convertible_to<std::string>;
        { T::description } -> std::convertible_to<std::string>;
    };
};

#endif