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

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string.hpp>

#include "BibGrafos.h"

using namespace std;
using namespace BibGrafos;

void SalvarGrafoEmArquivo(string arquivoSaida, Grafo *G)
{
	cout << "SALVANDO GRAFO EM ARQUIVO: " << arquivoSaida << endl;	
	cout << "ABRINDO ARQUIVO" << endl;
	FILE* file = fopen(arquivoSaida.c_str(), "wb");

	ParAdj conf;
	conf.v1 = G->N();
	conf.v2 = G->M();

	cout << "ADQUIRINDO DADOS" << endl;
	ParAdj* vertices = (ParAdj*) malloc (G->N() * sizeof(ParAdj));
	ParAdj* arestas = (ParAdj*) malloc (G->M() * sizeof(ParAdj));
	G->ObterListaVertices(vertices);
	G->ObterListaArestas(arestas);

	cout << "SALVANDO CONFIGURACOES" << endl;
	fwrite(&conf,sizeof(ParAdj), 1, file);
	cout << "SALVANDO VERTICES" << endl;
	fwrite(vertices,sizeof(ParAdj), G->N(), file);
	cout << "SALVANDO ARESTAS" << endl;
	fwrite(arestas,sizeof(ParAdj), G->M(), file);

	cout << "FECHANDO ARQUIVO" << endl;
	fclose(file);

	cout << "GRAFO SALVO." << endl;
}

void ProcessarGrafo(string arquivoVertices, string arquivoArestas, string arquivoSaida)
{
	cout << "PROCESSAR ARQUIVO: " << endl;
	cout << "ARQUIVO DE ARESTAS: " << arquivoArestas << endl;
	vector<int> rotulos;
	vector<pair<int, int>> arestas;

	std::ifstream fin;
	fin.open(arquivoArestas);

	bool verticesProcessados = false;

	if (arquivoVertices.empty() == false)
	{
		cout << "ARQUIVO DE VERTICES: " << arquivoVertices << endl;
		std::ifstream streamVertices;
		streamVertices.open(arquivoVertices);
		if (streamVertices.is_open())
		{
			while (!streamVertices.eof())
			{
				string srotulo;
				getline(streamVertices, srotulo);
				if (srotulo.find("#") == string::npos && srotulo != "" && srotulo != " ")
				{
					int rotulo = stoi(srotulo);
					rotulos.push_back(rotulo);
				}
			}
		}
		sort(rotulos.begin(), rotulos.end());
		verticesProcessados = true;
	}

	cout << "INICIANDO LEITURA" << endl;
	if (fin.is_open())
	{
		while (!fin.eof())
		{
			string out;
			getline(fin, out);

			if (out.find("#") == string::npos && out != "" && out != " ")
			{
				std::vector<std::string> parts;
				boost::split(parts, out, boost::is_any_of("\t,"));

				string sr1 = parts[0];
				string sr2 = parts[1];

				int r1 = stoi(sr1);
				int r2 = stoi(sr2);

				if (verticesProcessados == false)
				{
					vector<int>::iterator itr1 = lower_bound(rotulos.begin(), rotulos.end(), r1);
					if (itr1 == rotulos.end())
					{
						rotulos.insert(itr1, r1);
					}
					else
					{
						if (*itr1 != r1)
						{
							rotulos.insert(itr1, r1);
						}
					}

					vector<int>::iterator itr2 = lower_bound(rotulos.begin(), rotulos.end(), r2);
					if (itr2 == rotulos.end())
					{
						rotulos.insert(itr2, r2);
					}
					else
					{
						if (*itr2 != r2)
						{
							rotulos.insert(itr2, r2);
						}
					}
				}
				else
				{
					while (binary_search(rotulos.begin(), rotulos.end(), r1) == false)
						r1 = r1 / 10;
					while (binary_search(rotulos.begin(), rotulos.end(), r2) == false)
						r2 = r2 / 10;
				}

				arestas.push_back(make_pair(r1, r2));
			}
			cout << "\r" << arestas.size() << std::flush;
		}
		fin.close();

		cout << endl;
		cout << "LEITURA FINALIZADA" << endl;
		cout << "ENCONTRADOS: VERTICES: " << rotulos.size() << " -  ARESTAS: " << arestas.size() << endl;

		cout << "INICIANDO PROCESSAMENTO DO GRAFO" << endl;

		Grafo *GTemp = new Grafo();
		for (int r : rotulos)
		{
			Vertice v = GTemp->AdicionarVertice(r);
		}

		int arestaDuplicadas = 0;
		int loops = 0;
		for (pair<int, int> a : arestas)
		{
			if (a.first != a.second)
			{
				if (GTemp->AdicionarAdjacencia(a.first, a.second) == false)
				{
					arestaDuplicadas = arestaDuplicadas + 1;
				}
			}
			else
			{
				loops = loops + 1;
			}
		}

		int grau0 = 0;
		set<Vertice> V = GTemp->Vertices;
		for (Vertice v : V)
		{
			if (GTemp->Grau(v) == 0)
			{
				//cout << "GRAU 0: " << v.Id() << endl;
				GTemp->RemoverVertice(v);
				grau0 = grau0 + 1;
			}
		}

		//copiar GTemp para G corrigindo ids e rotulos
		Grafo *G = new Grafo();
		map<Vertice, Vertice> mapaRotulos;
		int i = 1;
		for (Vertice v : GTemp->Vertices)
		{
			Vertice vNovo = G->AdicionarVertice(i, v.Id());
			mapaRotulos.insert(make_pair(v, vNovo));
			i = i + 1;
		}

		for (pair<Vertice, Vertice> a : GTemp->ObterListaAdjacencias())
		{
			Vertice v = mapaRotulos[a.first];
			Vertice u = mapaRotulos[a.second];
			G->AdicionarAdjacencia(v, u);
		}

		for (Vertice v : G->Vertices)
		{
			if (v.Id() != v.Rotulo()) cout << "DIF: " << v.Id() << " - " << v.Rotulo() << endl;
		}

		cout << "RESULTADO PROCESSAMENTO: " << endl;
		cout << "ARESTAS DUPLICADAS: " << arestaDuplicadas << endl;
		cout << "LOOPS: " << loops << endl;
		cout << "VERTICES DE GRAU 0: " << grau0 << endl;

		cout << "GRAFO RESULTANTE" << endl;
		cout << "VERTICES (N) " << G->N() << endl;
		cout << "ARESTAS (M): " << G->M() << endl;
		cout << "GRAU MAXIMO: " << G->ObterGrauMaximo() << endl;

		cout << "SALVANDO OS DADOS" << endl;
		SalvarGrafoEmArquivo(arquivoSaida, G);
		cout << "PROCESSAMENTO CONCLUIDO" << endl;
	}
}

int main()
{
	
	return 0;
}