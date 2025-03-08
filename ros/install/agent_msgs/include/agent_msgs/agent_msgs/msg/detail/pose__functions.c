// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from agent_msgs:msg/Pose.idl
// generated code does not contain a copyright notice
#include "agent_msgs/msg/detail/pose__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `pos`
#include "geometry_msgs/msg/detail/point__functions.h"
// Member `tag`
#include "rosidl_runtime_c/string_functions.h"

bool
agent_msgs__msg__Pose__init(agent_msgs__msg__Pose * msg)
{
  if (!msg) {
    return false;
  }
  // pos
  if (!geometry_msgs__msg__Point__init(&msg->pos)) {
    agent_msgs__msg__Pose__fini(msg);
    return false;
  }
  // tag
  if (!rosidl_runtime_c__String__init(&msg->tag)) {
    agent_msgs__msg__Pose__fini(msg);
    return false;
  }
  return true;
}

void
agent_msgs__msg__Pose__fini(agent_msgs__msg__Pose * msg)
{
  if (!msg) {
    return;
  }
  // pos
  geometry_msgs__msg__Point__fini(&msg->pos);
  // tag
  rosidl_runtime_c__String__fini(&msg->tag);
}

bool
agent_msgs__msg__Pose__are_equal(const agent_msgs__msg__Pose * lhs, const agent_msgs__msg__Pose * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // pos
  if (!geometry_msgs__msg__Point__are_equal(
      &(lhs->pos), &(rhs->pos)))
  {
    return false;
  }
  // tag
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->tag), &(rhs->tag)))
  {
    return false;
  }
  return true;
}

bool
agent_msgs__msg__Pose__copy(
  const agent_msgs__msg__Pose * input,
  agent_msgs__msg__Pose * output)
{
  if (!input || !output) {
    return false;
  }
  // pos
  if (!geometry_msgs__msg__Point__copy(
      &(input->pos), &(output->pos)))
  {
    return false;
  }
  // tag
  if (!rosidl_runtime_c__String__copy(
      &(input->tag), &(output->tag)))
  {
    return false;
  }
  return true;
}

agent_msgs__msg__Pose *
agent_msgs__msg__Pose__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  agent_msgs__msg__Pose * msg = (agent_msgs__msg__Pose *)allocator.allocate(sizeof(agent_msgs__msg__Pose), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(agent_msgs__msg__Pose));
  bool success = agent_msgs__msg__Pose__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
agent_msgs__msg__Pose__destroy(agent_msgs__msg__Pose * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    agent_msgs__msg__Pose__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
agent_msgs__msg__Pose__Sequence__init(agent_msgs__msg__Pose__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  agent_msgs__msg__Pose * data = NULL;

  if (size) {
    data = (agent_msgs__msg__Pose *)allocator.zero_allocate(size, sizeof(agent_msgs__msg__Pose), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = agent_msgs__msg__Pose__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        agent_msgs__msg__Pose__fini(&data[i - 1]);
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
agent_msgs__msg__Pose__Sequence__fini(agent_msgs__msg__Pose__Sequence * array)
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
      agent_msgs__msg__Pose__fini(&array->data[i]);
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

agent_msgs__msg__Pose__Sequence *
agent_msgs__msg__Pose__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  agent_msgs__msg__Pose__Sequence * array = (agent_msgs__msg__Pose__Sequence *)allocator.allocate(sizeof(agent_msgs__msg__Pose__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = agent_msgs__msg__Pose__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
agent_msgs__msg__Pose__Sequence__destroy(agent_msgs__msg__Pose__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    agent_msgs__msg__Pose__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
agent_msgs__msg__Pose__Sequence__are_equal(const agent_msgs__msg__Pose__Sequence * lhs, const agent_msgs__msg__Pose__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!agent_msgs__msg__Pose__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
agent_msgs__msg__Pose__Sequence__copy(
  const agent_msgs__msg__Pose__Sequence * input,
  agent_msgs__msg__Pose__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(agent_msgs__msg__Pose);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    agent_msgs__msg__Pose * data =
      (agent_msgs__msg__Pose *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!agent_msgs__msg__Pose__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          agent_msgs__msg__Pose__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!agent_msgs__msg__Pose__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
