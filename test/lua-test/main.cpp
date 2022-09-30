// #include "./api-object.cpp"
#define SOL_ALL_SAFETIES_ON 1
// #include "../../src/lua-binding/lua-wrappers-binding.hpp"
#include "../../src/lua-binding/binding-types/all.hpp"
#include "../../src/data-wrappers/data-wrapper.hpp"
#include "./data/test-function.hpp"
#include "./data/test-class.hpp"
#include <lualib.h>
#include <iostream>
#include <cassert>
#include <sol.hpp>

using namespace APICore;
using namespace std;

void printTyping(std::string fieldName, std::shared_ptr<APICore::TypeWrapperRoot> typing, std::string padding = "")
{
	std::cout << padding << "API Field Name: " << fieldName << std::endl;
	std::cout << padding << "\tType: " << typing->getTypeName() << std::endl;
	std::cout << padding << "\tDescription: " << typing->getDescription() << std::endl;
	if (typing->getPrimitiveType() == DataPrimitive::object)
	{
		auto objectTyping = CastSharedPtr(TypeWrapper<DataPrimitive::object>, typing);
		for (auto field : objectTyping->getFields())
		{
			printTyping(field.first, field.second, padding + "\t");
		}
	}
}

int handler(lua_State *s, sol::optional<const std::exception &> o, sol::string_view v)
{
	return -1;
};

int main(int argc, char **argv)
{
	try
	{
		sol::state lua;
		

		lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table, sol::lib::math, sol::lib::string, sol::lib::coroutine, sol::lib::debug, sol::lib::os);

		auto array = std::shared_ptr<ArrayWrapper>(new ArrayContainerWrapper());
		array->push(std::shared_ptr<DataWrapper>(new StringContainerWrapper("Test string 1.")));
		array->push(std::shared_ptr<DataWrapper>(new StringContainerWrapper("Test string 2.")));
		array->push(std::shared_ptr<DataWrapper>(new StringContainerWrapper("Test string 3.")));

		auto classInstance = std::shared_ptr<ObjectWrapper>(new ObjectContainerWrapper(classInstanceType));
		classInstance->setField(
			"f1",
			std::shared_ptr<StringWrapper>(new StringContainerWrapper("Field string.")));

		auto objectValue = std::shared_ptr<ObjectWrapper>(new ObjectContainerWrapper());
		objectValue->setField(
			"field1",
			std::shared_ptr<StringWrapper>(new StringContainerWrapper("Field string.")));
		objectValue->setField(
			"field2",
			std::shared_ptr<Int32Wrapper>(new Int32ContainerWrapper(15)));

		auto apiMappings = std::map<std::string, std::shared_ptr<DataWrapper>>({{"TestClass", classDefinition}, {"testClassInstance", classInstance}, {"stringValue", std::shared_ptr<StringContainerWrapper>(new StringContainerWrapper("Test string."))}, {"intValue", std::shared_ptr<Int32ContainerWrapper>(new Int32ContainerWrapper(0))}, {"objectValue", objectValue}, {"arrayValue", array}, {"functionValue", functionExampleDefinition}});

		bool generateTypes = false;
		for (int i = 1; i < argc; i++)
		{
			std::string param = std::string(argv[i]);

			if (param == "--generateTypes")
			{
				generateTypes = true;
			}
		}

		if (generateTypes)
		{

			lua.set_exception_handler(handler);

			std::string typeFile = APILua::generateTypings("API", lua, apiMappings);

			std::cout << typeFile << "\n";

			return 0;
		}

		APILua::bind<std::string>("API", lua, apiMappings);
		for (auto mapping : apiMappings)
		{
			auto typing = mapping.second->getType();
			printTyping(mapping.first, typing);
		}

		try
		{
			// lua.script_file("../test/lua-test/test-lua.lua");
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
