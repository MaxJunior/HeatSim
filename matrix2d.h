/*
// Biblioteca de matrizes 2D alocadas dinamicamente, versao 4
// Sistemas Operativos, DEI/IST/ULisboa 2017-18
*/

#ifndef MATRIX_2D_H
#define MATRIX_2D_H

#include <stdio.h>

typedef struct int_matrix_2d {
  int     n_l;
  int     n_c;
  double *data;
} DoubleMatrix2D;

DoubleMatrix2D* dm2dNew(int lines, int columns);
void            dm2dFree (DoubleMatrix2D *matrix);
void            dm2dSetLineTo (DoubleMatrix2D *matrix, int line, double value);
void            dm2dSetColumnTo (DoubleMatrix2D *matrix, int column, double value);
void            dm2dPrint (FILE *filepointer,DoubleMatrix2D *matrix);
void            dm2dInitiate(DoubleMatrix2D *matrix, int N, double tSup, double tInf, double tEsq, double tDir);
DoubleMatrix2D *readMatrix2dFromFile(FILE *f, int l, int c);

#define         dm2dGetEntry(m,l,c)    m->data[((l)*m->n_c)+(c)]
#define         dm2dSetEntry(m,l,c,v)  m->data[((l)*m->n_c)+(c)]=v

#endif
