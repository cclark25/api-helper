// #include "./api-object.cpp"
#define SOL_ALL_SAFETIES_ON 1
#include "../../src/lua-binding/binding-types/all.hpp"
#include "../../src/data-wrappers/data-wrapper.hpp"
#include "./data/test-class.hpp"
#include <lualib.h>
#include <iostream>
#include <cassert>
#include <sol.hpp>
#include "../../../src/api-model.hpp"
#include "../../../src/type-model/type-model.hpp"
#include "../../../src/type-model/member-pointer.hpp"
#include "../../../src/type-model/member-function-pointer.hpp"
#include "../../../src/type-model/static-pointer.hpp"
#include "../../../src/type-model/static-function-pointer.hpp"
#include "../../../src/type-model/class-typing.hpp"
#include "../../../src/type-model/type-lookup.hpp"
#include "../../../src/type-model/type-generators/type-generator.hpp"
#include "../../../src/type-model/lua-binders/type-binder.hpp"

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
		else if(param == "--luaInput"){
			isInputFile = true;
		}
		else if(isInputFile){
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


	LuaBinder<void, CustomObjectData, "ANY">::bind(lua, nullptr);
	auto customInstance = std::shared_ptr<CustomObjectData>(new CustomObjectData());
	lua["testObject"] = customInstance;

	sol::state lua2;
	lua2["testObject"] = customInstance;

	lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table, sol::lib::math, sol::lib::string, sol::lib::coroutine, sol::lib::debug, sol::lib::os);
	lua2.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table, sol::lib::math, sol::lib::string, sol::lib::coroutine, sol::lib::debug, sol::lib::os);

	if(inputFile != ""){
		lua.safe_script_file(inputFile);
		lua2.safe_script_file(inputFile);
		return 0;
	}

	printTyping(TypeGenerator<CustomObjectData>::generateTyping());

	std::cout << "Before lua: " << CustomObjectData::d1 << std::endl;
	auto scriptResult = lua.script(R"(
		print("testObject's type: " .. type(testObject));
		print("CustomObjectData type: " .. type(CustomObjectData));	
		print("testObject.i1 type: " .. type(testObject.i1));	
		print("testObject.i1: " .. tostring(testObject.i1));	
		print("testObject.s1 type: " .. type(testObject.s1));	
		print("testObject.s1: " .. (testObject.s1));	
		print("testObject.o1 type: " .. type(testObject.o1));		
		print("CustomObjectData.d1 type: " .. type(CustomObjectData.d1));	
		print("CustomObjectData.d1: " .. tostring(CustomObjectData.d1));
		CustomObjectData.d1 = 11.111;	
		print("CustomObjectData.d1: " .. tostring(CustomObjectData.d1));
		print("testObject.doStuff type: " .. type(testObject.doStuff));	
		print("testObject.doStuff function result: " .. (testObject:doStuff(12, "stringParam")));	
		print("CustomObjectData.staticFunction type: " .. type(CustomObjectData.staticFunction));	
		print("CustomObjectData.staticFunction function result: " .. (CustomObjectData.staticFunction(12)));	
	)");

	lua.collect_garbage();

	std::cout << "After lua: " << CustomObjectData::d1 << std::endl;

	return 0;
}
