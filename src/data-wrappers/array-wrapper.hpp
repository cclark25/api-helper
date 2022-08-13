#ifndef ARRAY_DATA_WRAPPER
#define ARRAY_DATA_WRAPPER
#include <string>
#include <map>
#include <memory>
#include "../data-primitive.hpp"
#include "../macros.hpp"
#include "./data-wrapper.hpp"
#include "../type-wrapper/type-wrapper.hpp"

namespace APICore
{

    template <>
    class DataWrapperSub<DataPrimitive::array> : public DataWrapper
    {
    public:
        virtual DataPrimitive getDataType() { return DataPrimitive::array; };
        virtual Data<DataPrimitive::array> get() { throw "Not Implemented!"; };
        virtual void set(Data<DataPrimitive::array> data) { throw "Not Implemented!"; };
        virtual std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> getType()
        {
            return basicType<DataPrimitive::array>;
        }

        virtual std::shared_ptr<DataWrapper> getIndex(size_t index) { throw "Not Implemented!"; };
        virtual void setIndex(size_t index, std::shared_ptr<DataWrapper> value) { throw "Not Implemented!"; };
        virtual size_t push(std::shared_ptr<DataWrapper> value) { throw "Not Implemented!"; };
        virtual std::shared_ptr<DataWrapper> pop() { throw "Not Implemented!"; };
        virtual size_t insert(size_t index, std::shared_ptr<DataWrapper> value) { throw "Not Implemented!"; };
        virtual size_t length() { throw "Not Implemented!"; };
    };

    using ArrayWrapper = DataWrapperSub<DataPrimitive::array>;

    template <>
    class DataContainerWrapper<DataPrimitive::array> : public ArrayWrapper
    {
        Data<DataPrimitive::array> arrayData;

    public:
        virtual bool canGet() { return true; }
        virtual bool canSet() { return true; }

        DataContainerWrapper<DataPrimitive::array>(Data<DataPrimitive::array> data)
        {
            this->arrayData = data;
        }

        DataContainerWrapper<DataPrimitive::array>()
        {
            this->arrayData = Data<DataPrimitive::array>(new data_primitive_to_type<DataPrimitive::array>());
        }
        virtual Data<DataPrimitive::array> get()
        {
            return this->arrayData;
        }
        virtual void set(Data<DataPrimitive::array> data)
        {
            this->arrayData = CastSharedPtr(data_primitive_to_type<DataPrimitive::array>, data);
        }

        virtual std::shared_ptr<DataWrapper> getIndex(size_t index)
        {
            if (index < this->length())
            {
                return this->arrayData->at(index);
            }
            else
            {
                return nullptr;
            }
        };
        virtual void setIndex(size_t index, std::shared_ptr<DataWrapper> value)
        {
            if (index < this->length())
            {
                this->arrayData->at(index) = value;
            }
            else
            {
                throw "Cannot set at index because the index is out of range.";
            }
        };
        virtual size_t push(std::shared_ptr<DataWrapper> value)
        {
            this->arrayData->push_back(value);
            return this->length();
        };
        virtual std::shared_ptr<DataWrapper> pop()
        {
            size_t length = this->length();
            if (length > 0)
            {
                auto lastElement = this->getIndex(length - 1);
                this->arrayData->pop_back();
                return lastElement;
            }
            else
            {
                return nullptr;
            }
        };
        virtual size_t insert(size_t index, std::shared_ptr<DataWrapper> value)
        {
            if (index < this->length())
            {
                auto iter = this->arrayData->cbegin() += index;
                this->arrayData->insert(iter, value);

                return this->length();
            }
            else
            {
                throw "Cannot set at index because the index is out of range.";
            }
        };
        virtual size_t length() { return this->arrayData->size(); };
    };

    class ArrayContainerWrapper : public DataContainerWrapper<DataPrimitive::array>
    {
    };
}

#endif