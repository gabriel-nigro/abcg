# [Dust2 - Weapon Inspect](https://gabriel-nigro.github.io/abcg/headnTails/index.html)

O projeto trata-se de um visualizador de equipamento simples situado em um determinado cenário, no qual o é possível observar os detalhes de textura do objeto modelado, sendo capaz de rotacionar o equipamento a partir das ações do mouse.   



## Índice

* [Inspirações](#inspirações)
* [How to](#how-to)
* [Cenário](#cenário)
* [AK-47](#ak-47)
* [Apêndice](#apêndice)
* [Autores](#autores)

## Inspirações

O projeto visa ser análogo ao inspetor de equipamentos presente no jogo [Counter-Strike: Global Offensive](https://store.steampowered.com/app/730/CounterStrike_Global_Offensive/). Entretanto, no jogo é necessário que o usuário esteja em uma partida para inspecionar uma arma, o qual não é tão prático caso haja apenas a necessidade de visualização. 

![Inspect](https://i.ytimg.com/vi/vKdWVjcYDAo/maxresdefault.jpg)

> Visualizador de equipamento. Fonte: https://i.ytimg.com/vi/vKdWVjcYDAo/maxresdefault.jpg

## How To

Ao posicionar o mouse sobre a aplicação, basta clicar com o botão direito e arrastá-lo de acordo com a angulação desejada, de maneira que o objeto será rotacionado e disponibilizará a visualização por completo sobre o equipamento, trazendo a noção de inspeção desejada.

Isso é possível graças a função de `handleEvent` do OpenGL, obtendo o estado do mouse e suas ações.

```cpp
// openglwindow.cpp
void OpenGLWindow::handleEvent(SDL_Event &event) {
  // ...
  // Obtém os estados do mouse
  SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

  // Exemplo de captura das ações, no caso a movimentação
  if (event.type == SDL_MOUSEMOTION) {
    m_trackBallModel.mouseMove(mousePosition);
    m_trackBallLight.mouseMove(mousePosition);
  }
  // ...
}

```

## Cenário

A aplicação situa-se em um determinado cenário do mapa [Dust II](https://en.wikipedia.org/wiki/Dust_II). Para isso, foi necessário realizar o download e o resize da imagem do mapa, alocando-o no diretório `assets/maps/cube`

O mapeamento do cubo foi feito pelo seguinte trecho de código:
```cpp
// model.cpp
void Model::loadCubeTexture(const std::string& path) {
 // ...
 // ...

  // Realizada a leitura dos eixos do mapa referente ao cenário da Dust II
  m_cubeTexture = abcg::opengl::loadCubemap(
      {path + "posx.jpg", path + "negx.jpg", path + "posy.jpg",
       path + "negy.jpg", path + "posz.jpg", path + "negz.jpg"});
}
```

## AK-47

O objeto do equipamento foi construído a partir da tecnologia Wavefront e disponibilizado gratuitamente na internet. [Link](https://free3d.com/3d-model/ak-47--520670.html)

O mesmo contém os arquivos `.obj` e `.mtl`, o qual realiza o mapeamento das cores e das texturas. Podemos observar o seguinte trecho de código, o qual é responsável por aplicar a refletividade difusa do material, declarando a definição do material `map_Kd`:

```cpp
// ak.mtl
map_Kd ak_color.png
```

Assim, as declarações do arquivo `.mtl` disponibilizam as características de cores, texturas e reflexão do objeto.

Para a leitura do objeto, é executada a função `loadModel`, a qual também disponibiliza o "shader - texture"

```cpp
// openglwindow.cpp

 // Load default model
loadModel(getAssetsPath() + "ak.obj");

// ....
void OpenGLWindow::loadModel(std::string_view path) {
// ...
m_model.loadDiffuseTexture(getAssetsPath() + "maps/ak_color.png");
// ...
}
```

## Flying AKs

Para criar um efeito visual de projeção de perspectiva sobre a aplicação, é utilizada a técnica do ["efeito starfield"](http://professor.ufabc.edu.br/~harlen.batagelo/cg/starfield.html) vista no decorrer da disciplina. Dessa forma, são criada várias AKs, apelidadas de Flying AKs

Uma vez que declaramos uma variável contendo o número de AKs a serem renderizadas, as mesmas são iteradas e lidas, disponibilizando-as de maneira aleatória pela área da aplicação.
```cpp
// openglwindow.hpp
 
// Flying AKs
static const int m_flyingAks{50};

//openglwindow.cpp

for (const auto index : iter::range(m_flyingAks)) {
  // ...
  randomizeFlyingAks(position, rotation);
}
```

## Apêndice

Ao iniciarmos a aplicação, vimos que, por tratar-se de um objeto rico em detalhes e de custosa renderização, a mesma possui certa lentidão. Isso ocorre pela limitação física de processamento da máquina em que está sendo executada. Como "trabalhos futuros", vê-se a necessidade de otimizar o projeto para que haja melhor experiência para o usuário.

## Autores

| Nome                    | RA          |
|:------------------------|:------------|
| Gabriel Nigro Garbelini | 11201810443 |
| Joanis Basile           | 11201810529 |