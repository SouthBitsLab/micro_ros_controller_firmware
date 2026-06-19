# Copyright (c) 2022 TOKITA Hiroshi <tokita.hiroshi@fujitsu.com>
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# Add the workspace root as a devicetree root so bindings in dts/bindings are found.
list(APPEND DTS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(accel_polling)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})
