#ifndef API_OBJECT_TYPE
#define API_OBJECT_TYPE

#include "../api-object.h"

namespace APICore {

	const std::vector<FieldMap> APIObject::getFields() {
		return this->fields;
	}

	std::shared_ptr<APIObject> APIObject::makeObject(std::string name, std::string description, std::vector<FieldMap> fields) {
		std::shared_ptr<APIObject> newObject = std::shared_ptr<APIObject>(new APIObject(name, description));
		newObject->type = DataPrimitive::object;
		newObject->fields = fields;
		return newObject;
	}

} // namespace APICore

#endif