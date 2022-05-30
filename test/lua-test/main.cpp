// #include "./api-object.cpp"
#define SOL_ALL_SAFETIES_ON 1
// #include "../../src/lua-binding/lua-wrappers-binding.hpp"
#include "../../src/lua-binding/binding-types/basic-types.hpp"
#include "../../src/data-wrappers/data-wrapper.hpp"
#include <lualib.h>
#include <iostream>
#include <cassert>
#include <sol.hpp>

using namespace APICore;
using namespace std;

int main()
{
	try
	{
		sol::state lua;
		
		lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table, sol::lib::math, sol::lib::string, sol::lib::coroutine);

		auto objectValue = std::shared_ptr<ObjectWrapper>(new ObjectContainerWrapper());
		// auto arrayVector = std::vector<Data>();
		// arrayVector.push_back(std::shared_ptr<std::string>(new std::string("Test string 1.")));
		// arrayVector.push_back(std::shared_ptr<std::string>(new std::string("Test string 2.")));
		// arrayVector.push_back(std::shared_ptr<std::string>(new std::string("Test string 3.")));

		objectValue->setField(
			"field1",
			std::shared_ptr<StringWrapper>(new StringContainerWrapper("Field string.")));

		APILua::bind<std::string>("API", lua, std::map<std::string, std::shared_ptr<DataWrapper>>({
			{"stringValue", std::shared_ptr<StringContainerWrapper>(new StringContainerWrapper("Test string."))},
			{"intValue", std::shared_ptr<Int32ContainerWrapper>(new Int32ContainerWrapper(0))},
			{"objectValue", objectValue}
			// {"arrayValue", std::shared_ptr<ArrayWrapper>(new ArrayContainerWrapper(arrayVector))}
		}));

		try
		{
			lua.script_file("../test/lua-test/test-lua.lua");
		}
		catch (sol::error e)
		{
			cerr << "Sol error caught\n";
		}
	}
	catch (char const *e)
	{
		std::cerr << "Caught string error: " << e << std::endl;
	}
	catch (std::string e)
	{
		std::cerr << "Caught string error: " << e << std::endl;
	}

	return 0;
}
