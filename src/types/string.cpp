#ifndef API_STRING
#define API_STRING

#include "../api-object.h"

namespace APICore {
	std::shared_ptr<APIObject> APIObject::makeString(std::string name, std::string description, std::shared_ptr<DataWrapper> wrapper) {
		std::shared_ptr<APIObject> newObject = std::shared_ptr<APIObject>(new APIObject(name, description));
		newObject->type = DataPrimitive::string;
		newObject->dataWrapper = wrapper;
		return newObject;
	}
} // namespace APICore

#endif