#ifndef DATA_WRAPPER_CLASS_INSTANCE_TYPE
#define DATA_WRAPPER_CLASS_INSTANCE_TYPE
#include <string>
#include <memory>
#include <map>
#include "../data-primitive.hpp"

namespace APICore
{
    template<DataPrimitive Primitive>
    class DataWrapperSub;

    struct ClassInstance
	{
		std::shared_ptr<DataWrapperSub<DataPrimitive::classType>> classDef;
		std::shared_ptr<DataWrapperSub<DataPrimitive::object>> data;
	};
}

#endif