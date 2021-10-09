<h1 align=center>3DModelViewer</h1>

<p align=center>
 <img src="https://user-images.githubusercontent.com/25436568/136638979-7b398eec-9a15-4dea-bf58-dcec2d78adb9.PNG">
</p>
<p align=center>
  <em><a href="https://sketchfab.com/3d-models/modular-sci-fi-wall-test-2-060031f51d7246379186bbc626be13b9">Module Sci-Fi Wall Test 2</a>, rendered and lit by 3DModelViewer</em>
</p>

A simple 3D model viewer I made to test my knowledge of graphics programming in OpenGL (shoutout to https://learnopengl.com/ 😊)

## Features
This viewer can simluate lighting from upto 16 point lights, 16 spotlights and 1 directional light upon one model. The physically-based rendering model with metallic workflow is utilized. Albdeo, normal, metallic, roughness and AO maps are currently supported. All textures and lighting parameters (color, position, etc.) can be changed through a GUI at any point; the results will be seen in the next frame.

## Build
Use Visual Studio to build the `.sln` file in the root of the project directory. `msbuild` can also be used if a full Visual Studio installation is not desired. Copy the built executable into the project directory before running. Otherwise, you may get an error saying that `assimp-vc140-mt.dll` is missing.

## Dependencies
This viewer is built for Windows. OpenGL 4.6 has been used, but any version above 3.3 should be fine.
All of the following dependencies are already present in this repository.
- Assimp (model loading)
- Dear ImGUI (GUI)
- GLFW (windowing and input management)
- GLAD (OpenGL function loading)
- GLM (Vector and matrix mathematics)
- portable_file_dialogs (Windows file dialog handling, https://github.com/samhocevar/portable-file-dialogs)
- stb_image (texture handling, https://github.com/nothings/stb)

## Planned features
- Support for more map types, such as heightmaps, emission maps, etc
- Support for cubemaps and image-based lighting (IBL)
- Ability to open more than one model
- Support for shadows
- Option to use deferred rendering
- Allow unlimited number of lights of each type
- Ability to select lights from the screen instead of the GUI
