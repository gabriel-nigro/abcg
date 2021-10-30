#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>

#include "abcg.hpp"

void OpenGLWindow::handleEvent(SDL_Event &event) {
  // Keyboard events
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_SPACE) {
      m_gameData.m_input.set(static_cast<size_t>(Input::Fire));
      if (m_gameData.m_state == State::NotRunning){
        m_gameData.m_state = State::Playing;
        m_time.restart();
        update();
      }
    }
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_gameData.m_input.set(static_cast<size_t>(Input::Up));
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_gameData.m_input.set(static_cast<size_t>(Input::Down));
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_SPACE)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Fire));
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Up));
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Down));
  }

  // Mouse events
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_LEFT)
      m_gameData.m_input.set(static_cast<size_t>(Input::Jump));
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    if (event.button.button == SDL_BUTTON_LEFT)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Jump));
  }

}

void OpenGLWindow::initializeGL() {
  // Create program to render the stars
  m_starsProgram = createProgramFromFile(getAssetsPath() + "stars.vert",
                                         getAssetsPath() + "stars.frag");
  // Load a new font
  ImGuiIO &io{ImGui::GetIO()};
  const auto filename{getAssetsPath() + "Inconsolata-Medium.ttf"};
  m_font = io.Fonts->AddFontFromFileTTF(filename.c_str(), 39.0f);
  if (m_font == nullptr) {
    throw abcg::Exception{abcg::Exception::Runtime("Cannot load font file")};
  }

  // Create program to render the other objects
  m_objectsProgram = createProgramFromFile(getAssetsPath() + "objects.vert",
                                           getAssetsPath() + "objects.frag");

  abcg::glClearColor(0, 0, 0, 1);

#if !defined(__EMSCRIPTEN__)
  abcg::glEnable(GL_PROGRAM_POINT_SIZE);
#endif

   // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  restart();
}

void OpenGLWindow::restart() {
  m_gameData.m_state = State::NotRunning;
  m_starLayers.initializeGL(m_starsProgram, 25);
  m_square.initializeGL(m_objectsProgram);
  m_asteroids.terminateGL();
  m_asteroids.initializeGL(m_objectsProgram, 3);
}

void OpenGLWindow::update() {
  float deltaTime{static_cast<float>(getDeltaTime())};

  // Wait 5 seconds before restarting
  if (m_gameData.m_state == State::GameOver &&
      m_restartWaitTimer.elapsed() > 5) {
        m_surviveTime=0.0f;
        restart();
        return;
  }

  m_square.update(m_gameData, deltaTime);
  m_starLayers.update(m_square, deltaTime);
  m_asteroids.update(m_square, m_gameData, deltaTime);
  if (m_gameData.m_state == State::Playing) {
    checkCollisions();
  }
}

void OpenGLWindow::paintGL() {
  update();

  abcg::glClear(GL_COLOR_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);
  m_starLayers.paintGL();
  m_asteroids.paintGL();
  m_square.paintGL(m_gameData);
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();
  
  {
    const auto size{ImVec2(300, 200)};
    const auto position{ImVec2((m_viewportWidth - size.x) / 2.0f,
                         (m_viewportHeight - size.y) / 2.0f)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                           ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoInputs};
    ImGui::Begin(" ", nullptr, flags);
    ImGui::PushFont(m_font);

    if (m_gameData.m_state == State::GameOver) {
      ImGui::Text("Game Over!");
      ImGui::Text("Survived: %.1fs", m_surviveTime);
    } else if (m_gameData.m_state == State::NotRunning) {
      ImGui::Text("Press space");
    } 
    ImGui::PopFont();
    ImGui::End();
  }
  
  {
    ImGui::SetNextWindowSize(ImVec2(150, 70));
    ImGui::SetNextWindowPos(ImVec2(500, (m_viewportHeight - 85)));
    ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                           ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoInputs};
    ImGui::Begin("2", nullptr, flags);
    ImGui::PushFont(m_font);
    if(m_gameData.m_state == State::Playing){
      ImGui::Text("%.1f",m_time.elapsed());
    }
    ImGui::PopFont();
    ImGui::End();
  }
}
void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL() {
  abcg::glDeleteProgram(m_starsProgram);
  abcg::glDeleteProgram(m_objectsProgram);

  m_asteroids.terminateGL();
  m_square.terminateGL();
  m_starLayers.terminateGL();
}

void OpenGLWindow::checkCollisions() {
  // Check collision between square and asteroids
  for (const auto &asteroid : m_asteroids.m_asteroids) {
    const auto asteroidTranslation{asteroid.m_translation};
    const auto distance{glm::distance(m_square.m_translation, asteroidTranslation)};


    if (distance < m_square.m_scale * 0.055f + asteroid.m_scale * 0.55f) {
      m_gameData.m_state = State::GameOver;
      m_surviveTime = m_time.elapsed();
      m_restartWaitTimer.restart();
    }
  }

}