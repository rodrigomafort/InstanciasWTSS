#include "mpi.h"
#include <stdio.h>

#include <chrono>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <fstream>

#include "BibGrafos.h"

using namespace std;


int main(int argc, char* argv[])
{
	MPI::Init(argc,argv);
	int id = MPI::COMM_WORLD.Get_rank();
	int cores = MPI::COMM_WORLD.Get_size();
	
	if (argc >= 4)
	{
		char* arqin = argv[1];
		int n = atoi(argv[2]);
		int m = atoi(argv[3]);
		char* arqout = argv[4];

		MPI_Datatype dt_adj;
		MPI_Type_contiguous(2, MPI::INT, &dt_adj);
		MPI_Type_commit(&dt_adj);

		if (id == 0)
		{
			cout << "ABRINDO ARQUIVO DE ENTRADA" << endl;
			std::ifstream fin;
			fin.open(arqin);

			cout << "ABRINDO ARQUIVO DE SAIDA" << endl;
			MPI::File outFile = MPI::File::Open(MPI::COMM_WORLD, arqout, MPI::MODE_CREATE|MPI_MODE_RDWR, MPI::INFO_NULL);

			cout << "ARQUIVOS ABERTOS" << endl;

			ParAdj conf;
			conf.v1 = n;
			conf.v2 = m;
			MPI::Status status;
			outFile.Write(&conf,1,dt_adj,status);

			if (fin.is_open())
			{
				cout << "INCIANDO LEITURA" << endl;
				int i = 0;
				while (!fin.eof())
				{
					string out;
					fin >> out;
					stringstream ss(out);

					if (out != "" && out != " ")
					{
						string pt1, pt2;
						getline(ss,pt1,',');
						getline(ss,pt2,',');

						ParAdj p;
						p.v1 = stoi(pt1);
						p.v2 = stoi(pt2);
						MPI::Status status;
						outFile.Write(&p,1,dt_adj,status);
					}
				}
				fin.close();
			}

			outFile.Close();
		}
	}
	else
	{
		cout << "Erro nos Parametros" << endl;
	}

	MPI_Barrier(MPI::COMM_WORLD);

	MPI::Finalize();
	
	return 0;
}

