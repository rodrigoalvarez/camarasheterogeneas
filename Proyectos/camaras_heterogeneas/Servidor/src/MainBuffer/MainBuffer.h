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
		* Un frame está compuesto por información de una sola cámara de un cliente dado.
		* Esta función agrupa la info de c/cámara según un redondeo
		* del tiempo en el que haya sido usado.
		* Recibe por parámetro el frame, el id de la camara dentro del cliente y el id del cliente.
		*/
		void addFrame( ThreadData * frame , int cam, int cli);

        //int * buffer[MAX_BUFF_SIZE];

		/**
		* Retorna y quita el frame más viejo.
		* Si no hay más retorna null.
		*/
		//ThreadData * getHeadFrame();

		private:

		FieldManager * fm;

		long int getCamId(int cam, int cli);

};
