// #include "./api-object.cpp"
#define SOL_ALL_SAFETIES_ON 1
#include "../../src/lua-binding/binding-types/all.hpp"
#include "../../src/data-wrappers/data-wrapper.hpp"
#include "./data/test-class.hpp"
#include "../../src/type-model/lua-binders/type-binder.hpp"
#include <lualib.h>
#include <iostream>
#include <cassert>
#include <sol.hpp>

using namespace APICore;
using namespace std;

void printTyping(std::shared_ptr<APICore::TypeWrapperRoot> typing, std::string padding = "")
{
	std::cout << padding << "\tType: " << APICore::dataPrimitiveNameMap.at(typing->getPrimitiveType()) << std::endl;
	std::cout << padding << "\tName: " << typing->getTypeName() << std::endl;
	std::cout << padding << "\tDescription: " << typing->getDescription() << std::endl;
	if (typing->getPrimitiveType() == DataPrimitive::object)
	{
		auto objectTyping = CastSharedPtr(TypeWrapper<DataPrimitive::object>, typing);
		for (auto field : objectTyping->getFields())
		{
			printTyping(field.second, padding + "\t");
		}
	}
	else if (typing->getPrimitiveType() == DataPrimitive::classType)
	{
		auto classTyping = CastSharedPtr(TypeWrapper<DataPrimitive::classType>, typing);
		std::cout << padding << "\t"
				  << "Static Typing: " << std::endl;
		printTyping(classTyping->getStaticType(), padding + "\t\t");
		std::cout << padding << "\t"
				  << "Instance Typing: " << std::endl;
		printTyping(classTyping->getInstanceType(), padding + "\t\t");
	}
	else if (typing->getPrimitiveType() == DataPrimitive::function)
	{
		auto functionTyping = CastSharedPtr(TypeWrapper<DataPrimitive::function>, typing);
		size_t index = 0;
		for (auto field : functionTyping->getParams())
		{
			std::cout << padding << "\tParameter " << index++ << ":" << std::endl;
			printTyping(field, padding + "\t");
		}
		std::cout << padding << "\t"
				  << "Return Typing: " << std::endl;
		printTyping(functionTyping->getReturnType(), padding + "\t");
	}
}

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
		std::string typingJson = APILua::makeTypingObjectFromTypeDefinition(lua, TypeGenerator<CustomObjectData>::generateTyping()).dump();

		std::cout << typingJson << "\n";

		return 0;
	}

	sol::state lua2;
	LuaBinder<CustomObjectData>::declareType(lua);
	LuaBinder<CustomObjectData>::declareType(lua2);
	auto customInstance = std::shared_ptr<CustomObjectData>(new CustomObjectData());
	lua["testObject"] = customInstance;
	lua2["testObject"] = customInstance;
	lua["runNum"] = 1;
	lua2["runNum"] = 2;

	lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table, sol::lib::math, sol::lib::string, sol::lib::coroutine, sol::lib::debug, sol::lib::os);
	lua2.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table, sol::lib::math, sol::lib::string, sol::lib::coroutine, sol::lib::debug, sol::lib::os);

	if (inputFile != "")
	{
		lua.safe_script_file(inputFile);
		// -- TODO: Functions result in seg fault upon garbage collection as it would seem.
		lua.collect_garbage();
		lua2.safe_script_file(inputFile);
		lua2.collect_garbage();
		lua.safe_script_file(inputFile);
		lua.collect_garbage();
		lua2.safe_script_file(inputFile);
		lua2.collect_garbage();
		return 0;
	}


	printTyping(TypeGenerator<CustomObjectData>::generateTyping());

	return 0;
}
