# [Head 'n Tails](https://gabriel-nigro.github.io/abcg/headnTails/index.html)

## Índice

* [Inspirações](#inspirações)
* [Como jogar?](#como-jogar-?)
* [Coin](#coin)
* [Definição de vencedor](#definição-de-vencedor)
* [Autores](#autores)

O projeto trata-se de uma simulação de um jogo bem popular, o famoso "Cara ou Coroa". Nele, o usuário consegue interagir com um objeto 3D, no caso uma moeda, o qual realiza rotações contínuas até ser indicada pelo usuário para parar.

## Inspirações

Ao pesquisar por "Cara ou Coroa" no [Google](https://www.google.com/search?q=Cara+ou+Coroa), é possível visualizar um jogo interativo provido pela própria plataforma de pesquisa, o qual o comportamento assemelha-se ao projeto.  

## Como jogar?

A mecânica de jogo é bem simples e intuitiva. Basta selecionar a face, tendo como opções "Cara" ou "Coroa" e, após selecionada, marcar a opção de "Parar a moeda". Assim, o jogo indicará o vencedor na caixa de texto abaixo da moeda.

Para que a moeda volte a girar, basta desmarcar o "checkbox" e, ao marcá-lo novamente, aparecerá o novo vencedor.

## Coin

📀 O objeto "coin" foi construído a partir do [Blender](https://www.blender.org/) e disponibilizado gratuitamente na internet. Entretanto foi necessário realizar a edição do mesmo, no qual foram trocadas todas as cordenadas do eixo Y para o eixo Z, para que a moeda tenha a orientação de ficar "de pé".

🔄 A rotação do objeto fica a cargo da função `paintGL`, no seguinte trecho de código:

```cpp
// Animate angle by 360 degrees per second
  const float deltaTime{static_cast<float>(getDeltaTime())};
  m_angle = glm::wrapAngle(m_angle + glm::radians(360.0f) * deltaTime);
```

Para que o interrompimento da moeda seja interativo faz-se necessário utilizar a biblioteca `ImGui` do OpenGL, criando um "checkbox", armazenando valores booleanos em uma variável. A partir desse valor, é redefinida a variável de rotação de acordo com o resultado do jogo.

```cpp
// Checkbox armazenando valores booleanos na variável stop
static bool stop{};
ImGui::Checkbox("Parar a moeda", &stop);

// Caso o usuário interrompa a rotação
if (stop) {
  // Caso não seja um empate
  if (!m_draw) {
    // Pare a moeda
    m_angle = 0;
  // Caso seja um empate
  } else {
    // Gire a moeda para que ela fique em 90°
    m_angle = glm::wrapAngle(glm::radians(90.0f));
  }
// Caso ainda não haja interação do usuário para interrompimento da moeda
} else {
  m_draw = false;
  m_randomNumber = (rand() % 10) + 1;
}
```

## Definição de vencedor

Enquanto a moeda está girando, o código gera números aleatórios de 1 à 10 inúmeras vezes, os quais são armazenados em uma variável. Isto é necessário para que haja o caráter randômico característico do jogo. 

```cpp
// Armazena números aleatórios de 1 à 10 na variável m_randomNumber
m_randomNumber = (rand() % 10) + 1;
```

No momento em que a rotação da moeda é interrompida, define-se o vencedor, sendo o resultado orientado pela tabela abaixo:

| Número Aleatório | Vencedor | 
| :--------------- | :------- |
| < 5              | Cara     |
| > 5              | Coroa    |
| = 5              | Empate   |

Sabe-se que, em um jogo real de cara ou coroa é praticamente impossível que haja empate. Entretanto, para podermos explorar mais o objeto e a biblioteca utilizada, além de garantir chances iguais para ambas faces da moeda, criamos a possibilidade de empate.

## Autores

| Nome                    | RA          |
|:------------------------|:------------|
| Gabriel Nigro Garbelini | 11201810443 |
| Joanis Basile           | 11201810529 |