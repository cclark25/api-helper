#pragma once

#define CastSharedPtr(type, data) (*((std::shared_ptr<type>*)(&data)))
