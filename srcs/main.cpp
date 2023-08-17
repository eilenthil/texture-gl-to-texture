#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

static constexpr auto VERTEX_SHADER =
    "#version 330\n"
    "uniform mat4 proj;\n"
    "in vec3 in_position;\n"
    "in vec3 in_color;\n"
    "out vec3 o_color;\n"
    "void main()  {\n"
    "	gl_Position = proj * vec4(in_position, 1.f);\n"
    "	o_color = in_color;\n"
    "}";

static constexpr auto FRAGMENT_SHADER =
    "#version 330\n"
    "out vec4 fragColor;\n"
    "in vec3 o_color;\n"
    "void main() {\n"
    "	fragColor = vec4(o_color , 1.0f);\n"
    "};";

class program {
public:
  program(std::string const &vert_shader, std::string const &frag_shader);
  void use();
  bool init();
  GLuint get_uniform_location(std::string const &);
  template <class T> void set_uniform(T const &, GLuint uniform);

  void terminate();

private:
  std::string _vertex_shdaer_code;
  std::string _frag_shader_code;

  GLuint _vertex_shader;
  GLuint _fragment_shader;
  GLuint _program;

  bool compile_shader(char const *code, GLuint &obj, GLuint type);
};

template <class T> void program::set_uniform(T const &, GLuint uniform) {}

template <> void program::set_uniform(glm::mat4 const &m, GLuint uniform) {
  glUniformMatrix4fv(uniform, 1, GL_FALSE, &m[0][0]);
}
void program::use() { glUseProgram(_program); }
GLuint program::get_uniform_location(std::string const &name) {
  // return glUniformMatrix4fv(_program, name.c_str(), GL_FALSE, &mat[0][0]);
  std::cout << "Trying to fetch " << name << "\n";
  return glGetUniformLocation(_program, name.c_str());
}

program::program(std::string const &vert_shader, std::string const &frag_shader)
    : _vertex_shdaer_code(vert_shader), _frag_shader_code(frag_shader),
      _vertex_shader(0), _fragment_shader(0), _program(0) {}

void program::terminate() {
  glUseProgram(0);
  glDeleteProgram(_program);
  glDeleteShader(_vertex_shader);
  glDeleteShader(_fragment_shader);
}
bool program::init() {
  if (compile_shader(_vertex_shdaer_code.c_str(), _vertex_shader,
                     GL_VERTEX_SHADER)) {
    if (compile_shader(_frag_shader_code.c_str(), _fragment_shader,
                       GL_FRAGMENT_SHADER)) {
      _program = glCreateProgram();
      glAttachShader(_program, _vertex_shader);
      glAttachShader(_program, _fragment_shader);
      glLinkProgram(_program);
      GLint status = 0;
      glGetProgramiv(_program, GL_LINK_STATUS, &status);
      if (status == 0) {
        GLint msg_len;
        glGetProgramInfoLog(_program, 0, &msg_len, nullptr);
        if (msg_len > 0) {
          std::vector<char> msg;
          msg.resize(msg_len);

          glGetProgramInfoLog(_program, msg.size(), &msg_len, msg.data());
          std::cout
              << "============== Program link failed =======================\n";
          for (auto const &c : msg)
            std::cout << c;
          std::cout << "\n============== Program link failed "
                       "=======================\n";
        }
        glDeleteShader(_fragment_shader);
        glDeleteShader(_vertex_shader);
      } else {
        return true;
      }
    } else {
      glDeleteShader(_fragment_shader);
    }
  }
  return false;
}

bool program::compile_shader(char const *code, GLuint &shader, GLuint type) {
  shader = glCreateShader(type);
  glShaderSource(shader, 1, &code, nullptr);
  glCompileShader(shader);
  GLint status = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == 0) {
    GLint msg_len = 0;
    glGetShaderInfoLog(shader, 0, &msg_len, nullptr);
    if (msg_len > 0) {
      std::vector<char> msg;
      msg.resize(msg_len);
      glGetShaderInfoLog(shader, msg.size(), &msg_len, msg.data());
      std::cout
          << "============== Shader compile failed =======================\n";
      for (auto const &c : msg)
        std::cout << c;
      std::cout
          << "\n============== Shader compile failed =======================\n";
    }
    return false;
  }
  return true;
}

struct vertex {
  glm::vec3 position;
  glm::vec3 color;
};

class drawable {
public:
  bool init();
  void terminate();

  void bind();
  void draw();

private:
  GLuint _vertex_buffer_object;
  GLuint _index_buffer_object;
  GLuint _vertex_attrib_object;

  std::vector<vertex> _vertex_data;
  std::vector<unsigned short> _index_data;

  bool init_vertex_data();
  bool init_buffer_objects();
  bool init_attribs();

  void terminate_buffer_objects();
  void terminate_attribs();
};
bool drawable::init() {
  init_vertex_data();
  init_buffer_objects();
  glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer_object);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer_object);
  init_attribs();
  return true;
}

void drawable::terminate() {
  terminate_attribs();
  terminate_buffer_objects();
}

void drawable::bind() {
  glBindVertexArray(_vertex_attrib_object);
  glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer_object);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer_object);
}

void drawable::draw() {
  glDrawElements(GL_TRIANGLES, _index_data.size(), GL_UNSIGNED_SHORT, 0);
}

bool drawable::init_vertex_data() {
  vertex v;
  v.position = {-.5, -.5, .5};
  v.color = {.0, .0, .0};
  _vertex_data.push_back(v);
  v.position = {.5, -.5, .5};
  v.color = {1., .0, .0};
  _vertex_data.push_back(v);
  v.position = {-.5, .5, .5};
  v.color = {.0, 1.0, .0};
  _vertex_data.push_back(v);
  v.position = {.5, .5, .5};
  v.color = {1., 1.0, .0};
  _vertex_data.push_back(v);

  v.position = {-.5, -.5, -.5};
  v.color = {.0, 0.0, .0};
  _vertex_data.push_back(v);
  v.position = {.5, -.5, -.5};
  v.color = {1.0, 0.0, .0};
  _vertex_data.push_back(v);
  v.position = {-.5, .5, -.5};
  v.color = {.0, 1.0, .0};
  _vertex_data.push_back(v);
  v.position = {.5, .5, -.5};
  v.color = {1., 1.0, .0};
  _vertex_data.push_back(v);

  _index_data = {2, 6, 7, 2, 3, 7, 0, 4, 5, 0, 1, 5, 0, 2, 6, 0, 4, 6, 1,
                 3, 7, 1, 5, 7, 0, 2, 3, 0, 1, 2, 0, 1, 3, 4, 6, 7, 4, 5};

  return true;
}

bool drawable::init_buffer_objects() {
  glGenBuffers(1, &_vertex_buffer_object);
  glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer_object);
  glBufferData(GL_ARRAY_BUFFER, _vertex_data.size() * sizeof(vertex),
               _vertex_data.data(), GL_STATIC_DRAW);
  glGenBuffers(1, &_index_buffer_object);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer_object);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               _index_data.size() * sizeof(unsigned short), _index_data.data(),
               GL_STATIC_DRAW);
  return true;
}

void drawable::terminate_buffer_objects() {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glDeleteBuffers(1, &_vertex_buffer_object);
  glDeleteBuffers(1, &_index_buffer_object);
}

bool drawable::init_attribs() {
  glGenVertexArrays(1, &_vertex_attrib_object);
  glBindVertexArray(_vertex_attrib_object);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
                        (void *)(3 * sizeof(float)));
  return true;
}

void drawable::terminate_attribs() {
  glBindVertexArray(0);
  glDeleteVertexArrays(1, &_vertex_attrib_object);
}

int main() {
  GLFWwindow *window = nullptr;
  if (glfwInit()) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    window = glfwCreateWindow(800, 600, "Test", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
      glfwDestroyWindow(window);
      window = nullptr;
    }
    program gpu_program(VERTEX_SHADER, FRAGMENT_SHADER);
    drawable data;
    glm::vec3 cam_location = glm::vec3(4., 3., 3.f);
    glm::mat4 projection =
        glm::perspective(glm::radians(45.f), 1.33f, .1f, 100.f);
    glm::mat4 view = glm::lookAt(cam_location, glm::vec3(0.f, 0.f, 0.f),
                                 glm::vec3(0.f, 1.f, 0.f));
    GLuint mat_location = -3;
    if (window && gpu_program.init()) {
      data.init();
      gpu_program.use();
      data.bind();
      glViewport(0, 0, 800, 600);
      mat_location = gpu_program.get_uniform_location("proj");

      size_t iter = 0;
      glEnable(GL_DEPTH_TEST);
      while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
          glfwSetWindowShouldClose(window, true);
        }
        cam_location.x = 4.f + 2. * glm::sin(glm::radians(iter * .32f));
        cam_location.y = 3.f + 2. * glm::cos(glm::radians(iter * .32f));
        view = glm::lookAt(cam_location, glm::vec3(0.f, 0.f, 0.f),
                           glm::vec3(0.f, 1.f, 0.f));
        gpu_program.set_uniform(projection * view, mat_location);
        glClearColor(.1, .1, .1, 1.);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        data.draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
        ++iter;
      }

      data.terminate();
      gpu_program.terminate();
      glfwDestroyWindow(window);
    } else if (window) {
      glfwDestroyWindow(window);
    }
    glfwTerminate();
  }
  return 0;
}
