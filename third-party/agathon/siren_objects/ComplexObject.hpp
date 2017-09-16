#ifndef COMPLEXOBJECT_HPP
#define COMPLEXOBJECT_HPP

#include <StillImage.hpp>
#include <DataDictionary.hpp>

class ComplexObject{

    private:
        unsigned char type;
        //Adicionar novos tipos de dados
        StillImage *sImage;

    private:
        void open(string filename){

            if (sImage != NULL)
                delete (sImage);

            sImage = NULL;

            try{
                sImage =  new StillImage(filename);
            } catch (...) {
                sImage = NULL;
                throw new std::exception();
                //outro try para outros tipos de dados complexos
            }
        }

    public:
        ComplexObject(string filename){

            sImage = NULL;
            try{
                open(filename);
            } catch (...){
                throw new std::exception();
            }
        }

        ~ComplexObject(){

            if (sImage != NULL){
                delete (sImage);
            }
        }

        void addComplexObject(string filename){

            try{
                open(filename);
            } catch (...){
                throw new std::exception();
            }
        }

        bool isStillImage(){

            return (sImage != NULL);
        }

        StillImage *stillImage(){

            return sImage;
        }
};

namespace siren{

    //Include future complex types here
    enum ComplexType{
        STILLIMAGE = 0
    };

    //Including complex data class handling here
    class StillImage;
}


#endif // COMPLEXOBJECT_HPP
