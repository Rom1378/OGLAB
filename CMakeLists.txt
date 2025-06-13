cmake_minimum_required(VERSION 3.28)

project(CLC)

set(PROJECT_NAME CLC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(unofficial-omniverse-physx-sdk CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)




#collect sources

file(GLOB_RECURSE engine CORE/*.c CORE/*.cpp CORE/*.h CORE/*.hpp)
file(GLOB_RECURSE sources_src src/*.c src/*.cpp src/*.h src/*.hpp)
file(GLOB_RECURSE resources res/*)
#file(GLOB_RECURSE config Config/*)

#set(sources ${sources_src} ${engine} ${resources} ${config})
set(sources ${sources_src} ${engine} ${resources})
add_executable(${PROJECT_NAME} ${sources})


#Include dirs
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/CORE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)


# this copies all resource files in the build directory
# we need this, because we want to work with paths relative to the executable
file(COPY res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#file(COPY Config DESTINATION ${CMAKE_CURRENT_BINARY_DIR})



################################################################################
# Include directories
################################################################################
#if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
#    target_include_directories(${PROJECT_NAME} PUBLIC
#        "$ENV{VcpkgRoot}/installed/x64-windows/include/physx;"
#        "$ENV{VcpkgRoot}/installed/x64-windows/include/nlohmann"
#    )
#endif()

target_link_libraries(${PROJECT_NAME} 
    PRIVATE
    glfw
    glm::glm
    glad::glad
    imgui::imgui
    unofficial::omniverse-physx-sdk::sdk
    assimp::assimp
    nlohmann_json::nlohmann_json
)


if(TARGET unofficial::omniverse-physx-sdk::gpu-library)
        if(UNIX)
            # Add rpath setting to find .so libraries on unix based systems
            set_target_properties(${PROJECT_NAME} PROPERTIES
                BUILD_WITH_INSTALL_RPATH TRUE
                INSTALL_RPATH "$ORIGIN"
            )
        endif()
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        $<TARGET_FILE:unofficial::omniverse-physx-sdk::gpu-library>
                        $<TARGET_FILE_DIR:${PROJECT_NAME}>)
        if(WIN32)
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                            $<TARGET_FILE:unofficial::omniverse-physx-sdk::gpu-device-library>
                            $<TARGET_FILE_DIR:${PROJECT_NAME}>)
        endif()
    else()
        message(WARNING "GPU acceleration library target not defined - GPU acceleration will NOT be available!")
    endif()

