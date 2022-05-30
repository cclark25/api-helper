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

    template <>
    class DataWrapperSub<DataPrimitive::object> : public DataWrapper
    {
    public:
        virtual DataPrimitive getDataType() { return DataPrimitive::object; };
        virtual Data<DataPrimitive::object> get() { throw "Not Implemented!"; };
        virtual void set(Data<DataPrimitive::object> data) { throw "Not Implemented!"; };
        virtual std::shared_ptr<DataWrapper> getField(std::string key)
        {
            throw "Not Implemented!";
        };
        virtual void setField(std::string key, std::shared_ptr<DataWrapper> value)
        {
            throw "Not Implemented!";
        };
    };

    using ObjectWrapper = DataWrapperSub<DataPrimitive::object>;

    template <>
    class DataContainerWrapper<DataPrimitive::object> : public ObjectWrapper
    {
        std::shared_ptr<ObjectMap> objectData;
        std::shared_ptr<ObjectTypeWrapper> objectTyping;

    public:
        virtual bool canGet() { return true; }
        virtual bool canSet() { return true; }

        DataContainerWrapper<DataPrimitive::object>(std::shared_ptr<ObjectMap> data)
        {
            this->objectData = data;
        }
        DataContainerWrapper<DataPrimitive::object>(ObjectMap &data)
        {
            this->objectData = std::shared_ptr<ObjectMap>(new ObjectMap(data));
        }
        DataContainerWrapper<DataPrimitive::object>()
        {
            this->objectData = std::shared_ptr<ObjectMap>(new ObjectMap());
        }
        virtual Data<DataPrimitive::object> get()
        {
            return this->objectData;
        }
        virtual void set(Data<DataPrimitive::object> data)
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
                return std::shared_ptr<DataWrapperSub<DataPrimitive::unknown>>(nullptr);
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

    class ObjectContainerWrapper : public DataContainerWrapper<DataPrimitive::object>
    {
    };
}

#endif