// #ifndef CLASS_DATA_WRAPPER
// #define CLASS_DATA_WRAPPER
// #include <string>
// #include <map>
// #include <memory>
// #include "../data-primitive.hpp"
// #include "../macros.hpp"
// #include "./data-wrapper.hpp"

// namespace APICore
// {

//     template <>
//     class DataWrapperSub<DataPrimitive::classType> : public DataWrapper
//     {
//     public:
//         virtual DataPrimitive getDataType() { return DataPrimitive::classType; };
//         virtual Data<DataPrimitive::classType> get() { throw "Not Implemented!"; };
//         virtual void set(Data<DataPrimitive::classType> data) { throw "Not Implemented!"; };
//     };

//     using ClassWrapper = DataWrapperSub<DataPrimitive::classType>;

//     template <>
//     class DataContainerWrapper<DataPrimitive::classType> : public ClassWrapper
//     {
//         Data<DataPrimitive::classType> classData;

//     public:
//         virtual bool canGet() { return true; }
//         virtual bool canSet() { return true; }

//         DataContainerWrapper<DataPrimitive::classType>(Data<DataPrimitive::classType> data)
//         {
//             this->classData = data;
//         }
//         DataContainerWrapper<DataPrimitive::classType>(data_primitive_to_type<DataPrimitive::classType> &data)
//         {
//             this->classData = Data<DataPrimitive::classType>(new data_primitive_to_type<DataPrimitive::classType>(data));
//         }
//         DataContainerWrapper<DataPrimitive::classType>()
//         {
//             this->classData = Data<DataPrimitive::classType>(new data_primitive_to_type<DataPrimitive::classType>());
//         }
//         virtual Data<DataPrimitive::classType> get()
//         {
//             return this->classData;
//         }
//         virtual void set(Data<DataPrimitive::classType> data)
//         {
//             this->classData = CastSharedPtr(data_primitive_to_type<DataPrimitive::classType>, data);
//         }
//     };

//     class ObjectContainerWrapper : public DataContainerWrapper<DataPrimitive::object>
//     {
//     };
// }

// #endif