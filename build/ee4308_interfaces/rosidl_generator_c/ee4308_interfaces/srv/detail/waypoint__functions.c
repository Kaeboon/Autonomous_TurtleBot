// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from ee4308_interfaces:srv/Waypoint.idl
// generated code does not contain a copyright notice
#include "ee4308_interfaces/srv/detail/waypoint__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"

// Include directives for member types
// Member `waypoint`
#include "geometry_msgs/msg/detail/point__functions.h"

bool
ee4308_interfaces__srv__Waypoint_Request__init(ee4308_interfaces__srv__Waypoint_Request * msg)
{
  if (!msg) {
    return false;
  }
  // waypoint
  if (!geometry_msgs__msg__Point__init(&msg->waypoint)) {
    ee4308_interfaces__srv__Waypoint_Request__fini(msg);
    return false;
  }
  return true;
}

void
ee4308_interfaces__srv__Waypoint_Request__fini(ee4308_interfaces__srv__Waypoint_Request * msg)
{
  if (!msg) {
    return;
  }
  // waypoint
  geometry_msgs__msg__Point__fini(&msg->waypoint);
}

bool
ee4308_interfaces__srv__Waypoint_Request__are_equal(const ee4308_interfaces__srv__Waypoint_Request * lhs, const ee4308_interfaces__srv__Waypoint_Request * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // waypoint
  if (!geometry_msgs__msg__Point__are_equal(
      &(lhs->waypoint), &(rhs->waypoint)))
  {
    return false;
  }
  return true;
}

bool
ee4308_interfaces__srv__Waypoint_Request__copy(
  const ee4308_interfaces__srv__Waypoint_Request * input,
  ee4308_interfaces__srv__Waypoint_Request * output)
{
  if (!input || !output) {
    return false;
  }
  // waypoint
  if (!geometry_msgs__msg__Point__copy(
      &(input->waypoint), &(output->waypoint)))
  {
    return false;
  }
  return true;
}

ee4308_interfaces__srv__Waypoint_Request *
ee4308_interfaces__srv__Waypoint_Request__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  ee4308_interfaces__srv__Waypoint_Request * msg = (ee4308_interfaces__srv__Waypoint_Request *)allocator.allocate(sizeof(ee4308_interfaces__srv__Waypoint_Request), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(ee4308_interfaces__srv__Waypoint_Request));
  bool success = ee4308_interfaces__srv__Waypoint_Request__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
ee4308_interfaces__srv__Waypoint_Request__destroy(ee4308_interfaces__srv__Waypoint_Request * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    ee4308_interfaces__srv__Waypoint_Request__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
ee4308_interfaces__srv__Waypoint_Request__Sequence__init(ee4308_interfaces__srv__Waypoint_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  ee4308_interfaces__srv__Waypoint_Request * data = NULL;

  if (size) {
    data = (ee4308_interfaces__srv__Waypoint_Request *)allocator.zero_allocate(size, sizeof(ee4308_interfaces__srv__Waypoint_Request), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = ee4308_interfaces__srv__Waypoint_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        ee4308_interfaces__srv__Waypoint_Request__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
ee4308_interfaces__srv__Waypoint_Request__Sequence__fini(ee4308_interfaces__srv__Waypoint_Request__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      ee4308_interfaces__srv__Waypoint_Request__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

ee4308_interfaces__srv__Waypoint_Request__Sequence *
ee4308_interfaces__srv__Waypoint_Request__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  ee4308_interfaces__srv__Waypoint_Request__Sequence * array = (ee4308_interfaces__srv__Waypoint_Request__Sequence *)allocator.allocate(sizeof(ee4308_interfaces__srv__Waypoint_Request__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = ee4308_interfaces__srv__Waypoint_Request__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
ee4308_interfaces__srv__Waypoint_Request__Sequence__destroy(ee4308_interfaces__srv__Waypoint_Request__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    ee4308_interfaces__srv__Waypoint_Request__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
ee4308_interfaces__srv__Waypoint_Request__Sequence__are_equal(const ee4308_interfaces__srv__Waypoint_Request__Sequence * lhs, const ee4308_interfaces__srv__Waypoint_Request__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!ee4308_interfaces__srv__Waypoint_Request__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
ee4308_interfaces__srv__Waypoint_Request__Sequence__copy(
  const ee4308_interfaces__srv__Waypoint_Request__Sequence * input,
  ee4308_interfaces__srv__Waypoint_Request__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(ee4308_interfaces__srv__Waypoint_Request);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    ee4308_interfaces__srv__Waypoint_Request * data =
      (ee4308_interfaces__srv__Waypoint_Request *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!ee4308_interfaces__srv__Waypoint_Request__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          ee4308_interfaces__srv__Waypoint_Request__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!ee4308_interfaces__srv__Waypoint_Request__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `response`
#include "std_msgs/msg/detail/empty__functions.h"

bool
ee4308_interfaces__srv__Waypoint_Response__init(ee4308_interfaces__srv__Waypoint_Response * msg)
{
  if (!msg) {
    return false;
  }
  // response
  if (!std_msgs__msg__Empty__init(&msg->response)) {
    ee4308_interfaces__srv__Waypoint_Response__fini(msg);
    return false;
  }
  return true;
}

void
ee4308_interfaces__srv__Waypoint_Response__fini(ee4308_interfaces__srv__Waypoint_Response * msg)
{
  if (!msg) {
    return;
  }
  // response
  std_msgs__msg__Empty__fini(&msg->response);
}

bool
ee4308_interfaces__srv__Waypoint_Response__are_equal(const ee4308_interfaces__srv__Waypoint_Response * lhs, const ee4308_interfaces__srv__Waypoint_Response * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // response
  if (!std_msgs__msg__Empty__are_equal(
      &(lhs->response), &(rhs->response)))
  {
    return false;
  }
  return true;
}

bool
ee4308_interfaces__srv__Waypoint_Response__copy(
  const ee4308_interfaces__srv__Waypoint_Response * input,
  ee4308_interfaces__srv__Waypoint_Response * output)
{
  if (!input || !output) {
    return false;
  }
  // response
  if (!std_msgs__msg__Empty__copy(
      &(input->response), &(output->response)))
  {
    return false;
  }
  return true;
}

ee4308_interfaces__srv__Waypoint_Response *
ee4308_interfaces__srv__Waypoint_Response__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  ee4308_interfaces__srv__Waypoint_Response * msg = (ee4308_interfaces__srv__Waypoint_Response *)allocator.allocate(sizeof(ee4308_interfaces__srv__Waypoint_Response), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(ee4308_interfaces__srv__Waypoint_Response));
  bool success = ee4308_interfaces__srv__Waypoint_Response__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
ee4308_interfaces__srv__Waypoint_Response__destroy(ee4308_interfaces__srv__Waypoint_Response * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    ee4308_interfaces__srv__Waypoint_Response__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
ee4308_interfaces__srv__Waypoint_Response__Sequence__init(ee4308_interfaces__srv__Waypoint_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  ee4308_interfaces__srv__Waypoint_Response * data = NULL;

  if (size) {
    data = (ee4308_interfaces__srv__Waypoint_Response *)allocator.zero_allocate(size, sizeof(ee4308_interfaces__srv__Waypoint_Response), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = ee4308_interfaces__srv__Waypoint_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        ee4308_interfaces__srv__Waypoint_Response__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
ee4308_interfaces__srv__Waypoint_Response__Sequence__fini(ee4308_interfaces__srv__Waypoint_Response__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      ee4308_interfaces__srv__Waypoint_Response__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

ee4308_interfaces__srv__Waypoint_Response__Sequence *
ee4308_interfaces__srv__Waypoint_Response__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  ee4308_interfaces__srv__Waypoint_Response__Sequence * array = (ee4308_interfaces__srv__Waypoint_Response__Sequence *)allocator.allocate(sizeof(ee4308_interfaces__srv__Waypoint_Response__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = ee4308_interfaces__srv__Waypoint_Response__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
ee4308_interfaces__srv__Waypoint_Response__Sequence__destroy(ee4308_interfaces__srv__Waypoint_Response__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    ee4308_interfaces__srv__Waypoint_Response__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
ee4308_interfaces__srv__Waypoint_Response__Sequence__are_equal(const ee4308_interfaces__srv__Waypoint_Response__Sequence * lhs, const ee4308_interfaces__srv__Waypoint_Response__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!ee4308_interfaces__srv__Waypoint_Response__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
ee4308_interfaces__srv__Waypoint_Response__Sequence__copy(
  const ee4308_interfaces__srv__Waypoint_Response__Sequence * input,
  ee4308_interfaces__srv__Waypoint_Response__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(ee4308_interfaces__srv__Waypoint_Response);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    ee4308_interfaces__srv__Waypoint_Response * data =
      (ee4308_interfaces__srv__Waypoint_Response *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!ee4308_interfaces__srv__Waypoint_Response__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          ee4308_interfaces__srv__Waypoint_Response__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!ee4308_interfaces__srv__Waypoint_Response__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
