#ifndef API_MODEL
#define API_MODEL

#include <map>
#include <string>
#include <memory>
#include <vector>
#include <array>
#include <tuple>
#include <functional>
#include "./data-primitive.hpp"
#include "./lua-binding/binding-types/all.hpp"

namespace APICore
{

    template <typename... T>
    const std::size_t n = sizeof...(T);

    class APIModel
    {
    private:
        std::map<std::string, std::shared_ptr<APIModel>> fieldMap;
        APIModel *delayedParent = nullptr;
        std::string key = "";
        std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> typing = makeBasicType<DataPrimitive::null>();
        std::shared_ptr<DataWrapper> value = std::shared_ptr<UnknownWrapper>(new UnknownWrapper());

        template <DataPrimitive D>
        void finalize(const data_primitive_to_type<D> &value, std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> typing = nullptr)
        {
            if (this->delayedParent != nullptr)
            {
                if (typing == nullptr)
                {
                    this->typing = makeBasicType<D>();
                }
                else
                {
                    this->typing = typing;
                }
                std::shared_ptr<APIModel> modelCopy = std::shared_ptr<APIModel>(new APIModel(*this));
                this->delayedParent->fieldMap.insert_or_assign(this->key, modelCopy);
                this->delayedParent->typing = makeBasicType<DataPrimitive::object>();
                this->delayedParent = nullptr;
                modelCopy->delayedParent = nullptr;
            }

            this->value = std::shared_ptr<DataContainerWrapper<D>>(new DataContainerWrapper<D>(value));
        }

    public:
        APIModel() {}

        std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> getTyping()
        {
            if (this->typing->getPrimitiveType() == DataPrimitive::object)
            {
                std::map<std::string, std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>> fields;

                for (auto field : this->fieldMap)
                {
                    fields.insert_or_assign(field.first, field.second->getTyping());
                }

                auto objectTyping = std::shared_ptr<TypeWrapper<DataPrimitive::object>>(
                    new TypeWrapper<DataPrimitive::object>(
                        this->key,
                        "",
                        fields));

                return objectTyping;
            }
            else
            {
                return this->typing;
            }
        }

        APIModel operator[](const std::string &key)
        {
            if (this->fieldMap.count(key) == 0)
            {
                APIModel newField;
                newField.key = key;
                newField.delayedParent = this;
                return newField;
            }

            return *fieldMap.at(key);
        }

        APIModel &operator=(const std::string &value)
        {
            this->finalize<DataPrimitive::string>(value);
            return *this;
        }

        APIModel &operator=(const int &value)
        {
            this->finalize<DataPrimitive::int32>(value);
            return *this;
        }

        template <typename R, typename... Params>
        APIModel &operator=(std::tuple<std::array<std::string, n<(Params)> > , std::function<R(Params...)>> value)
        {
            data_primitive_to_type<DataPrimitive::function> internalValue = data_primitive_to_type<DataPrimitive::function>(
                [value](FunctionInternalType::FunctionParams params)
                {
                    // int i = 0;
                    // R result = value( (( (Params) params[i++]-> ) , ...) );
                    return std::shared_ptr<UnknownWrapper>(new UnknownWrapper());
                });
            std::vector<DataPrimitive> parameterPrimitives = this->getParameterTypes<Params...>();
            std::vector<std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>> parameterTypings;

            for (auto p : parameterPrimitives)
            {
                parameterTypings.push_back(makeBasicType(p));
            }

            this->typing = std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>(new TypeWrapper<DataPrimitive::function>("", "", parameterTypings, makeBasicType<DataPrimitive::unknown>()));

            this->finalize<DataPrimitive::function>(internalValue, this->typing);

            return *this;
        }

        template <typename T>
        inline DataPrimitive getPrimitiveType()
        {
            return DataPrimitive::unknown;
        }

        template <typename... T>
        inline std::vector<DataPrimitive> getParameterTypes()
        {
            std::vector<DataPrimitive> sub;
            (sub.push_back(this->getPrimitiveType<T>()), ...);
            return sub;
        }
    };

    template <>
    inline DataPrimitive APIModel::getPrimitiveType<std::string>()
    {
        return DataPrimitive::string;
    }
    template <>
    inline DataPrimitive APIModel::getPrimitiveType<int>()
    {
        return DataPrimitive::int32;
    }
    template <>
    inline DataPrimitive APIModel::getPrimitiveType<void>()
    {
        return DataPrimitive::null;
    }

}

#endif