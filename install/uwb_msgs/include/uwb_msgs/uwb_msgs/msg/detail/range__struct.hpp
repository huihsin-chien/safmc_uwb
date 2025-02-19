// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from uwb_msgs:msg/Range.idl
// generated code does not contain a copyright notice

#ifndef UWB_MSGS__MSG__DETAIL__RANGE__STRUCT_HPP_
#define UWB_MSGS__MSG__DETAIL__RANGE__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__uwb_msgs__msg__Range __attribute__((deprecated))
#else
# define DEPRECATED__uwb_msgs__msg__Range __declspec(deprecated)
#endif

namespace uwb_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct Range_
{
  using Type = Range_<ContainerAllocator>;

  explicit Range_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->range = 0.0;
      this->power = 0.0;
      this->from_id = "";
      this->to_id = "";
    }
  }

  explicit Range_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : from_id(_alloc),
    to_id(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->range = 0.0;
      this->power = 0.0;
      this->from_id = "";
      this->to_id = "";
    }
  }

  // field types and members
  using _range_type =
    double;
  _range_type range;
  using _power_type =
    double;
  _power_type power;
  using _from_id_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _from_id_type from_id;
  using _to_id_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _to_id_type to_id;

  // setters for named parameter idiom
  Type & set__range(
    const double & _arg)
  {
    this->range = _arg;
    return *this;
  }
  Type & set__power(
    const double & _arg)
  {
    this->power = _arg;
    return *this;
  }
  Type & set__from_id(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->from_id = _arg;
    return *this;
  }
  Type & set__to_id(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->to_id = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    uwb_msgs::msg::Range_<ContainerAllocator> *;
  using ConstRawPtr =
    const uwb_msgs::msg::Range_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<uwb_msgs::msg::Range_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<uwb_msgs::msg::Range_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      uwb_msgs::msg::Range_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<uwb_msgs::msg::Range_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      uwb_msgs::msg::Range_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<uwb_msgs::msg::Range_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<uwb_msgs::msg::Range_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<uwb_msgs::msg::Range_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__uwb_msgs__msg__Range
    std::shared_ptr<uwb_msgs::msg::Range_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__uwb_msgs__msg__Range
    std::shared_ptr<uwb_msgs::msg::Range_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Range_ & other) const
  {
    if (this->range != other.range) {
      return false;
    }
    if (this->power != other.power) {
      return false;
    }
    if (this->from_id != other.from_id) {
      return false;
    }
    if (this->to_id != other.to_id) {
      return false;
    }
    return true;
  }
  bool operator!=(const Range_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Range_

// alias to use template instance with default allocator
using Range =
  uwb_msgs::msg::Range_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace uwb_msgs

#endif  // UWB_MSGS__MSG__DETAIL__RANGE__STRUCT_HPP_
