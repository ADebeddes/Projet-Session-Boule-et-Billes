#pragma once
#include "stdafx.h"
#include "sommetbloc.h"
#include <iostream>
#include <fstream>
#include <string>
#include "FileObject.h"

using namespace std;
using namespace PM3D;

class ObjectImporter
{
public:
	FileObject object{};

	void import(string path);

};



