#ifndef DATA_PRIMITIVE
#define DATA_PRIMITIVE

#include <memory>
#include <functional>
#include <type_traits>
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace APICore {
	/**
	 * @brief Supported types for external API integrations.
	 * 
	 * @enum unknown - Internal type for when you don't know the actual type.
	 */
	enum DataPrimitive { null, string, int32, function, object, array, classType, unknown };	
	class DataWrapper;
    using ObjectMap = std::map<std::string, std::shared_ptr<DataWrapper>>;

	#define CreateDataType(Primitive, Type) template <> struct _data_primitive_to_type<Primitive> { using type = Type; }
	template<DataPrimitive D> struct _data_primitive_to_type { using type = void; };
	CreateDataType(DataPrimitive::string, std::string);
	CreateDataType(DataPrimitive::int32, int32_t);
	CreateDataType(DataPrimitive::array, std::vector<std::shared_ptr<void>>);
	CreateDataType(DataPrimitive::object, ObjectMap);
	CreateDataType(DataPrimitive::null, void);
	CreateDataType(DataPrimitive::unknown, void);
	// TODO: Flesh out the function and classType types
	// CreateDataType(DataPrimitive::function, void);
	// CreateDataType(DataPrimitive::classType, void);

	#undef CreateDataType
	
	template<DataPrimitive D>
	using data_primitive_to_type = typename _data_primitive_to_type<D>::type;

} // namespace APICore
#endif