#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -----------------------------
//      PROTÓTIPOS
// -----------------------------
void limpaBuffer();
void lerString(char *str, int tamanho);
int tamanho(FILE *arq);
void cadastrar(FILE *arq);
void consultar(FILE *arq);
FILE* excluir(FILE * arq);
void gerarRelatorio(FILE * arq);
void listar(FILE *arq);

// -----------------------------
//      STRUCT PRINCIPAL
// -----------------------------
typedef struct {
    char titulo[50];
    char autor[50];
    int isbn;
} Livro;

// -----------------------------
//      CAMINHOS DOS ARQUIVOS
// -----------------------------
#define ARQ_BIN "C:\\\\Ling_C\\\\biblioteca.bin"
#define ARQ_TEMP "C:\\\\Ling_C\\\\temp.bin"
#define ARQ_TXT "C:\\\\Ling_C\\\\relatorio.txt"

// -----------------------------
//      FUNÇÃO PRINCIPAL
// -----------------------------
int main() {

    FILE *arq = fopen(ARQ_BIN, "r+b");

    if (arq == NULL) {
        arq = fopen(ARQ_BIN, "w+b");
        if (arq == NULL) {
            printf("Erro ao criar arquivo!\n");
            exit(1);
        }
    }

    int opcao;

    do {
        printf("\n===== BIBLIOTECA PESSOAL =====\n");
        printf("1 - Cadastrar Livro\n");
        printf("2 - Consultar Livro\n");
        printf("3 - Quantidade de Registros\n");
        printf("4 - Excluir Livro\n");
        printf("5 - Gerar Relatorio (TXT)\n");
        printf("6 - Listar Todos os Livros\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);
        limpaBuffer();

        switch (opcao) {
            case 1: cadastrar(arq); break;
            case 2: consultar(arq); break;
            case 3: printf("Total de registros: %d\n", tamanho(arq)); break;
            case 4: arq = excluir(arq); break;
            case 5: gerarRelatorio(arq); break;
            case 6: listar(arq); break;
            case 0: printf("Saindo...\n"); break;
            default: printf("Opcao invalida!\n");
        }

    } while (opcao != 0);

    fclose(arq);
    return 0;
}

// -----------------------------
//      FUNÇÃO limpaBuffer()
// -----------------------------
void limpaBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// -----------------------------
//      Leitura Segura de Strings
// -----------------------------
void lerString(char *str, int tamanho) {
    fgets(str, tamanho, stdin);
    str[strcspn(str, "\n")] = '\0';
}

// -----------------------------
//      FUNÇÃO tamanho()
// -----------------------------
int tamanho(FILE *arq) {
    fseek(arq, 0, SEEK_END);
    long bytes = ftell(arq);
    return bytes / sizeof(Livro);
}

// -----------------------------
//      FUNÇÃO cadastrar()
// -----------------------------
void cadastrar(FILE *arq) {
    Livro L;

    printf("Titulo: ");
    lerString(L.titulo, 50);

    printf("Autor: ");
    lerString(L.autor, 50);

    printf("ISBN (apenas numeros): ");
    scanf("%d", &L.isbn);
    limpaBuffer();

    fseek(arq, 0, SEEK_END);
    fwrite(&L, sizeof(Livro), 1, arq);

    printf("\nLivro cadastrado com sucesso!\n");
}

// -----------------------------
//      FUNÇÃO consultar()
// -----------------------------
void consultar(FILE *arq) {
    int pos;
    Livro L;

    printf("Informe o indice do registro: ");
    scanf("%d", &pos);
    limpaBuffer();

    int total = tamanho(arq);

    if (pos < 0 || pos >= total) {
        printf("Indice invalido!\n");
        return;
    }

    fseek(arq, pos * sizeof(Livro), SEEK_SET);
    fread(&L, sizeof(Livro), 1, arq);

    printf("\n=== DADOS DO LIVRO ===\n");
    printf("Registro: %d\n", pos);
    printf("Titulo: %s\n", L.titulo);
    printf("Autor: %s\n", L.autor);
    printf("ISBN: %d\n", L.isbn);
}

// -----------------------------
//      FUNÇÃO listar()
// -----------------------------
void listar(FILE *arq) {
    int total = tamanho(arq);
    Livro L;

    if (total == 0) {
        printf("Nenhum livro cadastrado.\n");
        return;
    }

    fseek(arq, 0, SEEK_SET);

    printf("\n=== LISTA DE LIVROS ===\n");

    for (int i = 0; i < total; i++) {
        fread(&L, sizeof(Livro), 1, arq);
        printf("[%d] Titulo: %s | Autor: %s | ISBN: %d\n",
               i, L.titulo, L.autor, L.isbn);
    }

    printf("\n");
}

// -----------------------------
//      FUNÇÃO excluir()
// -----------------------------
FILE* excluir(FILE *arq) {
    int pos, total = tamanho(arq);

    printf("Informe o indice do livro que deseja excluir: ");
    scanf("%d", &pos);
    limpaBuffer();

    if (pos < 0 || pos >= total) {
        printf("Indice invalido!\n");
        return arq;
    }

    Livro L;

    fseek(arq, pos * sizeof(Livro), SEEK_SET);
    fread(&L, sizeof(Livro), 1, arq);

    printf("\n=== CONFIRMAR EXCLUSAO ===\n");
    printf("Titulo: %s\n", L.titulo);
    printf("Autor: %s\n", L.autor);
    printf("ISBN: %d\n", L.isbn);

    char resp;
    printf("\nTem certeza que deseja excluir? (S/N): ");
    scanf(" %c", &resp);
    limpaBuffer();

    if (!(resp == 'S' || resp == 's')) {
        printf("Exclusao cancelada!\n");
        return arq;
    }

    FILE *temp = fopen(ARQ_TEMP, "w+b");
    if (!temp) {
        printf("Erro ao criar arquivo temporario!\n");

