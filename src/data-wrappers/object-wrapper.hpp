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
        std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> typing = nullptr;

    public:
        virtual DataPrimitive getDataType() { return DataPrimitive::object; };
        virtual Data<DataPrimitive::object> get() { throw "Not Implemented!"; };
        virtual void set(Data<DataPrimitive::object> data) { throw "Not Implemented!"; };
        virtual std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> getType()
        {
            if (this->typing == nullptr)
            {
                std::map<std::string, std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>> fields;
                for (auto field : *this->get())
                {
                    fields.insert_or_assign(field.first, field.second->getType());
                }
                this->typing = std::shared_ptr<ObjectTypeWrapper>(new ObjectTypeWrapper("object", "", fields));
            }

            return this->typing;
        }
        virtual void setType(std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> newType)
        {
            this->typing = newType;
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

    using ObjectWrapper = DataWrapperSub<DataPrimitive::object>;

    template <>
    class DataContainerWrapper<DataPrimitive::object> : public ObjectWrapper
    {
        std::shared_ptr<ObjectMap> objectData;

    public:
        virtual bool canGet() { return true; }
        virtual bool canSet()
        {
            return true;
        }

        DataContainerWrapper<DataPrimitive::object>(
            std::shared_ptr<ObjectMap> data,
            std::shared_ptr<ObjectTypeWrapper> objectTyping = nullptr)
        {
            this->objectData = data;
            this->setType(objectTyping);
        }
        DataContainerWrapper<DataPrimitive::object>(
            ObjectMap &data,
            std::shared_ptr<ObjectTypeWrapper> objectTyping = nullptr)
        {
            this->objectData = std::shared_ptr<ObjectMap>(new ObjectMap(data));
            this->setType(objectTyping);
        }
        DataContainerWrapper<DataPrimitive::object>(std::shared_ptr<ObjectTypeWrapper> objectTyping = nullptr)
        {
            this->objectData = std::shared_ptr<ObjectMap>(new ObjectMap());
            this->setType(objectTyping);
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

    using ObjectContainerWrapper = DataContainerWrapper<DataPrimitive::object>;
}

#endif