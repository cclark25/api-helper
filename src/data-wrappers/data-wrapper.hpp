#ifndef API_DATA_WRAPPER
#define API_DATA_WRAPPER
#include <string>
#include <memory>
#include "../data-primitive.hpp"
#include "../macros.hpp"
#include "../type-wrapper/type-wrapper.hpp"

#define HasGetAccess(accessLevel) accessLevel == GetSet::readAndWrite || GetSet::readOnly
#define HasSetAccess(accessLevel) accessLevel == GetSet::readAndWrite || GetSet::writeOnly

namespace APICore
{
    using namespace APICore;

    template <DataPrimitive T>
    using Data = std::shared_ptr<data_primitive_to_type<T>>;

    class DataWrapper
    {
    public:
        virtual bool canGet() { throw "Not Implemented!"; };
        virtual bool canSet() { throw "Not Implemented!"; };
        virtual DataPrimitive getDataType() { throw "Not Implemented!"; };
        virtual std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> getType() { return nullptr; }
        virtual ~DataWrapper() {}
    };

    template <DataPrimitive T>
    class DataWrapperSub : public DataWrapper
    {
    public:
        virtual DataPrimitive getDataType() { return T; };
        virtual Data<T> get() { throw "Not Implemented!"; };
        virtual void set(Data<T> data) { throw "Not Implemented!"; };
        virtual std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> getType()
        {
            return makeBasicType<T>("", !this->canSet());
        }
    };

    template <DataPrimitive T>
    class DataContainerWrapper : public DataWrapperSub<T>
    {
        std::shared_ptr<data_primitive_to_type<T>> internalData;
        std::shared_ptr<TypeWrapper<T>> typing;

    public:
        DataContainerWrapper<T>(data_primitive_to_type<T> data, std::shared_ptr<TypeWrapper<T>> typing = nullptr)
        {
            this->internalData = std::shared_ptr<data_primitive_to_type<T>>(new data_primitive_to_type<T>(data));
            this->typing = typing;
        }

        virtual bool canGet() { return true; }
        virtual bool canSet() { 
            return true;
        }

        virtual Data<T> get()
        {
            return this->internalData;
        }
        virtual void set(Data<T> data)
        {
            *(this->internalData) = *(CastSharedPtr(data_primitive_to_type<T>, data));
        }

        virtual std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> getType()
        {
            if (this->typing == nullptr)
            {
                return makeBasicType<T>("", !this->canSet());
            }
            else {
                return this->typing;
            }
        }
    };

    template <>
    class DataContainerWrapper<DataPrimitive::unknown> : public DataWrapperSub<DataPrimitive::unknown>
    {
    };

    using Int32Wrapper = DataWrapperSub<DataPrimitive::int32>;
    using Int32ContainerWrapper = DataContainerWrapper<DataPrimitive::int32>;
    using StringWrapper = DataWrapperSub<DataPrimitive::string>;
    using StringContainerWrapper = DataContainerWrapper<DataPrimitive::string>;
    using UnknownWrapper = DataContainerWrapper<DataPrimitive::unknown>;
    using FunctionWrapper = DataWrapperSub<DataPrimitive::function>;
    using FunctionContainerWrapper = DataContainerWrapper<DataPrimitive::function>;
    using ClassWrapper = DataWrapperSub<DataPrimitive::classType>;
    using ClassContainerWrapper = DataContainerWrapper<DataPrimitive::classType>;
    // using ClassInstanceWrapper = DataWrapperSub<DataPrimitive::classInstance>;
    // using ClassInstanceContainerWrapper = DataContainerWrapper<DataPrimitive::classInstance>;
}

#include "./object-wrapper.hpp"
#include "./array-wrapper.hpp"

#endif