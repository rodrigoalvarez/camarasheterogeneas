#pragma once

#include "ofMain.h"
#include <vector>
#include "shareMesh.h"
#include "shareImage.h"


class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		vector<ShareImage*> images;
		vector<ShareMesh*> meshes;
};
