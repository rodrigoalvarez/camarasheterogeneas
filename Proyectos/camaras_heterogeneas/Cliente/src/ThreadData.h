#pragma once

class ThreadData {
	public:
        int     type;    // 1- 2D, 2- 3D
        double  timestamp;
        ofImage img;
        int     inited;
        void    updateData();
        ofShortPixels  spix;
};
