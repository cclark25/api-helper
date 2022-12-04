#ifndef BASE_TYPE_GENERATOR
#define BASE_TYPE_GENERATOR

#include <memory>
#include <string>
#include "../../type-wrapper/type-wrapper.hpp"

namespace APICore {
    template<class T, StringLiteral TypeGeneratorTag = "ANY", class... ExtraData>
    struct TypeGenerator;

    template<class... ExtraData>
    struct TypeGenerator<std::string, "ANY", ExtraData...> {
        static std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> generateTyping(){
            return makeBasicType<DataPrimitive::string>();
        }
    };
    
    template<class... ExtraData>
    struct TypeGenerator<int, "ANY", ExtraData...> {
        static std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> generateTyping(){
            return makeBasicType<DataPrimitive::int32>();
        }
    };

    template<class... ExtraData>
    struct TypeGenerator<void, "ANY", ExtraData...> {
        static std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> generateTyping(){
            return makeBasicType<DataPrimitive::null>();
        }
    };

    template<class... ExtraData>
    struct TypeGenerator<double, "ANY", ExtraData...> {
        static std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> generateTyping(){
            return makeBasicType<DataPrimitive::doubleType>();
        }
    };
}

#include "./class-generator.hpp"
#include "./function-generator.hpp"

#endif