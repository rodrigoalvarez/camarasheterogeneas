#pragma once

class ThreadData {
	public:
        time_t   timestamp;
        char *  timestampStr;
        int     cliId;   // ID de la configuración de Cliente. Puede haber N
        int     camId;   // ID de la cámara en la instalación.
        ofImage img;
        int     state; //0-No inited, 1-Only Image, 2-Only Point Cloud, 3-Ambas
        void    updateData();
        ofShortPixels  spix;
};
