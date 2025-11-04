#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 10
#define MAX_NOME 100
#define MAX_PISTA 200

// Estrutura para representar uma sala na mansao (arvore binaria)
typedef struct Sala {
    char nome[MAX_NOME];
    char pista[MAX_PISTA];
    struct Sala* esquerda;
    struct Sala* direita;
} Sala;

// Estrutura para representar um no da arvore de pistas (BST)
typedef struct PistaNode {
    char texto[MAX_PISTA];
    struct PistaNode* esquerda;
    struct PistaNode* direita;
} PistaNode;

// Estrutura para lista encadeada de suspeitos (para tratamento de colisao na hash)
typedef struct SuspeitoNode {
    char nome[MAX_NOME];
    int contador;
    struct SuspeitoNode* prox;
} SuspeitoNode;

// Estrutura para entrada da tabela hash
typedef struct HashEntry {
    char pista[MAX_PISTA];
    SuspeitoNode* suspeitos;
} HashEntry;

// Tabela hash global
HashEntry* tabelaHash[HASH_SIZE];

// Funcao para criar uma sala dinamicamente
Sala* criarSala(const char* nome, const char* pista) {
    Sala* novaSala = (Sala*)malloc(sizeof(Sala));
    if (novaSala == NULL) {
        printf("Erro ao alocar memoria para sala.\n");
        exit(1);
    }
    strcpy(novaSala->nome, nome);
    if (pista != NULL) {
        strcpy(novaSala->pista, pista);
    } else {
        strcpy(novaSala->pista, "");
    }
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

// Funcao para inserir pista na arvore BST
PistaNode* inserirPista(PistaNode* raiz, const char* texto) {
    if (raiz == NULL) {
        PistaNode* novaPista = (PistaNode*)malloc(sizeof(PistaNode));
        if (novaPista == NULL) {
            printf("Erro ao alocar memoria para pista.\n");
            exit(1);
        }
        strcpy(novaPista->texto, texto);
        novaPista->esquerda = NULL;
        novaPista->direita = NULL;
        return novaPista;
    }

    int comparacao = strcmp(texto, raiz->texto);
    if (comparacao < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, texto);
    } else if (comparacao > 0) {
        raiz->direita = inserirPista(raiz->direita, texto);
    }
    // Se comparacao == 0, a pista ja existe, nao inserimos duplicata

    return raiz;
}

// Funcao para exibir pistas em ordem alfabetica (percurso em ordem)
void exibirPistas(PistaNode* raiz) {
    if (raiz != NULL) {
        exibirPistas(raiz->esquerda);
        printf("  - %s\n", raiz->texto);
        exibirPistas(raiz->direita);
    }
}

// Funcao hash simples (soma dos valores ASCII mod HASH_SIZE)
int funcaoHash(const char* chave) {
    int soma = 0;
    for (int i = 0; chave[i] != '\0'; i++) {
        soma += (unsigned char)chave[i];
    }
    return soma % HASH_SIZE;
}

// Funcao para inicializar a tabela hash
void inicializarHash() {
    for (int i = 0; i < HASH_SIZE; i++) {
        tabelaHash[i] = NULL;
    }
}

// Funcao para inserir associacao pista-suspeito na tabela hash
void inserirNaHash(const char* pista, const char* suspeito) {
    int indice = funcaoHash(pista);

    // Verifica se ja existe entrada para esta pista
    if (tabelaHash[indice] == NULL) {
        tabelaHash[indice] = (HashEntry*)malloc(sizeof(HashEntry));
        strcpy(tabelaHash[indice]->pista, pista);
        tabelaHash[indice]->suspeitos = NULL;
    }

    // Procura o suspeito na lista encadeada
    SuspeitoNode* atual = tabelaHash[indice]->suspeitos;
    SuspeitoNode* anterior = NULL;

    while (atual != NULL) {
        if (strcmp(atual->nome, suspeito) == 0) {
            atual->contador++;
            return;
        }
        anterior = atual;
        atual = atual->prox;
    }

    // Suspeito nao encontrado, cria novo
    SuspeitoNode* novoSuspeito = (SuspeitoNode*)malloc(sizeof(SuspeitoNode));
    strcpy(novoSuspeito->nome, suspeito);
    novoSuspeito->contador = 1;
    novoSuspeito->prox = NULL;

    if (anterior == NULL) {
        tabelaHash[indice]->suspeitos = novoSuspeito;
    } else {
        anterior->prox = novoSuspeito;
    }
}

// Funcao para encontrar suspeito associado a uma pista
char* encontrarSuspeito(const char* pista) {
    int indice = funcaoHash(pista);

    if (tabelaHash[indice] != NULL && strcmp(tabelaHash[indice]->pista, pista) == 0) {
        if (tabelaHash[indice]->suspeitos != NULL) {
            return tabelaHash[indice]->suspeitos->nome;
        }
    }

    return NULL;
}

// Funcao para contar pistas associadas a um suspeito especifico
int contarPistasPorSuspeito(const char* suspeito) {
    int total = 0;

    for (int i = 0; i < HASH_SIZE; i++) {
        if (tabelaHash[i] != NULL) {
            SuspeitoNode* atual = tabelaHash[i]->suspeitos;
            while (atual != NULL) {
                if (strcmp(atual->nome, suspeito) == 0) {
                    total += atual->contador;
                }
                atual = atual->prox;
            }
        }
    }

    return total;
}

// Funcao para explorar as salas da mansao
void explorarSalas(Sala* salaAtual, PistaNode** arvorePistas) {
    if (salaAtual == NULL) {
        printf("Voce chegou a um beco sem saida.\n");
        return;
    }

    printf("\n=== Voce esta na sala: %s ===\n", salaAtual->nome);

    // Verifica se ha pista nesta sala
    if (strlen(salaAtual->pista) > 0) {
        printf(">>> Pista encontrada: %s\n", salaAtual->pista);
        *arvorePistas = inserirPista(*arvorePistas, salaAtual->pista);

        // Associa pista a suspeito baseado em regras
        if (strstr(salaAtual->pista, "cigarro") || strstr(salaAtual->pista, "cinzas")) {
            inserirNaHash(salaAtual->pista, "Sr. Monteiro");
        } else if (strstr(salaAtual->pista, "perfume") || strstr(salaAtual->pista, "lenco")) {
            inserirNaHash(salaAtual->pista, "Sra. Oliveira");
        } else if (strstr(salaAtual->pista, "lama") || strstr(salaAtual->pista, "botas")) {
            inserirNaHash(salaAtual->pista, "Jardineiro");
        } else if (strstr(salaAtual->pista, "livro") || strstr(salaAtual->pista, "papel")) {
            inserirNaHash(salaAtual->pista, "Bibliotecaria");
        } else if (strstr(salaAtual->pista, "faca") || strstr(salaAtual->pista, "sangue")) {
            inserirNaHash(salaAtual->pista, "Chef");
        } else {
            inserirNaHash(salaAtual->pista, "Desconhecido");
        }
    }

    // Menu de navegacao
    while (1) {
        printf("\nOpcoes:\n");
        if (salaAtual->esquerda != NULL) {
            printf("  [e] Ir para a esquerda\n");
        }
        if (salaAtual->direita != NULL) {
            printf("  [d] Ir para a direita\n");
        }
        printf("  [s] Sair e continuar exploracao\n");
        printf("Escolha: ");

        char escolha;
        scanf(" %c", &escolha);

        if (escolha == 'e' && salaAtual->esquerda != NULL) {
            explorarSalas(salaAtual->esquerda, arvorePistas);
            printf("\n=== Voce voltou para: %s ===\n", salaAtual->nome);
        } else if (escolha == 'd' && salaAtual->direita != NULL) {
            explorarSalas(salaAtual->direita, arvorePistas);
            printf("\n=== Voce voltou para: %s ===\n", salaAtual->nome);
        } else if (escolha == 's') {
            break;
        } else {
            printf("Opcao invalida ou caminho nao disponivel.\n");
        }
    }
}

// Funcao para verificar acusacao final
void verificarSuspeitoFinal(PistaNode* arvorePistas) {
    printf("\n\n========================================\n");
    printf("FASE FINAL: ACUSACAO\n");
    printf("========================================\n");

    printf("\nPistas coletadas durante a investigacao:\n");
    exibirPistas(arvorePistas);

    printf("\n\nSuspeitos conhecidos:\n");
    printf("  1. Sr. Monteiro\n");
    printf("  2. Sra. Oliveira\n");
    printf("  3. Jardineiro\n");
    printf("  4. Bibliotecaria\n");
    printf("  5. Chef\n");

    printf("\nQuem voce acusa como culpado? ");
    char acusado[MAX_NOME];
    scanf(" %[^\n]", acusado);

    int pistasDoAcusado = contarPistasPorSuspeito(acusado);

    printf("\n========================================\n");
    printf("VEREDITO\n");
    printf("========================================\n");
    printf("Pistas apontando para %s: %d\n", acusado, pistasDoAcusado);

    if (pistasDoAcusado >= 2) {
        printf("\nParabens! Voce reuniu evidencias suficientes!\n");
        printf("O juiz aceita sua acusacao contra %s.\n", acusado);
        printf("O caso foi resolvido com sucesso!\n");
    } else {
        printf("\nEvidencias insuficientes!\n");
        printf("Voce precisa de pelo menos 2 pistas para sustentar a acusacao.\n");
        printf("O caso permanece em aberto...\n");
    }
    printf("========================================\n");
}

// Funcao principal
int main() {
    printf("========================================\n");
    printf("   DETECTIVE QUEST - MYSTERY MANSION\n");
    printf("========================================\n");
    printf("Bem-vindo, detetive!\n");
    printf("Explore a mansao, colete pistas e descubra o culpado.\n\n");

    // Inicializa a tabela hash
    inicializarHash();

    // Arvore de pistas coletadas (BST)
    PistaNode* arvorePistas = NULL;

    // Construcao do mapa da mansao (arvore binaria)
    Sala* hallEntrada = criarSala("Hall de Entrada", "");

    // Nivel 1 - esquerda e direita do hall
    Sala* biblioteca = criarSala("Biblioteca", "Um livro aberto com paginas rasgadas");
    Sala* cozinha = criarSala("Cozinha", "Uma faca com manchas de sangue");
    hallEntrada->esquerda = biblioteca;
    hallEntrada->direita = cozinha;

    // Nivel 2 - filhos da biblioteca
    Sala* escritorio = criarSala("Escritorio", "Cinzas de cigarro no cinzeiro");
    Sala* salaEstar = criarSala("Sala de Estar", "Um lenco perfumado esquecido no sofa");
    biblioteca->esquerda = escritorio;
    biblioteca->direita = salaEstar;

    // Nivel 2 - filhos da cozinha
    Sala* despensa = criarSala("Despensa", "");
    Sala* jardim = criarSala("Jardim", "Marcas de lama e botas sujas");
    cozinha->esquerda = despensa;
    cozinha->direita = jardim;

    // Nivel 3 - folhas
    Sala* sotao = criarSala("Sotao", "Um cigarro apagado no chao");
    Sala* quarto = criarSala("Quarto", "Frasco de perfume caro sobre a comoda");
    escritorio->esquerda = sotao;
    salaEstar->direita = quarto;

    // Inicia a exploracao
    explorarSalas(hallEntrada, &arvorePistas);

    // Fase de acusacao
    if (arvorePistas != NULL) {
        verificarSuspeitoFinal(arvorePistas);
    } else {
        printf("\nVoce nao coletou nenhuma pista!\n");
        printf("Explore mais a mansao antes de fazer uma acusacao.\n");
    }

    printf("\nObrigado por jogar Detective Quest!\n");

    return 0;
}
