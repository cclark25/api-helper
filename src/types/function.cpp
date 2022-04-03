#ifndef API_FUNCTION
#define API_FUNCTION

#include "../api-object.h"

namespace APICore {
	std::function<std::shared_ptr<void>(APIFuncParams)> APIObject::getFunctionDefault() const {
		this->assertType(DataPrimitive::function);
		return this->functionVal;
	};

	std::shared_ptr<APIObject> APIObject::getThisType() const {
		return this->thisType;
	};
	StandardFunctionParameters APIObject::getParameterTypes() const {
		return this->parameterTypes;
	};
	std::shared_ptr<APIObject> APIObject::getReturnType() {
		return this->returnType;
	};

	std::shared_ptr<APIObject> APIObject::makeFunction(std::string name, std::string description,
	     std::function<std::shared_ptr<void>(APIFuncParams)> functionVal, StandardFunctionParameters params,
	     std::shared_ptr<APIObject> returnType, std::shared_ptr<APIObject> thisType) {
		std::shared_ptr<APIObject> newObject = std::shared_ptr<APIObject>(new APIObject(name, description));
		newObject->type = DataPrimitive::function;
		newObject->functionVal = functionVal;
		newObject->returnType = returnType;
		newObject->parameterTypes = params;
		newObject->thisType = thisType;
		return newObject;
	}
} // namespace APICore

#endif