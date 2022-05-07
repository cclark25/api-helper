#ifndef API_DATA_WRAPPER
#define API_DATA_WRAPPER
#include <string>
#include <memory>
#include "../data-primitive.hpp"
#include "../macros.hpp"

#define HasGetAccess(accessLevel) accessLevel == GetSet::readAndWrite || GetSet::readOnly
#define HasSetAccess(accessLevel) accessLevel == GetSet::readAndWrite || GetSet::writeOnly

namespace APICore
{
    using namespace APICore;
    typedef std::shared_ptr<void> Data;
    
    class DataWrapper
    {
    public:
        virtual bool canGet(){throw "Not Implemented!";};
        virtual bool canSet(){throw "Not Implemented!";};
        virtual DataPrimitive getDataType(){throw "Not Implemented!";};
        virtual Data get(){throw "Not Implemented!";};
        virtual void set(Data data){throw "Not Implemented!";};
        virtual ~DataWrapper(){}
    };

}

#include "./string-wrapper.hpp"
#include "./int-wrapper.hpp"
// #include "./object-wrapper.hpp"

#endif