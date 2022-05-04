#ifndef INT_DATA_WRAPPER
#define INT_DATA_WRAPPER
#include <string>
#include <memory>
#include "../data-primitive.cpp"
#include "../macros.hpp"
#include "./data-wrapper.hpp"

namespace APICore {

    class IntWrapper : public DataWrapper
    {
        Data intData;

    public:
        IntWrapper(int32_t data = 32) : DataWrapper(GetSet::readAndWrite)
        {
            this->dataType = DataPrimitive::int32;
            this->intData = std::shared_ptr<int32_t>(new int32_t(data));
        }
        virtual Data get()
        {
            return this->intData;
        }
        virtual void set(Data data)
        {
            *(CastSharedPtr(int32_t, this->intData)) = *(CastSharedPtr(int32_t, intData));
        }
    };

}

#endif