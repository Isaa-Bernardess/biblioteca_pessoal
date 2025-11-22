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
void excluirLivro(FILE *arq);
void gerarRelatorio(FILE *arq);

// -----------------------------
//      STRUCT PRINCIPAL
// -----------------------------
typedef struct {
    char titulo[50];
    char autor[50];
    int isbn;
} Livro;

// -----------------------------
//      FUNÇÃO PRINCIPAL
// -----------------------------
int main() {

    FILE *arq = fopen("biblioteca.bin", "r+b");

    if (arq == NULL) {
        arq = fopen("biblioteca.bin", "w+b");
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
        printf("5 - Gerar Relatorio TXT\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);
        limpaBuffer();

        switch (opcao) {
            case 1:
                cadastrar(arq);
                break;
            case 2:
                consultar(arq);
                break;
            case 3:
                printf("Total de registros: %d\n", tamanho(arq));
                break;
            case 4:
                excluirLivro(arq);
                break;
            case 5:
                gerarRelatorio(arq);
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opcao invalida!\n");
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
    printf("Titulo: %s\n", L.titulo);
    printf("Autor: %s\n", L.autor);
    printf("ISBN: %d\n", L.isbn);
}

// -----------------------------
//      FUNÇÃO excluirLivro()
// -----------------------------
void excluirLivro(FILE *arq) {
    int pos, total;
    total = tamanho(arq);

    printf("Informe o indice do livro para excluir: ");
    scanf("%d", &pos);
    limpaBuffer();

    if (pos < 0 || pos >= total) {
        printf("Indice invalido!\n");
        return;
    }

    // Ler dados para mostrar antes de excluir
    Livro L;
    fseek(arq, pos * sizeof(Livro), SEEK_SET);
    fread(&L, sizeof(Livro), 1, arq);

    printf("\n=== DADOS DO LIVRO A SER EXCLUIDO ===\n");
    printf("Titulo: %s\n", L.titulo);
    printf("Autor: %s\n", L.autor);
    printf("ISBN: %d\n", L.isbn);

    char confirma;
    printf("\nTem certeza que deseja excluir este livro? (S/N): ");
    scanf("%c", &confirma);
    limpaBuffer();

    if (confirma != 'S' && confirma != 's') {
        printf("Operacao cancelada. O livro NAO foi excluido.\n");
        return;
    }

    // Prossegue com exclusão
    FILE *temp = fopen("temp.bin", "w+b");
    if (!temp) {
        printf("Erro ao criar arquivo temporario!\n");
        return;
    }

    fseek(arq, 0, SEEK_SET);

    int i;
    for (i = 0; i < total; i++) {
        fread(&L, sizeof(Livro), 1, arq);
        if (i != pos) {
            fwrite(&L, sizeof(Livro), 1, temp);
        }
    }

    fclose(arq);
    fclose(temp);

    remove("biblioteca.bin");
    rename("temp.bin", "biblioteca.bin");

    arq = fopen("biblioteca.bin", "r+b");

    printf("\nLivro excluido com sucesso!\n");
}

// -----------------------------
//      FUNÇÃO gerarRelatorio()
// -----------------------------
void gerarRelatorio(FILE *arq) {
    FILE *txt = fopen("relatorio.txt", "w");

    if (!txt) {
        printf("Erro ao criar relatorio!\n");
        return;
    }

    int total = tamanho(arq);
    Livro L;

    fprintf(txt, "===== RELATORIO DA BIBLIOTECA =====\n");
    fprintf(txt, "Total de livros: %d\n\n", total);

    fseek(arq, 0, SEEK_SET);

    int i;
    for (i = 0; i < total; i++) {
        fread(&L, sizeof(Livro), 1, arq);
        fprintf(txt, "Registro %d\n", i);
        fprintf(txt, "Titulo: %s\n", L.titulo);
        fprintf(txt, "Autor: %s\n", L.autor);
        fprintf(txt, "ISBN: %d\n\n", L.isbn);
    }

    fclose(txt);

    printf("Relatorio gerado com sucesso! (relatorio.txt)\n");
}
