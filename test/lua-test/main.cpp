// #include "./api-object.cpp"
#define SOL_ALL_SAFETIES_ON 1
#include "../../src/lua-binding/lua-wrappers-binding.hpp"
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

		APILua::BindAPI("API", lua, {
			{
				"stringValue",
				std::shared_ptr<StringContainerWrapper>(new StringContainerWrapper("Test string."))
			},
			{
				"intValue",
				std::shared_ptr<Int32ContainerWrapper>(new Int32ContainerWrapper(0))
			}
		});

		try
		{
			lua.script_file("./test/lua-test/test-lua.lua");
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
