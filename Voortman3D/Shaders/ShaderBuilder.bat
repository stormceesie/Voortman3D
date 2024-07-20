@echo on

..\Dependencies\glslc.exe Shaders/model.frag -o Shaders/model.frag.spv
..\Dependencies\glslc.exe Shaders/model.frag -o Shaders/model.frag.spv

..\Dependencies\glslc.exe Shaders/pixelPerfectShader.frag -o Shaders/pixelPerfectShader.frag.spv
..\Dependencies\glslc.exe Shaders/pixelPerfectShader.vert -o Shaders/pixelPerfectShader.vert.spv


pause