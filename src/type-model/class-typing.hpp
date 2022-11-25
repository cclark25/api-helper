#ifndef CLASS_TYPING
#define CLASS_TYPING

#include "./member-pointer.hpp"
#include "./static-pointer.hpp"

namespace APICore
{
    template <typename T, class ClassType>
    concept ClassMember = requires
    {
        requires MemberPtrSpec<T>;
        requires std::is_same<typename T::classType, ClassType>::value;
    };

    template <typename T, class ClassType>
    concept ClassField = StaticPtrSpec<T> || ClassMember<T, ClassType>;

    template <class ClassType, ClassField<ClassType>... Fields>
    struct ClassTyping
    {
        using type = ClassType;

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
};

#endif