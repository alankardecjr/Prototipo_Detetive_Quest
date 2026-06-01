#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_STR 100
#define TAM_HASH 10

// ---------------------------------------------------------------
// ESTRUTURAS DE DADOS
// ---------------------------------------------------------------

typedef struct Sala {
    char nome[MAX_STR];
    char pista[MAX_STR];
    struct Sala* esquerda;
    struct Sala* direita;
    struct Sala* pai;
} Sala;

typedef struct PistaNode {
    char pista[MAX_STR];
    struct PistaNode* esquerda;
    struct PistaNode* direita;
} PistaNode;

typedef struct HashNode {
    char pista[MAX_STR];
    char suspeito[MAX_STR];
    struct HashNode* prox;
} HashNode;

// ---------------------------------------------------------------
// FUNÇÕES AUXILIARES
// ---------------------------------------------------------------

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

Sala* criarSala(const char* nome, const char* pista) {
    Sala* nova = (Sala*) malloc(sizeof(Sala));
    if (!nova) {
        fprintf(stderr, ">>> ERRO: Falha ao alocar memoria para sala %s\n", nome);
        exit(1);
    }
    strncpy(nova->nome, nome, MAX_STR - 1);
    nova->nome[MAX_STR - 1] = '\0';
    
    if (pista != NULL) {
        strncpy(nova->pista, pista, MAX_STR - 1);
        nova->pista[MAX_STR - 1] = '\0';
    } else {
        nova->pista[0] = '\0';
    }

    nova->esquerda = nova->direita = nova->pai = NULL;
    return nova;
}

// ---------------------------------------------------------------
// ÁRVORE DE PISTAS (BST)
// ---------------------------------------------------------------

PistaNode* criarPistaNode(const char* pista) {
    PistaNode* novo = (PistaNode*) malloc(sizeof(PistaNode));
    if (!novo) {
        fprintf(stderr, ">>> ERRO: Falha ao alocar memoria para pista\n");
        exit(1);
    }
    strncpy(novo->pista, pista, MAX_STR - 1);
    novo->pista[MAX_STR - 1] = '\0';
    novo->esquerda = novo->direita = NULL;
    return novo;
}

PistaNode* inserirPista(PistaNode* raiz, const char* pista) {
    if (raiz == NULL)
        return criarPistaNode(pista);
    if (strcmp(pista, raiz->pista) < 0)
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    else if (strcmp(pista, raiz->pista) > 0)
        raiz->direita = inserirPista(raiz->direita, pista);
    return raiz;
}

void exibirPistas(PistaNode* raiz) {
    if (raiz != NULL) {
        exibirPistas(raiz->esquerda);
        printf("- %s\n", raiz->pista);
        exibirPistas(raiz->direita);
    }
}

void liberarPistas(PistaNode* raiz) {
    if (raiz != NULL) {
        liberarPistas(raiz->esquerda);
        liberarPistas(raiz->direita);
        free(raiz);
    }
}

void liberarSalas(Sala* raiz) {
    if (raiz != NULL) {
        liberarSalas(raiz->esquerda);
        liberarSalas(raiz->direita);
        free(raiz);
    }
}

void liberarTabelaHash(HashNode* tabela[]) {
    for (int i = 0; i < TAM_HASH; i++) {
        HashNode* atual = tabela[i];
        while (atual != NULL) {
            HashNode* aux = atual;
            atual = atual->prox;
            free(aux);
        }
        tabela[i] = NULL;
    }
}

// ---------------------------------------------------------------
// TABELA HASH PARA PISTAS -> SUSPEITOS
// ---------------------------------------------------------------

int hash(const char* str) {
    int soma = 0;
    for (int i = 0; str[i] != '\0'; i++)
        soma += str[i];
    return soma % TAM_HASH;
}

void inserirNaHash(HashNode* tabela[], const char* pista, const char* suspeito) {
    int idx = hash(pista);
    HashNode* novo = (HashNode*) malloc(sizeof(HashNode));
    if (!novo) exit(1);
    strncpy(novo->pista, pista, MAX_STR - 1);
    novo->pista[MAX_STR - 1] = '\0';
    strncpy(novo->suspeito, suspeito, MAX_STR - 1);
    novo->suspeito[MAX_STR - 1] = '\0';
    novo->prox = tabela[idx];
    tabela[idx] = novo;
}

const char* encontrarSuspeito(HashNode* tabela[], const char* pista) {
    int idx = hash(pista);
    HashNode* atual = tabela[idx];
    while (atual != NULL) {
        if (strcmp(atual->pista, pista) == 0)
            return atual->suspeito;
        atual = atual->prox;
    }
    return NULL;
}

// ---------------------------------------------------------------
// JULGAMENTO
// ---------------------------------------------------------------

int contarPistasPorSuspeito(PistaNode* raiz, HashNode* tabela[], const char* suspeito) {
    if (raiz == NULL)
        return 0;

    int conta = 0;
    const char* suspeitoAtual = encontrarSuspeito(tabela, raiz->pista);
    if (suspeitoAtual && strcmp(suspeitoAtual, suspeito) == 0)
        conta = 1;

    return conta +
           contarPistasPorSuspeito(raiz->esquerda, tabela, suspeito) +
           contarPistasPorSuspeito(raiz->direita, tabela, suspeito);
}

void julgamentoFinal(PistaNode* pistas, HashNode* tabela[]) {
    char suspeito[MAX_STR];
    printf("\n===============================================================\n");
    printf("                 >>> JULGAMENTO FINAL <<<                      \n");
    printf("===============================================================\n");
    printf(">>> Acuse um suspeito baseado nas pistas:\n");
    printf(">>> Suspeitos possiveis: Mordomo, Secretaria, Jardineiro, Baba\n");
    printf(">>> Digite o nome do suspeito: ");
    
    if (fgets(suspeito, MAX_STR, stdin) != NULL) {
        suspeito[strcspn(suspeito, "\n")] = '\0';

        int total = contarPistasPorSuspeito(pistas, tabela, suspeito);
        if (total >= 2)
            printf("\n>>> Acusacao consistente! %s foi considerado CULPADO!\n", suspeito);
        else
            printf("\n>>> Acusacao fraca. %s foi considerado INOCENTE.\n", suspeito);
    }
}

// ---------------------------------------------------------------
// EXPLORAÇÃO COM MENU (Agora retorna bool)
// ---------------------------------------------------------------

bool explorarSalasComPistas(Sala* atual, PistaNode** pistasColetadas, HashNode* tabela[]) {
    printf("\n>>> Inicio da exploracao <<<\n");

    while (atual != NULL) {
        printf("\n>>> Voce esta: %s\n", atual->nome);

        if (strlen(atual->pista) > 0) {
            printf(">>> Pista encontrada: \"%s\"\n", atual->pista);
            *pistasColetadas = inserirPista(*pistasColetadas, atual->pista);
            atual->pista[0] = '\0';  
        } else {
            printf(">>> Nenhuma pista neste comodo.\n");
        }

        printf("\n>>> Escolha uma direcao ou acao:\n");
        if (atual->esquerda) printf("   (e) - Esquerda: %s\n", atual->esquerda->nome);
        if (atual->direita)  printf("   (d) - Direita:  %s\n", atual->direita->nome);
        if (atual->pai)      printf("   (v) - Voltar para: %s\n", atual->pai->nome);
        printf("   (p) - Ver pistas coletadas\n");
        printf("   (a) - Acusar um suspeito\n");
        printf("   (s) - Sair da exploracao\n");

        printf(">>> Opcao: ");
        char opcao;
        scanf(" %c", &opcao);
        limparBuffer();

        if (opcao == 'e' && atual->esquerda) {
            atual = atual->esquerda;
        } else if (opcao == 'd' && atual->direita) {
            atual = atual->direita;
        } else if (opcao == 'v' && atual->pai) {
            atual = atual->pai;
        } else if (opcao == 'p') {
            printf("\n>>> Pistas coletadas ate agora:\n");
            if (*pistasColetadas)
                exibirPistas(*pistasColetadas);
            else
                printf(">>> Nenhuma pista coletada ainda.\n");
        } else if (opcao == 'a') {
            return true; // Vai para o julgamento
        } else if (opcao == 's') {
            printf("\n>>> Exploracao encerrada pelo jogador <<<\n");
            return false; // Sai sem julgar
        } else {
            printf(">>> Comando invalido ou caminho inexistente.\n");
        }
    }
    return false;
}

// ---------------------------------------------------------------
// FUNÇÃO PRINCIPAL
// ---------------------------------------------------------------

int main() {
    Sala* hall        = criarSala("Hall de Entrada", "Pegadas de lama fresca");
    Sala* salaEstar   = criarSala("Sala de Estar", "Bilhete amassado");
    Sala* biblioteca  = criarSala("Biblioteca", "Agenda com pagina faltando");
    Sala* suite1      = criarSala("Suite 1", "Comoda revirada");
    Sala* sacada1     = criarSala("Sacada 1", "");
    Sala* suite2      = criarSala("Suite 2", "");
    Sala* sacada2     = criarSala("Sacada 2", "Luvas de couro");
    Sala* escritorio  = criarSala("Escritorio", "Lencos com manchas de sangue");
    Sala* copa        = criarSala("Copa", "Xicaras quebradas");
    Sala* porao       = criarSala("Porao", "Arma do crime");

    hall->esquerda = salaEstar;
    hall->direita = biblioteca;
    salaEstar->esquerda = suite1;
    suite1->esquerda = sacada1;
    salaEstar->direita = suite2;
    suite2->direita = sacada2;
    biblioteca->esquerda = copa;
    biblioteca->direita = escritorio;
    escritorio->direita = porao;

    salaEstar->pai = hall;
    biblioteca->pai = hall;
    suite1->pai = salaEstar;
    sacada1->pai = suite1;
    suite2->pai = salaEstar;
    sacada2->pai = suite2;
    copa->pai = biblioteca;
    escritorio->pai = biblioteca;
    porao->pai = escritorio;

    printf("\n===============================================================\n");
    printf("                      >>> Mapa da Mansao <<<                   \n");
    printf("===============================================================\n");
    printf("\n                        Hall de Entrada\n");
    printf("                               |\n");
    printf("                  -------------------------\n");
    printf("                  |                       |\n");
    printf("             Sala de Estar           Biblioteca\n");
    printf("                  |                       |\n");
    printf("            -------------            ------------\n");
    printf("            |           |            |          |\n");
    printf("         Suite 1     Suite 2       Copa    Escritorio\n");
    printf("            |           |                       |\n");
    printf("        ---------   ---------               ----------\n");
    printf("        |       |   |        |              |        |\n");
    printf("     Sacada 1       Sacada 2                  Porao\n");
    printf("\n===============================================================\n");
    printf("        >>> Estrutura da mansao criada com sucesso <<<         \n");
    printf("===============================================================\n");

    PistaNode* pistas = NULL;
    HashNode* tabelaHash[TAM_HASH] = {NULL};

    inserirNaHash(tabelaHash, "Pegadas de lama fresca", "Jardineiro");
    inserirNaHash(tabelaHash, "Bilhete amassado", "Baba");
    inserirNaHash(tabelaHash, "Agenda com pagina faltando", "Secretaria");
    inserirNaHash(tabelaHash, "Comoda revirada", "Mordomo");
    inserirNaHash(tabelaHash, "Luvas de couro", "Jardineiro");
    inserirNaHash(tabelaHash, "Lencos com manchas de sangue", "Baba");
    inserirNaHash(tabelaHash, "Xicaras quebradas", "Secretaria");
    inserirNaHash(tabelaHash, "Arma do crime", "Baba");

    // Recebe o resultado da exploração
    bool irParaJulgamento = explorarSalasComPistas(hall, &pistas, tabelaHash);

    if (irParaJulgamento) {
        printf("\n===============================================================\n");
        printf("               >>> PISTAS COLETADAS EM ORDEM <<<               \n");
        printf("===============================================================\n");

        if (pistas != NULL)
            exibirPistas(pistas);
        else
            printf(">>> Nenhuma pista foi coletada.\n");

        julgamentoFinal(pistas, tabelaHash);
    }

    // Liberação cuidadosa da memória em qualquer um dos cenários
    liberarSalas(hall);
    liberarPistas(pistas);
    liberarTabelaHash(tabelaHash);

    printf("\n===============================================================\n");
    printf("       >>> Fim da investigacao. Boa sorte, detetive! <<<       \n");
    printf("===============================================================\n");

    return 0;
}