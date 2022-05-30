#ifndef LUA_WRAPPERS_BINDING
#define LUA_WRAPPERS_BINDING
#include "../data-wrappers/data-wrapper.hpp"
#include "../data-wrappers/int-wrapper.hpp"
#include "../macros.hpp"
#include <sol/sol.hpp>
#include <map>
#include <functional>

namespace APILua
{
    using namespace APICore;
    using namespace sol;

    template <typename KeyType>
    void BindWrapper(
        sol::table luaState,
        KeyType key,
        std::shared_ptr<DataWrapperSub<DataPrimitive::unknown>> wrapper);

    template <typename KeyType, typename InternalType>
    void BindBasicWrapper(sol::table luaState,
                          KeyType key,
                          std::shared_ptr<DataWrapperSub<DataPrimitive::unknown>> wrapper);

    std::shared_ptr<DataWrapperSub<DataPrimitive::unknown>> BindLuaToData(sol::object data)
    {

        switch (data.get_type())
        {
        case sol::type::lua_nil:
            return nullptr;
            break;
        case sol::type::string:
        {
            auto newWrapper = std::shared_ptr<StringWrapper>(new StringContainerWrapper(data.as<std::string>()));
            return CastSharedPtr(UnknownWrapper, newWrapper);
            break;
        }
        case sol::type::number:
        {
            auto newWrapper = std::shared_ptr<Int32Wrapper>(new Int32ContainerWrapper(data.as<int32_t>()));
            return CastSharedPtr(UnknownWrapper, newWrapper);
            break;
        }
        // case sol::type::boolean:
        //     break;
        case sol::type::table:
        {
            auto tableData = data.as<sol::table>();
            sol::type firstKeyType = sol::type::none;

            auto newObject = std::shared_ptr<ObjectWrapper>(new ObjectContainerWrapper());
            for (auto field : tableData)
            {
                std::string key;
                switch (field.first.get_type())
                {
                case sol::type::string:
                    key = field.first.as<std::string>();
                    break;

                default:
                    throw sol::error("Cannot index the table with the given type.");
                    break;
                }
                newObject->setField(key, BindLuaToData(field.second));
            }

            return CastSharedPtr(UnknownWrapper, newObject);
        }
        break;
        // case sol::type::function:
        //     break;
        default:
            throw sol::error("Unsupported type.");
        }
    }

    // class TableDataWrapper : public DataWrapper<DataPrimitive::object>
    // {
    // public:
    //     std::shared_ptr<ObjectWrapper> src;
    //     sol::state_view state;
    //     TableDataWrapper(sol::state_view state, std::shared_ptr<ObjectWrapper> src) : state(state), src(src) {}
    //     virtual bool canGet() { return true; };
    //     virtual bool canSet() { return true; };
    //     virtual DataPrimitive getDataType() { return DataPrimitive::object; };
    //     virtual Data<DataPrimitive::unknown> get()
    //     {
    //         auto table = std::shared_ptr<sol::table>(new sol::table(this->state, sol::new_table()));
    //         auto data = src->get();
    //         auto fields = CastSharedPtr(ObjectMap, data);
    //         for (auto field : *fields)
    //         {
    //             BindWrapper<std::string>(*table, field.first, field.second);
    //         }
    //         return table;
    //     };
    //     virtual void set(Data<DataPrimitive::unknown> data)
    //     {
    //         auto newTable = CastSharedPtr(sol::table, data);
    //         auto bound = BindLuaToData(*newTable)->get();
    //         src->set(CastSharedPtr(data_primitive_to_type<DataPrimitive::unknown>, bound));
    //     };
    // };

    // class ArrayDataWrapper : public DataWrapper<DataPrimitive::unknown>
    // {
    // public:
    //     std::shared_ptr<ArrayWrapper> src;
    //     sol::state_view state;
    //     ArrayDataWrapper(sol::state_view state, std::shared_ptr<ArrayWrapper> src) : state(state), src(src) {}
    //     virtual bool canGet() { return true; };
    //     virtual bool canSet() { return true; };
    //     virtual DataPrimitive getDataType() { return DataPrimitive::array; };
    //     virtual Data<DataPrimitive::unknown> get()
    //     {
    //         return std::shared_ptr<void>();
    //         // switch (this->src->arrayOf())
    //         // {
    //         // case DataPrimitive:: :
    //         //     /* code */
    //         //     break;

    //         // default:
    //         //     break;
    //         // }
    //     };
    //     virtual void set(Data<DataPrimitive::unknown> data) { throw "Not Implemented!"; };

    // private:
    //     // template<typename Type>
    //     // virtual _get(){
    //     //     auto table = sol::table(this->state, sol::new_table());
    //     //     auto src = this->src;

    //     //     table["getIndex"] = [src](size_t index)
    //     //     {
    //     //         auto data = src->getIndex(index);
    //     //         return *CastSharedPtr(Type, data);
    //     //     };

    //     //     table["setIndex"] = [src](size_t index, Type value)
    //     //     {
    //     //         src->setIndex(index, std::shared_ptr<Type>(new Type(value)));
    //     //     };
    //     //     table["push"] = [src](Type value)
    //     //     {
    //     //         return src->push(std::shared_ptr<Type>(new Type(value)));
    //     //     };
    //     //     table["pop"] = [src]()
    //     //     {
    //     //         auto data = src->pop();
    //     //         return *CastSharedPtr(Type, data);
    //     //     };
    //     //     table["insert"] = [src](int index, Type value)
    //     //     {
    //     //         return src->insert(index, std::shared_ptr<Type>(new Type(value)));
    //     //     };
    //     //     table["length"] = [src]()
    //     //     {
    //     //         return src->getLength();
    //     //     };

    //     //     auto arrayBinder = this->state["____bindingHelpers"]["bindArrayWrapper"];
    //     //     sol::table arrayEntity = arrayBinder(table);

    //     //     return std::shared_ptr<sol::table>(new sol::table(arrayEntity));
    //     // }
    // };

    const std::string TEMPORARY_FIELD_NAME = "_temporaryField";

    sol::table BindDataWrapperLua(sol::state_view state, std::shared_ptr<DataWrapperSub<DataPrimitive::unknown>> wrapper)
    {
        sol::table dataWrapperTable(state, sol::new_table());
        if (wrapper->canGet())
        {
#define Getter(type, wrapper) dataWrapperTable["get"] = [wrapper]() \
{                                                                   \
    auto data = wrapper->get();                                     \
    return *CastSharedPtr(type, data);                              \
}

            switch (wrapper->getDataType())
            {
            case DataPrimitive::null:
                break;
            case DataPrimitive::int32:
                Getter(int32_t, wrapper);
                break;
            case DataPrimitive::string:
                Getter(std::string, wrapper);
                break;
            case DataPrimitive::object:
                Getter(sol::table, wrapper);
                break;
            case DataPrimitive::array:
                break;
            case DataPrimitive::function:
                break;
            case DataPrimitive::classType:
                break;
            }

#undef Getter
        }

        if (wrapper->canSet())
        {
#define Setter(type, wrapper) dataWrapperTable["set"] = [wrapper](sol::table self, type value) { \
    wrapper->set(std::shared_ptr<type>(new type(value)));                                        \
}

            switch (wrapper->getDataType())
            {
            case DataPrimitive::null:
                break;
            case DataPrimitive::int32:
                Setter(int32_t, wrapper);
                break;
            case DataPrimitive::string:
                Setter(std::string, wrapper);
                break;
            case DataPrimitive::object:
                Setter(sol::table, wrapper);
                break;
            case DataPrimitive::array:
                break;
            case DataPrimitive::function:
                break;
            case DataPrimitive::classType:
                break;
            }

#undef Setter
        }

        return dataWrapperTable;
    }

    template <typename KeyType, typename InternalType>
    void BindBasicWrapper(sol::table luaState,
                          KeyType key,
                          std::shared_ptr<DataWrapper> wrapper)
    {
        auto state = sol::state_view(luaState.lua_state());
        auto wrapperTable = BindDataWrapperLua(state, CastSharedPtr(DataWrapperSub<DataPrimitive::unknown>, wrapper));
        auto binder = sol::state_view(luaState.lua_state())["____bindingHelpers"]["bindWrapper"];
        binder(nullptr, key, wrapperTable, luaState);
        return;
    }

    // template <typename KeyType>
    // void BindObjectWrapper(sol::table luaState,
    //                        KeyType key,
    //                        std::shared_ptr<ObjectWrapper> wrapper)
    // {
    //     BindBasicWrapper<KeyType, sol::table>(
    //         luaState,
    //         key,
    //         std::shared_ptr<TableDataWrapper>(
    //             new TableDataWrapper(
    //                 sol::state_view(luaState.lua_state()),
    //                 wrapper)));
    // }

    // template <typename T>
    // void BindArrayWrapper(sol::table luaState,
    //                       size_t key,
    //                       std::shared_ptr<ArrayWrapper> wrapper)
    // {
    //     BindBasicWrapper<size_t, sol::table>(
    //         luaState,
    //         key,
    //         std::shared_ptr<ArrayDataWrapper>(
    //             new ArrayDataWrapper(
    //                 sol::state_view(luaState.lua_state()),
    //                 wrapper)));
    // }

    /**
     * KeyType is string or int
     */
    template <typename KeyType>
    void BindWrapper(
        sol::table luaState,
        KeyType key,
        std::shared_ptr<DataWrapper> wrapper)
    {
        switch (wrapper->getDataType())
        {
        case DataPrimitive::string:
            BindBasicWrapper<KeyType, std::string>(luaState, key, wrapper);
            break;
        case DataPrimitive::int32:
            BindBasicWrapper<KeyType, int32_t>(luaState, key, wrapper);
            break;
        case DataPrimitive::null:
            break;
        case DataPrimitive::object:
            // BindObjectWrapper<KeyType>(luaState, key, CastSharedPtr(ObjectWrapper, wrapper));
            break;
        case DataPrimitive::array:
            // BindArrayWrapper<int>(luaState, key, CastSharedPtr(ArrayWrapper, wrapper));
            break;

        default:
            throw sol::error("Unsupported type.");
            break;
        }
    }

   
    void BindAPI(std::string apiName, sol::state &luaState, std::map<std::string, std::shared_ptr<DataWrapper>> source)
    {
        luaState.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table, sol::lib::math, sol::lib::string);
        luaState.add_package_loader(LoadFile);

        luaState.script("____lualib = require('lualib_bundle');");
        luaState.script("____bindingHelpers = require('binding-helpers');");

        luaState[apiName] = sol::new_table();
        auto table = (sol::table)(luaState[apiName]);

        for (auto definition : source)
        {
            // bindAPI(table, definition);
            BindWrapper<std::string>(table, definition.first, definition.second);
        }
    }
}

#endif