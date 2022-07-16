#ifndef DATA_WRAPPER_CLASS_INTERNAL_TYPE
#define DATA_WRAPPER_CLASS_INTERNAL_TYPE
#include <string>
#include <memory>
#include <functional>
#include "./object-map.hpp"
#include "./function-internal-type.hpp"

namespace APICore {
    struct ClassInternalType
	{
		struct ExternalSource
		{
			std::shared_ptr<void> classDefinition;
			std::function<std::shared_ptr<void>(std::shared_ptr<ObjectMap>)> bindAs;
		};

		std::string className;
		std::function<ObjectMap(FunctionInternalType::FunctionParams)> constructor;
		std::map<std::string, std::shared_ptr<DataWrapper>> staticFields;
		/**
		 * @brief
		 * Optional source of class created by external APIs.
		 * For example, this could be the original Lua class object (using TypescriptToLua).
		 */
		std::shared_ptr<ExternalSource> externalSource;
		std::string externalSourceKeyName;

		~ClassInternalType()
		{
		}
	};
}

#endif