# 🕵️‍♂️ Jogo de Investigação: O Mistério da Mansão

Este é um jogo de texto interativo desenvolvido em **C** que simula uma investigação criminal dentro de uma mansão. O jogador assume o papel de um detetive, explorando diferentes cômodos, coletando pistas ocultas e, por fim, acusando o verdadeiro culpado com base nas evidências encontradas.

O projeto foi construído focando na aplicação prática de **estruturas de dados avançadas**, como Árvores Binárias e Tabelas Hash.

---

## 🛠️ Recursos e Estruturas de Dados

Para simular o ambiente e a lógica do jogo, foram utilizadas três estruturas principais:

1. **Árvore Binária de Busca com Ponteiro para Pai (Mapa da Mansão):** * Representa os cômodos da mansão. Cada quarto (`Sala`) aponta para seus filhos (`esquerda` e `direita`), permitindo avançar, e mantém uma referência para o `pai`, o que possibilita ao jogador a ação de voltar ao cômodo anterior.
2. **Árvore Binária de Busca - BST (Inventário de Pistas):**
   * Armazena as pistas coletadas pelo jogador de forma ordenada alfabeticamente. Garante que a exibição das pistas seja feita de maneira organizada ao final do jogo.
3. **Tabela Hash com Encadeamento Exterior (Mapeamento de Suspeitos):**
   * Associa cada pista encontrada a um suspeito específico (ex: "Arma do crime" $\rightarrow$ "Baba"). Permite uma busca rápida com complexidade média $O(1)$ para validar a acusação do jogador.

---

## 🗺️ Mapa da Mansão

A exploração segue a hierarquia de árvore representada abaixo:

```text
                        Hall de Entrada
                               |
                    -------------------------
                    |                       |
               Sala de Estar           Biblioteca
                    |                       |
              -------------            ------------
              |           |            |          |
           Suite 1     Suite 2       Copa    Escritorio
              |           |                       |
          ---------   ---------               ----------
          |       |   |       |               |        |
       Sacada 1       Sacada 2              Porao