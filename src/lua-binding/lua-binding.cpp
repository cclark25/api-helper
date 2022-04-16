#ifndef API_LUA_BINDING
#define API_LUA_BINDING
#include "../api-object.h"
#include <sol/sol.hpp>
#include <cstdarg>
#include <lualib_bundle.h>

namespace APILua
{
	using namespace APICore;
	using namespace sol;

	std::function<sol::variadic_results(sol::variadic_args)> wrapFunction(std::shared_ptr<APICore::APIObject> source)
	{
		auto functionSource = source->getFunctionDefault();
		auto functionParameters = source->getParameterTypes();
		auto wrapped = [functionSource, functionParameters](sol::variadic_args args)
		{
			std::vector<std::shared_ptr<void>> vectorArgs;

			for (int i = 0; i < functionParameters.size(); i++)
			{
				auto paramterType = functionParameters.at(i);
				sol::stack_proxy var = args[i];
				auto type = var.get_type();
				std::cout << "Param type: " << (int)type << std::endl;

#define ConvertData(type) std::shared_ptr<type>(new type(var.as<type>()))

				switch (paramterType->getType())
				{
				case DataPrimitive::null:
					vectorArgs.push_back(std::shared_ptr<void>(nullptr));
					break;
				case DataPrimitive::string:
					vectorArgs.push_back(ConvertData(std::string));
					break;
				case DataPrimitive::int32:
					vectorArgs.push_back(ConvertData(int32_t));
					break;
				case DataPrimitive::function:
					// TODO: add support for passing functions through.
					// vectorArgs.push_back(ConvertData());
					break;
				case DataPrimitive::object:
					// TODO: add support for objects
					// vectorArgs.push_back(ConvertData());
					break;
				case DataPrimitive::array:
					// TODO: add support for arrays
					// vectorArgs.push_back(ConvertData());
					break;
				default:
					break;
				}

#undef ConvertData
			}
			std::cout << "Vector args length: " << vectorArgs.size() << "\n";
			auto x = functionSource(nullptr, vectorArgs);
			return sol::variadic_results{};
		};

		return wrapped;
	}

	void bindAPI(
		sol::table &luaState, std::shared_ptr<APIObject> source, int index = -1, std::shared_ptr<void> dataOverride = std::shared_ptr<void>(nullptr))
	{

		auto placeHolderNameForSearch = source->getName();
		switch (source->getType())
		{

		case DataPrimitive::null:
		{
		}
		break;

		case DataPrimitive::string:
		{
			auto str = source->getStringDefault();
			if (index >= 0)
			{
				luaState[index] = dataOverride != nullptr ? *CastSharedPtr(std::string, dataOverride) : str;
			}
			else
			{
				luaState[placeHolderNameForSearch] = dataOverride != nullptr ? *CastSharedPtr(std::string, dataOverride) : str;
			}
		}
		break;
		case DataPrimitive::int32:
		{
			auto i = source->getInt32Default();
			if (index >= 0)
			{
				luaState[index] = dataOverride != nullptr ? *CastSharedPtr(int32_t, dataOverride) : i;
			}
			else
			{
				luaState[placeHolderNameForSearch] = dataOverride != nullptr ? *CastSharedPtr(int32_t, dataOverride) : i;
			}
		}
		break;
		case DataPrimitive::function:
		{
			luaState.set_function(placeHolderNameForSearch, wrapFunction(source));
		}
		break;
		case DataPrimitive::object:
		{
			if (index >= 0)
			{
				luaState[index] = sol::new_table();
			}
			else
			{
				luaState[placeHolderNameForSearch] = sol::new_table();
			}
			auto table = (index < 0 ? (sol::table)luaState[placeHolderNameForSearch] : (sol::table)luaState[index]);

			for (APICore::FieldMap field : source->getFields())
			{
				bindAPI(table, field.value);
			}
		}
		break;
		case DataPrimitive::array:
		{
			if (index >= 0)
			{
				luaState[index] = sol::new_table();
			}
			else
			{
				luaState[placeHolderNameForSearch] = sol::new_table();
			}
			auto table = (index < 0 ? (sol::table)luaState[placeHolderNameForSearch] : (sol::table)luaState[index]);
			for (int i = 1; i <= source->getArrayType().defaultValues.size(); i++)
			{
				bindAPI(table, source->getArrayType().type, i, source->getArrayType().defaultValues.at(i - 1));
			}
		}
		break;

		case DataPrimitive::classType:
		{
			auto state = sol::state::state_view(luaState.lua_state());

			auto createClass = state["____lualib"]["__TS__Class"];

			luaState[placeHolderNameForSearch] = createClass();
			luaState[placeHolderNameForSearch]["name"] = placeHolderNameForSearch;
			luaState[placeHolderNameForSearch]["prototype"]["____constructor"] = [](sol::table self){
				self["abc"] = "abc string";
				self["def"] = 123;
			};
		}
		break;

		default:
			std::cerr << "Unknown type: " << source->getType() << "\n";
			std::cerr << "Known type: " << DataPrimitive::null << "\n";
			throw "Unhandled type.";
			break;
		}
	}

	void bindAPI(std::string apiName, sol::state &luaState, std::vector<std::shared_ptr<APIObject>> source)
	{
		luaState.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table, sol::lib::math, sol::lib::string);
		luaState.require_script("lualib_bundle", ((char *)lualib_bundle), true);
		luaState.script("____lualib = require('lualib_bundle');") ; 

		luaState[apiName] = sol::new_table();
		auto table = (sol::table)(luaState[apiName]);

		for (auto definition : source)
		{
			bindAPI(table, definition);
		}
	}

} // namespace APILua

#endif