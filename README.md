## Setup Instructions

### Install Dependencies
1. Install `vcpkg` if not already installed:
2. Install required libraries:
3. Open Visual Studio and set `CMake Toolchain` to `vcpkg/scripts/buildsystems/vcpkg.cmake`.

### Building
Open the `.sln` file in Visual Studio and build the project.


vcpkg install glm glfw3 assimp glad imgui imgui[glfw-binding] imgui[opengl3-binding] physx imgui[docking-experimental]