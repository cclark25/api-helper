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

int main()
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

		APILua::bind<std::string>("API", lua, std::map<std::string, std::shared_ptr<DataWrapper>>({{"TestClass", classDefinition}, {"testClassInstance", classInstance}, {"stringValue", std::shared_ptr<StringContainerWrapper>(new StringContainerWrapper("Test string."))}, {"intValue", std::shared_ptr<Int32ContainerWrapper>(new Int32ContainerWrapper(0))}, {"objectValue", objectValue}, {"arrayValue", array}, {"functionValue", std::shared_ptr<FunctionWrapper>(new FunctionContainerWrapper({[](FunctionInternalType::FunctionParams params)
																																																																																																																   {
																																																																																																																	   return std::shared_ptr<StringContainerWrapper>(new StringContainerWrapper("functionValue return value."));
																																																																																																																   }}))}}));

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
