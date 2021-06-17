# GrassRenderer
Application for rendering geometric grass using hardware tessellation. <br /><br />
Part of my bachelor's thesis at Brno University of Technology - Faculty of Information Technology:<br />
https://www.fit.vut.cz/study/thesis/23782/.en<br />
Implementation inspired by this paper:<br />
https://www.cg.tuwien.ac.at/research/publications/2013/JAHRMANN-2013-IGR/JAHRMANN-2013-IGR-paper.pdf

# Third-party libraries
Qt version 5.12.9: https://download.qt.io/official_releases/qt/5.12/5.12.9/ <br />
glm: https://github.com/g-truc/glm <br />
GPUEngine: https://github.com/Rendering-FIT/GPUEngine <br />
ImGui: v1.82 https://github.com/ocornut/imgui <br />
qtimgui (Qt backend for ImGui): https://github.com/seanchas116/qtimgui

# Features
+ Real-time grass generation
+ Terrain generation from height map
+ Grass density and blade height from height map
+ Basic lighting (color gradient)
+ Advanced lighting using normals (experimental)
+ Wind function
+ Skybox

# Controls
- WASD: camera movement
- Space/X: camera ascend/descend
- RMB: camera rotation
- Ctrl + Mouse Wheel: camera zoom
- M: select height map from file system
- V: toggle wind
- Esc: toggle GUI

![final_version](https://user-images.githubusercontent.com/38842578/122383625-48841c80-cf6b-11eb-9a71-fa5e2c6c870b.png)
