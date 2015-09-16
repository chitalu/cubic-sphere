// ImGui GLFW binding with OpenGL3 + shaders // https://github.com/ocornut/imgui

struct GLFWwindow;

IMGUI_API bool imgui_init(GLFWwindow *window, bool install_callbacks);
IMGUI_API void imgui_shutdown();
IMGUI_API void imgui_newframe();

// Use if you want to reset your rendering device without losing ImGui state.
// IMGUI_API void imgui_InvalidateDeviceObjects();
// IMGUI_API bool imgui_CreateDeviceObjects();
//
// // GLFW callbacks (installed by default if you enable 'install_callbacks' during
// // initialization)
// // Provided here if you want to chain callbacks.
// // You can also handle inputs yourself and use those as a reference.
// IMGUI_API void imgui_MouseButtonCallback(GLFWwindow *window, int button,
//                                          int action, int mods);
// IMGUI_API void imgui_ScrollCallback(GLFWwindow *window, double xoffset,
//                                     double yoffset);
// IMGUI_API void imgui_KeyCallback(GLFWwindow *window, int key, int scancode,
//                                  int action, int mods);
// IMGUI_API void imgui_CharCallback(GLFWwindow *window, unsigned int c);
