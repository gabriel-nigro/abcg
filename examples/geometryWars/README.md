# [Geometry Wars](https://gabriel-nigro.github.io/abcg/geometryWars/index.html)

O projeto trata-se de um jogo interativo em que o usuário controla o personagem principal "Square", o qual possui um corpo quadrado. Sua missão é ficar vivo o máximo de tempo possível, tarefa qual não é simples, pois triângulos serão dispostos de maneira aleatória pela tela, aumentando sua velocidade conforme o tempo passa.

## Índice

* [Inspirações](#inspirações)
* [Comandos](#comandos)
* [Square](#square)
* [Triângulos](#triângulos)
* [Tempo de sobrevivência](#tempo-de-sobrevivência)
* [Autores](#autores)



## Inspirações

As mêcanicas do projeto foram inspiradas no jogo "Geometry Dash", no qual o usuário controla um quadrado que percorre uma tela em busca da linha de chegada, passando por alguns desafios. No caso do jogo, o quadrado fica fixo na parte esquerda da tela, e o cenário movimenta-se da direita para a esquerda.

Da mesma maneira, o Square também fica fixo na parte esquerda de tela, podendo apenas locomover-se perante o eixo Y, ou seja, para cima e para baixo.

## Comandos

| Tecla                     | Descrição                                               |
|:--------------------------|:--------------------------------------------------------|
| Barra de espaço           | Ao ser pressionada após solicitada pelo jogo, o inicia. |
| Direcional para cima (^)  | Locomove o Square para cima                             |
| Direcional para baixo (v) | Locomove o Square para baixo                            |

## Square

🟧 🟩 🟪 ⬛ 🟥 🟨 🟦 🟫 ⬜

Para definição de um corpo quadrado, foi necessário definir as coordenadas de posição como mostra o código abaixo:

```cpp
  std::array<glm::vec2, 16> positions{
      // Square
      glm::vec2{-30.0f, +15.0f}, glm::vec2{+30.0f, +15.0f},
      glm::vec2{+30.0f, -15.0f}, glm::vec2{-30.0f, -15.0f}
    
  };
```

Além disso, para adequar o personagem ao acréscimo de velocidade de seus oponentes, também é incrementada a velocidade de locomoção diante do eixo Y

```cpp
if (gameData.m_input[static_cast<size_t>(Input::Up)] &&
      gameData.m_state == State::Playing) {
    // Definindo a altura máxima
    if(m_translation.y < 0.88) {
     // m_translation.y += deltaTime * m_velocity.x;
    m_translation.y +=  m_velocity.x * (deltaTime * 1.2f);
    }
  }
```

> O código acima também é replicado para a ação de ir para baixo.

Sua coloração é definida de maneira aleatória, pelo seguinte código:

```cpp
glm::vec4 Square::getRandomVertexColor() {
  std::uniform_real_distribution<float> color_rd(0.0f, 1.0f);
  return glm::vec4{color_rd(m_randomEngine), color_rd(m_randomEngine),
                   color_rd(m_randomEngine), 1.0f};
}
```

## Triângulos

🔺 🔻

Para acrescentar dinamismo ao jogo, é incrementado gradativamente velocidade aos triângulos, fazendo com que o usuário precise se adaptar a nova velocidade.

```cpp
if (gameData.m_state == State::Playing) {
      asteroid.m_velocity += asteroid.m_velocity * (deltaTime * 0.09f);
}
```

Para construção do mesmo, é definido primordialmente a quantidade de lados do polígono, e depois utilizado funções da biblioteca OpenGl.

## Tempo de sobrevivência

Ao iniciar o jogo, é possível observar o tempo em que o Square está vivo. A ideia é que esse valor sirva de maneira como uma pontuação, e que a competitividade dos jogadores seja baseada nesse valor.

```cpp
// definição da variável no arquivo openglwindow.hpp
float m_surviveTime{};

// Utilização da função ImGui para disponibilizar o valor de tempo no canto inferior direito da tela.
// arquivo openglwindow.cpp
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
```


## Autores

| Nome                    | RA          |
|:------------------------|:------------|
| Gabriel Nigro Garbelini | 11201810443 |
| Joanis Basile           | 11201810529 |