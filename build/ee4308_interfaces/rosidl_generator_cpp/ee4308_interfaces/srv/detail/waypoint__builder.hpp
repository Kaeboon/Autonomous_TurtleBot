// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from ee4308_interfaces:srv/Waypoint.idl
// generated code does not contain a copyright notice

#ifndef EE4308_INTERFACES__SRV__DETAIL__WAYPOINT__BUILDER_HPP_
#define EE4308_INTERFACES__SRV__DETAIL__WAYPOINT__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "ee4308_interfaces/srv/detail/waypoint__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace ee4308_interfaces
{

namespace srv
{

namespace builder
{

class Init_Waypoint_Request_waypoint
{
public:
  Init_Waypoint_Request_waypoint()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::ee4308_interfaces::srv::Waypoint_Request waypoint(::ee4308_interfaces::srv::Waypoint_Request::_waypoint_type arg)
  {
    msg_.waypoint = std::move(arg);
    return std::move(msg_);
  }

private:
  ::ee4308_interfaces::srv::Waypoint_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::ee4308_interfaces::srv::Waypoint_Request>()
{
  return ee4308_interfaces::srv::builder::Init_Waypoint_Request_waypoint();
}

}  // namespace ee4308_interfaces


namespace ee4308_interfaces
{

namespace srv
{

namespace builder
{

class Init_Waypoint_Response_response
{
public:
  Init_Waypoint_Response_response()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::ee4308_interfaces::srv::Waypoint_Response response(::ee4308_interfaces::srv::Waypoint_Response::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::ee4308_interfaces::srv::Waypoint_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::ee4308_interfaces::srv::Waypoint_Response>()
{
  return ee4308_interfaces::srv::builder::Init_Waypoint_Response_response();
}

}  // namespace ee4308_interfaces

#endif  // EE4308_INTERFACES__SRV__DETAIL__WAYPOINT__BUILDER_HPP_
