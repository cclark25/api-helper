// #ifndef ARRAY_DATA_WRAPPER
// #define ARRAY_DATA_WRAPPER
// #include <string>
// #include <vector>
// #include <memory>
// #include "../data-primitive.hpp"
// #include "../macros.hpp"
// #include "./data-wrapper.hpp"
// #include "../type-wrapper/type-wrapper.hpp"

// namespace APICore
// {
//     typedef std::vector<Data> ArrayVector;

//     class ArrayWrapper : public DataWrapper
//     {
//     public:
//         virtual DataPrimitive getDataType()
//         {
//             return DataPrimitive::array;
//         }
//         virtual Data getIndex(size_t index)
//         {
//             throw "Not Implemented!";
//         };
//         virtual void setIndex(size_t index, Data value)
//         {
//             throw "Not Implemented!";
//         };
//         virtual size_t push(Data value)
//         {
//             throw "Not Implemented!";
//         };
//         virtual Data pop()
//         {
//             throw "Not Implemented!";
//         };
//         virtual size_t insert(size_t index, Data value)
//         {
//             throw "Not Implemented!";
//         };
//         virtual size_t getLength(){
//             throw "Not Implemented!";
//         };
//         virtual DataPrimitive arrayOf(){
//             throw "Not Implemented!";
//         }
//     };

//     template<DataPrimitive primitive, typename realType>
//     class ArrayContainerWrapper : public ArrayWrapper
//     {
//         std::shared_ptr<ArrayVector> arrayData;
//         std::shared_ptr<ObjectTypeWrapper> objectTyping;

//     public:
//         virtual bool canGet() { return true; }
//         virtual bool canSet() { return true; }

//         ArrayContainerWrapper(std::shared_ptr<ArrayVector> data)
//         {
//             this->arrayData = data;
//         }
//         ArrayContainerWrapper(ArrayVector &data)
//         {
//             this->arrayData = std::shared_ptr<ArrayVector>(new ArrayVector(data));
//         }
//         ArrayContainerWrapper()
//         {
//             this->arrayData = std::shared_ptr<ArrayVector>(new ArrayVector());
//         }
//         virtual Data get()
//         {
//             return this->arrayData;
//         }
//         virtual void set(Data data)
//         {
//             this->arrayData = CastSharedPtr(ArrayVector, data);
//         }

//         virtual Data getIndex(size_t index)
//         {
//             auto data = this->get();
//             auto arrayData = CastSharedPtr(ArrayVector, data);
//             if (arrayData->size() - 1 >= index)
//             {
//                 return arrayData->at(index);
//             }
//             else
//             {
//                 return nullptr;
//             }
//         }

//         virtual void setIndex(size_t index, Data value)
//         {
//             auto data = this->get();
//             auto arrayData = CastSharedPtr(ArrayVector, data);

//             arrayData->at(index) = value;
//         }

//         virtual size_t push(Data value){
//             auto data = this->get();
//             auto arrayData = CastSharedPtr(ArrayVector, data);

//             arrayData->push_back(value);
//             return arrayData->size();
//         };

//         virtual Data pop()
//         {
//             auto data = this->get();
//             auto arrayData = CastSharedPtr(ArrayVector, data);

//             Data last = arrayData->back();
//             arrayData->pop_back();
//             return last;
//         };
//         virtual size_t insert(size_t index, Data value)
//         {
//             auto data = this->get();
//             auto arrayData = CastSharedPtr(ArrayVector, data);

//             auto it = arrayData->begin() += index;

//             arrayData->insert(it, value);

//             return arrayData->size();
//         };
//         virtual size_t getLength(){
//             auto data = this->get();
//             auto arrayData = CastSharedPtr(ArrayVector, data);
//             return(arrayData->size());
//         };
        
//         virtual DataPrimitive arrayOf(){
//             return primitive;
//         }
//     };

// }

// #endif