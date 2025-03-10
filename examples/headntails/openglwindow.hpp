#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <random>
#include <vector>

#include "abcg.hpp"

struct Vertex {
  glm::vec3 position;

  bool operator==(const Vertex& other) const {
    return position == other.position;
  }
};

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLuint m_program{};

  int m_viewportWidth{};
  int m_viewportHeight{};

  float m_angle{};
  int m_verticesToDraw{};
  int m_randomNumber{};
  int m_side{};
  bool m_draw{};

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;
  std::default_random_engine m_randomEngine;

  void loadModelFromFile(std::string_view path);
  void standardize();
};

#endif