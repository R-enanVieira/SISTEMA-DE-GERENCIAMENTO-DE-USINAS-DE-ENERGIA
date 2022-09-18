#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#define TAM_DATA 11
#define TAM_CNPJ 19
#define TAM_NOME 100
#define TAM_CPF 15

typedef struct 
{
    char cnpj_usina[TAM_CNPJ];
    char nome_usina[TAM_NOME];
    char data_inicio_operacao[TAM_DATA];
    int potencia_estimada;
} USINAS;

typedef struct 
{
    char cnpj_consumidor[TAM_CNPJ];
    char cpf[TAM_CPF];
    char nome[TAM_NOME];
} CONSUMIDOR;

typedef struct 
{
    char cnpj_usina_contrato[TAM_CNPJ];
    char cpf_id[TAM_CPF];
    char cnpj_id[TAM_CNPJ];
    char data_inicio_contrato[TAM_DATA];
    int potencia_combinada;
} CONTRATO;

//calcula o percentual restante de uma usina quando o usuário for consultar ela
float percentual_potencia(char*cnpj)
{
    FILE* arquivo_usina;
    FILE* arquivo_contrato;
    float pot_rest, pot_total, pot_reservada;
    pot_reservada = 0;
    CONTRATO usina_pot_res;
    USINAS usina_pot_total;
    arquivo_usina = fopen("usinas_cadastradas.txt", "rb");
    arquivo_contrato = fopen("contratos.txt", "rb");
    if (arquivo_contrato == NULL || arquivo_usina == NULL)
    {
        printf("Probemas ao abrir os arquivos!\n");
        exit(1);
    } else
    {
        //leio o arquivo de contratos
        while(fread(&usina_pot_res, sizeof(CONTRATO), 1, arquivo_contrato) == 1)
        {
            //até achar os contratos da usina que corresponde ao cnpj recebido no argumento da função
            if (strcmp(usina_pot_res.cnpj_usina_contrato, cnpj) == 0)
            {
                //armazeno os valores de "potencia combinada" em cada contrato na variavel pot_reservada
                pot_reservada = pot_reservada + usina_pot_res.potencia_combinada;
            }
        }
        //leio o arquivo de usinas
        while(fread(&usina_pot_total, sizeof(USINAS), 1, arquivo_usina))
        {
            //até achar a usina que corresponde ao cnpj recebido como argumento da função
            if(strcmp(usina_pot_total.cnpj_usina, cnpj) == 0)
            {
                //recolher o valor que corrrsponde a potencia máxima que foi informada pela usia na hora cadastro
                pot_total = usina_pot_total.potencia_estimada;
            } 
        }
    }
    fclose(arquivo_contrato);
    fclose(arquivo_usina);
    //calculo o percentual restante
    pot_rest = 100 - ((pot_reservada*100.00)/pot_total);
    return pot_rest;
}

//calcula a quantidade restante de potencia de uma usina para não permitir que um contrato seja fechado com um valor maior que o disponível
int potencia_restante(char*cnpj)
{
    FILE* arquivo_usina;
    FILE* arquivo_contrato;
    int pot_rest, pot_total, pot_usada;
    pot_usada = 0;
    CONTRATO usina_pot_res;
    USINAS usina_pot_total;
    arquivo_usina = fopen("usinas_cadastradas.txt", "rb");
    arquivo_contrato = fopen("contratos.txt", "rb");
    if (arquivo_contrato == NULL || arquivo_usina == NULL)
    {
        printf("Problemas ao abrir os arquivos!\n");
        exit(1);
    } else
    {
        //segue a mesma lógica utilizada na função de calcular o percentual, o que difere é o cálculo final 
        while(fread(&usina_pot_res, sizeof(CONTRATO), 1, arquivo_contrato) == 1)
        {
            if (strcmp(usina_pot_res.cnpj_usina_contrato, cnpj) == 0)
            {
                pot_usada = pot_usada + usina_pot_res.potencia_combinada;
            }
        }
        while(fread(&usina_pot_total, sizeof(USINAS), 1, arquivo_usina))
        {
            if(strcmp(usina_pot_total.cnpj_usina, cnpj) == 0)
            {
                pot_total = usina_pot_total.potencia_estimada;
            } 
        }
    }

    fclose(arquivo_contrato);
    fclose(arquivo_usina);
    pot_rest = pot_total - pot_usada;
    return pot_rest;
}

//função que limpa a tela do usuário para não ficar muito poluido
void limpar_tela()
{
    system("cls");
}

//cabeçalho que acompanha toda a execução do código
void cabecalho()
{
    limpar_tela();
    printf("=============================================\n");
    printf("-------------USINAS FOTOVOLTAICAS------------\n");
    printf("=============================================\n\n");

}

//menu inicial que mostra ao usuário todas as opções/funções disponiveis no software
void menu()
{
    printf("Menu de opcoes:\n");
    printf("1) Cadastrar Usina\n");
    printf("2) Cadastrar Consumidor\n");
    printf("3) Cadastrar Contrato entre Consumidor e Usina\n");
    printf("4) Consultar Usina Cadastrada\n");
    printf("5) Consultar Consumidor Cadastrado\n");
    printf("6) Excluir Usina\n");
    printf("7) Excluir Consumidor\n");
    printf("8) Encerrar Programa\n\n");
    printf("Digite a opcao que deseja selecionar: ");
}

//função que faz a validação do cpf e não deixa o usuário escrever um cpf que não existe ou não está de acordo com a formatação pedida
int verificar_cpf(char*cpf_c)
{   
    int soma, produto, resto, cpf_i[11], i;
    soma = 0;
    //primeira verificação para conferir se o cpf está no formato ###.###.###-## com os pontos e traços no lugar e verificar se o cpf tem todos os digitos iguais
    if(cpf_c[3] != '.' || cpf_c[7] != '.' || cpf_c[11] != '-' || (cpf_c[0] == cpf_c[1] && cpf_c[1] == cpf_c[2] && cpf_c[2] == cpf_c[4] && cpf_c[4] == cpf_c[5] && cpf_c[5] == cpf_c[6] && cpf_c[6] == cpf_c[8] && cpf_c[8] == cpf_c[9] && cpf_c[9] == cpf_c[10] && cpf_c[10] == cpf_c[12] && cpf_c[12] == cpf_c[13]))
    {
        return 1;
        
    } else
    {
        //converter o cpf que antes estava em um vetor do tipo caracter para um vetor do tipo inteiro
        cpf_i[0] = cpf_c[0] - '0';
        cpf_i[1] = cpf_c[1] - '0';
        cpf_i[2] = cpf_c[2] - '0';

        cpf_i[3] = cpf_c[4] - '0';
        cpf_i[4] = cpf_c[5] - '0';
        cpf_i[5] = cpf_c[6] - '0';

        cpf_i[6] = cpf_c[8] - '0';
        cpf_i[7] = cpf_c[9] - '0';
        cpf_i[8] = cpf_c[10] - '0';

        cpf_i[9] = cpf_c[12] - '0';
        cpf_i[10] = cpf_c[13] - '0';

        //calculo para realizar a verificação do primeiro digito validador
        for(i = 0; i < 9; i++)
        {
            produto = cpf_i[i] * (10 - i);
            soma = soma + produto;
            
        }
        resto = soma*10%11;
        if ( resto == 10) 
        {
            resto = 0;
        } else
        {
            //verificação do primeiro digito validador
            if(resto == cpf_i[9])
            {
                return 0;
            } else 
            {
                return 1;
            }
        }

        //verificação do segundo digito verificador
        for(i = 0; i < 10; i++)
        {
            produto = cpf_i[i] * (11 - i);
            soma = soma + produto;    
        }
        resto = soma*10%11;
        if ( resto == 10) 
        {
            resto = 0;
        } else
        {   //verificação do segundo digito validador
            if(resto == cpf_i[10])
            {
                return 0;
            } else 
            {
                return 1;
            }
        }
    }
}

//função que faz a validação do cnpj e não deixa o usuário escrever um cnpj que não existe ou não está de acordo com a formatação pedida
int verificar_cnpj(char*cnpj_c){

    int produto, soma, resto, cnpj_i[15], i, j, validador;
    soma = 0;
    validador = 0;
    //primeira verificação para conferir se o cnpj está no formato ##.###.###/0001-## ou ##.###.###/0002-## com os pontos, barra e traço no lugar
    if((cnpj_c[2] !='.' || cnpj_c[6] !='.' || cnpj_c[10] !='/' || cnpj_c[15] !='-'))
    {
        return 1;
    } else
    {
        //transformando o vetor quem contem o cnpj do tipo char em um vetor do tipo int, necessario para realizar os cálucolos da validação
        cnpj_i[0] = cnpj_c[0] - '0';
        cnpj_i[1] = cnpj_c[1] - '0';
        
        cnpj_i[2] = cnpj_c[3] - '0';
        cnpj_i[3] = cnpj_c[4] - '0';
        cnpj_i[4] = cnpj_c[5] - '0';
        
        cnpj_i[5] = cnpj_c[7] - '0';
        cnpj_i[6] = cnpj_c[8] - '0';
        cnpj_i[7] = cnpj_c[9] - '0';
        
        cnpj_i[8] = cnpj_c[11] - '0';
        cnpj_i[9] = cnpj_c[12] - '0';
        cnpj_i[10] = cnpj_c[13] - '0';
        cnpj_i[11] = cnpj_c[14] - '0';

        cnpj_i[12] = cnpj_c[16] - '0';
        cnpj_i[13] = cnpj_c[17] - '0';

        // Calculo para encontrar o primeiro digito validador 
        for(i = 0; i < 4; i++) //multiplicado os 4 primeiros digitos do cnpj pelos pesos que começam em 4 e decressem até 2
        {   
            produto = cnpj_i[i] * (5 - i);
            soma = soma + produto;  
        }
        
        for (j = 4; j < 12; j++)
        {
            produto = cnpj_i[j] * (13 - j);
            soma = soma + produto;   
        }
        resto = soma % 11;
        //Conferindo se o primeiro digito validador calculado bate com odo cnpj digitado pelo usuário
        if(resto < 2) 
        {   
            resto = 0;
            if(resto == cnpj_i[12])
            {
                validador = validador + 1;
            }
        } else
        {
            if((11-resto) == cnpj_i[12])
            {
                validador = validador + 1;
            }
        }
        // calculo para encontrar o segundo digito de validação do cnpj
        soma = 0;
        resto = 0;
        for(i = 0; i < 5; i++)  //multiplicado os 4 primeiros digitos do cnpj pelos pesos que começam em 4 e decressem até 2
        {
            produto = cnpj_i[i] * (6 - i);
            soma = soma + produto;
        }
        for (j = 5; j < 13; j++) //multiplicado os 8 digitos que antecem a barra mais o primeiro digito validador pelos seus respectivos pesos que começam em 9 e decressem até 2
        {
            produto = cnpj_i[j] * (14 - j);
            soma = soma + produto;
        }
        resto = soma % 11;

        //Conferindo se o segundo digito validador calculado bate com o do cnpj digitado pelo usuário
        if( resto < 2) 
        {
            resto = 0;
            if(resto == cnpj_i[13])
            {
                validador = validador + 1;
            }
        } else
        {
            if((11-resto) == cnpj_i[13])
            {
                validador = validador + 1;
            }
        }

        if (validador == 2)
        {
            return 0;
            
        } else
        {
            return 1;
        }
    }
}

//vfunção que verifica se as datas inseridas pelo usuário realmente existem e estão de acordo com a formatação
int verificar_data(char*data_c) 
{
    int data_i[8], dia, mes, ano, validador_ano_bissesto;
    //primeira verificação para conferir se a data está no formato XX/XX/XXXX com as barras nos çugares certinho
    if(data_c[2] != '/' || data_c[5] != '/' || strlen(data_c) != 10)
    {
        return 1;
    } else 
    {
        //converter a data que antes estava em um vetor do tipo caracter para um vetor do tipo inteiro
        data_i[0] = data_c[0] - '0';
        data_i[1] = data_c[1] - '0';
        
        data_i[2] = data_c[3] - '0';
        data_i[3] = data_c[4] - '0';
        
        data_i[4] = data_c[6] - '0';
        data_i[5] = data_c[7] - '0';
        data_i[6] = data_c[8] - '0';
        data_i[7] = data_c[9] - '0';

        dia = data_i[0]*10 + data_i[1];
        mes = data_i[2]*10 + data_i[3];
        ano = data_i[4]*1000 + data_i[5]*100 + data_i[6]*10 + data_i[7];

        //verificando se é ano bissesto
        if((ano%400==0) || ((ano%4==0) && (ano%100!=0))) {
            validador_ano_bissesto = 2;
        }

		if((dia > 0 && dia <= 31) && (mes > 0 && mes <= 12) && ano > 0) 
        {
            if((mes==2 && dia <=28) || ((mes == 4 || mes == 6 || mes == 9  || mes == 11) && (dia <= 30)))
            {
                return 0;
            }else
            {
                if (((mes ==2 && dia == 29) && ( validador_ano_bissesto == 2)) || ((mes==1 || mes==3 || mes == 5 || mes == 7 || mes == 8 || mes == 10 || mes == 12) && (dia <= 31)))
                {
                    return 0;
                } else
                { 
                    return 1;
                }
            }
        }else
        {
            return 1;
        }
    }
}

//função para verificar se data usuada na hora de fechar o contrato é superior a data que marca o inicio de operação daquela usina
int compara_data(char*data_a, char*cnpj)
{
    FILE* arquivo1;
    USINAS usina;
    char data_c[TAM_DATA];
    int data_i[8];
    int dia, dia1, mes, mes1, ano, ano1;

    //converter a data que antes estava em um vetor do tipo caracter para um vetor do tipo inteiro
    data_i[0] = data_a[0] - '0';
    data_i[1] = data_a[1] - '0';
    
    data_i[2] = data_a[3] - '0';
    data_i[3] = data_a[4] - '0';
    
    data_i[4] = data_a[6] - '0';
    data_i[5] = data_a[7] - '0';
    data_i[6] = data_a[8] - '0';
    data_i[7] = data_a[9] - '0';

    dia = data_i[0]*10 + data_i[1];
    mes = data_i[2]*10 + data_i[3];
    ano = data_i[4]*1000 + data_i[5]*100 + data_i[6]*10 + data_i[7];

    arquivo1 = fopen("usinas_cadastradas.txt", "rb");
    if(arquivo1 == NULL)
    {
        printf("Problemas na abertura do arquivo! (Funcao de comparar data)");
    } else
    {
        //lendo o arquivo que contem as usinas cadastradas a fim de recolher a dta que marca o incio de operação da usina
        while(fread(&usina, sizeof(USINAS), 1, arquivo1) == 1)
        {
            if(strcmp(usina.cnpj_usina, cnpj) == 0)
            {
                strcpy(data_c, usina.data_inicio_operacao);
            }    
        }
        //converter a data que antes estava em um vetor do tipo caracter para um vetor do tipo inteiro
        data_i[0] = data_c[0] - '0';
        data_i[1] = data_c[1] - '0';
        
        data_i[2] = data_c[3] - '0';
        data_i[3] = data_c[4] - '0';
        
        data_i[4] = data_c[6] - '0';
        data_i[5] = data_c[7] - '0';
        data_i[6] = data_c[8] - '0';
        data_i[7] = data_c[9] - '0';

        dia1 = data_i[0]*10 + data_i[1];
        mes1 = data_i[2]*10 + data_i[3];
        ano1 = data_i[4]*1000 + data_i[5]*100 + data_i[6]*10 + data_i[7];  
    }
    fclose(arquivo1);

    //fazendo a verificação
    if(ano >= ano1)
    {
        if(ano == ano1)
        {
            if(mes >= mes1)
            {
                if(mes == mes1)
                {
                    if(dia >= dia1)
                    {
                        return 0;
                    } else
                    {
                        return 1;
                    }
                } else
                {
                    return 0;
                } 
            } else
            {
                return 1;
            }
        } else
        {
            return 0;
        }
    } else
    {
        return 1;
    }

}

//função que faz a animação de carregamento durante a exclusão de uma usina 
void loading() 
{
    printf("\n");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 20; j++) {
            printf(".");
            Sleep(60);
        }
        printf("\r");
        for (int j = 0; j < 30; j++) {
            printf(" "); // apaga a linha anterior
        }
        printf("\r");
    }
    printf("\n");
}

//função que complementa a de exluir usinas, excluindo os contratos que aquela usina que foi excluída tem vinculo
void excluir_contrato_u(char*cnpj)
{
    FILE* arquivo1;
    FILE* arquivo2;
    CONTRATO contratos;

    arquivo1 = fopen("contratos.txt", "rb");
    arquivo2 = fopen("contratos_temp.txt", "ab");
    if(arquivo1 == NULL || arquivo2 == NULL)
    {
        printf("Problemas na abertura do arquivo!");
    } else
    {
        while(fread(&contratos, sizeof(CONTRATO), 1, arquivo1) == 1)
        {
            if(strcmp(contratos.cnpj_usina_contrato, cnpj) != 0)
            {
                fwrite(&contratos, sizeof(CONTRATO), 1, arquivo2);
            }
        }
    }
    fclose(arquivo1);
    fclose(arquivo2);
    remove("contratos.txt");
    rename("contratos_temp.txt", "contratos.txt");

}

//função que complementa a de exluir consumidor, excluindo os contratos que aquele consumidor que foi excluído tem vinculo
void excluir_contrato_c_cpf(char*cpf)
{
    FILE* arquivo1;
    FILE* arquivo2;
    CONTRATO contratos;
    arquivo1 = fopen("contratos.txt", "rb");
    arquivo2 = fopen("contratos_temporariamente.txt", "ab");
    if(arquivo1 == NULL || arquivo2 == NULL)
    {
        printf("Problemas na abertura do arquivo\n");
    } else
    {
        while(fread(&contratos, sizeof(CONTRATO), 1, arquivo1) == 1)
        {
            if(strcmp(contratos.cpf_id, cpf) != 0)
            {
                fwrite(&contratos, sizeof(CONTRATO), 1, arquivo2);
            }
        }
    }
    fclose(arquivo1);
    fclose(arquivo2);
    remove("contratos.txt");
    rename("contratos_temporariamente.txt", "contratos.txt");
}

//função que complementa a de exluir consumidor, excluindo os contratos que aquele consumidor que foi excluído tem vinculo
void excluir_contrato_c_cnpj(char*cnpj)
{
    FILE* arquivo1;
    FILE* arquivo2;
    CONTRATO contratos;
    arquivo1 = fopen("contratos.txt", "rb");
    arquivo2 = fopen("contratos_temporariamente.txt", "ab");
    if(arquivo1 == NULL || arquivo2 == NULL)
    {
        printf("Problemas na abertura do arquivo\n");
    } else
    {
        while(fread(&contratos, sizeof(CONTRATO), 1, arquivo1) == 1)
        {
            if(strcmp(contratos.cnpj_id, cnpj) != 0)
            {
                fwrite(&contratos, sizeof(CONTRATO), 1, arquivo2);
            }
        }
    }
    fclose(arquivo1);
    fclose(arquivo2);
    remove("contratos.txt");
    rename("contratos_temporariamente.txt", "contratos.txt");
}

//função que recebe e verifica se os dados das usinas que estão sendo cadastradas são válidos
void cadastro_usina() 
{   
    FILE* arquivo;
    FILE* arquivo2;
    USINAS usinas_cadastro, usinas_verificacao;
    CONSUMIDOR consumidor_verificacao;
    int exit;
    
    arquivo = fopen("usinas_cadastradas.txt", "ab"); //A - Append

    if(arquivo == NULL)// pra ver se o arquivo abriu certinho
    {
        printf("Problemas na abertura do arquivo!\n");
    } else
    {   
        printf("Digite o CNPJ da usina que deseja cadastrar (formato -> XX.XXX.XXX/XXXX-XX): ");
        scanf(" %s", usinas_cadastro.cnpj_usina);
        //looping para caso um cnpj não seja válido
        if (verificar_cnpj(usinas_cadastro.cnpj_usina) == 1)
        {
            while(verificar_cnpj(usinas_cadastro.cnpj_usina) == 1)
            {  
                cabecalho();
                printf("DADOS INCORRETOS! POR FAVOR INSIRA UM CNPJ VALIDO NO FORMATO XX.XXX.XXX/XXXX-XX\n\n");
                printf("Digite o CNPJ da usina que deseja cadastrar: ");
                scanf(" %s", usinas_cadastro.cnpj_usina);
            }
        }

        //aqui iremos verificar se já existe alguma usina cadastrada com esse CNPJ
        arquivo2 = fopen("usinas_cadastradas.txt", "rb");
        if(arquivo2 == NULL)// pra ver se o arquivo abriu certinho
        {
            printf("Problemas na abertura do arquivo!\n");
        } else
        {
            while(fread(&usinas_verificacao, sizeof(USINAS), 1, arquivo2) == 1)
            {   
                if(strcmp(usinas_cadastro.cnpj_usina, usinas_verificacao.cnpj_usina) == 0)
                {
                    printf("\nEsse CNPJ ja esta cadastrado com uma usina!\n\n");
                    printf("Deseja tentar realizar o cadastro com outro CNPJ? (s/n)  ");
                    if(getch() == 'n' || getch() == 'N')
                    {
                        fclose(arquivo);
                        fclose(arquivo2);
                        return;
                    }
                    if(getch() == 's' || getch() == 'S')
                    {   
                        while(strcmp(usinas_cadastro.cnpj_usina, usinas_verificacao.cnpj_usina) == 0)
                        { 
                            cabecalho();          
                            printf("Como o CNPJ digitado anteriormente ja estava cadastrado. Digite novamente o CNPJ da usina que deseja cadastrar (formato -> XX.XXX.XXX/XXXX-XX): ");
                            scanf(" %s", usinas_cadastro.cnpj_usina);
                            //looping para caso um cnpj não seja válido
                            if (verificar_cnpj(usinas_cadastro.cnpj_usina))
                            {
                                while(verificar_cnpj(usinas_cadastro.cnpj_usina) == 1)
                                {  
                                    cabecalho();
                                    printf("DADOS INCORRETOS! POR FAVOR INSIRA UM CNPJ VÁLIDO NO FORMATO XX.XXX.XXX/XXXX-XX\n\n");
                                    printf("Digite o CNPJ da usina que deseja cadastrar: ");
                                    scanf(" %s", usinas_cadastro.cnpj_usina);
                                }
                            }
                        }
                    }
                }
            }
        }
        fclose(arquivo2);

        //verificação para conferir se o cnpj não pertence a algum usuário cadastrado
        arquivo2 = fopen("consumidores_cadastrados.txt", "rb");
        if(arquivo2 == NULL)
        {
            printf("Probemas na abertura do arquivo!\n");
        } else
        {
            while(fread(&consumidor_verificacao, sizeof(CONSUMIDOR), 1, arquivo2) == 1){
                if(strcmp(usinas_cadastro.cnpj_usina, consumidor_verificacao.cnpj_consumidor) == 0)
                {   
                    printf("%s\n", consumidor_verificacao.cnpj_consumidor);
                    printf("%s\n", usinas_cadastro.cnpj_usina);
                    printf("O CNPJ QUE VOCÊ TENTOU CADASTRADAR JA PERTENCE A ALGUM CONSUMIDOR CADASTRADO!\n\n");
                    printf("Deseja tentar o cadastro da usina novamente utilizando com outro CNPJ? (s/n)\n");
                    if(getch() == 'n' || getch() == 'N')
                    {
                        fclose(arquivo);
                        fclose(arquivo2);
                        return;
                    }
                    if(getch() == 's' || getch() == 'S')
                    {
                        while(strcmp(usinas_cadastro.cnpj_usina, consumidor_verificacao.cnpj_consumidor) == 0)
                        {
                            cabecalho();
                            printf("Como o CNPJ digitado anteriormente já estava cadastrado. Digite novamente o CNPJ da usina que deseja cadastrar (formato -> XX.XXX.XXX/XXXX-XX): ");
                            scanf(" %s", usinas_cadastro.cnpj_usina);
                            //looping para caso um cnpj não seja válido
                            if (verificar_cnpj(usinas_cadastro.cnpj_usina))
                            {
                                while(verificar_cnpj(usinas_cadastro.cnpj_usina) == 1)
                                {  
                                    cabecalho();
                                    printf("DADOS INCORRETOS! POR FAVOR INSIRA UM CNPJ VALIDO NO FORMATO XX.XXX.XXX/XXXX-XX\n\n");
                                    printf("Digite o CNPJ da usina que deseja cadastrar: ");
                                    scanf(" %s", usinas_cadastro.cnpj_usina);
                                }
                            }
                        }
                    }
                }
            }
        }
        fclose(arquivo2);

        printf("\nDigite o nome da empresa que deseja cadastrar: ");
        scanf(" %[^\n]s", usinas_cadastro.nome_usina);

        printf("\nDigite a data que marca o incio das operacoes da empresa que deseja cadastrar (formato -> XX/XX/XXX): ");
        scanf(" %s", usinas_cadastro.data_inicio_operacao);
        //looping para caso uma data não seja válido
        if (verificar_data(usinas_cadastro.data_inicio_operacao) == 1) 
        {
            do
            {   
                cabecalho();
                printf("DADOS INCORRETOS! POR FAVOR INSIRA UMA DATA VALIDA (formato -> XX/XX/XXX)\n");
                printf("\nDigite a data que marca o incio das operacoes da empresa que deseja cadastrar: ");
                scanf(" %s", usinas_cadastro.data_inicio_operacao);
                //procedimento para evitar ficar em um looping infinito mesmo com datas corretas
                if (verificar_data(usinas_cadastro.data_inicio_operacao) == 1)
                {
                    exit = 1;
                } else {
                    exit = 2;
                }
            }while((exit == 1));
        }

        printf("\nDigite a potencia de geracao estimada, em MW, da empresa que deseja cadastrar: ");
        scanf("%d", &usinas_cadastro.potencia_estimada);
        if (usinas_cadastro.potencia_estimada < 1) 
        {
            while(usinas_cadastro.potencia_estimada < 1)
            {
                cabecalho();
                printf("DADOS INCORRETOS! POR FAVOR INSIRA UMA POTENCIA VALIDA (>0).\n");
                printf("\nDigite a potencia de geracao estimada, em MW, da empresa que deseja cadastrar: ");
                scanf("%d", &usinas_cadastro.potencia_estimada);
            }
        }
    }
    fwrite(&usinas_cadastro, sizeof(USINAS), 1, arquivo);
    fclose(arquivo);
}

//função que recebe e verifica se os dados dos consumidores que estão sendo cadastradss são válidos
void cadastro_consumidor() 
{
    FILE* arquivo;
    FILE* arquivo2;
    CONSUMIDOR consumidor_cadastro, consumidor_verificacao;
    USINAS consumidor_verificacao_u;
    int escolha_consumidor;
    
    arquivo = fopen("consumidores_cadastrados.txt", "ab");

    if(arquivo == NULL)// pra ver se o arquivo abriu certinho
    {
        printf("Problemas na abertura do arquivo!\n");
    } else
    {
        printf("Digite '1' caso queria usar o CPF para realizar o cadastro ou digite '2' para usar o CNPJ: ");
        scanf("%d", &escolha_consumidor);

        //looping para caso o consumidor insira uma opçao que não existe
        while (escolha_consumidor < 1 || escolha_consumidor > 2 || escolha_consumidor == 0)
        {   
            fflush(stdin);
            cabecalho();
            printf("OPCAO INVALIDA! ESCOLHA ENTRE CNPJ(DIGITE '2') E CPF(DIGITE '1'): ");
            scanf("%d", &escolha_consumidor);
        }

        switch(escolha_consumidor)
        {
        case 1:
            cabecalho();
            printf("Digite o seu CPF (formato -> XXX.XXX.XXX-XX): ");
            scanf(" %s", consumidor_cadastro.cpf);
            //looping para caso um CPF não seja válido
            if (verificar_cpf(consumidor_cadastro.cpf)) 
            {
                while(verificar_cpf(consumidor_cadastro.cpf) == 1)
                {   
                    cabecalho();
                    printf("DADOS INCORRETOS! POR FAVOR INSIRA UM CPF VALIDO NO FORMATO XXX.XXX.XXX-XX\n");
                    printf("\nDigite novamente o CPF: ");
                    scanf(" %s", consumidor_cadastro.cpf);
                }
            }
            //verificar se não há nenhum outro usuário no sistema com esse cpf já cadastrado
            arquivo2 = fopen("consumidores_cadastrados.txt", "rb");
            if(arquivo2 == NULL)// pra ver se o arquivo abriu certinho
            {
                printf("Problemas na abertura do arquivo!\n");
            } else
            {
                while(fread(&consumidor_verificacao, sizeof(CONSUMIDOR), 1, arquivo2) == 1)
                {
                    if(strcmp(consumidor_cadastro.cpf, consumidor_verificacao.cpf) == 0)
                    {
                        printf("\nEsse CPF ja pertence a um consumidor cadastrado!\n\n");
                        printf("Deseja tentar realizar o cadastro com outro CPF? (s/n)  ");
                        if(getch() == 'n' || getch() == 'N')
                        {
                            fclose(arquivo);
                            fclose(arquivo2);
                            return;
                        }
                        if(getch() == 's' || getch() == 'S')
                        {
                            while(strcmp(consumidor_cadastro.cpf, consumidor_verificacao.cpf) == 0)
                            { 
                                cabecalho();
                                printf("Como o CPF digitado anteriormente ja estava cadastrado. Digite novamente CPF do consumidor que deseja cadastrar (formato -> XX.XXX.XXX/XXXX-XX): ");
                                scanf(" %s", consumidor_cadastro.cpf);
                                //looping para caso um CPF não seja válido
                                if (verificar_cpf(consumidor_cadastro.cpf)) 
                                {
                                    while(verificar_cpf(consumidor_cadastro.cpf) == 1)
                                    {   
                                        cabecalho();
                                        printf("DADOS INCORRETOS! POR FAVOR INSIRA UM CPF VALIDO NO FORMATO XXX.XXX.XXX-XX\n");
                                        printf("\nDigite novamente o CPF: ");
                                        scanf(" %s", consumidor_cadastro.cpf);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            fclose(arquivo2);
            break;
        case 2:
            cabecalho();
            printf("Digite o seu CNPJ (formato -> XX.XXX.XXX/XXXX-XX): ");
            scanf(" %s", consumidor_cadastro.cnpj_consumidor);
            //looping para caso um cnpj não seja válido
            if (verificar_cnpj(consumidor_cadastro.cnpj_consumidor))  
            {
                while(verificar_cnpj(consumidor_cadastro.cnpj_consumidor) == 1)
                {
                    cabecalho();
                    printf("DADOS INCORRETOS! POR FAVOR INSIRA UM CNPJ VALIDO NO FORMATO XX.XXX.XXX/XXXX-XX\n");
                    printf("\nDigite novamente o CNPJ: ");
                    scanf(" %s", consumidor_cadastro.cnpj_consumidor);
                }
            }
            //verificação para ver se há algum consumidor cadastrado com o CNPJ lido
            arquivo2 = fopen("consumidores_cadastrados.txt", "rb");
            if(arquivo2 == NULL)// pra ver se o arquivo abriu certinho
            {
                printf("Problemas na abertura do arquivo!\n");
            } else
            {
                while(fread(&consumidor_verificacao, sizeof(CONSUMIDOR), 1, arquivo2) == 1)
                {
                    if(strcmp(consumidor_cadastro.cnpj_consumidor, consumidor_verificacao.cnpj_consumidor) == 0)
                    {
                        printf("\nEsse CNPJ já pertence a um consumidor cadastrado!\n\n");
                        printf("Deseja tentar realizar o cadastro com outro CNPJ? (s/n)  ");
                        if(getch() == 'n' || getch() == 'N')
                        {
                            fclose(arquivo);
                            fclose(arquivo2);
                            return;
                        }
                        if(getch() == 's' || getch() == 'S')
                        {
                            while(strcmp(consumidor_cadastro.cnpj_consumidor, consumidor_verificacao.cnpj_consumidor) == 0)
                            { 
                                cabecalho();
                                printf("Como o CNPJ digitado anteriormente ja estava cadastrado. Digite novamente CNPJ do consumidor que deseja cadastrar (formato -> XX.XXX.XXX/XXXX-XX): ");
                                scanf(" %s", consumidor_cadastro.cnpj_consumidor);
                                //looping para caso um cnpj não seja válido
                                if (verificar_cnpj(consumidor_cadastro.cnpj_consumidor))  
                                {
                                    while(verificar_cnpj(consumidor_cadastro.cnpj_consumidor) == 1)
                                    {
                                        cabecalho();
                                        printf("DADOS INCORRETOS! POR FAVOR INSIRA UM CNPJ VALIDO NO FORMATO XX.XXX.XXX/XXXX-XX\n");
                                        printf("\nDigite novamente o CNPJ: ");
                                        scanf(" %s", consumidor_cadastro.cnpj_consumidor);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            fclose(arquivo2);

            //verificação para ver se esse CNPJ já pertence a alguma usina cadastrada
            arquivo2 = fopen("usinas_cadastradas.txt", "rb");
            if(arquivo2 == NULL)// pra ver se o arquivo abriu certinho
            {
                printf("Problemas na abertura do arquivo!\n");
            } else
            {
                while(fread(&consumidor_verificacao_u, sizeof(USINAS), 1, arquivo2) == 1)
                {
                    if(strcmp(consumidor_cadastro.cnpj_consumidor, consumidor_verificacao_u.cnpj_usina) == 0)
                    {
                        printf("\nEsse CNPJ já pertence a uma usina cadastrada!\n\n");
                        printf("Deseja tentar realizar o cadastro com outro CNPJ? (s/n)  ");
                        if(getch() == 'n' || getch() == 'N')
                        {
                            fclose(arquivo);
                            fclose(arquivo2);
                            return;
                        }
                        if(getch() == 's' || getch() == 'S')
                        {
                            while(strcmp(consumidor_cadastro.cnpj_consumidor, consumidor_verificacao_u.cnpj_usina) == 0)
                            { 
                                cabecalho();
                                printf("Como o CNPJ digitado anteriormente já estava cadastrado com uma usina. Digite novamente CNPJ do consumidor que deseja cadastrar (formato -> XX.XXX.XXX/XXXX-XX): ");
                                scanf(" %s", consumidor_cadastro.cnpj_consumidor);
                                //looping para caso um cnpj não seja válido
                                if (verificar_cnpj(consumidor_cadastro.cnpj_consumidor))  
                                {
                                    while(verificar_cnpj(consumidor_cadastro.cnpj_consumidor) == 1)
                                    {
                                        cabecalho();
                                        printf("DADOS INCORRETOS! POR FAVOR INSIRA UM CNPJ VÁLIDO NO FORMATO XX.XXX.XXX/XXXX-XX\n");
                                        printf("\nDigite novamente o CNPJ: ");
                                        scanf(" %s", consumidor_cadastro.cnpj_consumidor);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            fclose(arquivo2);
            break;
        }
            
        printf("\nDigite seu o nome: ");
        scanf(" %[^\n]s", consumidor_cadastro.nome);
    }
    fwrite(&consumidor_cadastro, sizeof(CONSUMIDOR), 1, arquivo);
    fclose(arquivo);
}

//função que firma um contrato entre um consumidor uma usina já cadastrados no sistema
void contrato() 
{   
    FILE* arquivo;
    FILE* arquivo2;
    CONTRATO contratos;
    USINAS verificacao_u;
    CONSUMIDOR verificacao_c;
    int escolha_usuario, exit, validador;

    arquivo = fopen("contratos.txt", "ab"); //A - Append

    if(arquivo == NULL)// pra ver se o arquivo abriu certinho
    {
        printf("Problemas na abertura do arquivo!\n");
    } else
    {
        printf("Digite o CNPJ da usina que deseja fechar o contrato (formato -> XX.XXX.XXX/XXXX-XX): ");
        scanf(" %s", contratos.cnpj_usina_contrato);
        //looping para caso um cnpj não seja válido
        if (verificar_cnpj(contratos.cnpj_usina_contrato)) 
        {
            while(verificar_cnpj(contratos.cnpj_usina_contrato) == 1)
            {  
                cabecalho();
                printf("DADOS INCORRETOS! POR FAVOR INSIRA UM CNPJ VALIDO NO FORMATO XX.XXX.XXX/XXXX-XX\n\n");
                printf("Digite novamente o CNPJ: ");
                scanf(" %s", contratos.cnpj_usina_contrato);
            }
        }
        //aqui iremos procurar no sistema se existe alguma usina cadastrada com esse CNPJ
        arquivo2 = fopen("usinas_cadastradas.txt", "rb");
        if(arquivo2 == NULL)// pra ver se o arquivo abriu certinho
        {
            printf("Problemas na abertura do arquivo!\n");
        } else
        {
            while(fread(&verificacao_u, sizeof(USINAS), 1, arquivo2) == 1)
            {   
                if(strcmp(contratos.cnpj_usina_contrato, verificacao_u.cnpj_usina) == 0)
                {
                    validador = 1;
                }
            }
        }
        fclose(arquivo2);

        if(validador == 1)
        {        
            printf("\nDigite '1' caso queria usar o CPF para localizar o seu cadastro ou digite '2' para usar o CNPJ: ");
            scanf("%d", &escolha_usuario);
            while((escolha_usuario != 1 && escolha_usuario != 2) || escolha_usuario == 0)
            {
                fflush(stdin);
                cabecalho();
                printf("OPCAO INVALIDA! ESCOLHA ENTRE CNPJ(DIGITE '2') E CPF(DIGITE '1'): ");
                scanf("%d", &escolha_usuario);
            }
            
            switch(escolha_usuario)
            {
            case 1:
                cabecalho();
                printf("Digite o seu CPF (formato -> XXX.XXX.XXX-XX): ");
                scanf(" %s", contratos.cpf_id);
                //looping para caso um CPF não seja válido
                if (verificar_cpf(contratos.cpf_id)) 
                {
                    while(verificar_cpf(contratos.cpf_id) == 1)
                    {   
                        cabecalho();
                        printf("DADOS INCORRETOS! POR FAVOR INSIRA UM CPF VALIDO NO FORMATO XXX.XXX.XXX-XX\n");
                        printf("\nDigite novamente o CPF: ");
                        scanf(" %s", contratos.cpf_id);
                    }
                }
                //verificar se existe no sistema algum usuário com esse cpf cadastrado
                arquivo2 = fopen("consumidores_cadastrados.txt", "rb");
                if(arquivo2 == NULL)// pra ver se o arquivo abriu certinho
                {
                    printf("Problemas na abertura do arquivo!\n");
                } else
                {
                    validador = 0;
                    while(fread(&verificacao_c, sizeof(CONSUMIDOR), 1, arquivo2) == 1)
                    {
                        if(strcmp(contratos.cpf_id, verificacao_c.cpf) == 0)
                        {
                            validador = 1;
                        }
                    }
                }
                fclose(arquivo2);
                break;
            case 2:
                cabecalho();
                printf("Digite o seu CNPJ (formato -> XX.XXX.XXX/XXXX-XX): ");
                scanf(" %s", contratos.cnpj_id);
                //looping para caso um cnpj não seja válidoS
                if(verificar_cnpj(contratos.cnpj_id) == 1)  
                {
                    while(verificar_cnpj(contratos.cnpj_id ) == 1)
                    {
                        cabecalho();
                        printf("DADOS INCORRETOS! POR FAVOR INSIRA UM CNPJ VALIDO NO FORMATO XX.XXX.XXX/XXXX-XX\n");
                        printf("\nDigite novamente o CNPJ: ");
                        scanf(" %s", contratos.cnpj_id);
                    }
                }
                //verificando se há algum consumidor com esse ID(CNPJ) no sistema
                arquivo2 = fopen("consumidores_cadastrados.txt", "rb");
                if(arquivo2 == NULL)// pra ver se o arquivo abriu certinho
                {
                    printf("Problemas na abertura do arquivo!\n");
                } else
                {
                    validador = 0;
                    while(fread(&verificacao_c, sizeof(CONSUMIDOR), 1, arquivo2) == 1)
                    {
                        if(strcmp(contratos.cnpj_id, verificacao_c.cnpj_consumidor) == 0)
                        {
                            validador = 2;
                        }
                    }
                }
                fclose(arquivo2);
                break;
            }
        } else
        {
            cabecalho();
            printf("O CONTRATO NAO FOI PROSSEGUIDO POIS NAO EXISTE NENHUMA USINA COM ESSE CNPJ NO SISTEMA\n");
            getch();
            fclose(arquivo);
            fclose(arquivo2);
            return;
        }

        if((validador =! 1 && validador != 2))
        {
            if(escolha_usuario == 1)
            {
                cabecalho();
                printf("O CONTRATO NAO FOI PROSSEGUIDO POIS NAO EXISTE NENHUM CONSUMIDOR CADASTRADO COM ESSE CPF NO SISTEMA\n");
                getch();
                fclose(arquivo);
                fclose(arquivo2);
                return;
            }
            if(escolha_usuario == 2)
            {
                cabecalho();
                printf("O CONTRATO NAO FOI PROSSEGUIDO POIS NAO EXISTE NENHUM CONSUMIDOR COM ESSE CNPJ NO SISTEMA\n");
                getch();
                fclose(arquivo);
                fclose(arquivo2);
                return;
            }
        } else
        {
            printf("\nDigite a data que marca o inicio deste contrato (formato -> XX/XX/XXX): ");
            scanf(" %s", contratos.data_inicio_contrato);
            //looping para caso uma data não seja válido
            if(verificar_data(contratos.data_inicio_contrato) == 1) 
            {
                do
                {
                    cabecalho();
                    printf("DADOS INCORRETOS! POR FAVOR INSIRA UMA DATA VALIDA\n");
                    printf("\nDigite novamente a data: (formato -> XX/XX/XXX) ");
                    scanf(" %s", contratos.data_inicio_contrato);
                    //procedimento para evitar ficar em um looping infinito mesmo com datas corretas
                    if(verificar_data(contratos.data_inicio_contrato) == 1)
                    {
                        exit = 1;
                    } else {
                        exit = 2;
                    }
                }while(exit == 1);
            }
            //aqui veremos se data inserida pelo usuario precede a data que marca o incio de operação da usina que o consumidor deseja fazer contrato
            if(compara_data(contratos.data_inicio_contrato, contratos.cnpj_usina_contrato) == 1)
            {
                do
                {
                    cabecalho();
                    printf("POR FAVOR INSIRA UMA DATA VALIDA QUE NAO ANTECEDA A DATA QUE MARCA O INCIO DE OPERACOES DA USINA\n");
                    printf("\nDigite novamente a data: (formato -> XX/XX/XXX) ");
                    scanf(" %s", contratos.data_inicio_contrato);
                    //procedimento para evitar ficar em um looping infinito mesmo com datas corretas
                    if(compara_data(contratos.data_inicio_contrato, contratos.cnpj_usina_contrato) == 1)
                    {
                        exit = 1;
                    } else {
                        exit = 2;
                    }
                }while(exit == 1);
            }

            printf("\nDigite a potencia que deseja contratar, em MW: ");
            scanf("%d", &contratos.potencia_combinada);
            if (contratos.potencia_combinada <= 1) 
            {
                while(contratos.potencia_combinada <= 0)
                {
                    printf("DADOS INCORRETOS! POR FAVOR INSIRA UMA POTENCIA VALIDA (>0).\n");
                    printf("\nDigite novamente a potencia, em MW, que deseja contratar: ");
                    scanf("%d", &contratos.potencia_combinada);
                }
            }
            if(contratos.potencia_combinada > potencia_restante(contratos.cnpj_usina_contrato))
            {
                while(contratos.potencia_combinada > potencia_restante(contratos.cnpj_usina_contrato))
                {
                    cabecalho();
                    printf("POTENCIA RESTANTE NA USINA E MENOR QUE O VALOR QUE VOCE DESEJA CONTRATAR.\n");
                    printf("\nDigite novamente a potencia, em MW, que deseja contratar: ");
                    scanf("%d", &contratos.potencia_combinada);
                }
            } 
        }   
    }
    fwrite(&contratos, sizeof(CONTRATO), 1, arquivo);
    fclose(arquivo);
} 

//função que irá disponibilizar ao usuário todas as usinas que estão cadastradas com toda suas informações atualizadas 
void consultar_usina()
{
    FILE* arquivo;
    FILE* arquivo2;
    FILE* arquivo3; 
    USINAS usinas_cadastradas;
    CONTRATO contratos_fechados;
    CONSUMIDOR nome;
    char cnpj[TAM_CNPJ];
    int validador = 0;

    arquivo = fopen("usinas_cadastradas.txt", "rb"); // R - read
    arquivo2 = fopen("contratos.txt", "rb");
    arquivo3 = fopen("consumidores_cadastrados.txt", "rb");
     if(arquivo == NULL || arquivo2 == NULL || arquivo3 == NULL)// pra ver se o arquivo abriu certinho
    {
        printf("Problemas na abertura do arquivo!\n");
    } else
    {
        printf("Digite o CNPJ da usina que deseja consultar: ");
        scanf(" %s", cnpj);
        //looping para ver se é um cnpj válido
        if (verificar_cnpj(cnpj) == 1)
        {
            while(verificar_cnpj(cnpj) == 1)
            {  
                cabecalho();
                printf("POR FAVOR INSIRA UM CNPJ VALIDO NO FORMATO XX.XXX.XXX/XXXX-XX\n\n");
                printf("Digite novamente o CNPJ da usina que deseja consultar: ");
                scanf(" %s", cnpj);
            }
        }
        while(fread(&usinas_cadastradas, sizeof(USINAS), 1, arquivo) == 1)
        {
            if(strcmp(cnpj, usinas_cadastradas.cnpj_usina) == 0)
            {
                validador = 1;
                printf("\nCNPJ: %s\n", usinas_cadastradas.cnpj_usina);
                printf("NOME DA USINA: %s\n", usinas_cadastradas.nome_usina);
                printf("DATA QUE MARCA O INICIO DE SUA OPERACAO: %s\n", usinas_cadastradas.data_inicio_operacao);
                printf("POTENCIA ESTIMADA: %d MW\n", usinas_cadastradas.potencia_estimada);
                printf("POTENCIA RESTANTE DA USINA: %.2f% \n", percentual_potencia(usinas_cadastradas.cnpj_usina));
                printf("-----------------------------------------------------------------------------------------\n\n");
            }
        }
        
        while(fread(&contratos_fechados, sizeof(CONTRATO), 1, arquivo2) == 1)
        {
            if(strcmp(cnpj, contratos_fechados.cnpj_usina_contrato) == 0)
            {   
                while(fread(&nome, sizeof(CONSUMIDOR), 1, arquivo3) == 1)
                {   if(strlen(contratos_fechados.cnpj_id) == 18)
                    {
                            if(strcmp(nome.cnpj_consumidor, contratos_fechados.cnpj_id) == 0)
                        {
                            printf("NOME: %s\n", nome.nome);
                        }
                    }else
                    {
                        if(strcmp(nome.cpf, contratos_fechados.cpf_id) == 0)
                        {
                            printf("NOME: %s\n", nome.nome);
                        }
                    }    
                }
                rewind(arquivo3);
                if(strlen(contratos_fechados.cnpj_id) == 18)
                {
                    printf("ID: %s\n", contratos_fechados.cnpj_id);
                } else
                {
                    printf("ID: %s\n", contratos_fechados.cpf_id);
                }  
                printf("DATA QUE MARCA O INICIO DO CONTRATO: %s\n", contratos_fechados.data_inicio_contrato);
                printf("POTENCIA COMBINADA: %d MW\n", contratos_fechados.potencia_combinada);
                printf("-----------------------------------------------------------------------------------------\n\n");
            }
        }
        if( validador != 1)
        {
            cabecalho();
            printf("NAO FOI POSSIVEL SER LOCALIZADA NENHUMA USINA CADASTRADA COM ESSE CNPJ\n");
            printf("\nPOR FAVOR CADASTRE A USINA PRIMEIRO!");
            getch();
            fclose(arquivo);
            return;
        }
    }
    
    fclose(arquivo);
    getch();
}

//função que consulta um consumidor especifico, determinado pelo usuário, e todos os seus vínculos
void consultar_consumidor()
{
    FILE* arquivo;
    FILE* arquivo2;
    FILE* arquivo3; 
    USINAS usinas_cadastradas;
    CONTRATO contratos_fechados;
    CONSUMIDOR dados;
    char cnpj[TAM_CNPJ];
    char cpf[TAM_CPF];
    char escolha_usuario[2];
    char escolha_um[2] = "1";
    char escolha_dois[2] = "2";
    int validador = 0;

    printf("DIGITE 1 CASO DESEJE PROCURAR O CONUSMIDOR PELO CPF E 2 CASO DESEJE BUSCAR PELO CNPJ: ");
    scanf(" %s", escolha_usuario);

    while (strcmp(escolha_usuario,escolha_um) != 0 && strcmp(escolha_usuario,escolha_dois) != 0)
    {
        cabecalho();
        printf("POR FAVOR INSIRA UMA OPCAO VALIDA!\n\n");
        printf("DIGITE 1 CASO DESEJE PROCURAR O CONUSMIDOR PELO CPF E 2 CASO DESEJE BUSCAR PELO CNPJ:");
        scanf(" %s", escolha_usuario);
    }

    if(strcmp(escolha_usuario,escolha_um) == 0)
    {
        cabecalho();
        printf("Digite o CPF do consumidor que deseja consultar: ");
        scanf(" %s", cpf);
         //looping para caso um CPF não seja válido
        if (verificar_cpf(cpf)) 
        {
            while(verificar_cpf(cpf) == 1)
            {   
                cabecalho();
                printf("POR FAVOR INSIRA UM CPF VALIDO NO FORMATO XXX.XXX.XXX-XX\n");
                printf("\nDigite novamente o CPF do consumidor que deseja consultar: ");
                scanf(" %s", cpf);
            }
        }
    } else
    {
        if(strcmp(escolha_usuario,escolha_dois) == 0)
        {   
            cabecalho();
            printf("Digite o CNPJ do consumidor que deseja consultar: ");
            scanf(" %s", cnpj);
            //looping para ver se é um cnpj válido
            if (verificar_cnpj(cnpj) == 1)
            {
                while(verificar_cnpj(cnpj) == 1)
                {  
                    cabecalho();
                    printf("POR FAVOR INSIRA UM CNPJ VALIDO NO FORMATO XX.XXX.XXX/XXXX-XX\n\n");
                    printf("Digite novamente o CNPJ do consumidor que deseja consultar: ");
                    scanf(" %s", cnpj);
                }
            }
        }
    }
    arquivo = fopen("consumidores_cadastrados.txt", "rb"); // R - read
    arquivo2 = fopen("contratos.txt", "rb");
    arquivo3 = fopen("usinas_cadastradas.txt", "rb");
     if(arquivo == NULL || arquivo2 == NULL || arquivo3 == NULL)// pra ver se o arquivo abriu certinho
    {
        printf("Problemas na abertura do arquivo!\n");
    } else
    {
        if(strcmp(escolha_usuario,escolha_um) == 0)
        {
            limpar_tela();
            //dados do consumidor
            while(fread(&dados, sizeof(CONSUMIDOR), 1, arquivo) == 1)
            {
                if(strcmp(cpf, dados.cpf) == 0)
                {
                    validador = 1;
                    cabecalho();
                    printf("NOME DO CONSUMIDOR: %s\n", dados.nome);
                    printf("ID: %s\n", dados.cpf);
                    printf("-----------------------------------------------------------------------------------------\n\n");
                    printf("=================CONTRATOS=================\n\n");
                }
            }
            //dados dos contratos/usina
            while(fread(&contratos_fechados, sizeof(CONTRATO), 1, arquivo2) == 1)
            {
                if(strcmp(cpf, contratos_fechados.cpf_id) == 0)
                {   
                    while(fread(&usinas_cadastradas, sizeof(USINAS), 1, arquivo3) == 1)
                    {    
                        if(strcmp(usinas_cadastradas.cnpj_usina, contratos_fechados.cnpj_usina_contrato) == 0)
                        {
                            printf("NOME DA USINA: %s\n", usinas_cadastradas.nome_usina);
                            printf("CNPJ: %s\n", usinas_cadastradas.cnpj_usina);
                            printf("DATA QUE MARCA O INICIO DE SUA OPERACAO: %s\n", usinas_cadastradas.data_inicio_operacao);
                            printf("POTENCIA ESTIMADA: %d MW\n", usinas_cadastradas.potencia_estimada);
                            printf("-----------------------------------------------------------------------------------------\n");
                            break;
                        }
                    }
                }
            }
        } else
        {
            cabecalho();
            //dados do consumidor
            while(fread(&dados, sizeof(CONSUMIDOR), 1, arquivo) == 1)
            {
                if(strcmp(cnpj, dados.cnpj_consumidor) == 0)
                {
                    validador = 1;
                    cabecalho();
                    printf("NOME DO CONSUMIDOR: %s\n", dados.nome);
                    printf("ID: %s\n", dados.cnpj_consumidor);
                    printf("-----------------------------------------------------------------------------------------\n\n");
                }
            }
            //dados dos contratos/usina
            printf("==================================CONTRATOS==================================\n\n");
            while(fread(&contratos_fechados, sizeof(CONTRATO), 1, arquivo2) == 1)
            {
                if(strcmp(cnpj, contratos_fechados.cnpj_id) == 0)
                {   
                    while(fread(&usinas_cadastradas, sizeof(USINAS), 1, arquivo3) == 1)
                    {    
                        if(strcmp(usinas_cadastradas.cnpj_usina, contratos_fechados.cnpj_usina_contrato) == 0)
                        {
                            printf("NOME DA USINA: %s\n", usinas_cadastradas.nome_usina);
                            printf("CNPJ: %s\n", usinas_cadastradas.cnpj_usina);
                            printf("DATA QUE MARCA O INICIO DE SUA OPERACAO: %s\n", usinas_cadastradas.data_inicio_operacao);
                            printf("POTENCIA ESTIMADA: %d MW\n", usinas_cadastradas.potencia_estimada);
                            printf("-----------------------------------------------------------------------------------------\n");
                            break;
                        }
                    }
                }
            }
        }
        if( validador != 1)
        {
            cabecalho();
            printf("NAO FOI POSSIVEL SER LOCALIZADA NENHUM CONSUMIDOR CADASTRADO COM ESSE ID\n");
            printf("\nPOR FAVOR CADASTRE O CONSUMIDOR PRIMEIRO!");
            getch();
            fclose(arquivo);
            fclose(arquivo2);
            fclose(arquivo3);
            return;
        }
    }
    fclose(arquivo);
    fclose(arquivo2);
    fclose(arquivo3);
    getch();
}

//função que exlui uma usina, que esteja cadastrada no sitema, selecionada pelo usuário
void excluir_usinas()
{
    FILE* arquivo1;
    FILE* arquivo2;
    USINAS usina;
    char cnpj[TAM_CNPJ];
    int validador = 0;
    arquivo1 = fopen("usinas_cadastradas.txt", "rb");
    arquivo2 = fopen("usinas_cadastradas_temporariamente.txt", "ab");
    if(arquivo1 == NULL || arquivo2 == NULL)
    {
        printf("Problemas na abertura do arquivo!");
    } else
    {
        printf("Digite o CNPJ referente a usina que deseja exluir: ");
        scanf(" %s", cnpj);
        //looping para caso não seja um cnpj válido
        if (verificar_cnpj(cnpj) == 1)
        {
            while(verificar_cnpj(cnpj) == 1)
            {  
                cabecalho();
                printf("POR FAVOR INSIRA UM CNPJ VALIDO NO FORMATO XX.XXX.XXX/XXXX-XX\n\n");
                printf("Digite novamente o CNPJ da usina que deseja excluir: ");
                scanf(" %s", cnpj);
            }
        }
        //aqui iremos procurar no sistema se existe alguma usina cadastrada com esse CNPJ
        while(fread(&usina, sizeof(USINAS), 1, arquivo1) == 1)
        {   
            if(strcmp(cnpj, usina.cnpj_usina) == 0)
            {
                validador = 1;
            }
        }
        rewind(arquivo1);
        if(validador == 1)
        {
            while(fread(&usina, sizeof(USINAS), 1,arquivo1) == 1)
            {   
                if(strcmp(cnpj, usina.cnpj_usina) != 0)
                {
                    fwrite(&usina, sizeof(USINAS), 1, arquivo2);
                }        
            }
            //excluindo contratos que pertencem a usina excluida
            rewind(arquivo1);
            excluir_contrato_u(cnpj);

        } else
        {
            cabecalho();
            printf("\e[1mNao foi possivel localizar nenhuma usina com esse CNPJ no sistema para ser feita a exclusao!\e\n");
            getch();
        } 
    }
    if(validador == 1)
    {
        fclose(arquivo1);
        fclose(arquivo2);
        remove("usinas_cadastradas.txt");
        rename("usinas_cadastradas_temporariamente.txt", "usinas_cadastradas.txt");
        loading();
        printf("\nUSINA EXCLUIDA COM SUCESSO!\n");
        getch();
    }
}
    
//função que exlui um consumidor, que esteja cadastrado no sitema, selecionado pelo usuário
void excluir_consumidor()
{
    FILE* arquivo1;
    FILE* arquivo2;
    CONSUMIDOR consumidores;
    char cnpj[TAM_CNPJ];
    char cpf[TAM_CPF];
    char escolha_usuario[1];
    char escolha_um[2] = "1";
    char escolha_dois[2] = "2";
    int validador = 0;
    arquivo1 = fopen("consumidores_cadastrados.txt", "rb");
    arquivo2 = fopen("consumidores_cadastrados_temporariamente.txt", "ab");
    if(arquivo1 == NULL || arquivo2 == NULL)
    {
        printf("Problemas na abertura do arquivo!");
    } else
    {
        printf("DIGITE 1 CASO DESEJE PROCURAR O CADASTRO DO CONUSMIDOR, QUE DEVE SER EXCLUIDO, PELO CPF E 2 CASO DESEJE BUSCAR PELO CNPJ: ");
        scanf(" %s", escolha_usuario);

        while (strcmp(escolha_usuario,escolha_um) != 0 && strcmp(escolha_usuario,escolha_dois) != 0)
        {
            cabecalho();
            printf("POR FAVOR INSIRA UMA OPCAO VALIDA!\n\n");
            printf("DIGITE 1 CASO DESEJE PROCURAR O CADASTRO DO CONUSMIDOR, QUE DEVE SER ESCLUÍDO, PELO CPF E 2 CASO DESEJE BUSCAR PELO CNPJ:");
            scanf(" %s", escolha_usuario);
        }

        if(strcmp(escolha_usuario, escolha_um) == 0)
        {
            cabecalho();
            printf("Digite o CPF do consumidor cadatrado que deseja excluir: ");
            scanf(" %s", cpf);
            //looping para caso um CPF não seja válido
            if (verificar_cpf(cpf)) 
            {
                while(verificar_cpf(cpf) == 1)
                {   
                    limpar_tela();
                    printf("POR FAVOR INSIRA UM CPF VALIDO NO FORMATO XXX.XXX.XXX-XX\n");
                    printf("\nDigite novamente o CPF do consumidor cadastrado que deseja excluir: ");
                    scanf(" %s", cpf);
                }
            }
            //aqui iremos procurar no sistema se existe algum consumidor cadastrado com esse CPF
            while(fread(&consumidores, sizeof(CONSUMIDOR), 1, arquivo1) == 1)
            {   
                if(strcmp(cpf, consumidores.cpf) == 0)
                {
                    validador = 1;
                }
            }
            rewind(arquivo1);
        } else
        {
            if(strcmp(escolha_usuario, escolha_dois) == 0)
            {   
                cabecalho();
                printf("Digite o CNPJ do consumidor cadastrado que deseja excluir: ");
                scanf(" %s", cnpj);
                //looping para ver se é um cnpj válido
                if (verificar_cnpj(cnpj) == 1)
                {
                    while(verificar_cnpj(cnpj) == 1)
                    {  
                        cabecalho();
                        printf("POR FAVOR INSIRA UM CNPJ VALIDO NO FORMATO XX.XXX.XXX/XXXX-XX\n\n");
                        printf("Digite novamente o CNPJ do consumidor cadastrado que deseja excluir: ");
                        scanf(" %s", cnpj);
                    }
                }
                //aqui iremos procurar no sistema se existe algum consumidor cadastrador com esse CNPJ
                while(fread(&consumidores, sizeof(CONSUMIDOR), 1, arquivo1) == 1)
                {   
                    if(strcmp(cnpj, consumidores.cnpj_consumidor) == 0)
                    {
                        validador = 1;
                    }
                }
            }
        }
 
        if(validador == 1)// o codigo só prossegue se o usuário tiver inseridor um ID pertencente a algum consumidor cadastrado no sitema
        {   
            if(strcmp(escolha_usuario, escolha_um) == 0)
            {
                while(fread(&consumidores, sizeof(CONSUMIDOR), 1, arquivo1) == 1)
                {
                    if(strcmp(cpf, consumidores.cpf) != 0)
                    {
                        fwrite(&consumidores, sizeof(CONSUMIDOR), 1, arquivo2);
                    }          
                }
                //excluindo contratos que pertencem a usina excluida
                excluir_contrato_c_cpf(cpf);
            } else
            {
                if(strcmp(escolha_usuario, escolha_dois) == 0)
                {
                    while(fread(&consumidores, sizeof(CONSUMIDOR), 1, arquivo1) == 1)
                    {
                        if(strcmp(cnpj, consumidores.cnpj_consumidor) != 0)
                        {
                            fwrite(&consumidores, sizeof(CONSUMIDOR), 1, arquivo2);
                        }          
                    }
                    //excluindo contratos que pertencem a usina excluida
                    excluir_contrato_c_cnpj(cnpj);
                }
            }
            
            fclose(arquivo1);
            fclose(arquivo2);
            remove("consumidores_cadastrados.txt");
            rename("consumidores_cadastrados_temporariamente.txt", "consumidores_cadastrados.txt");
            loading();
            printf("\nCONSUMIDOR EXLUIDO!\n");
            getch();
        } else
        {
            cabecalho();
            printf("Não foi possivel localizar nenhum consumidor com esse ID no sistema para ser feita a exclusao!\n");
            getch();
        }
    } 
}