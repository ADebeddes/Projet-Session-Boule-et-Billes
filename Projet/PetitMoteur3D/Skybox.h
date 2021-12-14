#pragma once

#include "DynamicObject.h"
class Skybox :public ImportedObject
{
public:
	Skybox(CDispositifD3D11* pDispositif_, string path, char* name = "Skybox");
	
	void Anime(float tempsEcoule) override;
	void Draw() override;
private:
	char* object_name = { "Skybox" };

};

