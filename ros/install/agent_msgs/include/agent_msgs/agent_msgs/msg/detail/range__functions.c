// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from agent_msgs:msg/Range.idl
// generated code does not contain a copyright notice
#include "agent_msgs/msg/detail/range__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `from_id`
// Member `to_id`
#include "rosidl_runtime_c/string_functions.h"

bool
agent_msgs__msg__Range__init(agent_msgs__msg__Range * msg)
{
  if (!msg) {
    return false;
  }
  // range
  // power
  // from_id
  if (!rosidl_runtime_c__String__init(&msg->from_id)) {
    agent_msgs__msg__Range__fini(msg);
    return false;
  }
  // to_id
  if (!rosidl_runtime_c__String__init(&msg->to_id)) {
    agent_msgs__msg__Range__fini(msg);
    return false;
  }
  return true;
}

void
agent_msgs__msg__Range__fini(agent_msgs__msg__Range * msg)
{
  if (!msg) {
    return;
  }
  // range
  // power
  // from_id
  rosidl_runtime_c__String__fini(&msg->from_id);
  // to_id
  rosidl_runtime_c__String__fini(&msg->to_id);
}

bool
agent_msgs__msg__Range__are_equal(const agent_msgs__msg__Range * lhs, const agent_msgs__msg__Range * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // range
  if (lhs->range != rhs->range) {
    return false;
  }
  // power
  if (lhs->power != rhs->power) {
    return false;
  }
  // from_id
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->from_id), &(rhs->from_id)))
  {
    return false;
  }
  // to_id
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->to_id), &(rhs->to_id)))
  {
    return false;
  }
  return true;
}

bool
agent_msgs__msg__Range__copy(
  const agent_msgs__msg__Range * input,
  agent_msgs__msg__Range * output)
{
  if (!input || !output) {
    return false;
  }
  // range
  output->range = input->range;
  // power
  output->power = input->power;
  // from_id
  if (!rosidl_runtime_c__String__copy(
      &(input->from_id), &(output->from_id)))
  {
    return false;
  }
  // to_id
  if (!rosidl_runtime_c__String__copy(
      &(input->to_id), &(output->to_id)))
  {
    return false;
  }
  return true;
}

agent_msgs__msg__Range *
agent_msgs__msg__Range__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  agent_msgs__msg__Range * msg = (agent_msgs__msg__Range *)allocator.allocate(sizeof(agent_msgs__msg__Range), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(agent_msgs__msg__Range));
  bool success = agent_msgs__msg__Range__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
agent_msgs__msg__Range__destroy(agent_msgs__msg__Range * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    agent_msgs__msg__Range__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
agent_msgs__msg__Range__Sequence__init(agent_msgs__msg__Range__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  agent_msgs__msg__Range * data = NULL;

  if (size) {
    data = (agent_msgs__msg__Range *)allocator.zero_allocate(size, sizeof(agent_msgs__msg__Range), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = agent_msgs__msg__Range__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        agent_msgs__msg__Range__fini(&data[i - 1]);
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
agent_msgs__msg__Range__Sequence__fini(agent_msgs__msg__Range__Sequence * array)
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
      agent_msgs__msg__Range__fini(&array->data[i]);
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

agent_msgs__msg__Range__Sequence *
agent_msgs__msg__Range__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  agent_msgs__msg__Range__Sequence * array = (agent_msgs__msg__Range__Sequence *)allocator.allocate(sizeof(agent_msgs__msg__Range__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = agent_msgs__msg__Range__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
agent_msgs__msg__Range__Sequence__destroy(agent_msgs__msg__Range__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    agent_msgs__msg__Range__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
agent_msgs__msg__Range__Sequence__are_equal(const agent_msgs__msg__Range__Sequence * lhs, const agent_msgs__msg__Range__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!agent_msgs__msg__Range__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
agent_msgs__msg__Range__Sequence__copy(
  const agent_msgs__msg__Range__Sequence * input,
  agent_msgs__msg__Range__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(agent_msgs__msg__Range);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    agent_msgs__msg__Range * data =
      (agent_msgs__msg__Range *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!agent_msgs__msg__Range__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          agent_msgs__msg__Range__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!agent_msgs__msg__Range__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
