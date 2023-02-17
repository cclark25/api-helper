#pragma once

#include <memory>
#include <future>
#include <thread>
#include <chrono>
#include <ratio>
#include <string>
#include "../../../src/type-lookup.hpp"
#include "../../../src/member-pointer.hpp"
#include "../../../src/member-function-pointer.hpp"
#include "../../../src/static-pointer.hpp"
#include "../../../src/static-function-pointer.hpp"
#include "../../../src/class-typing.hpp"
#include "../../../src/standard-bindings.hpp"
// #include "../../../src/json-typing/json-typing.hpp"

using namespace APICore;
using namespace std;

struct CustomObjectData
{
	CustomObjectData()
	{
		std::cout << "CustomObjectData constructed.\n";
	}

	~CustomObjectData()
	{
		int i = 0;
		return;
	}

	static double d1;
	int i1 = 15;
	std::string s1 = "ABC123";
	struct CustomObjectSubData
	{
		int i2 = 30;
		std::string s2 = "DEF456";
	};

	CustomObjectSubData *o1 = new CustomObjectSubData();
	std::shared_ptr<CustomObjectSubData> o2 = std::shared_ptr<CustomObjectSubData>(new CustomObjectSubData());
	CustomObjectSubData o3 = CustomObjectSubData();
	CustomObjectSubData &o4 = this->o3;

	static std::string staticFunction(int num)
	{
		return to_string(num * 2);
	}
	static std::shared_future<std::string> staticAsync(int i)
	{
		return std::async(std::launch::async, [i]()
						  {
			std::this_thread::sleep_for(std::chrono::seconds(i));
			auto result = std::string("Return Value from static async function.");
			return result; });
	}
	std::future<std::string> memberAsync(int i)
	{
		return (std::async(std::launch::async, [i, this]()
						   {
			std::this_thread::sleep_for(std::chrono::seconds(i));
			auto result = std::string("Return Value from member async function. i1 val: ").append(to_string(this->i1));
			return result; }));
	}

	std::function<int(int)> functionPointer = [](int i)
	{
		// std::cout << "Default functionPointer called.\n";
		return i * 12;
	};
	static std::function<int(int)> staticFunctionPointer;

	double testPassedFunction(size_t loopCount)
	{
		auto startTime = std::chrono::steady_clock::now();

		for (size_t i = 0; i < loopCount; i++)
		{
			this->functionPointer(1);
		}

		auto endTime = std::chrono::steady_clock::now();

		return ((double)loopCount) / std::chrono::duration<double, std::ratio<1, 1>>(endTime - startTime).count();
	}

	virtual std::string doStuff(int i, std::string s)
	{
		std::string r = s;
		for (int j = i; j > 0; j--)
		{
			r.append("\n").append(s);
		}
		return r;
	}

	std::string doStuff(int i)
	{
		std::string s = "NO STRING PROVIDED";
		std::string r = s;
		for (int j = i; j > 0; j--)
		{
			r.append("\n").append(s);
		}

		return r;
	}

	static std::string staticOverload(int i)
	{
		return "1";
	}
	static std::string staticOverload(std::string s)
	{
		return "2";
	}
};
double CustomObjectData::d1 = 12.678;
std::function<int(int)> CustomObjectData::staticFunctionPointer = [](int i)
{
	// std::cout << "Default functionPointer called.\n";
	return i * 12;
};

struct CustomObjectData2 : public CustomObjectData
{
	int secondI = 12;

	/*
		Since this function is virtual, there is no need to redeclare this member function
		in the type definition for CustomObjectData2.
		The function resolution is automatically handled by C++ mechanics.
	*/
	virtual std::string doStuff(int i, std::string s)
	{
		std::string r = s;
		for (int j = i; j > 0; j--)
		{
			r.append("\t").append(s);
		}
		return r;
	}

	std::string doStuff(std::string s)
	{
		return s;
	}

	static std::string staticOverload(std::string s, int i)
	{
		return "3";
	}
};

using CustomObjectSubDataSpec = ClassTyping<
	"CustomObjectSubData",
	"A custom class to test typing a class's sub class.",
	CustomObjectData::CustomObjectSubData,
	void,
	Member<"i2", &CustomObjectData::CustomObjectSubData::i2, "Sub data's instance int field.">,
	Member<"s2", &CustomObjectData::CustomObjectSubData::s2, "Sub data's instance string field.">>;

using CustomObjectDataDoStuffOverloads = MemberOverload<
	MemberFunction<
		"doStuff",
		(std::string(CustomObjectData::*)(int, std::string)) & CustomObjectData::doStuff,
		"An instance function that does stuff. Takes an int and a string.",
		ParameterPack<
			Parameter<"i", "int parameter">,
			Parameter<"s", "string parameter">>>,
	MemberFunction<
		"doStuff",
		(std::string(CustomObjectData::*)(int)) & CustomObjectData::doStuff,
		"An instance function that does stuff. Takes an int.",
		ParameterPack<
			Parameter<"i", "int parameter">>>

	>;

using CustomObjectDataStaticOverloads = StaticOverload<
	StaticFunction<
		"staticOverload",
		(std::string(*)(int)) & CustomObjectData::staticOverload,
		"A static function to test overloading. Takes an int.",
		ParameterPack<
			Parameter<"i", "int parameter to be used in the function passed.">>>,
	StaticFunction<
		"staticOverload",
		(std::string(*)(std::string)) & CustomObjectData::staticOverload,
		"A static function to test overloading. Takes a string.",
		ParameterPack<
			Parameter<"s", "string parameter to be used in the function passed.">>>>;

using F1 = Static<"d1", &CustomObjectData::d1, "A static double field.">;
using F2 = Member<"i1", &CustomObjectData::i1, "An instance int field.">;
using F3 = Member<"s1", &CustomObjectData::s1, "An instance string field.">;
using F4 = Member<"o1", &CustomObjectData::o1, "Sub class data pointer.">;
using F5 = Member<"o2", &CustomObjectData::o2, "Sub class data shared pointer.">;
using F6 = Member<"o3", &CustomObjectData::o3, "Sub class data non-pointer.">;
using F7 = Member<"o4", &CustomObjectData::o3, "Sub class data & reference.">;

using F8 = MemberFunction<
	"functionPointer",
	&CustomObjectData::functionPointer,
	"A function pointer to bind to lua.",
	ParameterPack<
		Parameter<"i", "int parameter to be used in the function passed.">>>;
using F9 = StaticFunction<
	"staticFunctionPointer",
	&CustomObjectData::staticFunctionPointer,
	"A function pointer to bind to lua.",
	ParameterPack<
		Parameter<"i", "int parameter to be used in the function passed.">>>;

using F10 = MemberFunction<
	"testPassedFunction",
	&CustomObjectData::testPassedFunction,
	"A function to test how many invocations of the functionPointer can be done per second.",
	ParameterPack<
		Parameter<"i", "int parameter">>>;

using F11 = StaticFunction<
	"staticFunction",
	&CustomObjectData::staticFunction,
	"An static function that does stuff.",
	ParameterPack<
		Parameter<"num", "int parameter">>>;

// TODO: generated json is typing these 2 functions the same for some reason.
using F12 = StaticFunction<
	"staticAsync",
	&CustomObjectData::staticAsync,
	"An static function that does stuff asynchronously and returns a promise.",
	ParameterPack<
		Parameter<"numSeconds", "Number of seconds to wait.">>>;
using F13 = MemberFunction<
	"memberAsync",
	&CustomObjectData::memberAsync,
	"An member function that does stuff asynchronously and returns a promise.",
	ParameterPack<
		Parameter<"numSeconds", "Number of seconds to wait.">>>;

using CustomConstructor =
	StaticOverload<
		Constructor<
			+[](int num)
			{
				return CustomObjectData();
			},
			"An inline lambda function attached as a constructor function.",
			ParameterPack<
				Parameter<"num", "int parameter">>>,
		Constructor<
			+[](std::string str)
			{
				return CustomObjectData();
			},
			"An overloaded inline lambda function attached as a constructor function. Takes a string instead of an int.",
			ParameterPack<
				Parameter<"str", "string parameter">>>>;

using CustomObjectDataSpec = ClassTyping<
	// Class name to bind to
	"CustomObjectData",
	// Description
	"A custom class to test typing.",
	// C++ type to bind
	CustomObjectData,
	// Inherits from nothing. It is its own class
	void,

	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,

	F8,
	F9,

	CustomObjectDataDoStuffOverloads,
	CustomObjectDataStaticOverloads,

	F10,
	F11,
	F12,
	F13,

	CustomConstructor>;

RegisterType(CustomObjectData, CustomObjectDataSpec);
RegisterType(CustomObjectData::CustomObjectSubData, CustomObjectSubDataSpec);

///////////////////////////////////////////////////////////////////////////////

using G1 = Member<"secondI", &CustomObjectData2::secondI, "A value only available on the child class CustomObjectData2.">;
using SecondCustomObjectDataDoStuffOverloads = CustomObjectDataDoStuffOverloads::AddOverloads<
	MemberFunction<
		"doStuff",
		(std::string(CustomObjectData2::*)(std::string)) & CustomObjectData2::doStuff,
		"An instance function that does stuff. Takes a string. Specifically for CustomObjectData2.",
		ParameterPack<
			Parameter<"s", "string parameter">>>>;

using SecondCustomObjectDataStaticOverloads = CustomObjectDataStaticOverloads::AddOverloads<
	StaticFunction<
		"staticOverload",
		&CustomObjectData2::staticOverload,
		"A static function to test overloading. Takes a string and an int. Specifically for CustomObjectData2.",
		ParameterPack<
			Parameter<"s", "string parameter to be used in the function passed.">,
			Parameter<"i", "int parameter to be used in the function passed.">>>>;

using CustomObjectDataSpec2 = ClassTyping<
	"CustomObjectData2",
	"A custom class to test typing.",
	CustomObjectData2,
	/*
		Specifies that CustomObjectData inherits from CustomObjectData, and all
		CustomObjectData bindings should also be bound to CustomObjectData2.
	*/
	CustomObjectData,
	G1,
	SecondCustomObjectDataDoStuffOverloads,
	SecondCustomObjectDataStaticOverloads,
	Constructor<
		+[](double d)
		{
			return CustomObjectData2();
		},
		"An inline lambda function attached as a constructor function. Specifically for CustomObjectData2.",
		ParameterPack<
			Parameter<"d", "double parameter">
		>
	>
>;

RegisterType(CustomObjectData2, CustomObjectDataSpec2);
