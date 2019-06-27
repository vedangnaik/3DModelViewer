# 3DModelViewer

A simple 3D model viewer I made to test my knowledge of graphics programming in OpenGL (shoutout to https://learnopengl.com/); I wouldn't recommened actually using it.

## Features
This viewer can simluate lighting from upto 16 point lights, 16 spotlights and 1 directional light upon one model. The physically-based rendering model with metallic workflow is utilized. Albdeo, normal, metallic, roughness and AO maps are currently supported. All textures and lighting parameters (color, position, etc.) can be changed through a GUI at any point; the results will be seen in the next frame.

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
I might add this stuff if I'm in the mood
- Support for more map types, such as heightmaps, emission maps, etc
- Support for cubemaps and image-based lighting (IBL)
- Ability to open more than one model
- Support for shadows
- Option to use deferred rendering
- Allow unlimited number of lights of each type
- Ability to select lights from the screen instead of the GUI
