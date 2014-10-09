#ifndef __MAIN_H__
#define __MAIN_H__

#include <windows.h>
#include "ofxSharedMemory.h"

/*  To use this exported function of dll, include this header
 *  in your project.
 */

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif


struct FaceStruct
{
    float p1[3];
    float p2[3];
    float p3[3];
};

#ifdef __cplusplus
extern "C"
{
#endif


void DLL_EXPORT SomeFunction(const LPCSTR sometext);

void DLL_EXPORT compartirMemoria(int numberFaces, FaceStruct* faces);

void DLL_EXPORT leerMemoria(int* numberFaces, FaceStruct** faces);

void DLL_EXPORT ReadSharedImage(int* Id, unsigned char** Pixels, int* Width, int* Height);

void DLL_EXPORT ShareImage(int* imageId, unsigned char** pixels, int* wPixels, int* hPixels);

void DLL_EXPORT ShareMesh(int meshId, int numberFaces, FaceStruct* faces);

void DLL_EXPORT ReadSharedMesh(int* Id, int* numberFaces, FaceStruct** faces);

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__
