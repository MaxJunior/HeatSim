/*
// Projeto SO - exercicio 1, version 03
// Sistemas Operativos, DEI/IST/ULisboa 2017-18
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "matrix2d.h"

typedef struct{
    int n_iter;
    int thread_id;
    int colunas;
    int n_tarefas;
    int tDir:
    int tEsq;
    int tInf;
    int tSup;
    }info_t;
    
    
    
/*--------------------------------------------------------------------
| Function: simul
---------------------------------------------------------------------*/

void simul(info_t *info) {

    +DoubleMatrix2D *matrix, *matrix_aux, *tmp;
    int iter, i, j, k, id, col, linha, cima, baixo, esq, dir, tarefas;
    double value;
  
    iter=info->n_iter;
    id=info->thread_id;
    col=info->colunas;
    tarefas=info->n_tarefas;
    linhas=col/tarefas+2;
    cima=info->tSup;
    baixo=info->tInf;
    esq=info->tEsq;
    dir=info->tDir;
  
    matrix=dm2dNew(linhas+2,col+2);
    matrix_aux=dm2dnew(linhas+2,col+2);
    dm2dSetColumnTo (*matrix, 0, esq);
    dm2dSetColumnTo (*matrix, col+1, dir);
    if (id==0){
      //atencao:ver +2)
        dm2dSetLineTo (*matrix, 0, cima);
    }
    if (id==tarefas-1){
        dm2dSetLineTo (*matrix, linhas+1, baixo);
    }
	
   	for (k=0;k<iter;k++){ 
        for (i = 1; i < linhas; i++){
            for (j = 1; j < col; j++) {
                value = (dm2dGetEntry(m, i-1, j) + dm2dGetEntry(m, i+1, j) + dm2dGetEntry(m, i, j-1) + dm2dGetEntry(m, i, j+1) ) / 4.0;
                dm2dSetEntry(aux, i, j, value);
            }
        }
		/*
        if (id%2==0){
            if (id!=0){
                ptr=recebe id-1;
                setLine(*matrix_aux,0,*ptr);
                envia id-1;
            }
            if (id!=tarefas-1){
                ptr=recebe id+1;
                setLine(*matrix_aux,linhas+1,*ptr);
                envia id+1;
            }
        }
            
            
        if (id%2==1){
            envia id-1;
            ptr=recebe id-1;
            setLine(*matrix_aux,0,*ptr);
            
            if (id!=tarefas-1){
                envia id+1;
                ptr=recebe id+1;
                setLine(*matrix_aux,linhas+1,*ptr);
            }
        }
		
        tmp = aux;
        aux = m;
        m = tmp;
    }
    envia para main; */
  
    return 1;
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
  }d
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
    int csz = parse_double_or_exit(arg[8], "csz");

    DoubleMatrix2D *matrix;


    fprintf(stderr, "\nArgumentos:\n"
	" N=%d tEsq=%.1f tSup=%.1f tDir=%.1f tInf=%.1f iteracoes=%d\n",
	N, tEsq, tSup, tDir, tInf, iteracoes);

    if(N < 1 || tEsq < 0 || tSup < 0 || tDir < 0 || tInf < 0 || iteracoes < 1 || N%trab!=0 || csz<0 || trab<0) {
        fprintf(stderr, "\nErro: Argumentos invalidos.\n"
        " Lembrar que N >= 1, temperaturas >= 0 e iteracoes >= 1\n\n");
    return 1;
    }

    int i;
    /*
    for (i=0;i<trab;i++){
        cria thread (id,simul,info_t);
    }
    
    FINAL
    for (i=0;i<trab;i++){
        recebe ponteiro
        if i=0
            cria matrix? 
        else
            concatena? copia?
        pthreadjoin
    */
    
        
  dm2dPrint(matrix);

  dm2dFree(matrix);
  
  return 0;
}
