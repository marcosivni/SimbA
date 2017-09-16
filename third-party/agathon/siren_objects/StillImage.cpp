#include "StillImage.hpp"

bool StillImage::isValidExtension(string filename){

    unsigned int found = filename.find_last_of("/\\");
    string extension = filename.substr(found+1);

    return ((extension.find(".jpg") != string::npos) ||
        (extension.find(".jpeg") != string::npos) ||
        (extension.find(".bmp") != string::npos) ||
        (extension.find(".png") != string::npos) ||
        (extension.find(".krl") != string::npos) ||
        (extension.find(".dcm") != string::npos));
}

void StillImage::openImage(string filename) throw (artemis::FileException*, artemis::FullHeapException*){


    try{
        if (img != NULL){
            delete (img);
        }

        if (isValidExtension(filename)){
            unsigned int found = filename.find_last_of("/\\");
            string extension = filename.substr(found+1);

            if (extension.find(".jpg") != string::npos){
                JPGImage *jpg = new JPGImage(filename);
                img = jpg;
            }
            if (extension.find(".jpeg") != string::npos){
                JPGImage *jpg = new JPGImage(filename);
                img = jpg;
            }
            if (extension.find(".bmp") != string::npos){
                BMPImage *bmp = new BMPImage(filename);
                img = bmp;
            }
            if (extension.find(".png") != string::npos){
                PNGImage *png = new PNGImage(filename);
                img = png;
            }
            if (extension.find(".krl") != string::npos){
                KRLImage *krl = new KRLImage(filename);
                img = krl;
            }
            if (extension.find(".dcm") != string::npos){
                DCMImage *dcm = new DCMImage(filename);
                img = (Image*) dcm;
            }

        } else {
            throw new artemis::FileException(0, "Failed to open Image", filename);
        }
    } catch (artemis::FileException *fe){
        throw fe;
    } catch (artemis::FullHeapException *fh){
        throw fh;
    } catch (...) {
        throw new artemis::FileException(0, "Failed to open Image", filename);
    }
}

StillImage::StillImage(string filename){

    img = NULL;
    if (!filename.empty()){
        try{
            openImage(filename);
        } catch (...){
            throw new std::exception();
        }
    }
}

StillImage::~StillImage(){

    if (img != NULL){
        delete (img);
    }
}

void StillImage::setImage(string filename){

    try{
        openImage(filename);
    } catch (...){
        throw new std::exception();
    }
}

void StillImage::setImage(Image *img){

    if (this->img != NULL){
        delete (this->img);
        this->img = img;
    }
}

bool StillImage::isValid(){

    if (img != NULL)
        return isValidExtension(img->getFilename());
    else
        return false;
}

Image* StillImage::getImage(){

    return img;
}
