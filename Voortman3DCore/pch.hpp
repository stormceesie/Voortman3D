// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"
#include "windows.h"
#include <cstdint>
#include <fcntl.h>
#include <io.h>
#include <ShellScalingApi.h>
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <vector>
#include <cassert>
#include <array>
#include <iostream>
#include <algorithm>
#include <assert.h>
#include <exception>
#include <fstream>
#include <iomanip>
#include <thread>
#include <memory>

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

#endif //PCH_H
