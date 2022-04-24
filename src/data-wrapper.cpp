#ifndef API_DATA_WRAPPER
#define API_DATA_WRAPPER
#include <string>
#include <memory>
#include "./data-primitive.cpp"
#include "./macros.hpp"

#define HasGetAccess(accessLevel) accessLevel == GetSet::readAndWrite || GetSet::readOnly
#define HasSetAccess(accessLevel) accessLevel == GetSet::readAndWrite || GetSet::writeOnly

namespace APICore
{
    typedef std::shared_ptr<void> Data;
    enum GetSet
    {
        readOnly,
        writeOnly,
        readAndWrite,
        noAccess
    };
    class DataWrapper
    {
    protected:
        DataPrimitive dataType = DataPrimitive::null;
        GetSet accessLevel;

    public:
        DataWrapper(GetSet accessLevel)
        {
            this->accessLevel = accessLevel;
        }
        GetSet getAccessLevel()
        {
            return this->accessLevel;
        }
        bool canGet()
        {
            return HasGetAccess(this->accessLevel);
        }
        bool canSet()
        {
            return HasSetAccess(this->accessLevel);
        }
        virtual DataPrimitive getDataType()
        {
            return this->dataType;
        }
        virtual Data get()
        {
            // TODO: Throw some standard error object instead of a string.
            throw "Could not read data. Getter not provided.";
        };
        virtual void set(Data data)
        {
            // TODO: Throw some standard error object instead of a string.
            throw "Could not write data. Setter not provided.";
        };
        virtual ~DataWrapper() {}
    };

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

    std::shared_ptr<DataWrapper> emptyDataWrapper(new DataWrapper(GetSet::noAccess));
}

#endif