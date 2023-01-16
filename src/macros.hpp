#ifndef API_MACROS
#define API_MACROS

#define CastSharedPtr(type, data) (*((std::shared_ptr<type>*)(&data)))

#endif