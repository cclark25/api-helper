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
#include "../../../src/api-model.hpp"
#include "../../../src/type-model/type-model.hpp"
#include "../../../src/type-model/member-pointer.hpp"
#include "../../../src/type-model/member-function-pointer.hpp"
#include "../../../src/type-model/static-pointer.hpp"
#include "../../../src/type-model/static-function-pointer.hpp"
#include "../../../src/type-model/class-typing.hpp"
#include "../../../src/type-model/type-lookup.hpp"
#include "../../../src/type-model/type-generators/type-generator.hpp"

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

int handler(lua_State *s, sol::optional<const std::exception &> o, sol::string_view v)
{
	return -1;
};

template <typename C, typename T>
using X = T C::*;
template <typename C, typename T, typename... P>
struct Y
{
	using TT = T (C::*)(P...);
	using ReturnType = T;
	// using Parameters = P...;
	using ClassType = C;
	TT v;
	Y(const TT &o)
	{
		this->v = o;
	}
};

struct CustomObjectData
{
	static double d1;
	int i1 = 15;
	std::string s1 = "ABC123";
	struct CustomObjectSubData
	{
		int i2 = 30;
		std::string s2 = "DEF456";
	} o1;

	static std::string staticFunction(int num){
		return to_string(num);
	}

	int doStuff(double d, std::string s)
	{
		return 15;
	}
};
double CustomObjectData::d1 = 12.678;

using CustomObjectSubDataSpec = ClassTyping<
	"CustomObjectSubData",
	"A custom class to test typing a class's sub class.",
	CustomObjectData::CustomObjectSubData,
	Member<"i2", &CustomObjectData::CustomObjectSubData::i2, "Sub data's instance int field.">,
	Member<"s2", &CustomObjectData::CustomObjectSubData::s2, "Sub data's instance string field.">>;
using CustomObjectDataSpec = ClassTyping<
	"CustomObjectData",
	"A custom class to test typing.",
	CustomObjectData,
	Static<"d1", &CustomObjectData::d1, "A static double field.">,
	Member<"i1", &CustomObjectData::i1, "An instance int field.">,
	Member<"s1", &CustomObjectData::s1, "An instance string field.">,
	Member<"o1", &CustomObjectData::o1, "An instance object field.">,
	// TODO: add templating to extract function return type and function parameters
	MemberFunction<
		"doStuff",
		&CustomObjectData::doStuff,
		"An instance function that does stuff.",
		APICore::ParameterPack<
			Parameter<"d", "double parameter">,
			Parameter<"s", "string parameter">
		>
	>,
	StaticFunction<
		"staticFunction",
		&CustomObjectData::staticFunction,
		"An static function that does stuff.",
		APICore::ParameterPack<
			Parameter<"num", "int parameter">>>>;

using i1Ptr = Member<"i1", &CustomObjectData::i1>;

RegisterType(CustomObjectData, CustomObjectDataSpec);
RegisterType(CustomObjectData::CustomObjectSubData, CustomObjectSubDataSpec);

std::vector<std::function<void(sol::state)>> handlerCallbacks;

template <class Field>
struct FieldHandler
{
	static void processMemberField()
	{
		std::cout << "Member Field Listed: " << Field::key << " => " << Field::ptr << std::endl;
	};
	static void processStaticField()
	{
		std::cout << "Static Field Listed: " << Field::key << " => " << Field::ptr << std::endl;
	};
};

int main(int argc, char **argv)
{
	auto xxx = &CustomObjectData::doStuff;
	// try
	// {
	// 	sol::state lua;

	// 	lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table, sol::lib::math, sol::lib::string, sol::lib::coroutine, sol::lib::debug, sol::lib::os);

	// 	auto array = std::shared_ptr<ArrayWrapper>(new ArrayContainerWrapper());
	// 	array->push(std::shared_ptr<DataWrapper>(new StringContainerWrapper("Test string 1.")));
	// 	array->push(std::shared_ptr<DataWrapper>(new StringContainerWrapper("Test string 2.")));
	// 	array->push(std::shared_ptr<DataWrapper>(new StringContainerWrapper("Test string 3.")));

	// 	auto classInstance = std::shared_ptr<ObjectWrapper>(new ObjectContainerWrapper(classInstanceType));
	// 	classInstance->setField(
	// 		"f1",
	// 		std::shared_ptr<StringWrapper>(new StringContainerWrapper("Field string.")));

	// 	auto objectValue = std::shared_ptr<ObjectWrapper>(new ObjectContainerWrapper());
	// 	objectValue->setField(
	// 		"field1",
	// 		std::shared_ptr<StringWrapper>(new StringContainerWrapper("Field string.")));
	// 	objectValue->setField(
	// 		"field2",
	// 		std::shared_ptr<Int32Wrapper>(new Int32ContainerWrapper(15)));

	// 	auto apiMappings = std::map<std::string, std::shared_ptr<DataWrapper>>({{"TestClass", classDefinition}, {"testClassInstance", classInstance}, {"stringValue", std::shared_ptr<StringContainerWrapper>(new StringContainerWrapper("Test string."))}, {"intValue", std::shared_ptr<Int32ContainerWrapper>(new Int32ContainerWrapper(0))}, {"objectValue", objectValue}, {"arrayValue", array}, {"functionValue", functionExampleDefinition}});

	// 	bool generateTypes = false;
	// 	for (int i = 1; i < argc; i++)
	// 	{
	// 		std::string param = std::string(argv[i]);

	// 		if (param == "--generateTypes")
	// 		{
	// 			generateTypes = true;
	// 		}
	// 	}

	// 	if (generateTypes)
	// 	{

	// 		lua.set_exception_handler(handler);

	// 		std::string typeFile = APILua::generateTypings("API", lua, apiMappings);

	// 		std::cout << typeFile << "\n";

	// 		return 0;
	// 	}

	// 	APILua::bind<std::string>("API", lua, apiMappings);
	// 	for (auto mapping : apiMappings)
	// 	{
	// 		auto typing = mapping.second->getType();
	// 		if (typing != nullptr)
	// 		{
	// 			printTyping(mapping.first, typing);
	// 		}
	// 	}

	// 	try
	// 	{
	// 		// lua.script_file("../test/lua-test/test-lua.lua");
	// 	}
	// 	catch (sol::error e)
	// 	{
	// 		cerr << "Sol error caught\n";
	// 	}
	// }
	// catch (char const *e)
	// {
	// 	std::cerr << "Caught string error: " << e << std::endl;
	// }
	// catch (std::string e)
	// {
	// 	std::cerr << "Caught string error: " << e << std::endl;
	// }

	printTyping(TypeGenerator<CustomObjectData>::generateTyping());

	sol::state lua;
	// auto apiMappings = std::map<std::string, std::shared_ptr<DataWrapper>>({{"TestClass", classDefinition}, {"stringValue", std::shared_ptr<StringContainerWrapper>(new StringContainerWrapper("Test string."))}, {"intValue", std::shared_ptr<Int32ContainerWrapper>(new Int32ContainerWrapper(0))}, {"functionValue", functionExampleDefinition}, {"newObjectTest", objectValue}});
	// std::string typeFile = APILua::generateTypings("API", lua, {});

	lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table, sol::lib::math, sol::lib::string, sol::lib::coroutine, sol::lib::debug, sol::lib::os);

	struct TestUserType
	{
		int intVal = 12;
		int getInt(int val)
		{
			return val;
		}
		virtual ~TestUserType()
		{
			// std::cout << "TestUserType is destroyed." << std::endl;
		}
	};

	sol::usertype<TestUserType> TestSolType = lua.new_usertype<TestUserType>("TestUserType", sol::constructors<TestUserType()>());
	TestSolType["intVal"] = &TestUserType::intVal;

	X<TestUserType, int> mptr = &TestUserType::intVal;
	Y<TestUserType, int, int> fmptr = &TestUserType::getInt;

	auto ptr = new TestUserType();
	{
		auto testValue = std::shared_ptr<TestUserType>(ptr);
		lua["value"] = testValue;
	}
	// std::cout << "Before lua: " << ptr->intVal << std::endl;
	lua.script(R"(
		print("class's intValue: " .. tostring(value.intVal ));
		value.intVal = 99;
		local value2 = value;
		value = nil;
	)");
	lua.collect_garbage();
	// std::cout << "After lua: " << ptr->intVal << std::endl;

	return 0;
}
