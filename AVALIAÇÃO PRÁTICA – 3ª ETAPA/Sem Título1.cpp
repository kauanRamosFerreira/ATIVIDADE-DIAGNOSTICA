#include <stdio.h>
#include <string.h>


struct Conta {
    int numero;
    char titular[50];
    float saldo;
};

typedef struct Conta Conta;

FILE *arquivo;

// Limpa o buffer do teclado
void limparBuffer() {//usando void pois nao retorna valor, é só para evitar o bug do enter (:
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

// Buscar uma conta
int buscarConta(int numero, Conta *conta) {
    arquivo = fopen("contas.dat", "rb");
    if (arquivo == NULL) return 0;

    while (fread(conta, sizeof(Conta), 1, arquivo)) {
        if (conta->numero == numero) {
            fclose(arquivo);
            return 1;
        }
    }
    fclose(arquivo);
    return 0;
}

// Atualizar dados da conta
int atualizarConta(Conta contaAtualizada) {
    Conta c;
    arquivo = fopen("contas.dat", "rb+");
    if (arquivo == NULL) return 0;

    while (fread(&c, sizeof(Conta), 1, arquivo)) {
        if (c.numero == contaAtualizada.numero) {
            fseek(arquivo, -sizeof(Conta), SEEK_CUR);
            fwrite(&contaAtualizada, sizeof(Conta), 1, arquivo);
            fclose(arquivo);
            return 1;
        }
    }
    fclose(arquivo);
    return 0;
}

// Gera o próximo número de conta
int gerarNumeroConta() {
    Conta c;
    int ultimo = 0;

    arquivo = fopen("contas.dat", "rb");
    if (arquivo == NULL) return 1;

    while (fread(&c, sizeof(Conta), 1, arquivo)) {
        ultimo = c.numero;
    }

    fclose(arquivo);
    return ultimo + 1;
}

// Criar nova conta
int novaConta() {
    Conta nova;

    printf("\nNome do titular: ");
    limparBuffer();
    fgets(nova.titular, 50, stdin);
    nova.titular[strcspn(nova.titular, "\n")] = '\0';

    nova.numero = gerarNumeroConta();
    nova.saldo = 0;

    arquivo = fopen("contas.dat", "ab");
    if (!arquivo) return 0;

    fwrite(&nova, sizeof(Conta), 1, arquivo);
    fclose(arquivo);

    printf("\nConta criada com sucesso!\nNúmero da conta: %d\n", nova.numero);
    return 1;
}

// Extrato da conta
int extrato() {
    int numero;
    Conta c;

    printf("\nNúmero da conta: ");
    scanf("%d", &numero);

    if (buscarConta(numero, &c)) {
        printf("\nTitular: %s\nConta: %d\nSaldo: R$ %.2f\n", c.titular, c.numero, c.saldo);
        return 1;
    } else {
        printf("\nConta não encontrada.\n");
        return 0;
    }
}

// Depósito
int deposito() {
    int numero;
    float valor;
    Conta c;

    printf("\nNúmero da conta: ");
    scanf("%d", &numero);

    if (!buscarConta(numero, &c)) {
        printf("\nConta não encontrada.\n");
        return 0;
    }

    printf("Valor do depósito: ");
    scanf("%f", &valor);

    c.saldo += valor;
    if (!atualizarConta(c)) return 0;

    printf("\nDepósito realizado!\n");
    return 1;
}

// Saque
int sacar() {
    int numero;
    float valor;
    Conta c;

    printf("\nNúmero da conta: ");
    scanf("%d", &numero);

    if (!buscarConta(numero, &c)) {
        printf("\nConta não encontrada.\n");
        return 0;
    }

    printf("Valor do saque: ");
    scanf("%f", &valor);

    if (valor > c.saldo) {
        printf("\nSaldo insuficiente!\n");
        return 0;
    }

    c.saldo -= valor;
    if (!atualizarConta(c)) return 0;

    printf("\nSaque realizado!\n");
    return 1;
}

// Transferência
int transferencia() {
    int origem, destino;
    float valor;
    Conta cOrigem, cDestino;

    printf("\nConta de origem: ");
    scanf("%d", &origem);

    printf("Conta de destino: ");
    scanf("%d", &destino);

    if (!buscarConta(origem, &cOrigem) || !buscarConta(destino, &cDestino)) {
        printf("\nConta(s) não encontrada(s).\n");
        return 0;
    }

    printf("Valor da transferência: ");
    scanf("%f", &valor);

    if (valor > cOrigem.saldo) {
        printf("\nSaldo insuficiente para completar a transação!\n");
        return 0;
    }

    cOrigem.saldo -= valor;
    cDestino.saldo += valor;

    if (!atualizarConta(cOrigem) || !atualizarConta(cDestino)) return 0;

    printf("\nTransferência realizada!\n");
    return 1;
}

// Lista todas as contas cadastradas (senha: ALFAUNIPAC)
int listarContas() {
    char senhaDigitada[50];
    char senhaDoSistema[] = "ALFAUNIPAC";

    printf("\nSenha de administrador: ");
    scanf("%s", senhaDigitada);

    if (strcmp(senhaDigitada, senhaDoSistema) != 0) {
        printf("\nSenha incorreta!\n");
        return 0;
    }

    Conta c;
    arquivo = fopen("contas.dat", "rb");
    if (arquivo == NULL) {
        printf("\nNenhuma conta cadastrada.\n");
        return 0;
    }

    printf("\n======= Lista de contas =======\n");
    while (fread(&c, sizeof(Conta), 1, arquivo)) {
        printf("\nConta: %d\nTitular: %s\nSaldo: R$ %.2f\n", c.numero, c.titular, c.saldo);
    }
    fclose(arquivo);
    return 1;
}

int main() {
    int opcao;

    do {
        printf("\n=========== Banco do Nerd :) ===========\n");
        printf("1 - Criar nova conta\n");
        printf("2 - Extrato\n");
        printf("3 - Depósito\n");
        printf("4 - Saque\n");
        printf("5 - Transferência\n");
        printf("6 - Listar todas as contas\n");
        printf("7 - Sair\n");
        printf("=====================================\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: novaConta(); break;
            case 2: extrato(); break;
            case 3: deposito(); break;
            case 4: sacar(); break;
            case 5: transferencia(); break;
            case 6: listarContas(); break;
            case 7: printf("\nSaindo...\n"); break;
            default: printf("\nOpção inválida!\n");
        }

    } while (opcao != 7);

    return 0;
}
