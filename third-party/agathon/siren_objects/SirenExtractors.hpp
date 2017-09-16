#ifndef SIRENEXTRACTORS_HPP
#define SIRENEXTRACTORS_HPP

#include <ComplexObject.hpp>
//Include future extractors here
#include <StillImageExtractors.hpp>

template <class Array>
class SirenExtractors{

    private:
        unsigned char types, extractorType;
        DataDictionary *dic;

    private:
        DataDictionary* dictionary(){

            return dic;
        }

    public:

        SirenExtractors(string dataType, string extractor, DataDictionary *dic) throw (std::exception*){

            this->dic = dic;
            addNewExtractor(extractor, dataType);
        }

        void addNewExtractor(string extractor, string dataType) throw (std::exception*){

          if (!dictionary()->isValidExtractorForThisType(dataType, extractor))
                throw (new std::exception);

            types = atoi(dictionary()->getComplexDataTypeCode(dataType).c_str());
            extractorType = atoi(dictionary()->getExtractorCode(extractor).c_str());
        }

        Array *extract(ComplexObject *complexObject) throw (std::exception*){

            if (types == siren::STILLIMAGE){
                if (complexObject->isStillImage()){
                    StillImageExtractors<Array> ext(extractorType);
                    return ext.extract(complexObject->stillImage());
                } else {
                    throw new std::exception();
                }
            }
        }
};

#endif // SIRENEXTRACTORS_HPP
