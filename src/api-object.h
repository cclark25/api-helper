// #ifndef API
// #define API
// #include <string>
// #include <iostream>
// #include <functional>
// #include <vector>
// #include <map>
// #include <memory>
// #include "./data-primitive.cpp"
// #include "./data-wrappers/data-wrapper.hpp"
// #include "./macros.hpp"


// namespace APICore {
// 	using namespace std;
// 	class APIObject;

// 	enum AccessModifier { publicField, privateField, protectedField, readOnlyField };
// 	struct FieldMap {
// 		AccessModifier access;
// 		shared_ptr<APIObject> value;
// 	};

// 	struct ArrayTyping {
// 		/** Type of each element in the array */
// 		shared_ptr<APIObject> type;
// 		/**
// 		 * Optional field restricting the length of the array.
// 		 * Negative numbers indicate unlimited length
// 		 */
// 		int length = -1;
// 		/**
// 		 * Array of values to use to populate the array.
// 		 */
// 		vector<shared_ptr<DataWrapper>> defaultValues;
// 	};

// 	typedef std::function<shared_ptr<void>(APIFuncParams)> StandardFunction;
// 	typedef vector<shared_ptr<APIObject>> StandardFunctionParameters;

// 	shared_ptr<APIObject> getNullObj();

// 	class APIObject {
// 	private:
// 		std::string name;
// 		std::string description;
// 		DataPrimitive type;

// 		void assertType(DataPrimitive type) const {
// 			if (type != this->type) {
// 				// TODO: Create standard error class and expand this message.
// 				std::cerr << "Type Assertion Failed.\n";
// 				throw "TypeError: Type assertion failed.";
// 			}
// 		}

// 	public:
// 		APIObject(std::string name, std::string description) {
// 			this->name = name;
// 			this->description = description;
// 			this->type = DataPrimitive::null;
// 		}
// 		APIObject(const APIObject& other) {
// 			this->name = other.name;
// 			this->description = other.description;
// 			this->type = other.type;
// 			switch (this->type) {

// 			case DataPrimitive::null:
// 			case DataPrimitive::int32:
// 			case DataPrimitive::string:
// 				break;
// 			case DataPrimitive::function:
// 				this->functionVal = other.functionVal;
// 				this->thisType = other.thisType;
// 				this->parameterTypes = this->parameterTypes;
// 				this->returnType = other.returnType;
// 				break;
// 			default:
// 				std::cerr << "Unsupported type.\n";
// 				throw "Unsupported type";
// 			}
// 		}
// 		APIObject(shared_ptr<APIObject> other) : APIObject(*other) {
// 		}
// 		DataPrimitive getType() const {
// 			return this->type;
// 		};
// 		std::string getName() const {
// 			return this->name;
// 		};
// 		std::string getDescription() const {
// 			return this->description;
// 		};


// 		// null

// 	public:
// 		static shared_ptr<APIObject> makeNull(std::string name, std::string description);
// 		//

// 		// int32
// 	private:

// 	public:
// 		static shared_ptr<APIObject> makeInt32(std::string name, std::string description, std::shared_ptr<DataWrapper> wrapper);
// 		//

// 		// string
// 	private:

// 	public:
// 		static shared_ptr<APIObject> makeString(std::string name, std::string description, std::shared_ptr<DataWrapper> wrapper);
// 		//

// 		// function
// 	private:
// 		/**
// 		 * @brief Function source for function object definitions.
// 		 * @param (Data*)thisPtr Value to use for member object references.
// 		 * @returns vector<Data> results - List of results returned by the function through the API.
// 		 */
// 		StandardFunction functionVal;
// 		/**
// 		 * @brief APIObject describing the type of the thisPtr object passed into the functionVal, if any.
// 		 */
// 		shared_ptr<APIObject> thisType = getNullObj();
// 		/**
// 		 * @brief List of parameter types definitions, in order, that will be passed into the call to functionVal
// 		 */
// 		StandardFunctionParameters parameterTypes = StandardFunctionParameters();
// 		/**
// 		 * @brief APIObject describing the return type of the call to functionVal, if any.
// 		 */
// 		shared_ptr<APIObject> returnType = getNullObj();


// 	public:
// 		std::function<shared_ptr<void>(APIFuncParams)> getFunctionDefault() const;
// 		shared_ptr<APIObject> getThisType() const;
// 		StandardFunctionParameters getParameterTypes() const;
// 		shared_ptr<APIObject> getReturnType();
// 		static shared_ptr<APIObject> makeFunction(std::string name, std::string description,
// 		     std::function<shared_ptr<void>(APIFuncParams)> functionVal, StandardFunctionParameters(params),
// 		     shared_ptr<APIObject> returnType = getNullObj(), shared_ptr<APIObject> thisType = getNullObj());

// 		//

// 		// object
// 	private:
// 		vector<FieldMap> fields;

// 	public:
// 		const vector<FieldMap> getFields();
// 		static shared_ptr<APIObject> makeObject(std::string name, std::string description, vector<FieldMap> fields);

// 		//

// 		// array
// 	private:
// 		ArrayTyping arrayType;

// 	public:
// 		const ArrayTyping getArrayType() const;
// 		static shared_ptr<APIObject> makeArray(std::string name, std::string description, ArrayTyping arrayType);

// 		//

// 		// classType
// 	private:
// 		shared_ptr<APIObject> prototype;
// 		shared_ptr<APIObject> constructor;

// 	public:
// 		shared_ptr<APIObject> getPrototype();
// 		shared_ptr<APIObject> getConstructor();

// 		static shared_ptr<APIObject> makeClass(std::string name, std::string description,
// 		     std::function<shared_ptr<void>(APIFuncParams)> constructor, StandardFunctionParameters(params), vector<FieldMap> staticFields,
// 		     vector<FieldMap> prototypeFields);

// 		//
// 	public:
// 		~APIObject() {
// 		}
// 	};


// } // namespace APICore

// #include "./types/null.cpp"
// #include "./types/string.cpp"
// #include "./types/int32.cpp"
// #include "./types/function.cpp"
// #include "./types/object.cpp"
// #include "./types/array.cpp"
// #include "./types/class.cpp"

// #endif