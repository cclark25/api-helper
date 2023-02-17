#define SOL_ALL_SAFETIES_ON 1
#include "./data/test-class.hpp"
#include "../../src/lua-binders/type-binder.hpp"
#include "../../src/json-typing/json-typing.hpp"
#include <lualib.h>
#include <iostream>
#include <cassert>
#include <sol.hpp>

using namespace APICore;
using namespace std;


int main(int argc, char **argv)
{
	bool generateTypes = false;

	bool isInputFile = false;
	std::string inputFile;
	for (int i = 1; i < argc; i++)
	{
		std::string param = std::string(argv[i]);

		if (param == "--generateTypes")
		{
			generateTypes = true;
		}
		else if (param == "--luaInput")
		{
			isInputFile = true;
		}
		else if (isInputFile)
		{
			inputFile = param;
			isInputFile = false;
		};
	}

	sol::state lua;
	if (generateTypes)
	{
		APICore::JsonTyper<CustomObjectData2>::declareType();
		auto typing = APICore::generateTypeMap();
		std::string typingJson = typing.dump(4);

		std::cout << typingJson << "\n";

		return 0;
	}

	sol::state lua2;
	LuaBinder<CustomObjectData>::declareType(lua);
	LuaBinder<CustomObjectData>::declareType(lua2);
	LuaBinder<CustomObjectData2>::declareType(lua);
	LuaBinder<CustomObjectData2>::declareType(lua2);
	auto customInstance = (new CustomObjectData());
	auto customInstance2 = (new CustomObjectData2());

	lua["testObject"] = customInstance2;
	lua2["testObject"] = customInstance2;
	lua["runNum"] = 1;
	lua2["runNum"] = 2;

	lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table, sol::lib::math, sol::lib::string, sol::lib::coroutine, sol::lib::debug, sol::lib::os);
	lua2.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table, sol::lib::math, sol::lib::string, sol::lib::coroutine, sol::lib::debug, sol::lib::os);

	if (inputFile != "")
	{
		lua.safe_script_file(inputFile);
		lua.collect_garbage();
		lua2.safe_script_file(inputFile);
		lua2.collect_garbage();
		lua.safe_script_file(inputFile);
		lua.collect_garbage();
		lua2.safe_script_file(inputFile);
		lua2.collect_garbage();

		delete customInstance;

		return 0;
	}

	return 0;
}
