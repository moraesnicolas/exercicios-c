#include <stdio.h>
#include <string.h>

#define MAX 100
#define ARQUIVO "contatos.txt"

typedef struct {
    char nome[50];
    char telefone[16];
} Contato;

void lerTexto(const char mensagem[], char texto[], int tamanho) {
    printf("%s", mensagem);

    if (fgets(texto, tamanho, stdin) == NULL) {
        texto[0] = '\0';
    } else {
        texto[strcspn(texto, "\n")] = '\0';
    }
}

int buscar(const Contato contatos[], int total, const char nome[]) {
    int i;

    for (i = 0; i < total; i++) {
        if (strcmp(contatos[i].nome, nome) == 0)
            return i;
    }

    return -1;
}

int carregar(Contato contatos[]) {
    FILE *arquivo = fopen(ARQUIVO, "r");
    char linha[80];
    char *separador;
    int total = 0;

    if (arquivo == NULL) {
        /* arquivo ainda nao existe: cria vazio com "w" */
        arquivo = fopen(ARQUIVO, "w");
        if (arquivo != NULL)
            fclose(arquivo);
        return 0;
    }

    while (total < MAX && fgets(linha, sizeof(linha), arquivo) != NULL) {
        linha[strcspn(linha, "\n")] = '\0';
        separador = strchr(linha, ';');

        if (separador != NULL) {
            *separador = '\0';
            snprintf(contatos[total].nome, sizeof(contatos[total].nome),
                     "%s", linha);
            snprintf(contatos[total].telefone,
                     sizeof(contatos[total].telefone), "%s", separador + 1);
            total++;
        }
    }

    fclose(arquivo);
    return total;
}

void salvarContato(const Contato *contato) {
    FILE *arquivo = fopen(ARQUIVO, "a");

    if (arquivo == NULL) {
        printf("Erro ao salvar o arquivo.\n");
        return;
    }

    fprintf(arquivo, "%s;%s\n", contato->nome, contato->telefone);
    fclose(arquivo);
}

int cadastrar(Contato contatos[], int total) {
    Contato novo;

    if (total == MAX) {
        printf("Agenda cheia.\n");
        return total;
    }

    lerTexto("Nome: ", novo.nome, sizeof(novo.nome));
    lerTexto("Telefone com DDD: ", novo.telefone,
             sizeof(novo.telefone));

    if (novo.nome[0] == '\0' || novo.telefone[0] == '\0') {
        printf("Nome e telefone sao obrigatorios.\n");
        return total;
    }

    if (strchr(novo.nome, ';') != NULL) {
        printf("O nome nao pode ter ponto e virgula.\n");
        return total;
    }

    if (buscar(contatos, total, novo.nome) != -1) {
        printf("Contato ja cadastrado.\n");
        return total;
    }

    contatos[total] = novo;
    salvarContato(&novo);
    printf("Contato cadastrado com sucesso!\n");

    return total + 1;
}

void listar(const Contato contatos[], int total) {
    int i;

    if (total == 0)
        printf("Agenda vazia.\n");

    for (i = 0; i < total; i++) {
        printf("%d - %s: %s\n", i + 1,
               contatos[i].nome, contatos[i].telefone);
    }
}

void consultar(const Contato contatos[], int total) {
    char nome[50];
    int posicao;

    lerTexto("Nome para buscar: ", nome, sizeof(nome));
    posicao = buscar(contatos, total, nome);

    if (posicao == -1) {
        printf("Contato nao encontrado.\n");
    } else {
        printf("%s: %s\n", contatos[posicao].nome,
               contatos[posicao].telefone);
    }
}

void resumoDDD(const Contato contatos[], int total) {
    char ddd[4];
    int quantidade = 0;
    int i;
    double porcentagem = 0;

    lerTexto("DDD para consultar: ", ddd, sizeof(ddd));

    if (strlen(ddd) != 2) {
        printf("Digite um DDD de 2 numeros.\n");
        return;
    }

    for (i = 0; i < total; i++) {
        if (strncmp(contatos[i].telefone, ddd, 2) == 0)
            quantidade++;
    }

    if (total > 0)
        porcentagem = quantidade * 100.0 / total;

    printf("Total: %d | DDD %s: %d (%.1f%%)\n",
           total, ddd, quantidade, porcentagem);
}

int main(void) {
    Contato contatos[MAX];
    char entrada[20];
    int total = carregar(contatos);
    int opcao;

    do {
        printf("\n=== AGENDA TELEFONICA ===\n");
        printf("1 - Cadastrar\n");
        printf("2 - Listar\n");
        printf("3 - Buscar\n");
        printf("4 - Resumo por DDD\n");
        printf("0 - Sair\n");
        printf("Opcao: ");

        if (fgets(entrada, sizeof(entrada), stdin) == NULL)
            break;

        if (sscanf(entrada, "%d", &opcao) != 1)
            opcao = -1;

        if (opcao == 1) {
            total = cadastrar(contatos, total);
        } else if (opcao == 2) {
            listar(contatos, total);
        } else if (opcao == 3) {
            consultar(contatos, total);
        } else if (opcao == 4) {
            resumoDDD(contatos, total);
        } else if (opcao != 0) {
            printf("Opcao invalida.\n");
        }

    } while (opcao != 0);

    return 0;
}
