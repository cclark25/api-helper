#ifndef API_STRING
#define API_STRING

#include "../api-object.h"

namespace APICore {
	std::string APIObject::getStringDefault() const {
		this->assertType(DataPrimitive::string);
		return this->stringVal;
	}

	std::shared_ptr<APIObject> APIObject::makeString(std::string name, std::string description, std::string stringVal) {
		std::shared_ptr<APIObject> newObject = std::shared_ptr<APIObject>(new APIObject(name, description));
		newObject->type = DataPrimitive::string;
		newObject->stringVal = std::string(stringVal);
		return newObject;
	}
} // namespace APICore

#endif