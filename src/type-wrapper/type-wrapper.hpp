#ifndef TYPE_WRAPPER
#define TYPE_WRAPPER

#include <string>
#include <map>
#include <memory>
#include "../data-primitive.hpp"
#include "../macros.hpp"

namespace APICore
{
    class TypeWrapperRoot
    {
    public:
        virtual DataPrimitive getPrimitiveType() { return DataPrimitive::unknown; };
        virtual std::string getTypeName() { throw "Not Implemented!"; };
        virtual std::string getDescription() { throw "Not Implemented!"; };
    };

    template <DataPrimitive Primitive>
    class TypeWrapper : public TypeWrapperRoot
    {
        std::string name;
        std::string description;

    public:
        TypeWrapper(std::string name, std::string description)
        {
            this->name = name;
            this->description = description;
        }
        virtual DataPrimitive getPrimitiveType() { return Primitive; };
        virtual std::string getTypeName()
        {
            return this->name;
        }
        virtual std::string getDescription()
        {
            return this->description;
        }
    };

    template <DataPrimitive D>
    std::shared_ptr<TypeWrapper<D>> basicType(new TypeWrapper<D>(data_primitive_definition<D>::name, ""));

    class ObjectTypeWrapper : public TypeWrapper<DataPrimitive::object>
    {
        std::map<std::string, std::shared_ptr<TypeWrapperRoot>> fields;

    public:
        ObjectTypeWrapper(std::string name, std::string description, std::map<std::string, std::shared_ptr<TypeWrapperRoot>> fields) : TypeWrapper<DataPrimitive::object>(name, description)
        {
            this->fields = fields;
        };

        virtual std::map<std::string, std::shared_ptr<TypeWrapperRoot>> getFields()
        {
            return this->fields;
        }
    };

    class FunctionTypeWrapper : public TypeWrapper<DataPrimitive::function>
    {
        std::vector<std::shared_ptr<TypeWrapperRoot>> parameters;
        std::shared_ptr<TypeWrapperRoot> returnType;

    public:
        FunctionTypeWrapper(std::string name, std::string description, std::vector<std::shared_ptr<TypeWrapperRoot>> parameters, std::shared_ptr<TypeWrapperRoot> returnType) : TypeWrapper<DataPrimitive::function>(name, description)
        {
            this->parameters = parameters;
            this->returnType = returnType;
        };
        virtual std::vector<std::shared_ptr<TypeWrapperRoot>> getParams()
        {
            return this->parameters;
        }

        virtual std::shared_ptr<TypeWrapperRoot> getReturnType()
        {
            return this->returnType;
        }
    };

    class ClassInstanceTypeWrapper;

    class ClassTypeWrapper : public TypeWrapper<DataPrimitive::classType>
    {
        std::shared_ptr<ObjectTypeWrapper> staticType;
        std::shared_ptr<ClassInstanceTypeWrapper> instanceType = nullptr;
        std::shared_ptr<FunctionTypeWrapper> constructor;

    public:
        ClassTypeWrapper(
            std::string name, std::string description,
            std::shared_ptr<ObjectTypeWrapper> staticType,
            std::vector<std::shared_ptr<TypeWrapperRoot>> constructorParameters) : TypeWrapper<DataPrimitive::classType>(name, description), staticType(staticType)
        {
            this->constructor = std::shared_ptr<FunctionTypeWrapper>(
                new FunctionTypeWrapper(
                    name.append("Constructor"),
                    "",
                    constructorParameters,
                    CastSharedPtr(TypeWrapperRoot, instanceType)));
        }

        virtual void setInstanceType(std::shared_ptr<ClassInstanceTypeWrapper> instanceType)
        {
            this->instanceType = instanceType;
        }
        virtual std::shared_ptr<ObjectTypeWrapper> getStaticType()
        {
            if (this->staticType == nullptr)
            {
                throw "Class instance not defined!";
            }
            return this->staticType;
        }
        virtual std::shared_ptr<ClassInstanceTypeWrapper> getInstanceType() { return this->instanceType; }
        virtual std::shared_ptr<FunctionTypeWrapper> getConstructor() { return this->constructor; }
    };

    class ClassInstanceTypeWrapper : public ObjectTypeWrapper, public TypeWrapper<DataPrimitive::classInstance>
    {
        std::shared_ptr<ClassTypeWrapper> classType;
        ClassInstanceTypeWrapper(std::shared_ptr<ClassTypeWrapper> classType, std::map<std::string, std::shared_ptr<TypeWrapperRoot>> fields) : ObjectTypeWrapper(classType->getTypeName() + "Instance", "Instance of " + classType->getTypeName(), fields), TypeWrapper<DataPrimitive::classInstance>(classType->getTypeName() + "Instance", "Instance of " + classType->getTypeName())
        {
            this->classType = classType;
        };
    };
}

#endif