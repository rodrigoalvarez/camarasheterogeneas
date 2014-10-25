#include "modelSetting.h"
#include <vector>
#include <limits>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>
#include "GlobalData.h"


using namespace std;

typedef void (*f_ReadSharedSetting)(int* numberValues, int** idsValues, float** values);

Model_SET::Model_SET() {
    /*char* dllName = "C:\\CamarasHeterogeneas\\Proyecto\\camarasheterogeneas\\Proyectos\\MemoriaCompartida\\bin\\MemoriaCompartida.dll";
    shareSettingLibrary =  LoadLibraryA(dllName);
    if (!shareSettingLibrary) {
        cout << "No se pudo cargar la libreria: " << dllName << endl;
    }*/
}

void Model_SET::MemoryLoad() {

    f_ReadSharedSetting readSetting = (f_ReadSharedSetting)GetProcAddress(shareSettingLibrary, "ReadSharedSetting");

    int* numberValues = new int;
    int* idsValues = new int;
    float** values;
    cout << "Antes de leer Settings: " << endl;
    readSetting(numberValues, &idsValues, values);
    cout << "Despues de leer Settings: " << endl;

    NValues = *numberValues;

    IdsValues = new int[NValues];
    memcpy(IdsValues, idsValues, sizeof(int) * NValues);

    Values = new float[NValues * 16];
    memcpy(Values, values, sizeof(float) * NValues * 16);

    delete numberValues;
    delete idsValues;

}

void Model_SET::FileLoad() {

    GlobalData* gdata = new GlobalData();
    gdata->loadCalibData("settings.xml");

    NValues = gdata->getTotal2D();//Total de camaras 2D

    IdsValues = new int[NValues];

    Values = new float[NValues * 16];

    int count2DCameras = 0;

    for (int i = 0 ; i < gdata->nClientes ; i++){// recorro los clientes

        for(int j = 0; j < gdata->sys_data[i].nCamaras ; j++){//recorro las camaras de un cliente
            if (gdata->sys_data[i].camera[j].use2D){
                if (gdata->sys_data[i].camera[j].use3D)
                    IdsValues[count2DCameras] =  gdata->sys_data[i].cliId*100 + 2 * 10 + gdata->sys_data[i].camera[j].id;
                else
                    IdsValues[count2DCameras] =  gdata->sys_data[i].cliId*100 + 1 * 10 + gdata->sys_data[i].camera[j].id;

                cout << "Id de camara: " << IdsValues[count2DCameras] << endl;

                Values[count2DCameras*16] = gdata->sys_data[i].camera[j].row1[0];
                Values[count2DCameras*16+1] = gdata->sys_data[i].camera[j].row1[1];
                Values[count2DCameras*16+2] = gdata->sys_data[i].camera[j].row1[2];
                Values[count2DCameras*16+3] = gdata->sys_data[i].camera[j].row1[3];

                Values[count2DCameras*16+4] = gdata->sys_data[i].camera[j].row2[0];
                Values[count2DCameras*16+5] = gdata->sys_data[i].camera[j].row2[1];
                Values[count2DCameras*16+6] = gdata->sys_data[i].camera[j].row2[2];
                Values[count2DCameras*16+7] = gdata->sys_data[i].camera[j].row2[3];

                Values[count2DCameras*16+8] = gdata->sys_data[i].camera[j].row3[0];
                Values[count2DCameras*16+9] = gdata->sys_data[i].camera[j].row3[1];
                Values[count2DCameras*16+10] = gdata->sys_data[i].camera[j].row3[2];
                Values[count2DCameras*16+11] = gdata->sys_data[i].camera[j].row3[3];

                Values[count2DCameras*16+12] = gdata->sys_data[i].camera[j].row4[0];
                Values[count2DCameras*16+13] = gdata->sys_data[i].camera[j].row4[1];
                Values[count2DCameras*16+14] = gdata->sys_data[i].camera[j].row4[2];
                Values[count2DCameras*16+15] = gdata->sys_data[i].camera[j].row4[3];

                count2DCameras++;

            }
        }
    }
}
