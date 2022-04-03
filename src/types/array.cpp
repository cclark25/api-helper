#ifndef API_ARRAY
#define API_ARRAY

#include "../api-object.h"

namespace APICore {
	const ArrayTyping APIObject::getArrayType() const {
		this->assertType(DataPrimitive::array);
		return this->arrayType;
	}

    std::shared_ptr<APIObject> APIObject::makeArray(std::string name, std::string description, ArrayTyping arrayType){
        std::shared_ptr<APIObject> newObject = std::shared_ptr<APIObject>(new APIObject(name, description));
		newObject->type = DataPrimitive::array;
        newObject->arrayType = arrayType;
        return newObject;
    }

} // namespace APICore

#endif