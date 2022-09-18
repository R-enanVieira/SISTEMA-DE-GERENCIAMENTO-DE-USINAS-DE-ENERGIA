#include <stdio.h>
#include <locale.h>
#include "221031363.h"

int main() 
{
    int opcao_escolhida;
    do
    {
        cabecalho();
        menu();
        do
        {
            fflush(stdin);
            scanf("%d", &opcao_escolhida);
            if(opcao_escolhida == 0 || opcao_escolhida < 0 || opcao_escolhida > 8)
            {   
                limpar_tela();
                printf("-------------------------------------------A OPCAO ESCOLHIDA NAO EXISTE!------------------------------------------------\n");
                menu();
            }
        }while((opcao_escolhida == 0 || opcao_escolhida < 0 || opcao_escolhida > 8));

        switch (opcao_escolhida)
        {
        case 1: //Realizar o cadastro de uma usina no sistema
            cabecalho();
            //listar_u();
            cadastro_usina();
            break;
        case 2: //Realizar o cadastro de um consumidor no sistema
            cabecalho();
            //listar_c();
            cadastro_consumidor();
            break;
        case 3: //Realizar o contrato entre uma usina e um consumidor já castrado
            cabecalho();
            contrato();
            break;
        case 4: //Consultar as usinas que já foram cadastradas
            cabecalho();
            consultar_usina();
            break;
        case 5: //Consultar consumidores cadastrados
            cabecalho();
            consultar_consumidor();
            break;
        case 6: //Excluir dados de uma usina do sistema
            cabecalho();
            excluir_usinas();
            break;
        case 7: //Excluir dados de um consumidor do sistema
            cabecalho();
            excluir_consumidor();
            break;
        case 8: //Encerrar o progama 
            limpar_tela();
            printf("--------------------------------------------------PROGAMA ENCERRADO!---------------------------------------------------");
            getch();
            break;
        default:
            cabecalho();
            printf("---------------OPCAO INVALIDA!---------------");
            printf("\nPressione qualquer tecla para continuar.");
            getch();
            break;
        }

        limpar_tela();
    }while(opcao_escolhida != 8);
    
    return 0;
}