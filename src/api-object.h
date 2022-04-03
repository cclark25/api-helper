#ifndef API
#define API
#include <string>
#include <iostream>
#include <functional>
#include <vector>
#include <map>
#include <memory>
#include "./data-primitive.cpp"

#define APIFuncParams std::shared_ptr<void> thisPtr, std::vector<std::shared_ptr<void>>&parameters
#define CastSharedPtr(type, data) (*((std::shared_ptr<type>*)(&data)))

namespace APICore {
	class APIObject;

	enum AccessModifier { publicField, privateField, protectedField, readOnlyField };
	struct FieldMap {
		AccessModifier access;
		std::shared_ptr<APIObject> value;
	};

	struct ArrayTyping {
		/** Type of each element in the array */
		std::shared_ptr<APIObject> type;
		/**
		 * Optional field restricting the length of the array.
		 * Negative numbers indicate unlimited length
		 */
		int length = -1;
		/**
		 * Array of values to use to populate the array.
		 */
		std::vector<std::shared_ptr<void>> defaultValues;
	};

	typedef std::function<std::shared_ptr<void>(APIFuncParams)> StandardFunction;
	typedef std::vector<std::shared_ptr<APIObject>> StandardFunctionParameters;

	std::shared_ptr<APIObject> getNullObj();

	class APIObject {
	private:
		std::string name;
		std::string description;
		DataPrimitive type;

		void assertType(DataPrimitive type) const {
			if (type != this->type) {
				// TODO: Create standard error class and expand this message.
				std::cerr << "Type Assertion Failed.\n";
				throw "TypeError: Type assertion failed.";
			}
		}

	public:
		APIObject(std::string name, std::string description) {
			this->name = name;
			this->description = description;
			this->type = DataPrimitive::null;
		}
		APIObject(const APIObject& other) {
			this->name = other.name;
			this->description = other.description;
			this->type = other.type;
			switch (this->type) {

			case DataPrimitive::null:
				break;
			case DataPrimitive::int32:
				this->int32Val = other.int32Val;
				break;
			case DataPrimitive::string:
				this->stringVal = other.stringVal;
				break;
			case DataPrimitive::function:
				this->functionVal = other.functionVal;
				this->thisType = other.thisType;
				this->parameterTypes = this->parameterTypes;
				this->returnType = other.returnType;
				break;
			default:
				std::cerr << "Unsupported type.\n";
				throw "Unsupported type";
			}
		}
		APIObject(std::shared_ptr<APIObject> other) : APIObject(*other) {
		}
		DataPrimitive getType() const {
			return this->type;
		};
		std::string getName() const {
			return this->name;
		};
		std::string getDescription() const {
			return this->description;
		};

		// null

	public:
		static std::shared_ptr<APIObject> makeNull(std::string name, std::string description);
		//

		// int32
	private:
		int32_t int32Val;

	public:
		int32_t getInt32Default() const;
		static std::shared_ptr<APIObject> makeInt32(std::string name, std::string description, int32_t int32Val);
		//

		// string
	private:
		std::string stringVal;

	public:
		std::string getStringDefault() const;
		static std::shared_ptr<APIObject> makeString(std::string name, std::string description, std::string stringVal);
		//

		// function
	private:
		/**
		 * @brief Function source for function object definitions.
		 * @param (Data*)thisPtr Value to use for member object references.
		 * @returns std::vector<Data> results - List of results returned by the function through the API.
		 */
		StandardFunction functionVal;
		/**
		 * @brief APIObject describing the type of the thisPtr object passed into the functionVal, if any.
		 */
		std::shared_ptr<APIObject> thisType = getNullObj();
		/**
		 * @brief List of parameter types definitions, in order, that will be passed into the call to functionVal
		 */
		StandardFunctionParameters parameterTypes = StandardFunctionParameters();
		/**
		 * @brief APIObject describing the return type of the call to functionVal, if any.
		 */
		std::shared_ptr<APIObject> returnType = getNullObj();


	public:
		std::function<std::shared_ptr<void>(APIFuncParams)> getFunctionDefault() const;
		std::shared_ptr<APIObject> getThisType() const;
		StandardFunctionParameters getParameterTypes() const;
		std::shared_ptr<APIObject> getReturnType();
		static std::shared_ptr<APIObject> makeFunction(std::string name, std::string description,
		     std::function<std::shared_ptr<void>(APIFuncParams)> functionVal, StandardFunctionParameters(params),
		     std::shared_ptr<APIObject> returnType = getNullObj(), std::shared_ptr<APIObject> thisType = getNullObj());

		//

		// object
	private:
		std::vector<FieldMap> fields;

	public:
		const std::vector<FieldMap> getFields();
		static std::shared_ptr<APIObject> makeObject(std::string name, std::string description, std::vector<FieldMap> fields);

		//

		// array
	private:
		ArrayTyping arrayType;

	public:
		const ArrayTyping getArrayType() const;
		static std::shared_ptr<APIObject> makeArray(std::string name, std::string description, ArrayTyping arrayType);

		//

		// classType
	private:
		std::shared_ptr<APIObject> prototype;
		std::shared_ptr<APIObject> constructor;

	public:
		std::shared_ptr<APIObject> getPrototype();
		std::shared_ptr<APIObject> getConstructor();

		static std::shared_ptr<APIObject> makeClass(std::string name, std::string description,
		     std::function<std::shared_ptr<void>(APIFuncParams)> constructor, StandardFunctionParameters(params), std::vector<FieldMap> staticFields,
		     std::vector<FieldMap> prototypeFields);

		//
	public:
		~APIObject() {
		}
	};


} // namespace APICore

#include "./types/null.cpp"
#include "./types/string.cpp"
#include "./types/int32.cpp"
#include "./types/function.cpp"
#include "./types/object.cpp"
#include "./types/array.cpp"

#endif