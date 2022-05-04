#ifndef API_DATA_WRAPPER
#define API_DATA_WRAPPER
#include <string>
#include <memory>
#include "../data-primitive.cpp"
#include "../macros.hpp"

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

    std::shared_ptr<DataWrapper> emptyDataWrapper(new DataWrapper(GetSet::noAccess));
}

#endif