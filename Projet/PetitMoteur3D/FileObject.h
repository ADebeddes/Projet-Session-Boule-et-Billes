#pragma once
#include "stdafx.h"
#include "sommetbloc.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using namespace PM3D;

template<typename POD>
std::ostream& serialize(std::ostream& os, std::vector<POD> const& v)
{
	// this only works on built in data types (PODs)
	static_assert(std::is_trivial<POD>::value && std::is_standard_layout<POD>::value,
		"Can only serialize POD types with this function");

	const auto size = v.size();
	os.write(reinterpret_cast<char const*>(&size), sizeof(size));
	os.write(reinterpret_cast<char const*>(v.data()), v.size() * sizeof(POD));
	return os;
}

template<typename POD>
std::istream& deserialize(std::istream& is, std::vector<POD>& v)
{
	static_assert(std::is_trivial<POD>::value && std::is_standard_layout<POD>::value,
		"Can only deserialize POD types with this function");

	decltype(v.size()) size;
	is.read(reinterpret_cast<char*>(&size), sizeof(size));
	v.resize(size);
	is.read(reinterpret_cast<char*>(v.data()), v.size() * sizeof(POD));
	return is;
}


class   FileObject
{
public:
	vector<unsigned int> index_{};
	vector< XMFLOAT3> points_{};
	vector< XMFLOAT3> normales_{};
	vector< XMFLOAT2> textures_{};
	vector< CSommetBloc> sommets_{};
	vector<UINT> indices_{};
	float max_x{ 0 }, max_y{ 0 }, max_z{ 0 };
	float min_x{ 0 }, min_y{ 0 }, min_z{ 0 };
	bool has_textture_{ false };


	void write(std::ostream& f)
	{
		serialize(f, index_);
		serialize(f, points_);
		serialize(f, normales_);
		serialize(f, textures_);
		serialize(f, sommets_);
		serialize(f, indices_);
		f.write((char*)&max_x, sizeof(max_x));
		f.write((char*)&max_y, sizeof(max_y));
		f.write((char*)&max_z, sizeof(max_z));
		f.write((char*)&min_x, sizeof(min_x));
		f.write((char*)&min_y, sizeof(min_y));
		f.write((char*)&min_z, sizeof(min_z));
		f.write((char*)&has_textture_, sizeof(has_textture_));
	}


	void read(std::istream& f)
	{
		deserialize(f, index_);
		deserialize(f, points_);
		deserialize(f, normales_);
		deserialize(f, textures_);
		deserialize(f, sommets_);
		deserialize(f, indices_);
		f.read((char*)&max_x, sizeof(max_x));
		f.read((char*)&max_y, sizeof(max_y));
		f.read((char*)&max_z, sizeof(max_z));
		f.read((char*)&min_x, sizeof(min_x));
		f.read((char*)&min_y, sizeof(min_y));
		f.read((char*)&min_z, sizeof(min_z));
		f.read((char*)&has_textture_, sizeof(has_textture_));
	}
};


