// #include "./api-object.cpp"
#define SOL_ALL_SAFETIES_ON 1
// #include "../../src/lua-binding/lua-wrappers-binding.hpp"
#include "../../src/lua-binding/binding-types/all.hpp"
#include "../../src/data-wrappers/data-wrapper.hpp"
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
		auto objectTyping = CastSharedPtr(ObjectTypeWrapper, typing);
		for (auto field : objectTyping->getFields())
		{
			printTyping(field.first, field.second, padding + "\t");
		}
	}
}

int handler(lua_State *s, sol::optional<const std::exception &> o, sol::string_view v) { 
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

		auto classStaticFields = std::map<std::string, std::shared_ptr<DataWrapper>>();
		classStaticFields.insert_or_assign(
			"s1",
			std::shared_ptr<StringWrapper>(new StringContainerWrapper("Static field string.")));

		auto classConstructor = [](FunctionInternalType::FunctionParams params)
		{
			auto obj = std::shared_ptr<ObjectWrapper>(new ObjectContainerWrapper());
			obj->setField(
				"f1",
				std::shared_ptr<StringWrapper>(new StringContainerWrapper("Member string.")));
			return std::map<std::string, std::shared_ptr<DataWrapper>>({{"f1", std::shared_ptr<StringWrapper>(new StringContainerWrapper("Member string."))}});
		};

		auto classDefinition = std::shared_ptr<ClassWrapper>(new ClassContainerWrapper({"TestClass", classConstructor, classStaticFields}));
		auto classInstanceData = std::shared_ptr<ObjectWrapper>(new ObjectContainerWrapper());
		classInstanceData->setField(
			"f1",
			std::shared_ptr<StringWrapper>(new StringContainerWrapper("Field string.")));

		auto classInstance = std::shared_ptr<ClassInstanceWrapper>(new ClassInstanceContainerWrapper({classDefinition, classInstanceData}));

		auto objectValue = std::shared_ptr<ObjectWrapper>(new ObjectContainerWrapper());
		objectValue->setField(
			"field1",
			std::shared_ptr<StringWrapper>(new StringContainerWrapper("Field string.")));
		objectValue->setField(
			"field2",
			std::shared_ptr<Int32Wrapper>(new Int32ContainerWrapper(15)));

		auto apiMappings = std::map<std::string, std::shared_ptr<DataWrapper>>({{"TestClass", classDefinition}, {"testClassInstance", classInstance}, {"stringValue", std::shared_ptr<StringContainerWrapper>(new StringContainerWrapper("Test string."))}, {"intValue", std::shared_ptr<Int32ContainerWrapper>(new Int32ContainerWrapper(0))}, {"objectValue", objectValue}, {"arrayValue", array}, {"functionValue", std::shared_ptr<FunctionWrapper>(new FunctionContainerWrapper({[](FunctionInternalType::FunctionParams params)
																																																																																																																					  {
																																																																																																																						  return std::shared_ptr<StringContainerWrapper>(new StringContainerWrapper("functionValue return value."));
																																																																																																																					  }}))}});

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
