#ifndef API_CLASS_TYPE
#define API_CLASS_TYPE

#include "../api-object.h"

namespace APICore {

	std::shared_ptr<APIObject> APIObject::getPrototype() {
		return this->prototype;
	}
	std::shared_ptr<APIObject> APIObject::getConstructor() {
		return this->constructor;
	}

	std::shared_ptr<APIObject> APIObject::makeClass(std::string name, std::string description, std::function<std::shared_ptr<void>(APIFuncParams)> constructor,
	     StandardFunctionParameters(params), std::vector<FieldMap> staticFields, std::vector<FieldMap> prototypeFields) {
		std::shared_ptr<APIObject> newObject = std::shared_ptr<APIObject>(new APIObject(name, description));
		newObject->type = DataPrimitive::classType;
		newObject->fields = staticFields;
		newObject->prototype = APIObject::makeObject("_" + name + "_prototype", "Prototype of the " + name + " class.", prototypeFields);
		newObject->constructor
		     = APIObject::makeFunction("_" + name + "_constructor", "Constructor of the " + name + "class.", constructor, params, getNullObj(), newObject);

		return newObject;
	}

} // namespace APICore

#endif