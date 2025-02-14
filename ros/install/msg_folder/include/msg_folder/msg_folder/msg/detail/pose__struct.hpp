// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from msg_folder:msg/Pose.idl
// generated code does not contain a copyright notice

#ifndef MSG_FOLDER__MSG__DETAIL__POSE__STRUCT_HPP_
#define MSG_FOLDER__MSG__DETAIL__POSE__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'pos'
#include "geometry_msgs/msg/detail/point__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__msg_folder__msg__Pose __attribute__((deprecated))
#else
# define DEPRECATED__msg_folder__msg__Pose __declspec(deprecated)
#endif

namespace msg_folder
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct Pose_
{
  using Type = Pose_<ContainerAllocator>;

  explicit Pose_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : pos(_init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->tag = "";
    }
  }

  explicit Pose_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : pos(_alloc, _init),
    tag(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->tag = "";
    }
  }

  // field types and members
  using _pos_type =
    geometry_msgs::msg::Point_<ContainerAllocator>;
  _pos_type pos;
  using _tag_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _tag_type tag;

  // setters for named parameter idiom
  Type & set__pos(
    const geometry_msgs::msg::Point_<ContainerAllocator> & _arg)
  {
    this->pos = _arg;
    return *this;
  }
  Type & set__tag(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->tag = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    msg_folder::msg::Pose_<ContainerAllocator> *;
  using ConstRawPtr =
    const msg_folder::msg::Pose_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<msg_folder::msg::Pose_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<msg_folder::msg::Pose_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      msg_folder::msg::Pose_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<msg_folder::msg::Pose_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      msg_folder::msg::Pose_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<msg_folder::msg::Pose_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<msg_folder::msg::Pose_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<msg_folder::msg::Pose_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__msg_folder__msg__Pose
    std::shared_ptr<msg_folder::msg::Pose_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__msg_folder__msg__Pose
    std::shared_ptr<msg_folder::msg::Pose_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Pose_ & other) const
  {
    if (this->pos != other.pos) {
      return false;
    }
    if (this->tag != other.tag) {
      return false;
    }
    return true;
  }
  bool operator!=(const Pose_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Pose_

// alias to use template instance with default allocator
using Pose =
  msg_folder::msg::Pose_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace msg_folder

#endif  // MSG_FOLDER__MSG__DETAIL__POSE__STRUCT_HPP_
