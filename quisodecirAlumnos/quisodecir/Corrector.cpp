/*****************************************************************************************************************
	UNIVERSIDAD NACIONAL AUTONOMA DE MEXICO
	FACULTAD DE ESTUDIOS SUPERIORES -ARAGON-

	Computadoras y programacion.
	PON TU NOMBRE AQUÍ
	Quiso decir: Programa principal de la aplicacion de la distancia de Levenstein.

******************************************************************************************************************/


#include "stdafx.h"
#include <string.h>
#include "corrector.h"
#include <stdlib.h>
#include <ctype.h>
#define LONGITUD 32
char abecedario[LONGITUD + 1] = "abcdefghijklmnaopqrstuvwxyzaaaaa";

void setAscii() {
	abecedario[27] = 225; abecedario[28] = 233;
	abecedario[29] = 237; abecedario[30] = 243;
	abecedario[31] = 250; abecedario[14] = 241;
}

void sort2(char palabras[][TAMTOKEN], int izq, int der);
void sort1(char palabras[][TAMTOKEN], int izq, int der, int iPeso[]);


//Funciones publicas del proyecto
/*****************************************************************************************************************
	DICCIONARIO: Esta funcion crea el diccionario completo
	char *	szNombre				:	Nombre del archivo de donde se sacaran las palabras del diccionario
	char	szPalabras[][TAMTOKEN]	:	Arreglo con las palabras completas del diccionario
	int		iEstadisticas[]			:	Arreglo con el numero de veces que aparecen las palabras en el diccionario
	int &	iNumElementos			:	Numero de elementos en el diccionario
******************************************************************************************************************/
void Diccionario(char* szNombre, char szPalabras[][TAMTOKEN], int iEstadisticas[], int& iNumElementos)
{
	setAscii();
	FILE* fp = fopen(szNombre, "r");
	iNumElementos = 0;
	char lectura, array[TAMTOKEN];
	int count = 0;
	char limites[] = " \t\n\r.,;()";
	if (fp == NULL)
		return;
	while ((lectura = getc(fp)) != EOF) {
		bool flag = true;
		lectura = tolower(lectura);
		for (int i = 0; i < strlen(limites); i++)
			if (limites[i] == lectura)
				flag = false;
		if (count < TAMTOKEN && flag) {
			array[count++] = lectura;
			continue;
		}
		else if (count == 0)
			continue;
		array[count] = '\0';
		for (int i = 0; i < iNumElementos && !flag; i++) {
			if (strcmp(szPalabras[i], array) == 0) {
				iEstadisticas[i]++; //1
				flag = true;
			}
		}
		if (!flag) {
			strcpy_s(szPalabras[iNumElementos], array);
			iEstadisticas[iNumElementos++]++;
		}
		count = 0;
	}
	fclose(fp);
	sort1(szPalabras, 0, iNumElementos - 1, iEstadisticas);
}

/*****************************************************************************************************************
	ClonaPalabras: toma una palabra y obtiene todas las combinaciones y permutaciones requeridas por el metodo
	char *	szPalabraLeida,						// Palabra a clonar   palavra
	char	szPalabrasSugeridas[][TAMTOKEN], 	//Lista de palabras clonadas
	int &	iNumSugeridas)						//Numero de elementos en la lista
******************************************************************************************************************/
void	ClonaPalabras(
	char* szPalabraLeida,						// Palabra a clonar
	char	szPalabrasSugeridas[][TAMTOKEN], 	//Lista de palabras clonadas
	int& iNumSugeridas)						//Numero de elementos en la lista
{
	setAscii();
	char aux[TAMTOKEN]; iNumSugeridas = 0;
	strcpy_s(aux, szPalabraLeida);
	//pasa el abecedario en cada caracter
	for (int i = 0; i < strlen(szPalabraLeida); i++) {
		for (int j = 0; j < LONGITUD; j++) {
			aux[i] = abecedario[j];
			strcpy_s(szPalabrasSugeridas[iNumSugeridas++], aux);
		} aux[i] = szPalabraLeida[i];
	}
	//pasa el abecedario entre los caracteres
	int k;
	for (k = 1; k < strlen(szPalabraLeida) + 1; k++) {
		aux[k] = szPalabraLeida[k - 1];
	} aux[k] = '\0';
	for (int i = 0; i < strlen(szPalabraLeida) + 1; i++) {
		for (int j = 0; j < LONGITUD; j++) {
			aux[i] = abecedario[j];
			strcpy(szPalabrasSugeridas[iNumSugeridas++], aux);
		}
		aux[i] = szPalabraLeida[i];
	}
	//suprime los caracteres
	int contador = 0;
	for (int i = 0; i < strlen(szPalabraLeida) && strlen(szPalabraLeida) != 1; i++) {
		for (int j = 0; j < strlen(szPalabraLeida); j++) {
			if (j != i)
				aux[contador++] = szPalabraLeida[j];
		} aux[contador] = '\0';
		strcpy_s(szPalabrasSugeridas[iNumSugeridas++], aux);
		strcpy_s(aux, szPalabraLeida);
		contador = 0;
	}
	//Transposición de caracteres
	for (int i = 0; i < strlen(szPalabraLeida) - 1; i++) {
		aux[i] = szPalabraLeida[i + 1];
		aux[i + 1] = szPalabraLeida[i];
		strcpy_s(szPalabrasSugeridas[iNumSugeridas++], aux);
		strcpy_s(aux, szPalabraLeida);
	}
	strcpy_s(szPalabrasSugeridas[iNumSugeridas++], szPalabraLeida);
	sort2(szPalabrasSugeridas, 0, iNumSugeridas - 1);
}

/*****************************************************************************************************************
	ListaCandidatas: Esta funcion recupera desde el diccionario las palabras validas y su peso
	Regresa las palabras ordenadas por su peso
	char	szPalabrasSugeridas[][TAMTOKEN],	//Lista de palabras clonadas
	int		iNumSugeridas,						//Lista de palabras clonadas
	char	szPalabras[][TAMTOKEN],				//Lista de palabras del diccionario
	int		iEstadisticas[],					//Lista de las frecuencias de las palabras
	int		iNumElementos,						//Numero de elementos en el diccionario
	char	szListaFinal[][TAMTOKEN],			//Lista final de palabras a sugerir
	int		iPeso[],							//Peso de las palabras en la lista final
	int &	iNumLista)							//Numero de elementos en la szListaFinal
******************************************************************************************************************/
void	ListaCandidatas(
	char	szPalabrasSugeridas[][TAMTOKEN],	//Lista de palabras clonadas
	int		iNumSugeridas,						//Lista de palabras clonadas
	char	szPalabras[][TAMTOKEN],				//Lista de palabras del diccionario
	int		iEstadisticas[],					//Lista de las frecuencias de las palabras
	int		iNumElementos,						//Numero de elementos en el diccionario
	char	szListaFinal[][TAMTOKEN],			//Lista final de palabras a sugerir
	int		iPeso[],							//Peso de las palabras en la lista final
	int& iNumLista)							//Numero de elementos en la szListaFinal
{
	setAscii();
	iNumLista = 0;
	for (int i = 0; i < iNumSugeridas; i++) {
		for (int j = 0; j < iNumElementos; j++) {
			if (strcmp(szPalabrasSugeridas[i], szPalabras[j]) == 0) {
				bool flag = false;
				for (int k = 0; k < iNumLista && !flag; k++)
					if (strcmp(szListaFinal[k], szPalabras[j]) == 0)
						flag = true;
				if (flag) continue;
				strcpy_s(szListaFinal[iNumLista], szPalabrasSugeridas[i]);
				iPeso[iNumLista++] = iEstadisticas[j];
			}
		}
	}
	for (int i = 0; i < iNumLista; i++) {
		for (int j = 0; j < iNumLista - 1; j++) {
			if (iPeso[j] < iPeso[j + 1]) {
				int iaux; char caux[50];
				strcpy_s(caux, szListaFinal[j + 1]); iaux = iPeso[j + 1];
				strcpy_s(szListaFinal[j + 1], szListaFinal[j]); iPeso[j + 1] = iPeso[j];
				strcpy_s(szListaFinal[j], caux); iPeso[j] = iaux;
			}
		}
	}
}


void sort2(char palabras[][TAMTOKEN], int izq, int der) {
	char pivote[TAMTOKEN];
	strcpy_s(pivote, palabras[izq]);
	int iConta = izq;
	int jConta = der;
	char aux[TAMTOKEN];

	while (iConta < jConta) {
		while (strcmp(palabras[iConta], pivote) <= 0 && iConta < jConta)
			iConta++;
		while (strcmp(palabras[jConta], pivote) > 0)
			jConta--;
		if (iConta < jConta) {
			strcpy_s(aux, palabras[iConta]);
			strcpy_s(palabras[iConta], palabras[jConta]);
			strcpy_s(palabras[jConta], aux);
		}
	}
	strcpy_s(palabras[izq], palabras[jConta]);
	strcpy_s(palabras[jConta], pivote);
	if (izq < jConta - 1)
		sort2(palabras, izq, jConta - 1);
	if (jConta + 1 < der)
		sort2(palabras, jConta + 1, der);
}

void sort1(char palabras[][TAMTOKEN], int izq, int der, int iPeso[]) {
	char pivote[TAMTOKEN];
	strcpy_s(pivote, palabras[izq]);
	int iConta = izq;
	int jConta = der;
	char aux[TAMTOKEN];
	int iaux, ipivote = iPeso[izq];

	while (iConta < jConta) {
		while (strcmp(palabras[iConta], pivote) <= 0 && iConta < jConta)
			iConta++;
		while (strcmp(palabras[jConta], pivote) > 0)
			jConta--;
		if (iConta < jConta) {
			strcpy_s(aux, palabras[iConta]);
			strcpy_s(palabras[iConta], palabras[jConta]);
			strcpy_s(palabras[jConta], aux);
			iaux = iPeso[iConta];
			iPeso[iConta] = iPeso[jConta];
			iPeso[jConta] = iaux;
		}
	}
	strcpy_s(palabras[izq], palabras[jConta]);
	strcpy_s(palabras[jConta], pivote);
	iPeso[izq] = iPeso[jConta];
	iPeso[jConta] = ipivote;

	if (izq < jConta - 1)
		sort1(palabras, izq, jConta - 1, iPeso);
	if (jConta + 1 < der)
		sort1(palabras, jConta + 1, der, iPeso);
}