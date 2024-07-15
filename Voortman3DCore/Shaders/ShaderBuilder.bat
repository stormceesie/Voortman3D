@echo on

..\Dependencies\glslc.exe Shaders/textoverlay.frag -o Shaders/textoverlay.frag.spv
..\Dependencies\glslc.exe Shaders/textoverlay.frag -o Shaders/textoverlay.frag.spv

..\Dependencies\glslc.exe Shaders/uioverlay.frag -o Shaders/uioverlay.frag.spv
..\Dependencies\glslc.exe Shaders/uioverlay.frag -o Shaders/uioverlay.frag.spv
pause