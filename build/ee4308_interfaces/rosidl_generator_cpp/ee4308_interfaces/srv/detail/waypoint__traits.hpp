// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from ee4308_interfaces:srv/Waypoint.idl
// generated code does not contain a copyright notice

#ifndef EE4308_INTERFACES__SRV__DETAIL__WAYPOINT__TRAITS_HPP_
#define EE4308_INTERFACES__SRV__DETAIL__WAYPOINT__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "ee4308_interfaces/srv/detail/waypoint__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'waypoint'
#include "geometry_msgs/msg/detail/point__traits.hpp"

namespace ee4308_interfaces
{

namespace srv
{

inline void to_flow_style_yaml(
  const Waypoint_Request & msg,
  std::ostream & out)
{
  out << "{";
  // member: waypoint
  {
    out << "waypoint: ";
    to_flow_style_yaml(msg.waypoint, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Waypoint_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: waypoint
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "waypoint:\n";
    to_block_style_yaml(msg.waypoint, out, indentation + 2);
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Waypoint_Request & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace ee4308_interfaces

namespace rosidl_generator_traits
{

[[deprecated("use ee4308_interfaces::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const ee4308_interfaces::srv::Waypoint_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  ee4308_interfaces::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use ee4308_interfaces::srv::to_yaml() instead")]]
inline std::string to_yaml(const ee4308_interfaces::srv::Waypoint_Request & msg)
{
  return ee4308_interfaces::srv::to_yaml(msg);
}

template<>
inline const char * data_type<ee4308_interfaces::srv::Waypoint_Request>()
{
  return "ee4308_interfaces::srv::Waypoint_Request";
}

template<>
inline const char * name<ee4308_interfaces::srv::Waypoint_Request>()
{
  return "ee4308_interfaces/srv/Waypoint_Request";
}

template<>
struct has_fixed_size<ee4308_interfaces::srv::Waypoint_Request>
  : std::integral_constant<bool, has_fixed_size<geometry_msgs::msg::Point>::value> {};

template<>
struct has_bounded_size<ee4308_interfaces::srv::Waypoint_Request>
  : std::integral_constant<bool, has_bounded_size<geometry_msgs::msg::Point>::value> {};

template<>
struct is_message<ee4308_interfaces::srv::Waypoint_Request>
  : std::true_type {};

}  // namespace rosidl_generator_traits

// Include directives for member types
// Member 'response'
#include "std_msgs/msg/detail/empty__traits.hpp"

namespace ee4308_interfaces
{

namespace srv
{

inline void to_flow_style_yaml(
  const Waypoint_Response & msg,
  std::ostream & out)
{
  out << "{";
  // member: response
  {
    out << "response: ";
    to_flow_style_yaml(msg.response, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Waypoint_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: response
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "response:\n";
    to_block_style_yaml(msg.response, out, indentation + 2);
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Waypoint_Response & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace ee4308_interfaces

namespace rosidl_generator_traits
{

[[deprecated("use ee4308_interfaces::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const ee4308_interfaces::srv::Waypoint_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  ee4308_interfaces::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use ee4308_interfaces::srv::to_yaml() instead")]]
inline std::string to_yaml(const ee4308_interfaces::srv::Waypoint_Response & msg)
{
  return ee4308_interfaces::srv::to_yaml(msg);
}

template<>
inline const char * data_type<ee4308_interfaces::srv::Waypoint_Response>()
{
  return "ee4308_interfaces::srv::Waypoint_Response";
}

template<>
inline const char * name<ee4308_interfaces::srv::Waypoint_Response>()
{
  return "ee4308_interfaces/srv/Waypoint_Response";
}

template<>
struct has_fixed_size<ee4308_interfaces::srv::Waypoint_Response>
  : std::integral_constant<bool, has_fixed_size<std_msgs::msg::Empty>::value> {};

template<>
struct has_bounded_size<ee4308_interfaces::srv::Waypoint_Response>
  : std::integral_constant<bool, has_bounded_size<std_msgs::msg::Empty>::value> {};

template<>
struct is_message<ee4308_interfaces::srv::Waypoint_Response>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace rosidl_generator_traits
{

template<>
inline const char * data_type<ee4308_interfaces::srv::Waypoint>()
{
  return "ee4308_interfaces::srv::Waypoint";
}

template<>
inline const char * name<ee4308_interfaces::srv::Waypoint>()
{
  return "ee4308_interfaces/srv/Waypoint";
}

template<>
struct has_fixed_size<ee4308_interfaces::srv::Waypoint>
  : std::integral_constant<
    bool,
    has_fixed_size<ee4308_interfaces::srv::Waypoint_Request>::value &&
    has_fixed_size<ee4308_interfaces::srv::Waypoint_Response>::value
  >
{
};

template<>
struct has_bounded_size<ee4308_interfaces::srv::Waypoint>
  : std::integral_constant<
    bool,
    has_bounded_size<ee4308_interfaces::srv::Waypoint_Request>::value &&
    has_bounded_size<ee4308_interfaces::srv::Waypoint_Response>::value
  >
{
};

template<>
struct is_service<ee4308_interfaces::srv::Waypoint>
  : std::true_type
{
};

template<>
struct is_service_request<ee4308_interfaces::srv::Waypoint_Request>
  : std::true_type
{
};

template<>
struct is_service_response<ee4308_interfaces::srv::Waypoint_Response>
  : std::true_type
{
};

}  // namespace rosidl_generator_traits

#endif  // EE4308_INTERFACES__SRV__DETAIL__WAYPOINT__TRAITS_HPP_
