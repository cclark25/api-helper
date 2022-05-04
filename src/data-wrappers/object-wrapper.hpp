#ifndef OBJECT_DATA_WRAPPER
#define OBJECT_DATA_WRAPPER
#include <string>
#include <map>
#include <memory>
#include "../data-primitive.cpp"
#include "../macros.hpp"
#include "./data-wrapper.hpp"

namespace APICore
{
    typedef std::map<std::string, std::shared_ptr<DataWrapper>> ObjectMap;

    class ObjectWrapper : public DataWrapper
    {
        Data objectData;

    public:
        ObjectWrapper(std::shared_ptr<ObjectMap> data) : DataWrapper(GetSet::readAndWrite)
        {
            this->dataType = DataPrimitive::object;
            this->objectData = data;
        }
        virtual Data get()
        {
            return this->objectData;
        }
        virtual void set(Data data)
        {
            *(CastSharedPtr(ObjectMap, this->objectData)) = *(CastSharedPtr(ObjectMap, data));
        }

        virtual std::shared_ptr<DataWrapper> getField(std::string key)
        {
            auto mappedFields = CastSharedPtr(ObjectMap, this->get());
            if (mappedFields->count(key) > 0)
            {
                return mappedFields->at(key);
            }
            else {
                return emptyDataWrapper;
            }
        }

        virtual void setField(std::string key, std::shared_ptr<DataWrapper> value){
            auto mappedFields = CastSharedPtr(ObjectMap, this->get());

            mappedFields->erase(key);
            mappedFields->insert_or_assign(key, value);
        }
    };

}

#endif