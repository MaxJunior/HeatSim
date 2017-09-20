/*
// Projeto SO - exercicio 1, version 03
// Sistemas Operativos, DEI/IST/ULisboa 2017-18
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "matrix2d.h"


/*--------------------------------------------------------------------
| Function: simul
---------------------------------------------------------------------*/

DoubleMatrix2D *simul(DoubleMatrix2D *matrix, DoubleMatrix2D *matrix_aux, int linhas, int colunas, int numIteracoes) {

  return matrix;
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
---------------------------------------------------------------------*/

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
| Function: integer_param_validation
---------------------------------------------------------------------*/

 void integer_param_validation(int arg, char param, int lowestValue){
   
   if(arg < lowestValue){

    switch(param){
      
        //  matrix dimension
      case('N'):
          fprintf(stderr, "\nErro no argumento N : dimensoes deve ser maior que %d.\n\n",(lowestValue - 1));
            exit(1);
             ;
       // iterations
      case('I'):
           
          fprintf(stderr, "\nErro no argumento iteracoes : iteracoes deve ser maior que %d.\n\n",(lowestValue - 1));
              exit(1);
          ;
      
    }

   }
 }

 /*--------------------------------------------------------------------
| Function: double_param_validation
---------------------------------------------------------------------*/
 void double_param_validation(double arg,double lowestValue){
  if(arg < lowestValue){
     fprintf(stderr, "\nErro na(s) temperatura(s) N : os valores de tEsq,tSup,tDir,tInf deve ser maior que %d.\n\n",(lowestValue - 1));
                exit(1);
  }
 }

/*--------------------------------------------------------------------
| Function: main
---------------------------------------------------------------------*/

int main (int argc, char** argv) {

  if(argc != 7) {
    fprintf(stderr, "\nNumero invalido de argumentos.\n");
    fprintf(stderr, "Uso: heatSim N tEsq tSup tDir tInf iteracoes\n\n");
    return 1;
  }

  /* argv[0] = program name */
  int N = parse_integer_or_exit(argv[1], "N");
  double tEsq = parse_double_or_exit(argv[2], "tEsq");
  double tSup = parse_double_or_exit(argv[3], "tSup");
  double tDir = parse_double_or_exit(argv[4], "tDir");
  double tInf = parse_double_or_exit(argv[5], "tInf");
  int iteracoes = parse_integer_or_exit(argv[6], "iteracoes");

  DoubleMatrix2D *matrix, *matrix_aux, *result;


  fprintf(stderr, "\nArgumentos:\n"
	" N=%d tEsq=%.1f tSup=%.1f tDir=%.1f tInf=%.1f iteracoes=%d\n",
	N, tEsq, tSup, tDir, tInf, iteracoes);


  matrix = dm2dNew(N+2, N+2);
  matrix_aux = dm2dNew(N+2, N+2);


  /* FAZER ALTERACOES AQUI */
     integer_param_validation(N,'N', 1);
   double_param_validation(tEsq, 0.0);
   double_param_validation(tSup, 0.0);
   double_param_validation(tDir, 0.0);
   double_param_validation(tInf, 0.0);
   integer_param_validation(iteracoes,'I', 1);


//----------------------------------
  dm2dSetLineTo (matrix, 0, tSup);
  dm2dSetLineTo (matrix, N+1, tInf);
  dm2dSetColumnTo (matrix, 0, tEsq);
  dm2dSetColumnTo (matrix, N+1, tDir);

  dm2dCopy (matrix_aux, matrix);

  result = simul(matrix, matrix_aux, N+2, N+2, iteracoes);

  dm2dPrint(result);

  dm2dFree(matrix);
  dm2dFree(matrix_aux);

  return 0;
}
