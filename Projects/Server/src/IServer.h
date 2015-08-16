#ifndef ISERVER_H
#define ISERVER_H

#include "ofMain.h"
#include "ThreadData.h"

class IServer : public ofBaseApp {
    public:
        virtual void computeFrames() = 0;
    protected:
    private:
};

#endif // ISERVER_H
