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
#define DATA_PRIMITIVE_VALUES                           \
	D(null, void)                                       \
	D(string, std::string)                              \
	D(int32, int32_t)                                   \
	D(function, FunctionInternalType)                   \
	D(object, ObjectMap)                                \
	D(array, std::vector<std::shared_ptr<DataWrapper>>) \
	D(classType, ClassInternalType)                     \
	D(classInstance, ClassInstance)                     \
	D(unknown, void)

#define D(Primitive, Type) Primitive,
	/**
	 * @brief Supported types for external API integrations.
	 *
	 * @enum unknown - Internal type for when you don't know the actual type.
	 */
	enum DataPrimitive
	{
		DATA_PRIMITIVE_VALUES
	};
#undef D
}

#include "./data-primitive-classes/class-instance-type.hpp"
#include "./data-primitive-classes/class-internal-type.hpp"
#include "./data-primitive-classes/function-internal-type.hpp"
#include "./data-primitive-classes/object-map.hpp"

namespace APICore {
#define D(Primitive, Type)                                     \
	template <>                                                \
	struct data_primitive_definition<DataPrimitive::Primitive> \
	{                                                          \
		static const std::string name;                         \
	};                                                         \
	const std::string data_primitive_definition<DataPrimitive::Primitive>::name = #Primitive;

	template <DataPrimitive Primitive>
	struct data_primitive_definition
	{
		static const std::string name;
	};

	DATA_PRIMITIVE_VALUES

#undef D

#define D(Primitive, Type)                                   \
	template <>                                              \
	struct _data_primitive_to_type<DataPrimitive::Primitive> \
	{                                                        \
		using type = Type;                                   \
	};
	template <DataPrimitive Primitive>
	struct _data_primitive_to_type
	{
		using type = void;
	};

	DATA_PRIMITIVE_VALUES

#undef D

	template <DataPrimitive D>
	using data_primitive_to_type = typename _data_primitive_to_type<D>::type;



#define D(Primitive, Type)                                   \
	{ DataPrimitive::Primitive, data_primitive_definition<DataPrimitive::Primitive>::name },

	std::map<DataPrimitive, std::string> dataPrimitiveNameMap = {
		DATA_PRIMITIVE_VALUES
	};

#undef D

	template <DataPrimitive D>
	using data_primitive_to_type = typename _data_primitive_to_type<D>::type;


} // namespace APICore
#endif