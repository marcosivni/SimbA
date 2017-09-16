#ifndef STILLIMAGE_HPP
#define STILLIMAGE_HPP

#include <third-party/artemis/Artemis.hpp>

class StillImage{

    private:
        Image *img;

    private:
        bool isValidExtension(string filename);
        void openImage(string filename)  throw (artemis::FileException*, artemis::FullHeapException*);

    public:
        StillImage(string filename = "");
        ~StillImage();

        void setImage(string filename);
        void setImage(Image *img);

        bool isValid();

        Image* getImage();
};

#endif // STILLIMAGE_HPP
