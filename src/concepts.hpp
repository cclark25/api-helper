#pragma once

#include <type_traits>
#include <string>
#include <functional>

namespace APICore
{
    template <class T>
    concept ReferenceTypeConcept = requires(T tval) {
                                       {
                                           *tval
                                       };

                                       requires !std::is_function_v<T>;
                                   };

    struct TemplateParamBase;
    template <class T>
    concept TemplatedDataConcept = requires {
                                       requires std::is_base_of_v<TemplateParamBase, T>;
                                   };

    template <class ClassType>
    concept ClassTypeConcept = requires {
                                   requires std::is_class_v<ClassType>;
                                   requires !ReferenceTypeConcept<ClassType>;
                                   requires !TemplatedDataConcept<ClassType>;
                                   requires !std::is_same_v<ClassType, std::string>;
                                   requires !std::is_function_v<ClassType>;
                               };
    template <typename T>
    concept ClassTypingDef = requires {
                                 requires std::is_same_v<typename T::type, typename T::type>;
                                 requires T::isClass;

                                 requires std::is_same_v<decltype(T::name), std::string>;
                                 requires std::is_same_v<decltype(T::description), std::string>;
                             };

    template <ReferenceTypeConcept T>
    struct dereference
    {
        using type = std::remove_reference_t<decltype(*T())>;
    };

    template <class T>
    struct IsFunctionObject
    {
        static const bool value = false;
    };
    // template<class T>
    // bool IsFunctionObject<T>::value = false;

    template <class R, class... P>
    struct IsFunctionObject<std::function<R(P...)>>
    {
        static const bool value = true;
    };
    // template<class R, class... P>
    // bool IsFunctionObject<std::function<R(P...)>>::value = true;

    template <typename T>
    concept MemberPtrSpec = requires {
                                requires T::isMember;
                                requires std::is_same_v<decltype(T::key), std::string>;
                                requires std::is_same_v<decltype(T::ptr), typename T::ptrType>;
                            };

    template <typename T>
    concept MemberFunctionPtrSpec = requires() {
                                        requires MemberPtrSpec<T>;
                                        requires std::is_class_v<typename T::functionTyping::classType>;
                                        requires std::is_same_v<
                                            typename T::functionTyping::returnType,
                                            std::type_identity_t<typename T::functionTyping::returnType>>;
                                    };

    template<typename T>
    concept StaticPtrSpec = requires  {
       requires std::is_same_v<decltype(T::key), std::string>;
       requires std::is_same_v<decltype(T::ptr), typename T::ptrType>;
       requires std::is_same_v<decltype(T::ptr), typename T::type*>;
       requires !T::isMember ;
    };

    template <typename T>
    concept StaticFunctionPtrSpec = requires() {
                                        requires StaticPtrSpec<T>;
                                        
                                        requires std::is_same_v<
                                            typename T::functionTyping::returnType,
                                            std::type_identity_t<typename T::functionTyping::returnType>>;
                                    };
}
