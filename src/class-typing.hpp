#pragma once

#include "./member-pointer.hpp"
#include "./member-function-pointer.hpp"
#include "./static-pointer.hpp"
#include <string>

namespace APICore
{
    template <typename T, class ClassType>
    concept ClassMember = requires {
        requires true;
                              requires MemberPtrSpec<T> || MemberFunctionPtrSpec<T> ;
                              requires std::is_same<typename T::classType, ClassType>::value;
                          };
    
    template <typename T, class ClassType>
    concept ClassField = StaticPtrSpec<T> || ClassMember<T, ClassType> || MemberOverloadSpec<T> || StaticOverloadSpec<T>;

    template <StringLiteral ClassName, StringLiteral ClassDescription, class ClassType, class InheritedFrom, ClassField<ClassType>... Fields>
    requires ( std::is_void_v<InheritedFrom> || std::derived_from<ClassType, InheritedFrom>)
    struct ClassTyping
    {
        using type = ClassType;
        using inheritedFrom = InheritedFrom;
        static const bool isClass = true;

        static std::string name;
        static std::string description;
    };
    template <StringLiteral ClassName, StringLiteral ClassDescription, class ClassType, class InheritedFrom, ClassField<ClassType>... Fields>
    std::string ClassTyping<ClassName, ClassDescription, ClassType, InheritedFrom, Fields...>::name = ClassName.value;
    template <StringLiteral ClassName, StringLiteral ClassDescription, class ClassType, class InheritedFrom, ClassField<ClassType>... Fields>
    std::string ClassTyping<ClassName, ClassDescription, ClassType, InheritedFrom, Fields...>::description = ClassDescription.value;

    template <typename T>
    concept ClassTypingDef = requires {
                                 requires std::is_same_v<typename T::type,typename T::type>;
                                 requires T::isClass;
                                 
                                 requires std::is_same_v<decltype(T::name),std::string>;
                                 requires std::is_same_v<decltype(T::description),std::string>;
                             };
};
