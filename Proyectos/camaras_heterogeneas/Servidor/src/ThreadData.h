#pragma once

class ThreadData {
	public:
        time_t   timestamp;
        char *  timestampStr;
        int     cliId;   // ID de la configuraci�n de Cliente. Puede haber N
        int     camId;   // ID de la c�mara en la instalaci�n.
        ofImage img;
        int     state; //0-No inited, 1-Only Image, 2-Only Point Cloud, 3-Ambas
        void    updateData();
        ofShortPixels  spix;
};
