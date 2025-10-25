# banewfn CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(banewfn)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# 静态编译选项
option(BUILD_STATIC "Build with static linking" ON)
if(BUILD_STATIC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif()

# 源文件
set(SOURCES
    src/banewfn.cpp
    src/config.cpp
    src/input.cpp
    src/ui.cpp
    src/utils.cpp
)

# 头文件
set(HEADERS
    src/config.h
    src/input.h
    src/ui.h
    src/utils.h
)

# 创建可执行文件
add_executable(banewfn ${SOURCES} ${HEADERS})

# 设置输出目录
set_target_properties(banewfn PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 安装目标
install(TARGETS banewfn
    RUNTIME DESTINATION bin
    COMPONENT banewfn
)

# 显示构建信息
message(STATUS "banewfn: Building with C++${CMAKE_CXX_STANDARD}")
if(BUILD_STATIC)
    message(STATUS "banewfn: Static linking enabled")
endif()
