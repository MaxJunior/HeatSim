/*
// Projeto SO - exercicio 1, version 03
// Sistemas Operativos, DEI/IST/ULisboa 2017-18
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#include "matrix2d.h"
#include "mplib3.h"



typedef struct{

    int n_iter;
    int t_id;
    int colunas;
    int n_tarefas;
    int dir:
    int esq;
    int baixo;
    int cima;
    }info_t;
    
    
    
/*--------------------------------------------------------------------
| Function: simul
---------------------------------------------------------------------*/

void simul(info_t *information) {

    //inicializacao
    DoubleMatrix2D *matrix, *matrix_aux, *tmp;
    int i, j, k, id, iter, col, linhas, up, down, left, right, tarefas;
    double value;
    id=information.t_id;
    iter=information.n_iter;
    col=information.colunas;
    tarefas=information.n_tarefas;
    linhas=col/tarefas;
    up=information.cima;
    down=information.baixo;
    left=information.esq;
    right=information.dir;
    double recebe_linha[col];
    matrix=dm2dNew(linhas+2,col+2);
    matrix_aux=dm2dnew(linhas+2,col+2);
    
    //poe temperatura das bordas na matrix
    dm2dSetColumnTo (*matrix, 0, left);
    dm2dSetColumnTo (*matrix, col+2, right);
    
    if (id==0){
        dm2dSetLineTo (*matrix, 0, up);
    }
    if (id==tarefas-1){
        dm2dSetLineTo (*matrix, linhas+2, down);
    }
	
   	//atualiza matrix
   	for (k=0;k<iter;k++){ 
        for (i = 1; i < linhas; i++){
            for (j = 1; j < col; j++) {
                value = (dm2dGetEntry(m, i-1, j) + dm2dGetEntry(m, i+1, j) + dm2dGetEntry(m, i, j-1) + dm2dGetEntry(m, i, j+1) ) / 4.0;
                dm2dSetEntry(aux, i, j, value);
            }
        }
		
		// se id par, recebe e depois envia mensagem
        if (id%2==0){
            if (id!=0){ // se nao a primeira thread, troca com a anterior
                receberMensagem(threads[id-1],threads[id],*recebe_linha, sizeof(recebe_linha)); 
                dm2dsetLine(*matrix_aux,0,*recebe_linha);
                enviarMensagem(threads[id],threads[id-1],*matrix_aux[col+2], sizeof(recebe_linha));
            }
            if (id!=tarefas-1){ // se nao a ultima thread, troca com a seguinte
                receberMensagem(threads[id+1],threads[id],*recebe_linha, sizeof(recebe_linha)); 
                dm2dsetLine(*matrix_aux,linhas+2,*recebe_linha);
                enviarMensagem(threads[id],threads[id+1],*matrix_aux[(linhas+1)*col], sizeof(recebe_linha));
            }
        }
            
        //se id impar, envia e depois recebe mensagem
        if (id%2==1){         
            //se impar, nunca e primeira
            enviarMensagem(threads[id],threads[id-1],*matrix_aux[col+2], sizeof(recebe_linha));
            receberMensagem(threads[id-1],threads[id],*recebe_linha, sizeof(recebe_linha)); 
            dm2dsetLine(*matrix_aux,0,*recebe_linha);
            
            if (id!=tarefas-1){ //se nao ultima thread, troca com a segyunte
                enviarMensagem(threads[id],threads[id+1],*matrix_aux[(linhas+1)*col], sizeof(recebe_linha));
                receberMensagem(threads[id+1],threads[id],*recebe_linha,sizeof(recebe_linha)); 
                dm2dsetLine(*matrix_aux,linhas+2,*recebe_linha);
                
            }
        }
		
        //troca aux com matrix
        tmp = aux;
        aux = matrix;
        matrix = tmp;
    }
    //envia para a main
    enviarMensagem(threads[id],main,*matrix,sizeof(matrix));
  
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

    if(argc != 9) {
        fprintf(stderr, "\nNumero invalido de argumentos.\n");
        fprintf(stderr, "Uso: heatSim N tEsq tSup tDir tInf iteracoes\n\n");
        return 1;
    }

    /* argv[0] = program name */
    int N = parse_integer_or_exit i(argv[1], "N");
    double tEsq = parse_double_or_exit(argv[2], "tEsq");
    double tSup = parse_double_or_exit(argv[3], "tSup");
    double tDir = parse_double_or_exit(argv[4], "tDir");
    double tInf = parse_double_or_exit(argv[5], "tInf");
    int iteracoes = parse_integer_or_exit(argv[6], "iteracoes");
    int trab = parse_integer_or_exit(argv[7], "trab");
    int csz = parse_integer_or_exit(argv[8], "csz");

    fprintf(stderr, "\nArgumentos:\n"
	" N=%d tEsq=%.1f tSup=%.1f tDir=%.1f tInf=%.1f iteracoes=%d\n",
	N, tEsq, tSup, tDir, tInf, iteracoes);

    if(N < 1 || tEsq < 0 || tSup < 0 || tDir < 0 || tInf < 0 || iteracoes < 1 || N%trab!=0 || csz<0 || trab<0) {
        fprintf(stderr, "\nErro: Argumentos invalidos.\n"
        " Lembrar que N >= 1, temperaturas >= 0 e iteracoes >= 1\n\n");
    return 1;
    }

    int i,k;
    DoubleMatrix2D *matrix_final;
    info_t info[trab]; //array com os ids das threads
    pthread_t threads[trab]; //array com estrutura para passar a cada thread
    
    inicializarMPlib(csz, trab+1);
    for (i=0;i<trab;i++){
        //atualiza info
        info[i].t_id=i;
        info[i].n_iter=iteracoes;
        info[i].colunas=N;
        info[i].n_tarefas=trab;
        info[i].cima=tSup;
        info[i].baixo=tInf;
        info[i].esq=tEsq;
        info[i].dir=tDir;
    
        pthread_create(&threads[i],NULL,simul,*info[i]);
    }
    
    //Saida
    matrix_final=dm2dNew(N,N);
    dm2dSetLineTo(matrix_final,0,tSup);
    dm2dSetLineTo(matrix_final,N+2,tInf);
    double recebe_fatia[(N+2)*(N/trab+2)];
    
    for (i=0;i<trab;i++){
        receberMensagem(threads[i],main,*recebe_fatia,sizeof(recebe_fatia));
        for (k=1;k<(N/trab+1);k++){
            dm2dsetLine(matrix_final,k*i,recebe_fatia[k*N]);
        }
        pthread_join(threads[i],NULL);
    }  
        
  dm2dPrint(matrix_final);
  
  libertarMPlib();
  dm2dFree(matrix_final);
  
  return 0;
}
