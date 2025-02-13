// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from msg_folder:msg/UwbMsg.idl
// generated code does not contain a copyright notice

#ifndef MSG_FOLDER__MSG__DETAIL__UWB_MSG__STRUCT_HPP_
#define MSG_FOLDER__MSG__DETAIL__UWB_MSG__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__msg_folder__msg__UwbMsg __attribute__((deprecated))
#else
# define DEPRECATED__msg_folder__msg__UwbMsg __declspec(deprecated)
#endif

namespace msg_folder
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct UwbMsg_
{
  using Type = UwbMsg_<ContainerAllocator>;

  explicit UwbMsg_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->range = 0.0f;
      this->power = 0.0f;
      this->from_address = "";
      this->anchor_key_x = 0;
      this->anchor_key_y = 0;
    }
  }

  explicit UwbMsg_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : from_address(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->range = 0.0f;
      this->power = 0.0f;
      this->from_address = "";
      this->anchor_key_x = 0;
      this->anchor_key_y = 0;
    }
  }

  // field types and members
  using _range_type =
    float;
  _range_type range;
  using _power_type =
    float;
  _power_type power;
  using _from_address_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _from_address_type from_address;
  using _anchor_key_x_type =
    uint16_t;
  _anchor_key_x_type anchor_key_x;
  using _anchor_key_y_type =
    uint16_t;
  _anchor_key_y_type anchor_key_y;

  // setters for named parameter idiom
  Type & set__range(
    const float & _arg)
  {
    this->range = _arg;
    return *this;
  }
  Type & set__power(
    const float & _arg)
  {
    this->power = _arg;
    return *this;
  }
  Type & set__from_address(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->from_address = _arg;
    return *this;
  }
  Type & set__anchor_key_x(
    const uint16_t & _arg)
  {
    this->anchor_key_x = _arg;
    return *this;
  }
  Type & set__anchor_key_y(
    const uint16_t & _arg)
  {
    this->anchor_key_y = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    msg_folder::msg::UwbMsg_<ContainerAllocator> *;
  using ConstRawPtr =
    const msg_folder::msg::UwbMsg_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<msg_folder::msg::UwbMsg_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<msg_folder::msg::UwbMsg_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      msg_folder::msg::UwbMsg_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<msg_folder::msg::UwbMsg_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      msg_folder::msg::UwbMsg_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<msg_folder::msg::UwbMsg_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<msg_folder::msg::UwbMsg_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<msg_folder::msg::UwbMsg_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__msg_folder__msg__UwbMsg
    std::shared_ptr<msg_folder::msg::UwbMsg_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__msg_folder__msg__UwbMsg
    std::shared_ptr<msg_folder::msg::UwbMsg_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const UwbMsg_ & other) const
  {
    if (this->range != other.range) {
      return false;
    }
    if (this->power != other.power) {
      return false;
    }
    if (this->from_address != other.from_address) {
      return false;
    }
    if (this->anchor_key_x != other.anchor_key_x) {
      return false;
    }
    if (this->anchor_key_y != other.anchor_key_y) {
      return false;
    }
    return true;
  }
  bool operator!=(const UwbMsg_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct UwbMsg_

// alias to use template instance with default allocator
using UwbMsg =
  msg_folder::msg::UwbMsg_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace msg_folder

#endif  // MSG_FOLDER__MSG__DETAIL__UWB_MSG__STRUCT_HPP_
