﻿# CMakeList.txt: PMCorrectionPlugins 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

# 项目名
project ("pmaiplugins")

# 设置c++语言标准，使用c++14
set(CMAKE_CXX_STANDARD 14)

cmake_minimum_required (VERSION 3.4)

# 源文件
file(GLOB pmai_plugins_src "src/*.cpp" )

# 增加动态库
add_library (${PROJECT_NAME} SHARED ${pmai_plugins_src})
SET_TARGET_PROPERTIES (${PROJECT_NAME} PROPERTIES VERSION 1.2.0 SOVERSION 1)

# 为windows添加宏
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
	add_definitions(-DPMAIPLUGINS_EXPORTS)
endif()

# 设置头文件搜索目录
include_directories(
	incl
)

# 添加链接库
target_link_libraries(${PROJECT_NAME} 
    uplancommon
	lishuttlectl
	uplangraphic
	swanutility
	netentity
	uantenna
	propagationinterface
)
