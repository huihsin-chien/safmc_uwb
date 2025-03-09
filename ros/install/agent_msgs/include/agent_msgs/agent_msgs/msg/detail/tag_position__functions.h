// generated from rosidl_generator_c/resource/idl__functions.h.em
// with input from agent_msgs:msg/TagPosition.idl
// generated code does not contain a copyright notice

#ifndef AGENT_MSGS__MSG__DETAIL__TAG_POSITION__FUNCTIONS_H_
#define AGENT_MSGS__MSG__DETAIL__TAG_POSITION__FUNCTIONS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "rosidl_runtime_c/visibility_control.h"
#include "agent_msgs/msg/rosidl_generator_c__visibility_control.h"

#include "agent_msgs/msg/detail/tag_position__struct.h"

/// Initialize msg/TagPosition message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * agent_msgs__msg__TagPosition
 * )) before or use
 * agent_msgs__msg__TagPosition__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_agent_msgs
bool
agent_msgs__msg__TagPosition__init(agent_msgs__msg__TagPosition * msg);

/// Finalize msg/TagPosition message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_agent_msgs
void
agent_msgs__msg__TagPosition__fini(agent_msgs__msg__TagPosition * msg);

/// Create msg/TagPosition message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * agent_msgs__msg__TagPosition__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_agent_msgs
agent_msgs__msg__TagPosition *
agent_msgs__msg__TagPosition__create();

/// Destroy msg/TagPosition message.
/**
 * It calls
 * agent_msgs__msg__TagPosition__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_agent_msgs
void
agent_msgs__msg__TagPosition__destroy(agent_msgs__msg__TagPosition * msg);

/// Check for msg/TagPosition message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_agent_msgs
bool
agent_msgs__msg__TagPosition__are_equal(const agent_msgs__msg__TagPosition * lhs, const agent_msgs__msg__TagPosition * rhs);

/// Copy a msg/TagPosition message.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source message pointer.
 * \param[out] output The target message pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer is null
 *   or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_agent_msgs
bool
agent_msgs__msg__TagPosition__copy(
  const agent_msgs__msg__TagPosition * input,
  agent_msgs__msg__TagPosition * output);

/// Initialize array of msg/TagPosition messages.
/**
 * It allocates the memory for the number of elements and calls
 * agent_msgs__msg__TagPosition__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_agent_msgs
bool
agent_msgs__msg__TagPosition__Sequence__init(agent_msgs__msg__TagPosition__Sequence * array, size_t size);

/// Finalize array of msg/TagPosition messages.
/**
 * It calls
 * agent_msgs__msg__TagPosition__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_agent_msgs
void
agent_msgs__msg__TagPosition__Sequence__fini(agent_msgs__msg__TagPosition__Sequence * array);

/// Create array of msg/TagPosition messages.
/**
 * It allocates the memory for the array and calls
 * agent_msgs__msg__TagPosition__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_agent_msgs
agent_msgs__msg__TagPosition__Sequence *
agent_msgs__msg__TagPosition__Sequence__create(size_t size);

/// Destroy array of msg/TagPosition messages.
/**
 * It calls
 * agent_msgs__msg__TagPosition__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_agent_msgs
void
agent_msgs__msg__TagPosition__Sequence__destroy(agent_msgs__msg__TagPosition__Sequence * array);

/// Check for msg/TagPosition message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_agent_msgs
bool
agent_msgs__msg__TagPosition__Sequence__are_equal(const agent_msgs__msg__TagPosition__Sequence * lhs, const agent_msgs__msg__TagPosition__Sequence * rhs);

/// Copy an array of msg/TagPosition messages.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source array pointer.
 * \param[out] output The target array pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer
 *   is null or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_agent_msgs
bool
agent_msgs__msg__TagPosition__Sequence__copy(
  const agent_msgs__msg__TagPosition__Sequence * input,
  agent_msgs__msg__TagPosition__Sequence * output);

#ifdef __cplusplus
}
#endif

#endif  // AGENT_MSGS__MSG__DETAIL__TAG_POSITION__FUNCTIONS_H_
