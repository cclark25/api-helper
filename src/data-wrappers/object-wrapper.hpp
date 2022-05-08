#ifndef OBJECT_DATA_WRAPPER
#define OBJECT_DATA_WRAPPER
#include <string>
#include <map>
#include <memory>
#include "../data-primitive.hpp"
#include "../macros.hpp"
#include "./data-wrapper.hpp"
#include "../type-wrapper/type-wrapper.hpp"

namespace APICore
{
    typedef std::map<std::string, std::shared_ptr<DataWrapper>> ObjectMap;

    class ObjectWrapper : public DataWrapper
    {
    public:
        virtual DataPrimitive getDataType()
        {
            return DataPrimitive::object;
        }
        virtual std::shared_ptr<DataWrapper> getField(std::string key)
        {
            throw "Not Implemented!";
        };
        virtual void setField(std::string key, std::shared_ptr<DataWrapper> value)
        {
            throw "Not Implemented!";
        };
    };

    class ObjectContainerWrapper : public ObjectWrapper
    {
        std::shared_ptr<ObjectMap> objectData;
        std::shared_ptr<ObjectTypeWrapper> objectTyping;

    public:
        virtual bool canGet() { return true; }
        virtual bool canSet() { return true; }

        ObjectContainerWrapper(std::shared_ptr<ObjectMap> data)
        {
            this->objectData = data;
        }
        ObjectContainerWrapper(ObjectMap &data)
        {
            this->objectData = std::shared_ptr<ObjectMap>(new ObjectMap(data));
        }
        ObjectContainerWrapper()
        {
            this->objectData = std::shared_ptr<ObjectMap>(new ObjectMap());
        }
        virtual Data get()
        {
            return this->objectData;
        }
        virtual void set(Data data)
        {
            this->objectData = CastSharedPtr(ObjectMap, data);
        }

        virtual std::shared_ptr<DataWrapper> getField(std::string key)
        {
            auto data = this->get();
            auto mappedFields = CastSharedPtr(ObjectMap, data);
            if (mappedFields->count(key) > 0)
            {
                return mappedFields->at(key);
            }
            else
            {
                return std::shared_ptr<DataWrapper>(nullptr);
            }
        }

        virtual void setField(std::string key, std::shared_ptr<DataWrapper> value)
        {
            auto data = this->get();
            auto mappedFields = CastSharedPtr(ObjectMap, data);

            mappedFields->erase(key);
            mappedFields->insert_or_assign(key, value);
        }
    };

}

#endif