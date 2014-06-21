#include "Transmitter.h"
#include "Base64.h"
#include "ToHex.h"
#include "Grabber.h"

void Transmitter::threadedFunction() {

    //grabber->updateThreadData();
    int counter         = 0;
    int size            = 0;
    unsigned int i      = 0;

    while(isThreadRunning()) {
        //if(yacorrio)
        size    = 0;
        grabber->updateThreadData();
        ThreadData * tdata = grabber->tData;
        int destlen;
        int srclen;
        char   * destpix;

        //for(int i = 0; i < (grabber->total2D + grabber->total3D); i++ ) {
        //    size += sizeof(tdata);
            if(tdata[i].inited) {
                unsigned char * pix     = tdata[i].img.getPixels();
                const char * dest       = HexEncode(pix);
                unsigned char * dpix    = (unsigned char *) HexDecode(dest);

                ofImage img;
                img.setFromPixels( pix, tdata[i].img.width, tdata[i].img.height, OF_IMAGE_COLOR, true);
                img.saveImage("img_deco.png");

                ofFile file( "filename.txt", ofFile::WriteOnly );
                // "size: " + " - " + ofToString(destlen)
                file << dest;
                file.close();
            }

            /*srclen  = (tdata[i].img.width*tdata[i].img.height);

            destlen = (srclen+2)*3*4;
            destpix = new char [destlen];
            int res = base64_encode(pix, srclen, destpix, destlen);
            if(res > 0) {
                unsigned char * decopix = new unsigned char [srclen];
                int res2 = base64_decode(destpix, decopix, srclen);
                //ofImage img;
                if(res2 > 1) {
                    img.setFromPixels(decopix, tdata[i].img.width, tdata[i].img.height, OF_IMAGE_COLOR, true);
                    img.saveImage("img_deco.png");
                    ofFile file( "filename.txt", ofFile::WriteOnly );
                    // "size: " + " - " + ofToString(destlen)
                    file << ofToString(res2);
                    file.close();
                }
                ofFile file( "filename_data.txt", ofFile::WriteOnly );
                // "size: " + " - " + ofToString(destlen)
                file << ofToString(destlen) + " " + ofToString(strlen(destpix));
                file.close();
                //unsigned char * pix = tdata[i].img.getPixels();
                //size += sizeof(pix);
                // size += tdata[i].spix.size();
            }*/

        //}

        //Para c/camara
        //  si es 2D
        //      scaledown imagen
        //  si es 3D
        //      scaledown imagen
        //      scaledown profundidad
        /*
        if(tdata[0].inited) {
            //ofImage img      = tdata[0].img;
            //tdata[0].img.saveImage("transmitter/timg-"+ofToString(counter)+".png");
            counter++;
        }
        */
    }
}
