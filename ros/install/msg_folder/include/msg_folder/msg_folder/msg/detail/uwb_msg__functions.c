// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from msg_folder:msg/UwbMsg.idl
// generated code does not contain a copyright notice
#include "msg_folder/msg/detail/uwb_msg__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `from_address`
#include "rosidl_runtime_c/string_functions.h"

bool
msg_folder__msg__UwbMsg__init(msg_folder__msg__UwbMsg * msg)
{
  if (!msg) {
    return false;
  }
  // range
  // power
  // from_address
  if (!rosidl_runtime_c__String__init(&msg->from_address)) {
    msg_folder__msg__UwbMsg__fini(msg);
    return false;
  }
  // anchor_key_x
  // anchor_key_y
  return true;
}

void
msg_folder__msg__UwbMsg__fini(msg_folder__msg__UwbMsg * msg)
{
  if (!msg) {
    return;
  }
  // range
  // power
  // from_address
  rosidl_runtime_c__String__fini(&msg->from_address);
  // anchor_key_x
  // anchor_key_y
}

bool
msg_folder__msg__UwbMsg__are_equal(const msg_folder__msg__UwbMsg * lhs, const msg_folder__msg__UwbMsg * rhs)
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
  // from_address
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->from_address), &(rhs->from_address)))
  {
    return false;
  }
  // anchor_key_x
  if (lhs->anchor_key_x != rhs->anchor_key_x) {
    return false;
  }
  // anchor_key_y
  if (lhs->anchor_key_y != rhs->anchor_key_y) {
    return false;
  }
  return true;
}

bool
msg_folder__msg__UwbMsg__copy(
  const msg_folder__msg__UwbMsg * input,
  msg_folder__msg__UwbMsg * output)
{
  if (!input || !output) {
    return false;
  }
  // range
  output->range = input->range;
  // power
  output->power = input->power;
  // from_address
  if (!rosidl_runtime_c__String__copy(
      &(input->from_address), &(output->from_address)))
  {
    return false;
  }
  // anchor_key_x
  output->anchor_key_x = input->anchor_key_x;
  // anchor_key_y
  output->anchor_key_y = input->anchor_key_y;
  return true;
}

msg_folder__msg__UwbMsg *
msg_folder__msg__UwbMsg__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  msg_folder__msg__UwbMsg * msg = (msg_folder__msg__UwbMsg *)allocator.allocate(sizeof(msg_folder__msg__UwbMsg), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(msg_folder__msg__UwbMsg));
  bool success = msg_folder__msg__UwbMsg__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
msg_folder__msg__UwbMsg__destroy(msg_folder__msg__UwbMsg * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    msg_folder__msg__UwbMsg__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
msg_folder__msg__UwbMsg__Sequence__init(msg_folder__msg__UwbMsg__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  msg_folder__msg__UwbMsg * data = NULL;

  if (size) {
    data = (msg_folder__msg__UwbMsg *)allocator.zero_allocate(size, sizeof(msg_folder__msg__UwbMsg), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = msg_folder__msg__UwbMsg__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        msg_folder__msg__UwbMsg__fini(&data[i - 1]);
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
msg_folder__msg__UwbMsg__Sequence__fini(msg_folder__msg__UwbMsg__Sequence * array)
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
      msg_folder__msg__UwbMsg__fini(&array->data[i]);
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

msg_folder__msg__UwbMsg__Sequence *
msg_folder__msg__UwbMsg__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  msg_folder__msg__UwbMsg__Sequence * array = (msg_folder__msg__UwbMsg__Sequence *)allocator.allocate(sizeof(msg_folder__msg__UwbMsg__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = msg_folder__msg__UwbMsg__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
msg_folder__msg__UwbMsg__Sequence__destroy(msg_folder__msg__UwbMsg__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    msg_folder__msg__UwbMsg__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
msg_folder__msg__UwbMsg__Sequence__are_equal(const msg_folder__msg__UwbMsg__Sequence * lhs, const msg_folder__msg__UwbMsg__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!msg_folder__msg__UwbMsg__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
msg_folder__msg__UwbMsg__Sequence__copy(
  const msg_folder__msg__UwbMsg__Sequence * input,
  msg_folder__msg__UwbMsg__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(msg_folder__msg__UwbMsg);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    msg_folder__msg__UwbMsg * data =
      (msg_folder__msg__UwbMsg *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!msg_folder__msg__UwbMsg__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          msg_folder__msg__UwbMsg__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!msg_folder__msg__UwbMsg__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
