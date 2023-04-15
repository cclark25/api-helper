#pragma once
#include "./concepts.hpp"
#include <memory>
#include <vector>
#include <string>

namespace APICore {
    template<class T, StringLiteral... TemplateParameters>
    requires (ClassTypingDef<T> || MemberFunctionPtrSpec<T> || StaticFunctionPtrSpec<T>)
    struct Templated : public T {
        static std::vector<std::string> templateParameterNames;
        static const bool isTemplated = true;
    };
    template<ClassTypingDef T, StringLiteral... TemplateParameters>
    std::vector<std::string> Templated<T, TemplateParameters...>::templateParameterNames = {
        std::string(TemplateParameters.value)...
    };

    template<class T>
    concept TemplatedDef = requires {
        requires T::isTemplated;
    };


    struct TemplateParamBase {
        static const bool isTemplateParam = true;
        std::shared_ptr<void> data = nullptr;
    };
    template<StringLiteral Name>
    struct TemplateParam : public TemplateParamBase {
        static std::string name;
        template<class T>
        TemplateParam(std::shared_ptr<T> ptr){
            this->data = ptr;
        }
    };
    template<StringLiteral Name>
    std::string TemplateParam<Name>::name = Name.value;

    template<TemplatedDataConcept Param, class Definition>
    struct SpecializedTemplateParam {
        static const bool isSpecializedTemplate = true;
        using parameter = Param;
        using type = Definition;
    };

    template<class T>
    concept SpecializedTemplateDef = requires {
        requires T::isSpecializedTemplate;
    };

    template<class TemplatedType, SpecializedTemplateDef... Definitions>
    struct TemplateSpecialization : public TypeLookup<TemplatedType>::registeredType {
        using templatedType = TemplatedType;
    };
}