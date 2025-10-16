add_subdirectory(Synthesizer)

add_subdirectory(UserInterface)

add_subdirectory(MidiInterface)


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        android
        #log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log
)


# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries(
        AeolusSynthesizer
        log
        SynthesizerBase
        AeolusUserInterface
        AeolusMidiInterface
        aeolus
)





