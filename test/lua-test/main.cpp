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
#include "../../../src/type-model/static-pointer.hpp"
#include "../../../src/type-model/class-typing.hpp"

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
	else if (typing->getPrimitiveType() == DataPrimitive::function)
	{
		auto functionTyping = CastSharedPtr(TypeWrapper<DataPrimitive::function>, typing);
		for (auto field : functionTyping->getParams())
		{
			printTyping("", field, padding + "\t");
		}
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

	int doStuff(double d, std::string s)
	{
		return 15;
	}
};
double CustomObjectData::d1 = 12.678;

using CustomObjectSubDataSpec = ClassTyping<
	CustomObjectData::CustomObjectSubData,
	Member<"i1", &CustomObjectData::CustomObjectSubData::i2>,
	Member<"s1", &CustomObjectData::CustomObjectSubData::s2>>;
using CustomObjectDataSpec = ClassTyping<
	CustomObjectData,
	Static<"d1", &CustomObjectData::d1>,
	Member<"i1", &CustomObjectData::i1>,
	Member<"s1", &CustomObjectData::s1>,
	Member<"o1", &CustomObjectData::o1>,
	// TODO: add templating to extract function return type and function parameters
	Member<"doStuff", &CustomObjectData::doStuff>>;

using i1Ptr = Member<"i1", &CustomObjectData::i1>;

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

	APIModel x;
	x["s1"] = "string value";
	x["n1"] = 123;
	// x["f1"] = {std::array<std::string, 3>(), std::function([](std::string s, int i, int j)
	// 							 { return ""; })};

	printTyping("x", x.getTyping());

	auto params = x.getPrimitiveTypes<int, std::string, int, void>();

	for (auto p : params)
	{
		std::cout << "Param: " << dataPrimitiveNameMap.at(p) << std::endl;
	}

	static char pptr[] = "param1";

	struct abc
	{
		int abc;
	};

	auto fun = std::function([](Named<int, "param1"> param1, Named<std::string, "param2"> param2)
							 { 
								param1 += 7;
								param2 = param2.append(": ");
								std::string result = param2;
								for(int i = param1 * 2; i > 0; i--){
									result = result
										.append("(")
										.append(dataPrimitiveNameMap.at(Named<int, "param1">::getPrimitiveType()))
										.append(") ")
										.append (std::to_string(i))
										.append("\n");
								}
								return result; });
	std::cout << fun(12, "Function result with string parameter") << std::endl;

	auto pNames = getVariableNames(fun);
	for (auto p : pNames)
	{
		std::cout << "Parameter name: " << p << std::endl;
	}

	CustomObjectDataSpec::memberCallback<FieldHandler>();
	CustomObjectDataSpec::staticFieldCallback<FieldHandler>();

	CustomObjectData cObj;
	cObj.i1 = 12;
	std::cout << i1Ptr::key << ": " << cObj.*(i1Ptr::ptr) << std::endl;

	// using CustomObject = TypeDefinition<CustomObjectData, FieldIndex<int, "i1">, FieldIndex<std::string, "s1">>;
	// auto xx = CustomObject::field<"i1">();

	// CustomObject::field<"i1">() = &CustomObjectData::i1;
	// CustomObject::field<"s1">() = &CustomObjectData::s1;

	// printTyping("x", CustomObject::type.generateTyping());

	sol::state lua;
	auto objectValue = std::shared_ptr<ObjectWrapper>(new ObjectContainerWrapper());
	// objectValue->setType(CustomObject::generateTyping());
	auto apiMappings = std::map<std::string, std::shared_ptr<DataWrapper>>({{"TestClass", classDefinition}, {"stringValue", std::shared_ptr<StringContainerWrapper>(new StringContainerWrapper("Test string."))}, {"intValue", std::shared_ptr<Int32ContainerWrapper>(new Int32ContainerWrapper(0))}, {"functionValue", functionExampleDefinition}, {"newObjectTest", objectValue}});
	std::string typeFile = APILua::generateTypings("API", lua, apiMappings);
	// std::cout << typeFile << std::endl;

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
