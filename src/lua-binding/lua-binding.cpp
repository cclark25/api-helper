// #ifndef API_LUA_BINDING
// #define API_LUA_BINDING
// #include "../api-object.h"
// #include "../data-wrappers/data-wrapper.hpp"
// #include <sol/sol.hpp>
// #include <cstdarg>
// #include <lualib_bundle.h>
// #include <memory>

// namespace APILua
// {
// 	using namespace APICore;
// 	using namespace sol;

// 	template <typename underType>
// 	void BindGetterSetter(
// 		std::shared_ptr<DataWrapper> dataOverride,
// 		std::shared_ptr<APIObject> source,
// 		APICore::DataPrimitive dataPrimative,
// 		sol::table luaState,
// 		std::string placeHolderNameForSearch,
// 		int index)
// 	{
// 		std::shared_ptr<DataWrapper> wrapper = dataOverride != emptyDataWrapper ? dataOverride
// 																				: source->dataWrapper;
// 		if (wrapper->getDataType() != dataPrimative)
// 		{
// 			throw std::string("Wrong data primitive. Got: ") + (to_string(wrapper->getDataType())) + std::string(", Expected: ") + to_string(dataPrimative);
// 		}
// 		auto setDescriptor = sol::state_view(luaState.lua_state())["____lualib"]["__TS__SetDescriptor"];

// 		sol::table getterSetterObject(luaState.lua_state(), sol::new_table());

// 		if (wrapper->canGet())
// 		{
// 			getterSetterObject.set("get", [wrapper]()
// 								   {
// 				auto dataPtr = wrapper->get();
// 				return *(CastSharedPtr(underType, dataPtr)); });
// 		}
// 		if (wrapper->canSet())
// 		{
// 			getterSetterObject.set("set", [wrapper](sol::table self, underType value)
// 								   { wrapper->set(std::shared_ptr<underType>(new underType(value))); });
// 		}

// 		if (index >= 0)
// 		{
// 			setDescriptor(luaState, index, getterSetterObject, false);
// 		}
// 		else
// 		{
// 			setDescriptor(luaState, placeHolderNameForSearch, getterSetterObject, false);
// 		}
// 	}

// 	std::function<sol::variadic_results(sol::variadic_args, sol::this_state)> wrapFunction(std::shared_ptr<APICore::APIObject> source)
// 	{
// 		auto functionSource = source->getFunctionDefault();
// 		auto functionParameters = source->getParameterTypes();
// 		auto wrapped = [functionSource, functionParameters, source](sol::variadic_args args, sol::this_state L)
// 		{
// 			std::vector<std::shared_ptr<void>> vectorArgs;

// 			for (int i = 0; i < functionParameters.size(); i++)
// 			{
// 				auto paramterType = functionParameters.at(i);
// 				sol::stack_proxy var = args[i];
// 				auto type = var.get_type();
// 				std::cout << "Param type: " << (int)type << std::endl;

// #define ConvertData(type) std::shared_ptr<type>(new type(var.as<type>()))

// 				switch (paramterType->getType())
// 				{
// 				case DataPrimitive::null:
// 					vectorArgs.push_back(std::shared_ptr<void>(nullptr));
// 					break;
// 				case DataPrimitive::string:
// 					vectorArgs.push_back(ConvertData(std::string));
// 					break;
// 				case DataPrimitive::int32:
// 					vectorArgs.push_back(ConvertData(int32_t));
// 					break;
// 				case DataPrimitive::function:
// 					// TODO: add support for passing functions through.
// 					// vectorArgs.push_back(ConvertData());
// 					break;
// 				case DataPrimitive::object:
// 					// TODO: add support for objects
// 					// vectorArgs.push_back(ConvertData());
// 					break;
// 				case DataPrimitive::array:
// 					// TODO: add support for arrays
// 					// vectorArgs.push_back(ConvertData());
// 					break;
// 				default:
// 					break;
// 				}

// #undef ConvertData
// 			}
// 			std::cout << "Vector args length: " << vectorArgs.size() << "\n";
// 			auto result = functionSource(nullptr, vectorArgs);
// 			sol::variadic_results results;
// 			auto returnType = source->getReturnType();

// 			#define WrapResult(typename) results.push_back({ L, sol::in_place, *CastSharedPtr(typename, result) })

// 			switch (returnType->getType())
// 			{
// 			case DataPrimitive::string:
// 				WrapResult(std::string);
// 				break;

// 			case DataPrimitive::int32:
// 				WrapResult(std::int32_t);
// 				break;

// 			case DataPrimitive::null :
// 				break;

// 			case DataPrimitive::object :
// 			 	// TODO: Add support for returning objects
// 				// WrapResult(std::string);
// 				break;

// 			case DataPrimitive::function :
// 				// TODO: Add support for returning functions
// 				// WrapResult(std::function);
// 				break;

// 			case DataPrimitive::array :
// 			 	// TODO: Add support for returning arrays
// 				// WrapResult(std::string);
// 				break;
			
// 			default:
// 				break;
// 			}
			
// 			#undef WrapResult
// 			return results;
// 		};

// 		return wrapped;
// 	}

// 	void bindAPI(
// 		sol::table &luaState,
// 		std::shared_ptr<APIObject> source,
// 		int index = -1,
// 		std::shared_ptr<DataWrapper> dataOverride = emptyDataWrapper)
// 	{

// 		auto placeHolderNameForSearch = source->getName();
// 		switch (source->getType())
// 		{

// 		case DataPrimitive::null:
// 		{
// 		}
// 		break;

// 		case DataPrimitive::string:
// 		{
// 			// BindGetterSetter(std::string, DataPrimitive::string)
// 			BindGetterSetter<std::string>(dataOverride, source, DataPrimitive::string, luaState, placeHolderNameForSearch, index);
// 		}
// 		break;
// 		case DataPrimitive::int32:
// 		{
// 			// BindGetterSetter(int32_t, DataPrimitive::int32)
// 			BindGetterSetter<int32_t>(dataOverride, source, DataPrimitive::int32, luaState, placeHolderNameForSearch, index);
// 		}
// 		break;
// 		case DataPrimitive::function:
// 		{
// 			luaState.set_function(placeHolderNameForSearch, wrapFunction(source));
// 		}
// 		break;
// 		case DataPrimitive::object:
// 		{
// 			if (index >= 0)
// 			{
// 				luaState[index] = sol::new_table();
// 			}
// 			else
// 			{
// 				luaState[placeHolderNameForSearch] = sol::new_table();
// 			}
// 			auto table = (index < 0 ? (sol::table)luaState[placeHolderNameForSearch] : (sol::table)luaState[index]);

// 			for (APICore::FieldMap field : source->getFields())
// 			{
// 				bindAPI(table, field.value);
// 			}
// 		}
// 		break;
// 		case DataPrimitive::array:
// 		{
// 			if (index >= 0)
// 			{
// 				luaState[index] = sol::new_table();
// 			}
// 			else
// 			{
// 				luaState[placeHolderNameForSearch] = sol::new_table();
// 			}
// 			auto table = (index < 0 ? (sol::table)luaState[placeHolderNameForSearch] : (sol::table)luaState[index]);
// 			for (int i = 1; i <= source->getArrayType().defaultValues.size(); i++)
// 			{
// 				bindAPI(table,
// 						source->getArrayType().type,
// 						i,
// 						source->getArrayType().defaultValues.at(i - 1));
// 			}
// 		}
// 		break;

// 		case DataPrimitive::classType:
// 		{
// 			auto state = sol::state::state_view(luaState.lua_state());

// 			auto createClass = state["____lualib"]["__TS__Class"];

// 			luaState[placeHolderNameForSearch] = createClass();
// 			luaState[placeHolderNameForSearch]["name"] = placeHolderNameForSearch;

// 			auto createdTable = (sol::table)luaState[placeHolderNameForSearch];
// 			for (APICore::FieldMap field : source->getFields())
// 			{
// 				bindAPI(createdTable, field.value);
// 			}

// 			luaState[placeHolderNameForSearch]["prototype"]["____constructor"] = [source](sol::table self)
// 			{
// 				for (APICore::FieldMap field : source->getPrototype()->getFields())
// 				{
// 					bindAPI(self, field.value);
// 				}
// 			};
// 		}
// 		break;

// 		default:
// 			std::cerr << "Unknown type: " << source->getType() << "\n";
// 			std::cerr << "Known type: " << DataPrimitive::null << "\n";
// 			throw "Unhandled type.";
// 			break;
// 		}
// 	}

// 	void bindAPI(std::string apiName, sol::state &luaState, std::vector<std::shared_ptr<APIObject>> source)
// 	{
// 		luaState.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table, sol::lib::math, sol::lib::string);
// 		luaState.require_script("lualib_bundle", ((char *)lualib_bundle), true);
// 		luaState.script("____lualib = require('lualib_bundle');");

// 		luaState[apiName] = sol::new_table();
// 		auto table = (sol::table)(luaState[apiName]);

// 		for (auto definition : source)
// 		{
// 			bindAPI(table, definition);
// 		}
// 	}

// } // namespace APILua

// #endif