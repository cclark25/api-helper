#ifndef DATA_PRIMITIVE
#define DATA_PRIMITIVE

#include <memory>
#include <functional>
#include <type_traits>
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace APICore
{
	/**
	 * @brief Supported types for external API integrations.
	 *
	 * @enum unknown - Internal type for when you don't know the actual type.
	 */
	enum DataPrimitive
	{
		null,
		string,
		int32,
		function,
		object,
		array,
		classType,
		unknown
	};
	class DataWrapper;
	using ObjectMap = std::map<std::string, std::shared_ptr<DataWrapper>>;
	struct FunctionInternalType
	{
		using FunctionParams = std::vector<std::shared_ptr<DataWrapper>>;
		using APIFunction = std::function<std::shared_ptr<DataWrapper>(FunctionParams)>;
		APIFunction function;
		/**
		 * @brief
		 * Optional source of functions created by external APIs.
		 * For example, this could be the original Lua function used to create this function.
		 * This lua function can be passed back into Lua instead of adding the overhead
		 * of translating lua paramaters to internal types and then the return value
		 * to Lua.
		 */
		std::shared_ptr<void> externalSource;
		std::string externalSourceKeyName;

		FunctionInternalType(APIFunction function, std::string externalSourceKeyName = "", std::shared_ptr<void> externalSource = nullptr) : function(function), externalSourceKeyName(externalSourceKeyName), externalSource(externalSource) {}
	};

#define CreateDataType(Primitive, Type)       \
	template <>                               \
	struct _data_primitive_to_type<Primitive> \
	{                                         \
		using type = Type;                    \
	}
	template <DataPrimitive D>
	struct _data_primitive_to_type
	{
		using type = void;
	};
	CreateDataType(DataPrimitive::string, std::string);
	CreateDataType(DataPrimitive::int32, int32_t);
	CreateDataType(DataPrimitive::array, std::vector<std::shared_ptr<DataWrapper>>);
	CreateDataType(DataPrimitive::object, ObjectMap);
	CreateDataType(DataPrimitive::null, void);
	CreateDataType(DataPrimitive::unknown, void);
	CreateDataType(DataPrimitive::function, FunctionInternalType);
	// TODO: Flesh out the classType type
	// CreateDataType(DataPrimitive::classType, void);

#undef CreateDataType

	template <DataPrimitive D>
	using data_primitive_to_type = typename _data_primitive_to_type<D>::type;

} // namespace APICore
#endif