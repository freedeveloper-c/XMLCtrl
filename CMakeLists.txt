#定义cmake版本
cmake_minimum_required(VERSION 3.9.5)

#设置项目名称
set(LIB_NAME XMLTrans)
project(XMLCtrl)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--as-needed -fPIE -fPIC") #-Wl,--as-needed 减少无效的库链接，用不到的库需要清理

#查找依赖关系的包信息
find_package(PkgConfig REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
include(FindPkgConfig)

#源文件、头文件和资源文件
file(GLOB_RECURSE c_files RELATIVE ${PROJECT_SOURCE_DIR} *.cpp)
file(GLOB_RECURSE h_files RELATIVE ${PROJECT_SOURCE_DIR} *.h)

#指定头文件路径
include_directories(${Qt5Widgets_INCLUDE_DIRS})

#生成动静态库
add_library(${LIB_NAME} SHARED ${h_files} ${c_files})

#链接动静态库
target_link_libraries(${LIB_NAME}
    Qt5::Widgets
    Qt5::Xml
)

