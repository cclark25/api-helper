#ifndef STRING_DATA_WRAPPER
#define STRING_DATA_WRAPPER
#include <string>
#include <memory>
#include "../data-primitive.cpp"
#include "../macros.hpp"
#include "./data-wrapper.hpp"

namespace APICore {
    class StringWrapper : public DataWrapper
    {
        Data stringData;

    public:
        StringWrapper(std::string data = "Default string data") : DataWrapper(GetSet::readAndWrite)
        {
            this->dataType = DataPrimitive::string;
            this->stringData = std::shared_ptr<std::string>(new std::string(data));
        }
        virtual Data get()
        {
            return this->stringData;
        }
        virtual void set(Data data)
        {
            *(CastSharedPtr(std::string, this->stringData)) = *(CastSharedPtr(std::string, data));
        }
    };

}

#endif