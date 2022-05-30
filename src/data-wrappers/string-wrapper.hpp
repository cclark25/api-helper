// #ifndef STRING_DATA_WRAPPER
// #define STRING_DATA_WRAPPER
// #include <string>
// #include <memory>
// #include "../data-primitive.hpp"
// #include "../macros.hpp"
// #include "./data-wrapper.hpp"

// namespace APICore {
//     class StringWrapper : public DataWrapper {
//         virtual DataPrimitive getDataType(){
//             return DataPrimitive::string;
//         }
//     };

//     class StringContainerWrapper : public StringWrapper
//     {
//         Data stringData;

//     public:
//         StringContainerWrapper(std::string data = "")
//         {
//             this->stringData = std::shared_ptr<std::string>(new std::string(data));
//         }
//         virtual Data get()
//         {
//             return this->stringData;
//         }
//         virtual void set(Data data)
//         {
//             *(CastSharedPtr(std::string, this->stringData)) = *(CastSharedPtr(std::string, data));
//         }
//         virtual DataPrimitive getDataType(){
//             return DataPrimitive::string;
//         }
        
//         virtual bool canGet() { return true; }
//         virtual bool canSet() { return true; }
//     };

// }

// #endif