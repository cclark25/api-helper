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
    class TypeWrapper;

    template <>
    class TypeWrapper<DataPrimitive::unknown> : public TypeWrapperRoot
    {
        std::string name;
        std::string description;
        DataPrimitive primitive;

    public:
        TypeWrapper(std::string name, std::string description, DataPrimitive primitive)
        {
            this->name = name;
            this->description = description;
            this->primitive = primitive;
        }
        TypeWrapper(std::shared_ptr<TypeWrapperRoot> other) : TypeWrapper<DataPrimitive::unknown>(
                                                                  other->getTypeName(),
                                                                  other->getDescription(),
                                                                  DataPrimitive::unknown) {}

        virtual DataPrimitive getPrimitiveType() { return this->primitive; };
        virtual std::string getTypeName()
        {
            return this->name;
        }
        virtual std::string getDescription()
        {
            return this->description;
        }
    };

    template <DataPrimitive Primitive>
    class TypeWrapper : public TypeWrapper<DataPrimitive::unknown>
    {
    public:
        TypeWrapper(std::string name, std::string description) : TypeWrapper<DataPrimitive::unknown>(name, description, Primitive) {}
    };

    template <DataPrimitive D>
    std::shared_ptr<TypeWrapper<D>> basicType(nullptr);

    template <>
    class TypeWrapper<DataPrimitive::object> : public TypeWrapper<DataPrimitive::unknown>
    {
        std::map<std::string, std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>> fields;

    public:
        TypeWrapper(std::string name, std::string description, std::map<std::string, std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>> fields) : TypeWrapper<DataPrimitive::unknown>(name, description, DataPrimitive::object)
        {
            this->fields = fields;
        };

        virtual std::map<std::string, std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>> getFields()
        {
            return this->fields;
        }
    };

    using ObjectTypeWrapper = TypeWrapper<DataPrimitive::object>;

    template <>
    class TypeWrapper<DataPrimitive::function> : public TypeWrapper<DataPrimitive::unknown>
    {
        std::vector<std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>> parameters;
        std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> returnType;

    public:
        TypeWrapper<DataPrimitive::function>(std::string name, std::string description, std::vector<std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>> parameters, std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> returnType) : TypeWrapper<DataPrimitive::unknown>(name, description, DataPrimitive::function)
        {
            this->parameters = parameters;
            this->returnType = returnType;
        };
        virtual std::vector<std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>> getParams()
        {
            return this->parameters;
        }

        virtual std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> getReturnType()
        {
            return this->returnType;
        }
    };

    class ClassInstanceTypeWrapper;

    class ClassTypeWrapper : public TypeWrapper<DataPrimitive::classType>
    {
        std::shared_ptr<TypeWrapper<DataPrimitive::object>> staticType;
        std::shared_ptr<ClassInstanceTypeWrapper> instanceType = nullptr;
        std::shared_ptr<TypeWrapper<DataPrimitive::function>> constructor;

    public:
        ClassTypeWrapper(
            std::string name, std::string description,
            std::shared_ptr<TypeWrapper<DataPrimitive::object>> staticType,
            std::vector<std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>> constructorParameters) : TypeWrapper<DataPrimitive::classType>(name, description), staticType(staticType)
        {
            this->constructor = std::shared_ptr<TypeWrapper<DataPrimitive::function>>(
                new TypeWrapper<DataPrimitive::function>(
                    name.append("Constructor"),
                    "",
                    constructorParameters,
                    CastSharedPtr(TypeWrapper<DataPrimitive::unknown>, instanceType)));
        }

        virtual void setInstanceType(std::shared_ptr<ClassInstanceTypeWrapper> instanceType)
        {
            this->instanceType = instanceType;
        }
        virtual std::shared_ptr<TypeWrapper<DataPrimitive::object>> getStaticType()
        {
            if (this->staticType == nullptr)
            {
                throw "Class instance not defined!";
            }
            return this->staticType;
        }
        virtual std::shared_ptr<ClassInstanceTypeWrapper> getInstanceType() { return this->instanceType; }
        virtual std::shared_ptr<TypeWrapper<DataPrimitive::function>> getConstructor() { return this->constructor; }
    };

    class ClassInstanceTypeWrapper : public TypeWrapper<DataPrimitive::object>, public TypeWrapper<DataPrimitive::classInstance>
    {
        std::shared_ptr<ClassTypeWrapper> classType;
        ClassInstanceTypeWrapper(std::shared_ptr<ClassTypeWrapper> classType, std::map<std::string, std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>> fields) : TypeWrapper<DataPrimitive::object>(classType->getTypeName() + "Instance", "Instance of " + classType->getTypeName(), fields), TypeWrapper<DataPrimitive::classInstance>(classType->getTypeName() + "Instance", "Instance of " + classType->getTypeName())
        {
            this->classType = classType;
        };
    };
}

#endif