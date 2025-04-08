OpenGL Playground

![Project Screenshot](ca.png)

A personal playground for experimenting with OpenGL and real-time graphics techniques. Current features include:

- PhysX-powered basic cube and sphere collision physics
- 3D model loading using Assimp
- Shadow mapping implementation
- Modern OpenGL rendering pipeline
- Immediate-mode UI with ImGui

**Acknowledgements**: Special thanks to [LearnOpenGL](https://learnopengl.com) for their tutorials.

### Dependencies
```bash
vcpkg install glm glfw3 assimp glad imgui \
    imgui[glfw-binding] imgui[opengl3-binding] \
    physx imgui[docking-experimental]
```
