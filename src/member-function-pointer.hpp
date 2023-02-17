#pragma once

#include "./string-literal.hpp"
#include "./member-pointer.hpp"
#include "./type-lookup.hpp"
#include "./function-parameters.hpp"
#include <type_traits>
#include <string>
#include <vector>
#include <functional>

namespace APICore
{
    template <StringLiteral Key, auto Pointer, StringLiteral Description, ParameterPackDefinition Parameters>
    struct MemberFunction : public MemberPtr<Pointer>
    {
        using pointerTypeLookup = void;
        using parameterPack = Parameters;
        static std::string key;
        static std::string description;
    };
    template <StringLiteral Key, auto Pointer, StringLiteral Description, ParameterPackDefinition Parameters>
    std::string MemberFunction<Key, Pointer, Description, Parameters>::key = Key.value;
    template <StringLiteral Key, auto Pointer, StringLiteral Description, ParameterPackDefinition Parameters>
    std::string MemberFunction<Key, Pointer, Description, Parameters>::description = Description.value;

    template <typename PointerType, ParameterPackDefinition ParameterDescriptions>
    struct MemberFunctionTyping
    {
        using classType = void;
        using returnType = void;

        template<class T>
        using childMemberCast = void;

        static const size_t parameterCount = 0;
    };

    template <class ClassType, typename ReturnType, ParameterPackDefinition ParameterDescriptions, typename... Parameters>
    struct MemberFunctionTyping<ReturnType (ClassType::*)(Parameters...), ParameterDescriptions>
    {
        using classType = ClassType;
        using returnType = ReturnType;

        template<class T>
        requires std::is_base_of_v<ClassType, T>
        using childMemberCast = ReturnType (T::*)(Parameters...);
        
        static const size_t parameterCount = sizeof...(Parameters);

        template<size_t N> using ParameterType =
        typename std::tuple_element<N, std::tuple<Parameters...>>::type;
    };

    template <typename PointerType, StringLiteral Key, PointerType Pointer, StringLiteral Description, ParameterPackDefinition Parameters>
    struct MemberFunction<Key, Pointer, Description, Parameters> : public MemberPtr<Pointer>
    {
        using functionTyping = MemberFunctionTyping<decltype(Pointer), Parameters>;
        using parameterPack = Parameters;
        static std::vector<std::pair<std::string, std::string>> parameters;
        static std::string key;
        static std::string description;
        static bool lookupCreated;
    };

    template <typename PointerType, StringLiteral Key, PointerType Pointer, StringLiteral Description, ParameterPackDefinition Parameters>
    std::string MemberFunction<Key, Pointer, Description, Parameters>::key = Key.value;
    template <typename PointerType, StringLiteral Key, PointerType Pointer, StringLiteral Description, ParameterPackDefinition Parameters>
    std::string MemberFunction<Key, Pointer, Description, Parameters>::description = Description.value;

    template <typename T, typename ClassToBind>
    concept MemberLikeFunction = requires(T v, ClassToBind &c) {
                                     {
                                         std::bind(v, c)
                                     };
                                 };

    template <typename T>
    concept MemberFunctionPtrSpec = requires() {
                                        requires MemberPtrSpec<T>;
                                        requires std::is_class_v<typename T::functionTyping::classType>;
                                        requires std::is_same_v<
                                            typename T::functionTyping::returnType,
                                            std::type_identity_t<typename T::functionTyping::returnType>>;
                                    };


    template <class T>
    concept MemberOverloadSpec = requires() {
        requires T::isMemberOverload;
    };
    
    template<MemberFunctionPtrSpec FirstFunction, MemberFunctionPtrSpec... Functions>
    struct MemberOverload {
        static const bool isMemberOverload = true;
        static std::string getKey() {
            return FirstFunction::key;
        }
        
        template<MemberFunctionPtrSpec... OtherFunctions>
        using AddOverloads = MemberOverload<FirstFunction, OtherFunctions..., Functions...>;
    };

};
