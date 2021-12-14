#include "StdAfx.h"
#include "ObjectImporter.h"

#include <sstream>
#include <istream>
#include <shlwapi.h>
#include <limits>


using namespace PM3D;
using namespace DirectX;

void ObjectImporter::import(string path)
{
	string delimiter = " ";

	/********************** import****************************/
	string name_file = path.substr(0, path.find("."));
	ifstream input_file(name_file+".data", ios::binary);

	if (input_file)
	{
		object.read(input_file); 
		return; 
	}
	input_file.close();
	
		float max_x = -FLT_MAX, max_y = -FLT_MAX, max_z = -FLT_MAX, min_x = FLT_MAX, min_y = FLT_MAX, min_z = FLT_MAX;
	string line; 
	std::ifstream objfile(path);
	int nb_sommet = 0;
	int nb_text = 0; 
	do
	{
		getline(objfile, line);
		XMFLOAT3 current{};
		XMFLOAT2 texture{};
		vector<string> tab{};

		stringstream ss = stringstream(line);
		string tmp;
		while (getline(ss, tmp, ' '))
		{
			tab.push_back(tmp);
		}

		if (tab.size() > 0)
		{
			if (tab[0] == "v")
			{
				current.x = std::stof(tab[1]);
				current.y = std::stof(tab[2]);
				current.z = std::stof(tab[3]);

				max_x = current.x > max_x ? current.x : max_x;
				max_y = current.y > max_y ? current.y : max_y;
				max_z = current.z > max_z ? current.z : max_z;
				min_x = current.x < min_x ? current.x : min_x;
				min_y = current.y < min_y ? current.y : min_y;
				min_z = current.z < min_z ? current.z : min_z;

				object.points_.push_back(current);

			}
			else if (tab[0] == "vn")
			{
				current.x = std::stof(tab[1]);
				current.y = std::stof(tab[2]);
				current.z = std::stof(tab[3]);
				object.normales_.push_back(current);

			}
			else if (tab[0] == "vt")
			{
				texture.x = std::stof(tab[1]);
				texture.y = std::stof(tab[2]);
				object.textures_.push_back(texture);
			}

			else if (tab[0] == "f")
			{
				object.max_x =max_x ;
				object.max_y = max_y ;
				object.max_z = max_z ;
				object.min_x = min_x;
				object.min_y = min_y;
				object.min_z = min_z;
				vector<string> small_tab{};
				for (int i = 1; i < tab.size(); i++)
				{
					string a = tab[i].substr(0, tab[i].find("/"));
					string b = tab[i].substr(tab[i].find("/") + 1, tab[i].find_last_of("/"));
					string c = tab[i].substr(tab[i].find_last_of("/") + 1);

					object.index_.push_back(static_cast<unsigned int>(nb_sommet++));
					object.indices_.push_back(std::stoi(a)-1);
					
					CSommetBloc sb;

					if (object.textures_.size() > 0) {
						const int point_i{ std::stoi(a) - 1 };

						const int texture_j{ std::stoi(b) - 1 } ;

						const int normale_k{ std::stoi(c) - 1 } ;

						sb = CSommetBloc(object.points_[point_i], object.normales_[normale_k], object.textures_[texture_j]);
					}
					else {
						const int point_i{ std::stoi(a) - 1 } ;

						const int normale_k{ std::stoi(c) - 1 } ;

						if (nb_sommet % 10 == 0)
						{
							nb_text++; 
						} 
						const int x = static_cast<int>(object.points_[point_i].x);
						const int z = static_cast<int>(object.points_[point_i].z);
						const float x_a = fmod(x / ( (abs(object.max_x - object.min_x)+1) ) , 1.0f);
						const float x_b = fmod(z / ( (abs(object.max_z - object.min_z) +1) ) , 1.0f);
						sb = CSommetBloc(object.points_[point_i], object.normales_[normale_k], XMFLOAT2(x_a , x_b));
					}

					object.sommets_.push_back(sb);
				}

			}
		}
	} while (!objfile.eof());
	
	if (object.textures_.size() > 0) {
		object.has_textture_ = true;
	}
	cout << "bonk";

	

	ofstream output_file (name_file + ".data", ios::binary);
	if (output_file)
	{
		object.write(output_file);
		
	}
	output_file.close(); 
}
