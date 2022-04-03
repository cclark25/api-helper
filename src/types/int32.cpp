#ifndef API_INT32
#define API_INT32

#include "../api-object.h"

namespace APICore {
	int32_t APIObject::getInt32Default() const {
		this->assertType(DataPrimitive::int32);
		return this->int32Val;
	}

	std::shared_ptr<APIObject> APIObject::makeInt32(std::string name, std::string description, int32_t int32Val) {
		std::shared_ptr<APIObject> newObject = std::shared_ptr<APIObject>(new APIObject(name, description));
		newObject->type = DataPrimitive::int32;
		newObject->int32Val = int32Val;
		return newObject;
	}
} // namespace APICore

#endif