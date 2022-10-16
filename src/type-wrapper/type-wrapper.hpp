#ifndef TYPE_WRAPPER
#define TYPE_WRAPPER

#include <string>
#include <map>
#include <memory>
#include <type_traits>
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
        virtual bool isReadonly() { return true; };
    };

    template <DataPrimitive Primitive>
    class TypeWrapper;
    template <>
    class TypeWrapper<DataPrimitive::classType>;

    using ClassTypeWrapper = TypeWrapper<DataPrimitive::classType>;

    template <>
    class TypeWrapper<DataPrimitive::unknown> : public TypeWrapperRoot
    {
        std::string name;
        std::string description;
        DataPrimitive primitive;
        bool readonly = true;

    public:
        TypeWrapper(std::string name, std::string description, DataPrimitive primitive, bool readonly = true)
        {
            this->name = name;
            this->description = description;
            this->primitive = primitive;
            this->readonly = readonly;
        }
        TypeWrapper(std::shared_ptr<TypeWrapperRoot> other) : TypeWrapper<DataPrimitive::unknown>(
                                                                  other->getTypeName(),
                                                                  other->getDescription(),
                                                                  DataPrimitive::unknown,
                                                                  other->isReadonly()) {}

        virtual DataPrimitive getPrimitiveType() { return this->primitive; };
        virtual std::string getTypeName()
        {
            return this->name;
        }
        virtual std::string getDescription()
        {
            return this->description;
        }
        virtual bool isReadonly() { return this->readonly; };
    };

    template <DataPrimitive Primitive>
    class TypeWrapper : public TypeWrapper<DataPrimitive::unknown>
    {
    public:
        TypeWrapper(std::string name, std::string description) : TypeWrapper<DataPrimitive::unknown>(name, description, Primitive) {}
    };

    template <>
    class TypeWrapper<DataPrimitive::object> : public TypeWrapper<DataPrimitive::unknown>
    {
        std::map<std::string, std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>> fields;
        std::shared_ptr<ClassTypeWrapper> instanceOf = nullptr;

    public:
        TypeWrapper(std::string name, std::string description, std::map<std::string, std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>> fields, std::shared_ptr<ClassTypeWrapper> instanceOf = nullptr) : TypeWrapper<DataPrimitive::unknown>(name, description, DataPrimitive::object)
        {
            this->fields = fields;
            this->instanceOf = instanceOf;
        };

        virtual std::map<std::string, std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>> getFields()
        {
            return this->fields;
        }

        virtual std::shared_ptr<TypeWrapper<DataPrimitive::classType>> getInstanceOf()
        {
            return this->instanceOf;
        }

        virtual bool isInstanceOf(ClassTypeWrapper &classDefinition)
        {
            return &classDefinition == this->instanceOf.get();
        }
        virtual bool isInstanceOf(ClassTypeWrapper *classDefinition)
        {
            return isInstanceOf(*classDefinition);
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
        virtual void setParams(std::vector<std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>> params)
        {
            this->parameters = params;
        }

        virtual std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> getReturnType()
        {
            return this->returnType;
        }

        virtual void setReturnType(std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> newReturnType)
        {
            this->returnType = newReturnType;
        }
    };

    // using ClassInstanceTypeWrapper = TypeWrapper<DataPrimitive::classInstance>;

    template <>
    class TypeWrapper<DataPrimitive::classType> : public TypeWrapper<DataPrimitive::unknown>
    {
        std::shared_ptr<TypeWrapper<DataPrimitive::object>> staticType;
        std::shared_ptr<ObjectTypeWrapper> instanceType = nullptr;
        std::shared_ptr<TypeWrapper<DataPrimitive::function>> constructor;

    public:
        TypeWrapper(
            std::string name, std::string description,
            std::shared_ptr<TypeWrapper<DataPrimitive::object>> staticType,
            std::vector<std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>> constructorParameters) : TypeWrapper<DataPrimitive::unknown>(name, description, DataPrimitive::classType), staticType(staticType)
        {

            this->constructor = std::shared_ptr<TypeWrapper<DataPrimitive::function>>(
                new TypeWrapper<DataPrimitive::function>(
                    name.append("Constructor"),
                    "",
                    constructorParameters,
                    nullptr));
        }

        virtual bool setInstanceType(std::shared_ptr<ObjectTypeWrapper> instanceType)
        {
            if (this->instanceType != nullptr)
            {
                throw "Class already has an instance type.";
            }
            this->instanceType = instanceType;
            this->constructor->setReturnType(CastSharedPtr(TypeWrapper<DataPrimitive::unknown>, instanceType));
            return true;
        }
        virtual std::shared_ptr<TypeWrapper<DataPrimitive::object>> getStaticType()
        {
            if (this->staticType == nullptr)
            {
                throw "Class instance not defined!";
            }
            return this->staticType;
        }
        virtual std::shared_ptr<ObjectTypeWrapper> getInstanceType() { return this->instanceType; }
        virtual std::shared_ptr<TypeWrapper<DataPrimitive::function>> getConstructor() { return this->constructor; }
    };

    // template <>
    // class TypeWrapper<DataPrimitive::classInstance> : public TypeWrapper<DataPrimitive::object>
    // {
    // public:
    //     std::shared_ptr<ClassTypeWrapper> classType;
    //     TypeWrapper(
    //         std::shared_ptr<ClassTypeWrapper> classType,
    //         std::map<std::string, std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>> fields)

    //         : TypeWrapper<DataPrimitive::object>(
    //               classType->getTypeName() + "Instance",
    //               "Instance of " + classType->getTypeName(),
    //               fields)
    //     {
    //         this->classType = classType;

    //     };
    // };

    std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> makeBasicType(DataPrimitive D, std::string description = "", bool readonly = true)
    {
        if (D == DataPrimitive::function)
        {
            std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>(
                new TypeWrapper<DataPrimitive::function>(
                    dataPrimitiveNameMap.at(D),
                    description,
                    std::vector<std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>>(),
                    makeBasicType(DataPrimitive::unknown)));
        }
        return std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>(new TypeWrapper<DataPrimitive::unknown>(dataPrimitiveNameMap.at(D), description, D, readonly));
    }
    template <DataPrimitive D>
    std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> makeBasicType(std::string description = "", bool readonly = true)
    {
        return makeBasicType(D, description, readonly);
    }

}

#endif