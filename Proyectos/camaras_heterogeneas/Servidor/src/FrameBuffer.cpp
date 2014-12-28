#include "FrameBuffer.h"

FrameBuffer::FrameBuffer() {
    tope = 0;
    base = 0;

    for(int i = 0; i<MAX_BUFF_SIZE; i++) {
        buffer[i] = NULL;
    }
}

FrameBuffer::~FrameBuffer() {
    tope = 0;
    base = 0;
    ofLogVerbose() << "[FrameBuffer::~FrameBuffer]";
    for(int i = 0; i<MAX_BUFF_SIZE; i++) {
        try {
           delete(buffer[i]);
        } catch (int e) {
            ofLogWarning() << "[FrameBuffer::~FrameBuffer]: An exception occurred. Exception Nr. " << e;
        }
    }
}

void FrameBuffer::addFrame(ThreadData * frame, int totalCams) {
    if(totalCameras == 0 ) return;
    ofLogVerbose() << "[FFrameBuffer::addFrame] - " << "base " << base << ", tope " << tope << ", totalCams " << totalCams;

    if((tope == base) && (buffer[base] != NULL)) {
        try {
            int i=0;
            ThreadData * prevFrame = buffer[base];
            for(i=0; i<totalCameras[base]; i++) {
                if(prevFrame[i].state > 0) {
                    cout << "Hago delete" << endl;
                    /*if((prevFrame[i].state == 1) || (prevFrame[i].state == 3)) {
                        cout << "delete 1" << endl;

                    }
                    if((prevFrame[i].state == 2) || (prevFrame[i].state == 3)) {
                        cout << "delete 2" << endl;
                        delete prevFrame[i].xpix;
                        delete prevFrame[i].ypix;
                        delete prevFrame[i].zpix;
                    }*/
                    delete(&prevFrame[i]);
                }
            }
            /*for(i=0; i<totalCams; i++) {
                if(prevFrame[i].state > 0) {
                    cout << "Hago delete" << endl;
                    if((prevFrame[i].state == 1) || (prevFrame[i].state == 3)) {
                        cout << "delete 1" << endl;
                        delete prevFrame[i].compImg;
                    }
                    if((prevFrame[i].state == 2) || (prevFrame[i].state == 3)) {
                        cout << "delete 2" << endl;
                        delete prevFrame[i].xpix;
                        delete prevFrame[i].ypix;
                        delete prevFrame[i].zpix;
                    }
                }
            }*/
            delete(buffer[base]);
        } catch (int e) {
            ofLogWarning() << "[FrameBuffer::addFrame]: An exception occurred. Exception Nr. " << e;
        }
        buffer[base] = NULL;
        base         = ((base + 1) % MAX_BUFF_SIZE);
    }

    buffer[tope]        = frame;
    totalCameras[tope]  = totalCams;

    tope         = ((tope + 1) % MAX_BUFF_SIZE);
}

std::pair <int, ThreadData *> FrameBuffer::getHeadFrame() {
    std::pair <int, ThreadData *> retVal;
    if(buffer[base] == NULL) {
        retVal.first    = 0;
        retVal.second   = NULL;
        ofLogWarning() << "[FrameBuffer::getHeadFrame]: Se llamó a getHeadFrame y estaba vacío. "  << "base " << base << ", tope " << tope;
        return retVal;
        //return NULL;
    }
    retVal.first        = totalCameras[base];
    retVal.second       = buffer[base];
    buffer[base] = NULL;

    base         = ((base + 1) % MAX_BUFF_SIZE);
    return retVal;
}
