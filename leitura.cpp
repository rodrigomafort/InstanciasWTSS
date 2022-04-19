#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <climits>
#include <condition_variable>
#include <fstream> 
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <mutex>
#include <queue>
#include <random>
#include <set>
#include <signal.h>
#include <sstream>
#include <sstream>
#include <sstream>
#include <stdexcept>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <utility>
#include <vector>

#include "BibGrafos.h"

using namespace std;
using namespace BibGrafos;

Grafo* LerGrafoMPI(string arq)
{
	int n;
	int m;

	FILE *file;

	cout << "ABRINDO ARQUIVO" << endl;
	file = fopen(arq.c_str(), "rb");
	
	if (file != NULL)
	{	
		cout << "OBTENDO TAMANHO" << endl;

		ParAdj conf;
		fread(&conf, sizeof(ParAdj), 1, file);

		n = conf.v1;
		m = conf.v2;

		cout << "N:" << n << endl;
		cout << "M:" << m << endl;

		cout << "ALOCANDO MEMORIA" << endl;
		ParAdj *vertices = (ParAdj*) malloc(n * sizeof(ParAdj));
		ParAdj *adjs = (ParAdj*) malloc(m * sizeof(ParAdj));

		cout << "LENDO ARQUIVO PARA VETOR" << endl;
		for (int i = 0; i < n; i++)
		{
			fread(&vertices[i], sizeof(ParAdj), 1, file);
		}
		for (int i = 0; i < m; ++i)
		{
			fread(&adjs[i], sizeof(ParAdj), 1, file);
		}
		cout << "LEITURA FINALIZADA. FECHANDO ARQUIVO" << endl;
		fclose(file);
		
		cout << "RECONSTRUIR GRAFO" << endl;
		Grafo *G = new Grafo(n, m, vertices, adjs);

		free(vertices);
		free(adjs);
		return G;
	}
	else
	{
		cout << "ERRO NO ARQUIVO" << endl;
		return NULL;
	}
}

int main()
{
	Grafo* G = LerGrafoMPI("../InstanciasTSS/BlogCatalog/BlogCatalog.bin");
	if (G != NULL)
		cout << G->N();
	return 0;

}