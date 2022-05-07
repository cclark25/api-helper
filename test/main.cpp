// // #include "./api-object.cpp"
// #define SOL_ALL_SAFETIES_ON 1
// #include "../src/api-object.h"
// #include "../src/lua-binding/lua-binding.cpp"
// #include <lualib.h>
// #include <iostream>
// #include <cassert>
// #include <sol.hpp>

// using namespace APICore;

// void printStandard(shared_ptr<APIObject> example, std::string leftPad = "");

// class A
// {
// public:
// 	int n = 0;
// 	A()
// 	{
// 	}
// 	A(const A &oth)
// 	{
// 	}
// 	~A()
// 	{
// 		std::cout << "A destructor\n";
// 	}
// };

// void testSafePtrs()
// {
// 	vector<shared_ptr<void>> x;
// 	for (int i = 0; i < 20; i++)
// 	{
// 		x.push_back(shared_ptr<A>(new A()));
// 	}
// }

// using namespace std;

// int main()
// {
// 	auto _a = A();
// 	shared_ptr<void> *a = (shared_ptr<void> *)new shared_ptr<A>(new A(_a));
// 	delete a;
// 	std::cout << "\n\n\n";

// 	auto int32Data = std::shared_ptr<IntWrapper>(new IntWrapper());
// 	shared_ptr<APIObject> int32Example = APIObject::makeInt32("testInt", "Test description", int32Data);
// 	auto stringData = std::shared_ptr<StringWrapper>(new StringWrapper());
// 	shared_ptr<APIObject> stringExample = APIObject::makeString("testString", "Test description", stringData);
// 	shared_ptr<APIObject> functionExample = APIObject::makeFunction(
// 		"testFunction",
// 		"Test function description",
// 		[](APIFuncParams)
// 		{
// 			if (parameters.size() == 0)
// 			{
// 				return shared_ptr<void>(nullptr);
// 			}

// 			std::cout << "Vector args length: " << parameters.size() << "\n";
// 			auto number = CastSharedPtr(int32_t, parameters.at(0));
// 			auto str = CastSharedPtr(std::string, parameters.at(1));
// 			std::cout << "Number passed in: " << *number << endl;
// 			std::cout << "String passed in: " << *str << endl;
// 			shared_ptr<void> result = shared_ptr<std::string>(new std::string("testFunction return string"));
// 			return result;
// 		},
// 		StandardFunctionParameters({APIObject::makeInt32("param1", "First parameter to the function.", std::shared_ptr<IntWrapper>(new IntWrapper(55))),
// 									APIObject::makeString("param2", "Second parameter to the function", std::shared_ptr<StringWrapper>(new StringWrapper("function retuned")))}),
// 									stringExample);
// 	shared_ptr<APIObject> functionExample2 = APIObject::makeFunction(
// 		"testFunction2",
// 		"Test function2 description",
// 		[](APIFuncParams)
// 		{
// 			std::cout << "Function example2 called :)" << endl;
// 			return shared_ptr<int32_t>(new int32_t(222));
// 		},
// 		StandardFunctionParameters({APIObject::makeInt32("param1", "First parameter to the function.", int32Data),
// 									APIObject::makeString("param2", "Second parameter to the function", stringData)}),
// 		APIObject::makeInt32("function2ReturnType", "Int32 returned by function 2", int32Data),
// 		APIObject::makeInt32("functionWrapper", "Parent object of the function", stringData));
// 	shared_ptr<APIObject> functionExample3 = APIObject::makeFunction(
// 		"testFunction3",
// 		"Test function3 description",
// 		[](APIFuncParams)
// 		{
// 			std::cout << "Function example3 called :)" << endl;
// 			return shared_ptr<std::string>(new std::string("Function 3 return string."));
// 		},
// 		StandardFunctionParameters({APIObject::makeInt32("param1", "First parameter to the function.", int32Data),
// 									APIObject::makeString("param2", "Second parameter to the function", stringData)}),
// 		APIObject::makeString("function3ReturnType", "String returned by function 3", stringData),
// 		APIObject::makeInt32("functionWrapper", "Parent object of the function", int32Data));

// 	shared_ptr<APIObject> objectExample = APIObject::makeObject("objectExample",
// 																"An example object definition.",
// 																vector<FieldMap>({{AccessModifier::publicField, int32Example},
// 																				  {AccessModifier::publicField, stringExample},
// 																				  {AccessModifier::publicField, functionExample3}}));

// 	auto arrayExample = APIObject::makeArray("arrayExample",
// 											 "An example array definition.",
// 											 {stringExample,
// 											  -1,
// 											  {shared_ptr<StringWrapper>(new StringWrapper("Array string value.")),
// 											   shared_ptr<StringWrapper>(new StringWrapper("Array second string value.")),
// 											   shared_ptr<StringWrapper>(new StringWrapper("Array third string value."))}});

// 	auto classExample = APIObject::makeClass(
// 		"TestClass", "A test class.", [](APIFuncParams)
// 		{ return shared_ptr<void>(new std::string("Returned string")); },
// 		StandardFunctionParameters(), vector<APICore::FieldMap>({{AccessModifier::publicField, stringExample}}), vector<APICore::FieldMap>({{AccessModifier::publicField, int32Example}}));

// 	auto x = shared_ptr<APIObject>(int32Example);
// 	auto y = shared_ptr<APIObject>(x);
// 	y = x;
// 	try
// 	{
// 		// printStandard(int32Example, "");
// 		// printStandard(stringExample, "");
// 		// printStandard(functionExample, "");
// 		// printStandard(functionExample2, "");
// 		// printStandard(functionExample3, "");
// 		// printStandard(objectExample, "");

// 		sol::state lua;
// 		// lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table, sol::lib::math, sol::lib::string);

// 		// lua.require_script("lualib_bundle", ((char*)lualib_bundle), true);
// 		APILua::bindAPI("API", lua, {stringExample, int32Example, objectExample, functionExample, arrayExample, classExample});

// 		// lua["classThing"] = ((sol::function) lua["__TS__Class"])();

// 		try
// 		{
// 			lua.script_file("./test/test-lua.lua");
// 		}
// 		catch (sol::error e)
// 		{
// 			cerr << "Sol error caught\n";
// 		}
// 	}
// 	catch (char const *e)
// 	{
// 		std::cerr << "Caught string error: " << e << std::endl;
// 	}
// 	catch (std::string e)
// 	{
// 		std::cerr << "Caught string error: " << e << std::endl;
// 	}
// 	return 0;
// }

// void printStandard(shared_ptr<APIObject> example, std::string leftPad)
// {
// 	std::cout << leftPad << "Type: " << example->getType() << endl;
// 	std::cout << leftPad << "Name: " << example->getName() << endl;
// 	std::cout << leftPad << "Description: " << example->getDescription() << endl;

// 	APICore::Data data = std::shared_ptr<void>();
// 	if (example->dataWrapper->canGet())
// 	{
// 		data = example->dataWrapper->get();
// 	}
// 	switch (example->getType())
// 	{
// 	case DataPrimitive::int32:
// 		std::cout << leftPad << "Int32 Value: " << *CastSharedPtr(int32_t, data) << endl;
// 		break;
// 	case DataPrimitive::string:
// 		std::cout << leftPad << "String Value: " << *CastSharedPtr(std::string, data) << endl;
// 		break;
// 	case DataPrimitive::function:
// 	{
// 		auto f = example->getFunctionDefault();
// 		auto thisType = example->getThisType();

// 		if (thisType->getType() != DataPrimitive::null)
// 		{
// 			std::cout << leftPad << "This type:\n";
// 			printStandard(thisType, leftPad + "\t");
// 		}
// 		else
// 		{
// 			std::cout << leftPad << "This type: null\n";
// 		}

// 		std::cout << leftPad << "Function Parameters:\n";
// 		for (auto p : example->getParameterTypes())
// 		{
// 			printStandard(p, leftPad + "\t");
// 		}

// 		auto returnType = example->getReturnType();
// 		if (returnType->getType() != DataPrimitive::null)
// 		{
// 			std::cout << leftPad << "Return Type:\n";
// 			printStandard(returnType, leftPad + "\t");
// 		}
// 		else
// 		{
// 			std::cout << leftPad << "Return Type: null\n";
// 		}

// 		auto params = vector<shared_ptr<void>>();
// 		auto result = f(nullptr, params);
// 		auto rp = (void *)&result;

// 		switch (returnType->getType())
// 		{
// 		case DataPrimitive::string:
// 			std::cout << leftPad << "String returned: " << *CastSharedPtr(std::string, result) << endl;
// 			break;
// 		case DataPrimitive::int32:
// 			std::cout << leftPad << "Int32 returned: " << *CastSharedPtr(int32_t, result) << endl;
// 			break;

// 		default:
// 			break;
// 		}
// 	}
// 	break;

// 	case DataPrimitive::object:
// 	{
// 		auto fields = example->getFields();
// 		std::cout << leftPad << "Object Fields: " << endl;
// 		for (FieldMap field : fields)
// 		{
// 			std::string accessString = "";
// 			switch (field.access)
// 			{
// 			case AccessModifier::privateField:
// 				accessString = "Private";
// 				break;
// 			case AccessModifier::publicField:
// 				accessString = "Public";
// 				break;
// 			case AccessModifier::protectedField:
// 				accessString = "Protected";
// 				break;
// 			case AccessModifier::readOnlyField:
// 				accessString = "ReadOnly";
// 				break;

// 			default:
// 				break;
// 			}
// 			std::cout << leftPad + "\t"
// 					  << "(" << accessString << ")" << endl;
// 			printStandard(field.value, leftPad + "\t");
// 		}
// 	}
// 	break;

// 	default:
// 		break;
// 	}

// 	std::cout << "\n\n\n";
// }