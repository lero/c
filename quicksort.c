#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct info 
{
    int codigo, quantidade;
    float valor;
    char descricao[80];
};
    
struct produtos
{
    struct info *info;
    struct produtos *proximo, *anterior;
} *inicio, *ultimo;

int menu();
void cadastro();
void grava();
void abre();
void lista();
void exclui();
void altera();
void edita();
void ordena();
void quicksort(struct produtos *);
void listaligada(struct produtos *);

int main()
{
    int i;
    inicio = ultimo = NULL;

    for (;;) {
        i = menu();
        switch (i) {
            case 1: cadastro(); break;
            case 2: abre(); break;
            case 3: grava(); break;
            case 4: edita(); break;
            case 5: exit(0);
        }
    }
}

void clean_stdin(void)
{
    int c;

    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

int menu()
{
    int i;

    system("clear");
    printf("\n\t\tMenu\n\n");
    printf("1. Cadastro\n");
    printf("2. Abre arquivo\n");
    printf("3. Grava arquivo\n");
    printf("4. Edita (lista, altero, exclui, ordena)\n");
    printf("5. Sai\n");
    printf("\nEntre com a opcao: ");
    scanf("%d", &i);

    return i;
}

void cadastro()
{
    struct produtos *p;
    struct info *info;
    static int i = 0;

    system("clear");
    printf("\t\tCadastro\n\n");

    for (;;) {
        printf("\nItem %d", i + 1);
        printf("\nDescricao produto: ");
        p = (struct produtos *) malloc(sizeof(struct produtos));
        p->info = (struct info *) malloc(sizeof(struct info));
        clean_stdin(); gets(p->info->descricao);
        if (!p->info->descricao[0])
            break;
        printf("Quantidade: ");
        scanf("%d", &p->info->quantidade);
        printf("Valor: ");
        scanf("%f", &p->info->valor);
        p->info->codigo = i + 1;
        listaligada(p);
        i++;
    }
}

void listaligada(struct produtos *p)
{
    if (inicio == NULL) {
        inicio = p;
        ultimo = p;
        p->proximo = NULL;
        p->anterior = NULL;
        return;
    }
    else {
        ultimo->proximo = p;
        p->anterior = ultimo;
        p->proximo = NULL;
        ultimo = p;
    }
}

void grava()
{
    struct produtos *p;
    FILE *fp;
    char arq[40];
    int i = 0;

    system("clear");
    printf("\n\n\t\tGrava\n\n");
    printf("\nEntre com o nome do arquivo: ");
    clean_stdin(); gets(arq);

    if ((fp = fopen(arq, "wb")) == NULL) {
        printf("\nNao pode abrir %s", arq);
        return;
    }

    p = inicio;
    while (p != NULL) {
        fwrite(p, sizeof(struct produtos), 1, fp);
        fwrite(p->info, sizeof(struct info), 1, fp);
        p = p->proximo;
        i++;
    }
    printf("\nForam gravados %d registros\n", i);
    fclose(fp);
    getchar();
}

void abre()
{
    struct produtos *p;
    FILE *fp;
    char arq[40];
    int i = 0;
    p = (struct produtos *) malloc(sizeof(struct produtos));

    system("clear");
    printf("\n\n\t\tAbre\n\n");
    printf("Entre com o nome do arquivo: ");
    clean_stdin(); gets(arq);

    if ((fp = fopen(arq, "rb")) == NULL) {
        printf("\nArquivo %s nao existe", arq);
        getchar();
        return;
    }

    inicio = ultimo = NULL;
    while ((fread(p, sizeof(struct produtos), 1, fp) > 0)) {
        p->info = (struct info *) malloc(sizeof(struct info));
        if (fread(p->info, sizeof(struct info), 1, fp) > 0) {
            listaligada(p);
            p = (struct produtos *) malloc(sizeof(struct produtos));
            i++;
        }
    }
    printf("\nForam lidos %d registros", i);
    fclose(fp);
    getchar();
}

void edita()
{
    int i;

    for (;;) {
        system("clear");
        printf("\n\n\t\tEdita\n\n");
        printf("1. Lista\n");
        printf("2. Altera\n");
        printf("3. Exclui\n");
        printf("4. Ordena\n");
        printf("5. Volta\n\n");
        printf("Entre com a opcao: ");
        scanf("%d", &i);
        switch (i) {
            case 1: lista(); break;
            case 2: altera(); break;
            case 3: exclui(); break;
            case 4: ordena(); break;
            case 5: return;
        }
    }
}

void lista()
{
    struct produtos *p;

    system("clear");
    printf("\n\n\t\tLista\n\n");

    p = inicio;
    while (p != NULL) {
        printf("Codigo: %d", p->info->codigo);
        printf("\nDescricao: %s", p->info->descricao);
        printf("\nQuantidade: %d", p->info->quantidade);
        printf("\nValor: %.2f\n\n", p->info->valor);
        p = p->proximo;
    }
    clean_stdin(); getchar();
}

void altera()
{
    struct produtos *p;
    char temp[80];
    int codigo, nachou = 1;

    system("clear");
    printf("\n\n\t\tAltera\n\n");
    printf("Entre com o codigo a alterar: ");
    clean_stdin(); scanf("%d", &codigo);
    p = inicio;

    while (p != NULL) {
        if (p->info->codigo == codigo) {
            nachou = 0;
            printf("\n*Enter para nao alterar*\n");
            printf("\nDescricao: %s", p->info->descricao);
            clean_stdin(); printf("\nAlterar para: ");
            gets(temp);
            if (temp[0])
                strcpy(p->info->descricao, temp);
            printf("\nQuantidade: %d", p->info->quantidade);
            printf("\nAltera para: ");
            gets(temp);
            if (temp[0])
                p->info->quantidade = atoi(temp);
            printf("\nValor: %.2f", p->info->valor);
            printf("\nAltera para: ");
            gets(temp);
            if (temp[0])
                p->info->valor = atof(temp);
        }
        p = p->proximo;
    }
    if (nachou == 1) {
        clean_stdin();
        printf("\nCodigo %d nao localizado", codigo);
        getchar();
    }
}

void exclui()
{
    int codigo;
    struct produtos *p, *p1, *p2;

    system("clear");
    printf("\n\n\t\tExclui\n\n");
    printf("Entre com o codigo a excluir: ");
    scanf("%d", &codigo);
    p = inicio;

    while (p != NULL){
        if (p->info->codigo == codigo){
            if (p->anterior == NULL){
                if (p->proximo == NULL) {
                    inicio = ultimo = NULL;
                    free(p);
                    break;
                }
                else {
                    inicio = p->proximo;
                    inicio->anterior = NULL;
                    free(p);
                    break;
                }
            }
            else if (p->proximo == NULL) {
                ultimo = p->anterior;
                ultimo->proximo = NULL;
                free(p);
                break;
            }
            else {
                p1 = p->anterior;
                p2 = p->proximo;
                p1->proximo = p2;
                p2->anterior = p1;
                free(p);
                break;
            }
        }
        p = p->proximo;
    }
    if (p == NULL)
        printf("\nItem codigo %d nao existe", codigo);
    else 
        printf("\nItem codigo %d deletado", codigo);
    clean_stdin(); getchar();
}

void ordena()
{
    quicksort(inicio);
    system("clear");
    printf("\n\n\t\tOrdena\n\n");
    struct produtos *p = inicio;
    while (p != NULL) {
        printf("Codigo: %d", p->info->codigo);
        printf("\nDescricao: %s", p->info->descricao);
        printf("\nQuantidade: %d", p->info->quantidade);
        printf("\nValor: %.2f\n\n", p->info->valor);
        p = p->proximo;
    }
    clean_stdin(); getchar();
}

void troca(struct produtos *a, struct produtos *b)
{   
    struct info *info;
    info = a->info;
    a->info = b->info;
    b->info = info;
}

struct produtos *final(struct produtos *t)
{
    while (t && t->proximo) 
    {
        t = t->proximo;
    }
    return t;
}

struct produtos *particiona(struct produtos *l, struct produtos *r)
{
    char *x  = r->info->descricao;
 
    struct produtos *i = l->anterior;
    struct produtos *j;

    for (j = l; j != r; j = j->proximo)
    {
        if (strcmp(j->info->descricao, x) <= 0)
        {
            i = (i == NULL)? l : i->proximo;
            troca(i, j);
        }
    }

    i = (i == NULL)? l : i->proximo; 
    troca(i, r);
    
    return i;
}

void qs(struct produtos *l, struct produtos *r)
{
    if (r != NULL && l != r && l != r->proximo)
    {
        struct produtos *p = particiona(l, r);
        qs(l, p->anterior);
        qs(p->proximo, r);
    }
}

void quicksort(struct produtos *t)
{
    struct produtos *h = final(t);
    qs(t, h);
}
