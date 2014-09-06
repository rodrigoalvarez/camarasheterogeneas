#pragma once

#include "ofMain.h"
#include "../Constants.h"
#include "../ThreadData.h"

#include <map>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;



class Field {
	public:
        time_t time;
        int totFrames;
        int id;

        Field();
		~Field();

        bool hasCam(long int camId);
        void addFrame(ThreadData *, long int);
};
