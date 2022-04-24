#ifndef API_INT32
#define API_INT32

#include "../api-object.h"

namespace APICore {
	std::shared_ptr<APIObject> APIObject::makeInt32(std::string name, std::string description, std::shared_ptr<DataWrapper> wrapper) {
		std::shared_ptr<APIObject> newObject = std::shared_ptr<APIObject>(new APIObject(name, description));
		newObject->type = DataPrimitive::int32;
		newObject->dataWrapper = wrapper;
		return newObject;
	}
} // namespace APICore

#endif