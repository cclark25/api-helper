#ifndef ARRAY_DATA_WRAPPER
#define ARRAY_DATA_WRAPPER
#include <string>
#include <vector>
#include <memory>
#include "../data-primitive.hpp"
#include "../macros.hpp"
#include "./data-wrapper.hpp"
#include "../type-wrapper/type-wrapper.hpp"

namespace APICore
{
    typedef std::vector<std::shared_ptr<DataWrapper>> ArrayVector;

    class ArrayWrapper : public DataWrapper
    {
    public:
        virtual DataPrimitive getDataType()
        {
            return DataPrimitive::array;
        }
        virtual std::shared_ptr<DataWrapper> getIndex(size_t index)
        {
            throw "Not Implemented!";
        };
        virtual void setIndex(size_t index, std::shared_ptr<DataWrapper> value)
        {
            throw "Not Implemented!";
        };
        virtual size_t push(std::shared_ptr<DataWrapper> value)
        {
            throw "Not Implemented!";
        };
        virtual std::shared_ptr<DataWrapper> pop()
        {
            throw "Not Implemented!";
        };
        virtual size_t insert(size_t index, std::shared_ptr<DataWrapper> value)
        {
            throw "Not Implemented!";
        };
    };

    class ArrayContainerWrapper : public ArrayWrapper
    {
        std::shared_ptr<ArrayVector> arrayData;
        std::shared_ptr<ObjectTypeWrapper> objectTyping;

    public:
        virtual bool canGet() { return true; }
        virtual bool canSet() { return true; }

        ArrayContainerWrapper(std::shared_ptr<ArrayVector> data)
        {
            this->arrayData = data;
        }
        ArrayContainerWrapper(ArrayVector &data)
        {
            this->arrayData = std::shared_ptr<ArrayVector>(new ArrayVector(data));
        }
        ArrayContainerWrapper()
        {
            this->arrayData = std::shared_ptr<ArrayVector>(new ArrayVector());
        }
        virtual Data get()
        {
            return this->arrayData;
        }
        virtual void set(Data data)
        {
            this->arrayData = CastSharedPtr(ArrayVector, data);
        }

        virtual std::shared_ptr<DataWrapper> getIndex(size_t index)
        {
            auto data = this->get();
            auto arrayData = CastSharedPtr(ArrayVector, data);
            if (arrayData->size() - 1 >= index)
            {
                return arrayData->at(index);
            }
            else
            {
                return std::shared_ptr<DataWrapper>(nullptr);
            }
        }

        virtual void setIndex(size_t index, std::shared_ptr<DataWrapper> value)
        {
            auto data = this->get();
            auto arrayData = CastSharedPtr(ArrayVector, data);

            arrayData->at(index) = value;
        }

        virtual size_t push(std::shared_ptr<DataWrapper> value){
            auto data = this->get();
            auto arrayData = CastSharedPtr(ArrayVector, data);

            arrayData->push_back(value);
            return arrayData->size();
        };

        virtual std::shared_ptr<DataWrapper> pop()
        {
            auto data = this->get();
            auto arrayData = CastSharedPtr(ArrayVector, data);

            std::shared_ptr<APICore::DataWrapper> last = arrayData->back();
            arrayData->pop_back();
            return last;
        };
        virtual size_t insert(size_t index, std::shared_ptr<DataWrapper> value)
        {
            auto data = this->get();
            auto arrayData = CastSharedPtr(ArrayVector, data);

            auto it = arrayData->begin() += index;

            arrayData->insert(it, value);

            return arrayData->size();
        };
    };

}

#endif