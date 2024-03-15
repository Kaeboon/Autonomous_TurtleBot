// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from ee4308_interfaces:srv/Waypoint.idl
// generated code does not contain a copyright notice

#ifndef EE4308_INTERFACES__SRV__DETAIL__WAYPOINT__STRUCT_H_
#define EE4308_INTERFACES__SRV__DETAIL__WAYPOINT__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'waypoint'
#include "geometry_msgs/msg/detail/point__struct.h"

/// Struct defined in srv/Waypoint in the package ee4308_interfaces.
typedef struct ee4308_interfaces__srv__Waypoint_Request
{
  geometry_msgs__msg__Point waypoint;
} ee4308_interfaces__srv__Waypoint_Request;

// Struct for a sequence of ee4308_interfaces__srv__Waypoint_Request.
typedef struct ee4308_interfaces__srv__Waypoint_Request__Sequence
{
  ee4308_interfaces__srv__Waypoint_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} ee4308_interfaces__srv__Waypoint_Request__Sequence;


// Constants defined in the message

// Include directives for member types
// Member 'response'
#include "std_msgs/msg/detail/empty__struct.h"

/// Struct defined in srv/Waypoint in the package ee4308_interfaces.
typedef struct ee4308_interfaces__srv__Waypoint_Response
{
  std_msgs__msg__Empty response;
} ee4308_interfaces__srv__Waypoint_Response;

// Struct for a sequence of ee4308_interfaces__srv__Waypoint_Response.
typedef struct ee4308_interfaces__srv__Waypoint_Response__Sequence
{
  ee4308_interfaces__srv__Waypoint_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} ee4308_interfaces__srv__Waypoint_Response__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // EE4308_INTERFACES__SRV__DETAIL__WAYPOINT__STRUCT_H_
