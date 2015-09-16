#include "base.h"
#include "camera.h"
#include <imgui.h>
#include "gui.h"
#include "nullspace.h"
#include "demo.h"

#include <cprintf/cprintf.hpp>

#include <stdarg.h>

camera_t cam;

GLFWwindow *window = NULL;
int window_width = (1024 * 2);
int window_height = (768 * 2);
double cursor_posx(0);
double cursor_posy(0);
bool executing = true;
bool gui_enabled = false;

demo_app_t *demo = NULL;

// In case a GLFW function fails, an error is reported to the
// GLFW error callback
static void pfn_glfw_err_cb(int error, const char *description) {
  printf("glfw error [%d]: %s ", error, description);
}

static void pfn_glfw_curspos_cb(GLFWwindow *window, double xpos, double ypos) {
  cursor_posx = xpos;
  cursor_posy = ypos;
}

// key/input event register callback
static void pfn_glfw_key_cb(GLFWwindow *window, int key, int scancode,
                            int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    executing = false;
    return;
  }

  if (key == GLFW_KEY_G && action == GLFW_PRESS) {
    gui_enabled = !gui_enabled;
    return;
  }

  cam.process_input(key, scancode, action, mods);

  demo->input(key, scancode, action, mods);
}

const char *load_src(const char *path) {
  char *data_buf = NULL;
  size_t length;
  FILE *fp = fopen(path, "r");

  if (!fp) {
    fprintf(stderr, "ERROR: failed to load file: %s\n", path);
    exit(1);
  }

  fseek(fp, 0, SEEK_END);
  length = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  data_buf = (char *)malloc(length);
  if (!data_buf) {
    fprintf(stderr, "ERROR: failed to allocate file mem\n");
    exit(1);
  }

  fread(data_buf, 1, length, fp);
  fclose(fp);

  return data_buf;
}

GLuint create_shader(GLenum type, const char *src) {
  GLuint shader;
  GLint shader_ok;
  GLsizei log_length;
  char info_log[8192];

  shader = glCreateShader(type);
  if (!shader) {
    fprintf(stderr, "ERROR: failed to create shader object\n");
    exit(1);
  }

  glShaderSource(shader, 1, (const GLchar **)&src, NULL);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);

  if (!shader_ok) {
    fprintf(stderr, "ERROR: Failed to compile %s shader\n",
            (type == GL_FRAGMENT_SHADER) ? "fragment" : "vertex");

    glGetShaderInfoLog(shader, 8192, &log_length, info_log);
    fprintf(stderr, "BUILD LOG: \n%s\n\n", info_log);
    glDeleteShader(shader);
    exit(1);
  }
  return shader;
}

GLuint create_shader_program(int32_t shader_count, ...) {
  GLint program_ok;
  GLint program = glCreateProgram();
  if (!program) {
    fprintf(stderr, "EEROR: failed to create shader program object\n");
    exit(1);
  }

  va_list arg_list;
  va_start(arg_list, shader_count);
  for (int32_t shader_iter = 0; shader_iter < shader_count; ++shader_iter)
    glAttachShader(program, va_arg(arg_list, GLint));
  va_end(arg_list);

  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &program_ok);

  if (!program_ok) {
    fprintf(stderr, "ERROR: failed to link shader program\n");

    GLsizei log_length;
    char info_log[8192];
    glGetProgramInfoLog(program, 8192, &log_length, info_log);
    fprintf(stderr, "ERROR LOG: \n%s\n\n", info_log);
    glDeleteProgram(program);
    exit(1);
  }
  return program;
}

void setup(void) {
  cprintf(L"$c*`begin$? program setup\n");

  glfwSetErrorCallback(pfn_glfw_err_cb);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  window =
      glfwCreateWindow(window_width, window_height, "OpenGL Basic", NULL, NULL);

  if (!window)
    exit(1);

  // make current OpenGL context
  glfwMakeContextCurrent(window);

  GLFWmonitor *monitor = glfwGetWindowMonitor(window);
  if (monitor) {
    const GLFWvidmode *video_mode = glfwGetVideoMode(monitor);

    window_width = video_mode->width / 2;
    window_height = video_mode->height / 2;
    glfwSetWindowSize(window, window_width, window_width);
  }
  // load fucntion pointers
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  // how often we swap render buffers i.e. front and back buffers upon
  // invoking a swap-buffers command
  glfwSwapInterval(1);

  glfwSetKeyCallback(window, pfn_glfw_key_cb);

  // set a cursor position callback for "window".
  // glfwSetCursorPosCallback(window, pfn_glfw_curspos_cb);

  glfwGetFramebufferSize(window, &window_width, &window_height);

  // GLFW_CURSOR_DISABLED hides and grabs the cursor, providing virtual and
  // unlimited cursor movement. This is useful for implementing for example
  // 3D camera controls.
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Reset mouse position to be at centre
  glfwSetCursorPos(window, window_width / 2, window_height / 2);

  // Setup ImGui binding
  imgui_init(window, true);

  // Load Fonts
  // (see extra_fonts/README.txt for more details)
  // ImGuiIO& io = ImGui::GetIO();
  // io.Fonts->AddFontDefault();
  // io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf",
  // 15.0f);
  // io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
  // io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyClean.ttf", 13.0f);
  // io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
  // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f,
  // NULL, io.Fonts->GetGlyphRangesJapanese());

  // Merge glyphs from multiple fonts into one (e.g. combine default font with
  // another with Chinese glyphs, or add icons)
  // ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 };
  // ImFontConfig icons_config; icons_config.MergeMode = true;
  // icons_config.PixelSnapH = true;
  // io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 18.0f);
  // io.Fonts->AddFontFromFileTTF("../../extra_fonts/fontawesome-webfont.ttf",
  // 18.0f, &icons_config, icons_ranges);

  glViewport(0, 0, window_width, window_height);
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

  glEnable(GL_DEPTH_TEST);
#if ENABLE_NULLSPACE
  nullspace_init();
#endif
  cam.setup(glm::vec3(0.0f, 2.0f, 0.0f), 45.0f,
            (float)window_height / (float)window_width, 1.0f, 1000.0f);

  demo = new demo_app_t;

  if (!demo->init()) {
    cprintf<CPF_STDE>(
        L"$r*FATAL ERROR$?: demo data could not be initialised\n");
    abort();
  }

  cprintf(L"program setup $g*success$?`!\n");
}

void teardown(void) {
  cprintf(L"$c*`begin$? program teardown\n");

  if (!demo->teardown()) {
    cprintf<CPF_STDE>(L"$r*FATAL ERROR$?: demo data could not be destroyed\n");
    abort();
  }
  cam.teardown();

#if ENABLE_NULLSPACE
  nullspace_teardown();
#endif

  imgui_shutdown();

  if (window)
    glfwDestroyWindow(window);

  // destroys any remaining windows and releases any other
  // resources allocated by GLFW
  glfwTerminate();

  cprintf(L"program teardown $g*success$?`!\n");
}

void imgui_update(void) {
  if (gui_enabled == false) {
    // imgui modifies the cursor's input mode, so it must be reset
    if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    return;
  }
  bool show_test_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImColor(114, 144, 154);

  imgui_newframe();

  // 1. Show a simple window
  // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a
  // window automatically called "Debug"
  {
    static float f = 0.0f;
    ImGui::Text("Hello, world!");
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
    ImGui::ColorEdit3("clear color", (float *)&clear_color);
    if (ImGui::Button("Test Window"))
      show_test_window ^= 1;
    if (ImGui::Button("Another Window"))
      show_another_window ^= 1;
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  }

  // 2. Show another simple window, this time using an explicit Begin/End pair
  if (show_another_window) {
    ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Another Window", &show_another_window);
    ImGui::Text("Hello");
    ImGui::End();
  }

  // 3. Show the ImGui test window. Most of the sample code is in
  // ImGui::ShowTestWindow()
  if (show_test_window) {
    ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
    ImGui::ShowTestWindow(&show_test_window);
  }
}

void imgui_render(void) {
  if (!gui_enabled)
    return;

  ImGui::Render();
}

void run(void) {
  tsamplr_t time_sampler(NULL);
  float dt = 0.0f;

  while (executing) {
    time_sampler.sample();
    dt = time_sampler.get_dt(tsamplr_t::_s_);

    // update ...
    imgui_update();
    cam.apply(dt);
    demo->update(dt);

    // render
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
      imgui_render();

#if ENABLE_NULLSPACE
      nullspace_render();
#endif
      demo->render();
    }
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

int main(int argc, char const *argv[]) {
  std::atexit(teardown);
  setup();
  run();
  return 0;
}
