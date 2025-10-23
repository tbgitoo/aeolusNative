# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# This file is basically from https://medium.com/swlh/creating-a-fluidsynth-hello-world-app-for-android-5e112454a8eb
# but with a bunch of modifications to make it work in current android

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("midi_synth")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
#
# In this top level CMakeLists.txt, ${CMAKE_PROJECT_NAME} is used to define
# the target library name; in the sub-module's CMakeLists.txt, ${PROJECT_NAME}
# is preferred for the same purpose.
#

set (OBOE_DIR ${CMAKE_SOURCE_DIR}/oboe)

add_subdirectory(oboe)
include_directories (${OBOE_DIR}/include)
add_subdirectory(SynthesizerBase)
add_subdirectory(aeolus)
add_subdirectory(aeolusSynthesizer)
add_subdirectory(aeolusJNI)
add_subdirectory(clthreads)
find_library( # Sets the name of the path variable.
        android
        #log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log
)


add_library(${CMAKE_PROJECT_NAME} SHARED
        # List C/C++ source files with relative paths to this CMakeLists.txt.

        AppMidiManager.cpp
)





target_link_libraries(${CMAKE_PROJECT_NAME}
        amidi
        log
        AeolusAndroid
)


