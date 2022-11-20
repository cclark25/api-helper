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

    template<typename T, class ClassType>
    concept ClassField = StaticPtrSpec<T> || ClassMember<T,ClassType>;

    template <class ClassType, ClassField<ClassType>... Members>
    struct ClassTyping
    {
        using type = ClassType;
    };
};

#endif