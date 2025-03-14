/***************INICIO: conteudo original de calculos.c (Duarte)***************/
#include <stdio.h>
#include <math.h>
#define M_PI 3.14159265358979323846

/* Função para calcular o coeficiente de sustentação (CL)*/
double calcular_CL(double alpha, double AR) {
    return (alpha * M_PI * AR) / (1 + sqrt(1 + pow(AR / 2.0, 2)));
}

/* Função para calcular o coeficiente de arrasto (CD) */
double calcular_CD(double CL, double CD0, double e, double AR) {
    return CD0 + (1 / (M_PI * e * AR)) * pow(CL, 2);
}

/* Função para calcular a força de sustentação (Lift) */
double calcular_Lift(double CL, double rho, double V, double S) {
    return CL * 0.5 * rho * V * V * S;
}

/* Função para calcular a força de arrasto (Drag) */
double calcular_Drag(double CD, double rho, double V, double S) {
    return CD * 0.5 * rho * V * V * S;
}

/*Falta definir AR = (b*b)/S ja defini na linha 212*/
/***************FIM: conteudo original de calculos.c ***************/


/***************INICIO: conteudo original de ler_ficheiro.c (Duarte)/(Manuel)***************/
#include <stdio.h>

/* Funcao para ler as variaveis*/
int ler_variaveis(double *tf, double *dt,
    double *S, double *b, double *m,
    double *g, double *ro, double *cd0,
    double *e, double *alpha, double *v0,
    double *gamma0, double *x0, double *h0)
{
    char linha[100];  
    int parametros_lidos = 0;
    FILE *arquivo = fopen("config_modelo.txt", "r");
   
    if (arquivo == NULL) {
        printf("Erro: Arquivo nao encontrado!\n");
        return 0;
    }
   
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {

        /********************************************************
         * CODIGO ADICIONADO (sem remover nada do original)/Manuel:
         estava a haver um erro em relacao a leitura dos espacos brancos
         * Ignorar linhas que tenham apenas espacos/tabs.
         Desculpa la ter mexido sem dizer nada mas eram 03:42 :-)
         o \r e o carriage return btw
         ********************************************************/
        {
            int so_espacos = 1;
            int i;
            for (i = 0; linha[i] != '\0'; i++) {
                if (linha[i] != ' ' && linha[i] != '\t' &&
                    linha[i] != '\n' && linha[i] != '\r') {
                    so_espacos = 0;
                    break;
                }
            }
            if (so_espacos) {
                /* Se for so espacos, ignora esta linha e vai pra proxima */
                continue;
            }
        }
        /*************** FIM do bocado ADICIONADO ****************/

        if (linha[0] == '%' || linha[0] == '\n') {
            continue; /* Ignorar comentarios e linhas vazias */
        }
        /*Acho que o Bertinho tinha limitado o numero de linhas n tenho a certeza*/
        switch (parametros_lidos) {
            case 0: sscanf(linha, "%lf", tf); break;
            case 1: sscanf(linha, "%lf", dt); break;
            case 2: sscanf(linha, "%lf", S); break;
            case 3: sscanf(linha, "%lf", b); break;
            case 4: sscanf(linha, "%lf", m); break;
            case 5: sscanf(linha, "%lf", g); break;
            case 6: sscanf(linha, "%lf", ro); break;
            case 7: sscanf(linha, "%lf", cd0); break;
            case 8: sscanf(linha, "%lf", e); break;
            case 9: sscanf(linha, "%lf", alpha); break;
            case 10: sscanf(linha, "%lf", v0); break;
            case 11: sscanf(linha, "%lf", gamma0); break;
            case 12: sscanf(linha, "%lf", x0); break;
            case 13: sscanf(linha, "%lf", h0); break;
            default: break;
        }
       
        parametros_lidos++;
    }
   
    fclose(arquivo);
   
    if (parametros_lidos == 14) {
        return 1; /* Todos os parametros foram lidos corretamente */
    } else {
        return 0; /* Faltaram parametros */
    }
}
/***************FIM: conteudo original de ler_ficheiro.c ***************/


/***************INICIO: conteudo original de menu_selecao.c/ aconselho a tirar todos os acentos ou caracteres especiais  ***************/
#include <stdio.h>

void simular_movimento();
void calcular_min_max();

void menu_de_selecao() {
    int opcao;

    do {
        printf("\nLista de opcoes\n");
        printf("0 - Terminar o programa\n");
        printf("1 - Simular o movimento da aeronave\n");
        printf("2 - Determinar os valores minimos e maximos\n");
        printf("Escolha uma opcao: ");
       
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada inválida! Tente novamente.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (opcao) {
            case 0:
                printf("A terminar o programa...\n");
                break;
            case 1:
                printf("A iniciar simulação...\n");
                simular_movimento();
                break;
            case 2:
                printf("A calcular valores mínimos e máximos...\n");
                calcular_min_max();
                break;
            default:
                printf("Opção inválida! Tente novamente.\n");
        }
    } while (opcao != 0);
}
/***************FIM: conteudo original de menu_selecao.c ***************/


/* --------------------------------------------------------------------
   A PARTIR DESTE PONTO:
   - Implementacoes de simular_movimento() e calcular_min_max()
   - Funcao main()
   -Herobrine
   -Pfv verifica se n me enganei em algo 
   -Adicionar comandos para grafs no octave (15/03/2025)
   ---------------------------------------------------------------------*/

#include <math.h> /* repetido, sem problema */

/* Precisamos das funcoes declaradas nos ficheiros originais: */
int ler_variaveis(double *tf, double *dt,
    double *S, double *b, double *m,
    double *g, double *ro, double *cd0,
    double *e, double *alpha, double *v0,
    double *gamma0, double *x0, double *h0);

double calcular_CL(double alpha, double AR);
double calcular_CD(double CL, double CD0, double e, double AR);
double calcular_Lift(double CL, double rho, double V, double S);
double calcular_Drag(double CD, double rho, double V, double S);

/* --------------------------------------------------------------------
   Calculos + link com leitura de ficheiros
   ---------------------------------------------------------------------*/
void simular_movimento() {
    double tf, dt, S, b, m, g, rho, cd0, e, alpha;
    double V, gamma, x, h;
    int ok;
    FILE *f;
    double AR, CL, CD;
    double t;
    double lift, drag;
    double dV, dGamma, dx, dh;

    ok = ler_variaveis(&tf, &dt, &S, &b, &m, &g, &rho, &cd0, &e, &alpha, &V, &gamma, &x, &h);
    if (!ok) {
        /* Se return 0, a leitura de config_modelo.txt falhou */
        return;
    }

    /* visualizacao no ecran: */
    printf("\n[Simular] Parametros lidos:\n");
    printf(" tf=%f\n dt=%e\n S=%g\n b=%g\n m=%g\n g=%g\n rho=%g\n cd0=%g\n e=%g\n alpha=%g\n",
           tf, dt, S, b, m, g, rho, cd0, e, alpha);
    printf(" V0=%g, gamma0=%g, x0=%g, h0=%g\n\n", V, gamma, x, h);

    f = fopen("voo_sim.txt", "w");
    if(!f) {
        printf("Erro ao criar voo_sim.txt!\n");
        return;
    }

    /* Cabecalho no ficheiro de saida */
    fprintf(f, "# tf=%g dt=%g S=%g b=%g m=%g g=%g rho=%g cd0=%g e=%g alpha=%g\n",
            tf, dt, S, b, m, g, rho, cd0, e, alpha);
    fprintf(f, "# t  V  gamma  x  h\n");

    AR = (b*b)/S;            
    CL = calcular_CL(alpha, AR);
    CD = calcular_CD(CL, cd0, e, AR);

    t = 0.0;
    /* Escreve estado inicial */
    fprintf(f, "%.6f %.6f %.6f %.6f %.6f\n", t, V, gamma, x, h);

    while(t < tf && h > 0.0) {
        lift = calcular_Lift(CL, rho, V, S);
        drag = calcular_Drag(CD, rho, V, S);

        dV     = (1.0/m)*(-drag - m*g*sin(gamma));
        dGamma = (1.0/(m*V))*(lift - m*g*cos(gamma));
        dx     = V*cos(gamma);
        dh     = V*sin(gamma);

        V     += dV * dt;
        gamma += dGamma * dt;
        x     += dx * dt;
        h     += dh * dt;

        t     += dt;

        fprintf(f, "%.6f %.6f %.6f %.6f %.6f\n", t, V, gamma, x, h);
    }

    fclose(f);
    printf("Simulacao concluida. Resultados guardados em voo_sim.txt\n");
}

/* --------------------------------------------------------------------
   calcular_min_max()
   ---------------------------------------------------------------------*/
void calcular_min_max() {
    FILE *f;
    double t, V, gamma, x, h;
    double t_min, t_max;
    double V_min, V_max;
    double g_min, g_max;
    double x_min, x_max;
    double h_min, h_max;
    int primeiro;
    char linha[256];
    /*if(!f) igual a if (f == NULL)*/
    f = fopen("voo_sim.txt","r");
    if(!f) {
        printf("Erro: nao foi possivel abrir voo_sim.txt.\n");
        return;
    }

    primeiro = 1;
    while(fgets(linha, sizeof(linha), f)) {
        if(linha[0] == '#') {
            continue;
        }
        if(sscanf(linha, "%lf %lf %lf %lf %lf", &t, &V, &gamma, &x, &h) == 5) {
            if(primeiro) {
                t_min = t_max = t;
                V_min = V_max = V;
                g_min = g_max = gamma;
                x_min = x_max = x;
                h_min = h_max = h;
                primeiro = 0;
            } else {
                if(t < t_min) t_min = t;
                if(t > t_max) t_max = t;
                if(V < V_min) V_min = V;
                if(V > V_max) V_max = V;
                if(gamma < g_min) g_min = gamma;
                if(gamma > g_max) g_max = gamma;
                if(x < x_min) x_min = x;
                if(x > x_max) x_max = x;
                if(h < h_min) h_min = h;
                if(h > h_max) h_max = h;
            }
        }
    }

    fclose(f);

    if(primeiro) {
        printf("Nao foram encontrados dados validos em voo_sim.txt!\n");
        return;
    }

    printf("\n--- VALORES MINIMOS E MAXIMOS ---\n");
    printf("t_min = %g,  t_max = %g\n", t_min, t_max);
    printf("V_min = %g,  V_max = %g\n", V_min, V_max);
    printf("gamma_min = %g,  gamma_max = %g\n", g_min, g_max);
    printf("x_min = %g,  x_max = %g\n", x_min, x_max);
    printf("h_min = %g,  h_max = %g\n", h_min, h_max);
}


int main() {
    menu_de_selecao();
    return 0;
}
