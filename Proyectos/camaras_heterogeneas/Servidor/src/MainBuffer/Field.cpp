#include "Field.h"

using namespace std;
map< long int, ThreadData * > frame_map;

Field::Field() {
    totFrames   = 0;
}

Field::~Field() {
    for (map< long int, ThreadData * >::iterator it = frame_map.begin(); it != frame_map.end(); ++it) {
        delete ((ThreadData *) it->second);
    }
    ofLogVerbose() << "[Field::~Field]";
}

bool Field::hasCam(long int camId) {
    try {
        return (frame_map.find( camId ) != frame_map.end());
    } catch (std::out_of_range e) {}
    return false;
}

void Field::addFrame(ThreadData * frame, long int camId) {
    frame_map.insert ( std::pair<long int , ThreadData * > (camId, frame) );
}
