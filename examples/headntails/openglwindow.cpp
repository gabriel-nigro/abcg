#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>
#include <tiny_obj_loader.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/hash.hpp>
#include <iostream>
using namespace std;
#include <unistd.h>

#include <unordered_map>

// Explicit specialization of std::hash for Vertex
namespace std {
template <>
struct hash<Vertex> {
  size_t operator()(Vertex const& vertex) const noexcept {
    const std::size_t h1{std::hash<glm::vec3>()(vertex.position)};
    return h1;
  }
};
}  // namespace std

void OpenGLWindow::initializeGL() {
  abcg::glClearColor(160.0f / 255.0f, 82.0f / 255.0f, 45.0f / 255.0f, 1);

  // Enable depth buffering
  abcg::glEnable(GL_DEPTH_TEST);

  // Create program
  m_program = createProgramFromFile(getAssetsPath() + "loadmodel.vert",
                                    getAssetsPath() + "loadmodel.frag");

  // Load model
  loadModelFromFile(getAssetsPath() + "coin.obj");
  standardize();

  m_verticesToDraw = m_indices.size();

  // Generate VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices[0]) * m_vertices.size(),
                     m_vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  abcg::glGenBuffers(1, &m_EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(m_indices[0]) * m_indices.size(), m_indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void OpenGLWindow::loadModelFromFile(std::string_view path) {
  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data())) {
    if (!reader.Error().empty()) {
      throw abcg::Exception{abcg::Exception::Runtime(
          fmt::format("Failed to load model {} ({})", path, reader.Error()))};
    }
    throw abcg::Exception{
        abcg::Exception::Runtime(fmt::format("Failed to load model {}", path))};
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  const auto& attrib{reader.GetAttrib()};
  const auto& shapes{reader.GetShapes()};

  m_vertices.clear();
  m_indices.clear();

  // A key:value map with key=Vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop over shapes
  for (const auto& shape : shapes) {
    // Loop over indices
    for (const auto offset : iter::range(shape.mesh.indices.size())) {
      // Access to vertex
      const tinyobj::index_t index{shape.mesh.indices.at(offset)};

      // Vertex position
      const int startIndex{3 * index.vertex_index};
      const float vx{attrib.vertices.at(startIndex + 0)};
      const float vy{attrib.vertices.at(startIndex + 1)};
      const float vz{attrib.vertices.at(startIndex + 2)};

      Vertex vertex{};
      vertex.position = {vx, vy, vz};

      // If hash doesn't contain this vertex
      if (hash.count(vertex) == 0) {
        // Add this index (size of m_vertices)
        hash[vertex] = m_vertices.size();
        // Add this vertex
        m_vertices.push_back(vertex);
      }

      m_indices.push_back(hash[vertex]);
    }
  }
}

void OpenGLWindow::standardize() {
  // Center to origin and normalize largest bound to [-1, 1]

  // Get bounds
  glm::vec3 max(std::numeric_limits<float>::lowest());
  glm::vec3 min(std::numeric_limits<float>::max());
  for (const auto& vertex : m_vertices) {
    max.x = std::max(max.x, vertex.position.x);
    max.y = std::max(max.y, vertex.position.y);
    max.z = std::max(max.z, vertex.position.z);
    min.x = std::min(min.x, vertex.position.x);
    min.y = std::min(min.y, vertex.position.y);
    min.z = std::min(min.z, vertex.position.z);
  }

  // Center and scale
  const auto center{(min + max) / 2.0f};
  const auto scaling{2.0f / glm::length(max - min)};
  for (auto& vertex : m_vertices) {
    vertex.position = (vertex.position - center) * scaling;
  }
}

void OpenGLWindow::paintGL() {
  // Animate angle by 15 degrees per second
  const float deltaTime{static_cast<float>(getDeltaTime())};
  m_angle = glm::wrapAngle(m_angle + glm::radians(360.0f) * deltaTime);

  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  abcg::glUseProgram(m_program);
  abcg::glBindVertexArray(m_VAO);

  // Update uniform variable
  const GLint angleLoc{abcg::glGetUniformLocation(m_program, "angle")};
  abcg::glUniform1f(angleLoc, m_angle);

  // Draw triangles
  abcg::glDrawElements(GL_TRIANGLES, m_verticesToDraw, GL_UNSIGNED_INT,
                       nullptr);

  abcg::glBindVertexArray(0);
  abcg::glUseProgram(0);
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  // Create a window for the other widgets
  {
    auto widgetSize{ImVec2(m_viewportWidth - 5, 80)};
    ImGui::SetNextWindowPos(ImVec2(5, 5));
    ImGui::SetNextWindowSize(widgetSize);
    ImGui::Begin("Widget window", nullptr, ImGuiWindowFlags_NoDecoration);

    static bool stop{};
    ImGui::Checkbox("Parar a moeda", &stop);
    ImGui::Separator();
    ImGui::Text("Selecione a face:");
    ImGui::RadioButton("Cara", &m_side, 1);
    ImGui::SameLine();
    ImGui::RadioButton("Coroa", &m_side, 2);
    ImGui::SameLine();

    if (stop) {
      if (!m_draw) {
        m_angle = 0;
      } else {
        m_angle = glm::wrapAngle(glm::radians(90.0f));
      }
    } else {
      m_draw = false;
      m_randomNumber = (rand() % 10) + 1;
    }

    ImGui::End();
  }

  {
    ImGui::SetNextWindowPos(ImVec2(5, m_viewportHeight - 40));
    ImGui::SetNextWindowSize(ImVec2(m_viewportWidth - 10, -1));
    ImGui::Begin("Message Window", nullptr, ImGuiWindowFlags_NoDecoration);

    // Create a slider to control the number of rendered triangles
    {
      // Slider will fill the space of the window
      ImGui::PushItemWidth(m_viewportWidth - 25);
      if (m_angle == 0) {
        if (m_randomNumber < 5) {
          if (m_side == 1) {
            ImGui::Text("Você ganhou");
          } else {
            ImGui::Text("Você perdeu");
          }
        }

        if (m_randomNumber > 5) {
          if (m_side == 1) {
            ImGui::Text("Você perdeu");
          } else {
            ImGui::Text("Você ganhou");
          }
        }

        if (m_randomNumber == 5) {
          m_draw = true;
        }

      } else {
        if (!m_draw) {
          ImGui::Text("Pare a moeda para ver o resultado!");
        } else {
          ImGui::Text("Draw");
        }
      }
      ImGui::PopItemWidth();
    }

    ImGui::End();
  }
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;
}

void OpenGLWindow::terminateGL() {
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}