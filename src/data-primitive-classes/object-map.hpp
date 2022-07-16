#ifndef DATA_WRAPPER_OBJECT_MAP
#define DATA_WRAPPER_OBJECT_MAP
#include <string>
#include <memory>
#include <map>

namespace APICore
{
	class DataWrapper;
    using ObjectMap = std::map<std::string, std::shared_ptr<DataWrapper>>;
}

#endif