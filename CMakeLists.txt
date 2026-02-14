cmake_minimum_required(VERSION 3.28)

project(CLC)

set(PROJECT_NAME CLC)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


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

set(sources ${sources_src} ${engine} )
add_executable(${PROJECT_NAME} ${sources} "CORE/Systems/Renderer/Mesh.cpp")



#Include dirs
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${VCPKG_INSTALLED_DIR}/x64-windows/include)



# this copies all resource files in the build directory
# we need this, because we want to work with paths relative to the executable
# file(COPY ${resources} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
# file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#copy shaders too 
# file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/CORE/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/res
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
    COMMENT "Copying resources to build directory including shader"
)

##add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
 #   COMMAND ${CMAKE_COMMAND} -E copy_directory
 #   ${CMAKE_CURRENT_SOURCE_DIR}/CORE/shaders
 #   $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
 #   COMMENT "Copying shaders to build directory"
#)


#file(COPY Config DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


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

# After target_link_libraries
get_target_property(PHYSX_INCLUDES unofficial::omniverse-physx-sdk::sdk INTERFACE_INCLUDE_DIRECTORIES)
if (PHYSX_INCLUDES)
    target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${PHYSX_INCLUDES})
endif()


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



if (MSVC AND WIN32 AND NOT MSVC_VERSION VERSION_LESS 142)

    target_compile_options(${PROJECT_NAME} PRIVATE /MP)
    target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:/INCREMENTAL>)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)

    # Suppress specific warnings if needed
    # target_compile_options(${PROJECT_NAME} PRIVATE /wd4201 /wd4100)

endif()

# Enable warnings for GCC/Clang
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE 
        -Wall -Wextra -Wpedantic
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Release>:-O3>
    )
endif()

