#ifndef __TEST_CLASS_DEFINITION
#define __TEST_CLASS_DEFINITION
#include <memory>
#include "../../../src/type-lookup.hpp"
#include <string>
#include "../../../src/member-pointer.hpp"
#include "../../../src/member-function-pointer.hpp"
#include "../../../src/static-pointer.hpp"
#include "../../../src/static-function-pointer.hpp"
#include "../../../src/class-typing.hpp"
#include "../../../src/json-typing/json-typing.hpp"

using namespace APICore;
using namespace std;

struct CustomObjectData
{
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

	std::string doStuff(int i, std::string s)
	{
		std::string r = s;
		for (int j = i; j > 0; j--)
		{
			r.append("\n").append(s);
		}
		return r;
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
	Member<"o1", &CustomObjectData::o1, "Sub class data pointer.">,
	Member<"o2", &CustomObjectData::o2, "Sub class data shared pointer.">,
	Member<"o3", &CustomObjectData::o3, "Sub class data non-pointer.">,
	Member<"o4", &CustomObjectData::o3, "Sub class data & reference.">,
	MemberFunction<
		"doStuff",
		&CustomObjectData::doStuff,
		"An instance function that does stuff.",
		ParameterPack<
			Parameter<"i", "int parameter">,
			Parameter<"s", "string parameter">>>,
	StaticFunction<
		"staticFunction",
		&CustomObjectData::staticFunction,
		"An static function that does stuff.",
		ParameterPack<
			Parameter<"num", "int parameter">>>>;

RegisterType(CustomObjectData, CustomObjectDataSpec);
RegisterType(CustomObjectData::CustomObjectSubData, CustomObjectSubDataSpec);

#endif