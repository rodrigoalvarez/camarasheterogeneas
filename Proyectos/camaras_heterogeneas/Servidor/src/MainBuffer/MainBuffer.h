#pragma once

#include "ofMain.h"
#include "../Constants.h"
#include "../ThreadData.h"
#include "FieldManager.h"
#include <map>
#include <string>
#include <iostream>
#include <sstream>

class MainBuffer {
	public:
        MainBuffer();
		~MainBuffer();

		/**
		* Agrega un Frame al buffer principal.
		* Un frame est� compuesto por informaci�n de una sola c�mara de un cliente dado.
		* Esta funci�n agrupa la info de c/c�mara seg�n un redondeo
		* del tiempo en el que haya sido usado.
		* Recibe por par�metro el frame, el id de la camara dentro del cliente y el id del cliente.
		*/
		void addFrame( ThreadData * frame , int cam, int cli);

        //int * buffer[MAX_BUFF_SIZE];

		/**
		* Retorna y quita el frame m�s viejo.
		* Si no hay m�s retorna null.
		*/
		//ThreadData * getHeadFrame();

		private:

		FieldManager * fm;

		long int getCamId(int cam, int cli);

};
