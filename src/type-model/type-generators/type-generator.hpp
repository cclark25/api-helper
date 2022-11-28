#ifndef BASE_TYPE_GENERATOR
#define BASE_TYPE_GENERATOR

#include <memory>
#include <string>
#include "../../type-wrapper/type-wrapper.hpp"

namespace APICore {
    template<class T>
    struct TypeGenerator;

    template<>
    struct TypeGenerator<std::string> {
        static std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> generateTyping(){
            return makeBasicType<DataPrimitive::string>();
        }
    };

    template<>
    struct TypeGenerator<int> {
        static std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> generateTyping(){
            return makeBasicType<DataPrimitive::int32>();
        }
    };

    template<>
    struct TypeGenerator<void> {
        static std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> generateTyping(){
            return makeBasicType<DataPrimitive::null>();
        }
    };

    template<>
    struct TypeGenerator<double> {
        static std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> generateTyping(){
            return makeBasicType<DataPrimitive::unknown>();
        }
    };
}

#include "./class-generator.hpp"
#include "./function-generator.hpp"

#endif