# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

# Path to micro-ROS Zephyr module inside this west workspace.
get_filename_component(
  MICRO_ROS_ZEPHYR_MODULE_DIR
  "${CMAKE_CURRENT_LIST_DIR}/../../micro_ros_zephyr_module"
  ABSOLUTE
)

# micro-ROS actual Zephyr module is under modules/libmicroros.
list(APPEND ZEPHYR_EXTRA_MODULES
  "${MICRO_ROS_ZEPHYR_MODULE_DIR}/modules/libmicroros"
)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(rover_controller)

# Common/shared types and utilities.
target_sources(app PRIVATE
  src/common/time_utils.c
)

# Hardware Abstraction Layer.
target_sources(app PRIVATE
  src/hal/imu.c
  src/hal/gnss.c
  # src/hal/odometry.c
  # src/hal/wheel_states.c
  # src/hal/battery.c
)

# # Safety layer.
# target_sources(app PRIVATE
#   src/safety/fault.c
#   src/safety/watchdog.c
#   src/safety/safety_monitor.c
# )

# # Application / control layer.
# target_sources(app PRIVATE
#   src/app/robot_command.c
#   src/app/state_machine.c
#   src/app/kinematics.c
#   src/app/odometry.c
#   src/app/control_loop.c
#   src/app/diagnostics.c
# )

# ROS 2 interface layer.
target_sources(app PRIVATE
  src/ros/node.c
  src/ros/publishers/heartbeat_pub.c
  src/ros/publishers/imu_pub.c
  src/ros/publishers/gnss_pub.c
  # src/ros/publishers/odom_pub.c
  # src/ros/publishers/wheel_states_pub.c
  # src/ros/publishers/battery_pub.c
  # src/ros/publishers/diagnostics_pub.c
  # src/ros/subscribers/cmd_vel_sub.c
)

# Application entry point.
target_sources(app PRIVATE
  src/main.c
)

# Ensure local headers are discoverable.
target_include_directories(app PRIVATE src)

# Application-specific Kconfig is handled by Zephyr automatically when Kconfig
# is present in the application directory.
