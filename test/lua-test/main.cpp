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

template <size_t Length>
struct StringLiteral
{
	char value[Length]{};
	static constexpr std::size_t size = Length - 1;

	[[nodiscard]] std::string_view view() const
	{
		return {value, value + size};
	}

	consteval StringLiteral() {}
	consteval StringLiteral(const char (&str)[Length]) : StringLiteral()
	{
		std::copy_n(str, Length, value);
	}
};

template <StringLiteral Name, typename T>
struct Named
{
	using type = T;
	constexpr static const char *name = Name.value;
	T value;
};

template <typename R, typename... P>
std::vector<std::string> getParamNames(std::function<R(P...)> p)
{
	std::vector<std::string> result;

	(result.push_back(P::name), ...);

	return result;
}

#define Parameter( Type, Name ) Named< #Name,Type> Name

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

	auto fun = std::function ([](Parameter(int, param1), Parameter(std::string, param2))
							 { return param1.value * 2; });

	auto pNames = getParamNames(fun);
	for (auto p : pNames)
	{
		std::cout << "Name: " << p << std::endl;
	}
	return 0;
}
