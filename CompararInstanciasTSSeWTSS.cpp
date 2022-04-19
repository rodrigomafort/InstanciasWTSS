#include <iostream>
#include <sstream>
#include <stdexcept>
#include <fstream>
#include <string>
#include <vector>
#include <set>

using namespace std;

typedef struct
{
	int v1;
	int v2;
} ParAdj;

int LerWTSS(string arq, vector<pair<int,int>> &arestas, vector<pair<int,int>> &pesos, vector<pair<int,int>> &requisitos)
{
	int n;
	int m;
	
	string linha;
	ifstream arquivoEntrada(arq);
	int flag = 0;
	if (arquivoEntrada.is_open())
	{
		while (getline(arquivoEntrada,linha))
		{
			int contaEspaco = 0;
			for (int i = 0; i < linha.length(); i++)
			{
				if (linha[i] == ' ')
					contaEspaco++;
				if (contaEspaco > 1)
					break;
			}
			
			if (contaEspaco == 1)
			{
				stringstream ss(linha);
				string pt1, pt2;
				getline(ss,pt1,' ');
				getline(ss,pt2,' ');
				arestas.push_back(make_pair(stoi(pt1),stoi(pt2)));
			}
			else
			{
				stringstream ss(linha);
				if (flag == 0)
				{					
					int id = 0;
					while (!ss.eof())
					{
						string peso;
						getline(ss,peso,' ');						
						pesos.push_back(make_pair(id,stoi(peso)));						
						id++;
					}
					
					flag = 1;
				}
				else
				{
					int id = 0;
					while (!ss.eof())
					{
						string req;
						getline(ss,req,' ');						
						pesos.push_back(make_pair(id,stoi(req)));						
						id++;
					}
				}
			}
		}
		arquivoEntrada.close();
	}
	
	int max = -1;
	for (pair<int,int> p : arestas)
	{
		if (p.first > max) max = p.first;
		if (p.second > max) max = p.second;
	}
	cout << "MAX: " << max << endl;
	
	return max+1;	
}

void LerTSS(string arq, ParAdj *vertices, ParAdj *adjs, int &n, int &m)
{
	FILE *file;

	file = fopen(arq.c_str(), "rb");
	
	ParAdj conf;
	fread(&conf, sizeof(ParAdj), 1, file);

	n = conf.v1;
	m = conf.v2;

	cout << "N:" << n << endl;
	cout << "M:" << m << endl;

	vertices = (ParAdj*) malloc(n * sizeof(ParAdj));
	adjs = (ParAdj*) malloc(m * sizeof(ParAdj));

	for (int i = 0; i < n; i++)
	{
		fread(&vertices[i], sizeof(ParAdj), 1, file);
	}
	
	for (int i = 0; i < m; ++i)
	{
		fread(&adjs[i], sizeof(ParAdj), 1, file);
	}

	fclose(file);
}

int main()
{
	vector<pair<int,int>> lista_arestas;
	vector<pair<int,int>> pesos;
	vector<pair<int,int>> requisitos;
	int N = LerWTSS("douban_0.txt",lista_arestas,pesos,requisitos);
	
	vector<set<int>> lista1(N,set<int>());
	
	for (pair<int,int> p : lista_arestas)
	{
		lista1[p.first].insert(p.second);
		lista1[p.second].insert(p.first);
	}
	
	ParAdj *vertices;
	ParAdj *arestas;	
	
	int n1, m1;
	LerTSS("Douban.bin", vertices, arestas, n1, m1);
	
	vector<set<int>> lista2(n1,set<int>());
	for (int x = 0; x < m1; x++)
	{
		lista2[arestas[x].v1 - 1].insert(arestas[x].v2 - 1);
		lista2[arestas[x].v2 - 1].insert(arestas[x].v1 - 1);
	}
	
	for (int i = 0; i < N; i++)
		if (lista1[i] != lista2[i])
			cout << "DIF: " << i << endl;

	
	return 0;
}