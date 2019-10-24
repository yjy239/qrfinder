# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
#判断编译器类型,如果是gcc编译器,则在编译选项中加入c++11支持
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    message(STATUS "optional:-std=c++11")
endif(CMAKE_COMPILER_IS_GNUCXX)

#把当前目录作为源文件目录赋值给 SRC_LIST
AUX_SOURCE_DIRECTORY(src/main/cpp SRC_LIST)

#需要引入我们头文件,以这个配置的目录为基准
include_directories(src/main/cpp/include)

message(WARNING "message PROJECT: ${CMAKE_CURRENT_SOURCE_DIR}")

# 添加依赖 opencv.so 库
add_library(
        opencv_java
        SHARED
        IMPORTED)
set_target_properties(
        opencv_java
        PROPERTIES IMPORTED_LOCATION
        ../../../../src/main/jniLibs/armeabi-v7a/libopencv_java3.so)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${SRC_LIST})

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.


# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        native-lib opencv_java jnigraphics

        # Links the target library to the log library
        # included in the NDK.
        log)