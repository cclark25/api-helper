#ifndef TYPE_WRAPPER
#define TYPE_WRAPPER

#include <string>
#include <map>
#include <memory>
#include "../data-primitive.hpp"
#include "../type-wrapper/type-wrapper.hpp"

namespace APICore
{
    class TypeWrapper;
    typedef std::map<std::string, std::shared_ptr<TypeWrapper>> ObjectFieldTypeMap;

    class TypeWrapper
    {
    public:
        virtual DataPrimitive getPrimitiveType();
        virtual std::string getName();
        virtual std::string getDescription();
        virtual std::shared_ptr<ObjectFieldTypeMap> getFieldTypes(){
            return std::shared_ptr<ObjectFieldTypeMap>({});
        }
        virtual std::shared_ptr<TypeWrapper> getArrayOf(){return std::shared_ptr<TypeWrapper>(nullptr);}
    };

    class ArrayTypeWrapper : public TypeWrapper {
        virtual DataPrimitive getPrimitiveType(){
            return DataPrimitive::array;
        }
        virtual std::shared_ptr<TypeWrapper> getArrayOf();
    };

    class ObjectTypeWrapper : TypeWrapper {
        virtual DataPrimitive getPrimitiveType(){
            return DataPrimitive::object;
        }
        
        virtual std::shared_ptr<ObjectFieldTypeMap> getFieldTypes();
    };
}

#endif