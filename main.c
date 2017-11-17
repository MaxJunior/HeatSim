/*
// Projeto SO - exercicio 2
// Sistemas Operativos, DEI/IST/ULisboa 2017-18

Grupo 61

Aluno : Luís Pedro Olivera Ferreira, num:83500
Aluno : Maxwell SMart Ntido Junior , num:79457
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <sys/unistd.h>

#include "matrix2d.h"

pthread_mutex_t   espera_mutex;
pthread_mutex_t   saiu_mutex;
pthread_mutex_t   max_mutex;
pthread_cond_t    espera_por_todos;
pthread_cond_t    wait_to_iterate;

double max=0; //guarda o valor de diferenca maximo dessa iteracao(todas as tarefas) 
int tem_esperar=0; //numero de threads que estao na barreira
int saiu=0; //numero de threads que sairam da barreira
DoubleMatrix2D *matrix,*matrix_aux,*tmp;

typedef struct{
    double maxD;
    int n_iter;
    int first_line;
    int last_line;
    int colunas;
    int n_tarefas;
    char *filename;
    int periodoS;
    }info_t;
    
/*--------------------------------------------------------------------
| Function: simul
---------------------------------------------------------------------*/

void *simul(void *information) {

    //inicializacao das variaveis
    info_t *slave_info=(info_t *)information;
    
    const int first_line=slave_info->first_line;
    const int last_line=slave_info->last_line;
    const int iter=slave_info->n_iter;
    const int col=slave_info->colunas;
    const int tarefas=slave_info->n_tarefas;
    const double maxD=slave_info->maxD;
    double diference;
    int k,i,j;
    double value;
    
   	//atualiza matrix
   	for (k=0;k<iter;k++){
        
        if (first_line==1){
            int pid;
            printf("checkpoint_simulA\n");
            const int periodoS=slave_info->periodoS;
            const char *filename=slave_info->filename;
            printf("checkpoint_simulA1\n");

            if (periodoS!=0){
                if ((k-1)%periodoS==0){
                    printf("checkpoint_simulB\n");

                    pid=fork();
                    if (pid==0){
                        printf("saving iter:%d\n",k-1);

                        FILE *filepointer=fopen(filename,"w");
                        dm2dPrint(matrix,filepointer);
                        fclose(filepointer);
                        exit(0);
                    }
                    else if (pid<0){
                        fprintf(stderr, "\nErro ao criar filho\n");
                    }                        
                }
            }
            printf("checkpoint_simulC\n");

        }
            
                   
        
        //nao comeca iteracao seguinte enquanto todas as threads nao sairem da barreira
        if(pthread_mutex_lock(&saiu_mutex) != 0) {
            fprintf(stderr, "\nErro ao bloquear mutex\n");
            exit(-1);
        }       
        
        while(saiu!=0){
            if(pthread_cond_wait(&wait_to_iterate,&saiu_mutex) != 0) {
                fprintf(stderr, "\nErro ao esperar pela variável de condição\n");
                exit(-1);
            }
        }
        if(pthread_mutex_unlock(&saiu_mutex) != 0) {
            fprintf(stderr, "\nErro ao desbloquear mutex\n");
            exit(-1);
        }
        
        for (i = first_line; i <= last_line; i++){
            for (j = 1; j < col+1; j++) {
                value = (dm2dGetEntry(matrix, i-1, j) + dm2dGetEntry(matrix, i+1, j) + dm2dGetEntry(matrix, i, j-1) + dm2dGetEntry(matrix, i, j+1) ) / 4.0;
                dm2dSetEntry(matrix_aux, i, j, value);
                diference=value-dm2dGetEntry(matrix,i,j);
                               
                if(pthread_mutex_lock(&max_mutex) != 0) {
                    fprintf(stderr, "\nErro ao bloquear mutex\n");
                    exit(-1);
                }
                if (diference>max){
                    max=diference;
                }
                if(pthread_mutex_unlock(&max_mutex) != 0) {
                    fprintf(stderr, "\nErro ao desbloquear mutex\n");
                    exit(-1);
                }
            }
        }

        if(pthread_mutex_lock(&espera_mutex) != 0) {
            fprintf(stderr, "\nErro ao bloquear mutex\n");
            exit(-1);
        }      
       
        tem_esperar++;
        
        //troca matrizes apenas uma vez
        if (tem_esperar==1){            
            tmp = matrix_aux;
            matrix_aux = matrix;
            matrix = tmp;
        }
        
        
        //barreira
        while (tem_esperar!=tarefas){
            if(pthread_cond_wait(&espera_por_todos,&espera_mutex) != 0) {
                fprintf(stderr, "\nErro ao esperar pela variável de condição\n");
                exit(-1);
            }
        }
        
        if(pthread_cond_broadcast(&espera_por_todos) != 0) {
            fprintf(stderr, "\nErro ao desbloquear variável de condição\n");
            exit(-1);
        }

        if(pthread_mutex_unlock(&espera_mutex) != 0) {           
            fprintf(stderr, "\nErro ao desbloquear mutex\n");
            exit(-1);
        }
        
        if (max<maxD){
            return 0;
        }      
        
        if(pthread_mutex_lock(&saiu_mutex) != 0) {
            fprintf(stderr, "\nErro ao bloquear mutex\n");
            exit(-1);
        }     
        
        saiu++;
        //reinicializa variaveis
        if (saiu==tarefas){
            tem_esperar=0;
            max=0;      
            saiu=0;
            if(pthread_cond_broadcast(&wait_to_iterate) != 0) {
                fprintf(stderr, "\nErro ao desbloquear variável de condição\n");
                exit(-1);
            }
        }
        
        if(pthread_mutex_unlock(&saiu_mutex) != 0) {           
            fprintf(stderr, "\nErro ao desbloquear mutex\n");
            exit(-1);
        }
        
        
    }
    return 0;
}
  

/*--------------------------------------------------------------------
| Function: parse_integer_or_exit
---------------------------------------------------------------------*/

int parse_integer_or_exit(char const *str, char const *name)
{
  int value;
 
  if(sscanf(str, "%d", &value) != 1) {
    fprintf(stderr, "\nErro no argumento \"%s\".\n\n", name);
    exit(1);
  }
  return value;
}

/*--------------------------------------------------------------------
| Function: parse_double_or_exit
--------------------------------- i------------------------------------*/

double parse_double_or_exit(char const *str, char const *name)
{
  double value;

  if(sscanf(str, "%lf", &value) != 1) {
    fprintf(stderr, "\nErro no argumento \"%s\".\n\n", name);
    exit(1);
  }
  return value;
}


/*--------------------------------------------------------------------
| Function: main
---------------------------------------------------------------------*/

int main (int argc, char** argv) {

    if(argc != 11) {
        fprintf(stderr, "\nNumero invalido de argumentos.\n");
        fprintf(stderr, "Uso: heatSim N tEsq tSup tDir tInf iteracoes trab maxD fichS periodoS\n\n");
        return 1;
    }

    /* argv[0] = program name */
    int N = parse_integer_or_exit(argv[1], "N");
    double tEsq = parse_double_or_exit(argv[2], "tEsq");
    double tSup = parse_double_or_exit(argv[3], "tSup");
    double tDir = parse_double_or_exit(argv[4], "tDir");
    double tInf = parse_double_or_exit(argv[5], "tInf");
    int iteracoes = parse_integer_or_exit(argv[6], "iteracoes");
    int trab = parse_integer_or_exit(argv[7], "trab");
    double maxD = parse_double_or_exit(argv[8], "maxD");
    char* fichS = argv[9];
    int periodoS = parse_integer_or_exit(argv[10], "periodoS");

    fprintf(stderr, "\nArgumentos:\n"
	" N=%d tEsq=%.1f tSup=%.1f tDir=%.1f tInf=%.1f iteracoes=%d trab=%d maxD=%.1f fichS=%s periodoS=%d\n",
	N, tEsq, tSup, tDir, tInf, iteracoes, trab, maxD, fichS, periodoS);

    if(N < 1 || tEsq < 0 || tSup < 0 || tDir < 0 || tInf < 0 || iteracoes < 1 || N%trab!=0 || maxD<0 || trab<0 ||periodoS<0) {
        fprintf(stderr, "\nErro: Argumentos invalidos.\n"
        " Lembrar que N >= 1, temperaturas >= 0, iteracoes >= 1, N is divisble by trab, trab>0, maxD>=0 e periodoS>0\n\n");
    return 1;
    }
    
    printf("checkpointA\n");
    FILE *ficheiro;
    ficheiro=fopen(fichS,"r");
    printf("checkpointA1\n");

    if (ficheiro!=NULL){
        matrix=readMatrix2dFromFile(ficheiro, N, N);
        fclose(ficheiro);

    }    
    printf("checkpointB\n");

    
    if(matrix==NULL){
        //inicializacao matrix
        matrix=dm2dNew(N+2,N+2);
        matrix_aux=dm2dNew(N+2,N+2);    
        dm2dSetLineTo(matrix,0,tSup);
        dm2dSetLineTo(matrix_aux,0,tSup);    
        dm2dSetLineTo(matrix,N+1,tInf);
        dm2dSetLineTo(matrix_aux,N+1,tInf);
        dm2dSetColumnTo (matrix, 0, tEsq);
        dm2dSetColumnTo (matrix_aux, 0, tEsq);
        dm2dSetColumnTo (matrix, N+1, tDir);
        dm2dSetColumnTo (matrix_aux, N+1, tDir);
    }
    
    printf("checkpointC\n");
    dm2dPrint(matrix,stdout);

    
    if(pthread_mutex_init(&espera_mutex, NULL) != 0) {
        fprintf(stderr, "\nErro ao inicializar mutex\n");
        return -1;
    }
    
    if(pthread_mutex_init(&saiu_mutex, NULL) != 0) {
        fprintf(stderr, "\nErro ao inicializar mutex\n");
        return -1;
    }
    
    if(pthread_mutex_init(&max_mutex, NULL) != 0) {
        fprintf(stderr, "\nErro ao inicializar mutex\n");
        return -1;
    }
  
    if(pthread_cond_init(&espera_por_todos, NULL) != 0) {
        fprintf(stderr, "\nErro ao inicializar variável de condição\n");
        return -1;
    }
    if(pthread_cond_init(&wait_to_iterate, NULL) != 0) {
        fprintf(stderr, "\nErro ao inicializar variável de condição\n");
        return -1;
    }
    
    info_t *info;
    info=(info_t*)malloc(trab*sizeof(info_t)); //array de estruturas com a informacao a passar a cada thread

    pthread_t *threads;
    threads=(pthread_t*)malloc(trab*sizeof(pthread_t)); //array com id de cada thread
    
    if (info==NULL||threads==NULL){
        fprintf(stderr, "\nErro ao alocar memoria\n");
    }
    int i;
    
    printf("checkpointD\n");

        
    for (i=0;i<trab;i++){
        //atualiza info para cada thread
        info[i].maxD=maxD;
        info[i].n_iter=iteracoes;
        info[i].first_line=i*N/trab+1;
        info[i].last_line=(i+1)*N/trab;        
        info[i].colunas=N;
        info[i].n_tarefas=trab;
        info[i].filename=fichS;
        info[i].periodoS=periodoS;
        if (pthread_create(&threads[i],NULL,simul, &info[i])!=0){
            fprintf(stderr, "\nErro ao criar um escravo\n");
        }
    }
    printf("checkpointE\n");

    //Saida
    for (i=0;i<trab;i++){
        if (pthread_join(threads[i],NULL)){
            fprintf(stderr, "\nErro ao esperar por um escravo\n");
            return -1;
        }
    }
    printf("checkpointF\n");

    dm2dPrint(matrix,stdout);
    printf("checkpointG\n");

    unlink(fichS);
    printf("checkpointH\n");
   
   if(pthread_mutex_destroy(&espera_mutex) != 0) {
        fprintf(stderr, "\nErro ao destruir mutex\n");
        exit(EXIT_FAILURE);
    }
    
     if(pthread_mutex_destroy(&saiu_mutex) != 0) {
        fprintf(stderr, "\nErro ao destruir mutex\n");
        exit(EXIT_FAILURE);
    }
    
    if(pthread_mutex_destroy(&max_mutex) != 0) {
        fprintf(stderr, "\nErro ao destruir mutex\n");
        exit(EXIT_FAILURE);
    }
  
    if(pthread_cond_destroy(&espera_por_todos) != 0) {
        fprintf(stderr, "\nErro ao destruir variável de condição\n");
        exit(EXIT_FAILURE);
    }
    if(pthread_cond_destroy(&wait_to_iterate) != 0) {
        fprintf(stderr, "\nErro ao destruir variável de condição\n");
        exit(EXIT_FAILURE);
    }
  
   dm2dFree(matrix);
   dm2dFree(matrix_aux);
   free(threads);
   free(info);
  
  
  return 0;
}
