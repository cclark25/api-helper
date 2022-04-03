#ifndef API_NULL
#define API_NULL

#include "../api-object.h"

namespace APICore {
	std::shared_ptr<APIObject> nullObj = std::shared_ptr<APIObject>(new APIObject("", ""));

	std::shared_ptr<APIObject> getNullObj() {
		return nullObj;
	}

	std::shared_ptr<APIObject> APIObject::makeNull(std::string name, std::string description) {
		std::shared_ptr<APIObject> newObject = std::shared_ptr<APIObject>(new APIObject(name, description));
		newObject->type = DataPrimitive::null;
		return newObject;
	}
} // namespace APICore

#endif