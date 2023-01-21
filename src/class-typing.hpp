#ifndef CLASS_TYPING
#define CLASS_TYPING

#include "./member-pointer.hpp"
#include "./member-function-pointer.hpp"
#include "./static-pointer.hpp"
#include <string>

namespace APICore
{
    template <typename T, class FunctionType>
    concept ClassMember = requires {
        requires true;
                            //   requires MemberPtrSpec<T> || MemberFunctionPtrSpec<T>;
                            //   requires std::is_same<typename T::classType, FunctionType>::value;
                          };

    template <typename T, class FunctionType>
    concept ClassField = StaticPtrSpec<T> || ClassMember<T, FunctionType>;

    template <StringLiteral ClassName, StringLiteral ClassDescription, class FunctionType, ClassField<FunctionType>... Fields>
    struct ClassTyping
    {
        using type = FunctionType;
        using isClass = void;

        static std::string name;
        static std::string description;
    };
    template <StringLiteral ClassName, StringLiteral ClassDescription, class FunctionType, ClassField<FunctionType>... Fields>
    std::string ClassTyping<ClassName, ClassDescription, FunctionType, Fields...>::name = ClassName.value;
    template <StringLiteral ClassName, StringLiteral ClassDescription, class FunctionType, ClassField<FunctionType>... Fields>
    std::string ClassTyping<ClassName, ClassDescription, FunctionType, Fields...>::description = ClassDescription.value;

    template <typename T>
    concept ClassTypingDef = requires {
                                 {
                                     T::type
                                 };
                                 {
                                     T::isClass
                                 };
                                 {
                                     T::name
                                     } -> std::convertible_to<std::string>;
                                 {
                                     T::description
                                     } -> std::convertible_to<std::string>;
                             };
};

#endif