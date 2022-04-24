#ifndef API_MACROS
#define API_MACROS

#define APIFuncParams shared_ptr<void> thisPtr, vector<shared_ptr<void>> &parameters
#define CastSharedPtr(type, data) (*((std::shared_ptr<type>*)(&data)))

#endif