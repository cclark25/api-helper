#ifndef INT_DATA_WRAPPER
#define INT_DATA_WRAPPER
#include <string>
#include <memory>
#include "../data-primitive.hpp"
#include "../macros.hpp"
#include "./data-wrapper.hpp"

namespace APICore
{

    class Int32Wrapper : public DataWrapper
    {
        virtual DataPrimitive getDataType()
        {
            return DataPrimitive::int32;
        }
    };
    class Int32ContainerWrapper : public Int32Wrapper
    {
        std::shared_ptr<int32_t> intData;

    public:
        Int32ContainerWrapper(int32_t data = 0)
        {
            this->intData = std::shared_ptr<int32_t>(new int32_t(data));
        }

        virtual bool canGet() { return true; }
        virtual bool canSet() { return true; }

        virtual Data get()
        {
            return this->intData;
        }
        virtual void set(Data data)
        {
            *(this->intData) = *(CastSharedPtr(int32_t, data));
        }
    };

}

#endif