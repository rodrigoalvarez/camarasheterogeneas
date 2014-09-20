#ifndef __MAIN_H__
#define __MAIN_H__

#include <windows.h>
#include "masterPly.h"

/*  To use this exported function of dll, include this header
 *  in your project.
 */

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C"
{
#endif

struct NubePuntos{
    float* x;
    float* y;
    float* z;
    int largo;
};

void DLL_EXPORT SomeFunction(const char* sometext, int* number, char** salida);
void DLL_EXPORT generarMalla(NubePuntos* nbIN, FaceStruct** faces, int* numberFaces, int nroFrame);
#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__
