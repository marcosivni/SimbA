#ifndef STILLIMAGEEXTRACTORS_HPP
#define STILLIMAGEEXTRACTORS_HPP

#include <DataDictionary.hpp>
#include <StillImage.hpp>

template <class Array>
class StillImageExtractors{

    private:
        unsigned char type;

    public:
        enum Extractors{
            none = 0,
            histogramext = 1,
            metrichistogramext = 2,
            colorhistogramext = 3,
            zernikeext = 4,
            haralickext = 5,
            waveletshaarext = 6,
            waveletsdaubehiesext = 7,
            mpegext = 8
        };

    public:
        StillImageExtractors(int ext = 0){

            setExtractorMethod(ext);
        }

        void setExtractorMethod(int ext){

            type = ext;
        }

        Array* extract(StillImage *img){

            if (getCurrentExtractorMethod() != 0){

                Array *array = new Array();

                if (getCurrentExtractorMethod() == StillImageExtractors::histogramext){
                    NormalizedHistogramExtractor<Array> *nh = new NormalizedHistogramExtractor<Array>();
                    nh->generateSignature((*img->getImage()), (*array));
                    delete (nh);
                }
                if (getCurrentExtractorMethod() == StillImageExtractors::metrichistogramext){
                    MetricHistogramExtractor<Array> *mh = new MetricHistogramExtractor<Array>();
                    mh->generateSignature((*img->getImage()), (*array));
                    delete (mh);
                }
                if (getCurrentExtractorMethod() == StillImageExtractors::colorhistogramext){
                    TotalColorHistogramExtractor<Array> *tc = new TotalColorHistogramExtractor<Array>();
                    tc->generateSignature((*img->getImage()), (*array));
                    delete (tc);
                }
                if (getCurrentExtractorMethod() == StillImageExtractors::zernikeext){
                    ZernikeExtractor<Array> *ze = new ZernikeExtractor<Array>();
                    ze->generateSignature((*img->getImage()), (*array));
                    delete (ze);
                }
                if (getCurrentExtractorMethod() == StillImageExtractors::haralickext){
                    HaralickExtractor<Array> *he = new HaralickExtractor<Array>();
                    he->generateSignature((*img->getImage()), (*array));
                    delete (he);
                }
                if (getCurrentExtractorMethod() == StillImageExtractors::waveletshaarext){
                    HaarWaveletExtractor<Array> *ha = new HaarWaveletExtractor<Array>();
                    ha->generateSignature((*img->getImage()), (*array));
                    delete (ha);
                }
                if (getCurrentExtractorMethod() == StillImageExtractors::waveletsdaubehiesext){
                    DaubechiesWaveletExtractor<Array> *wd = new DaubechiesWaveletExtractor<Array>();
                    wd->generateSignature((*img->getImage()), (*array));
                    delete (wd);
                }

                return array;

            } else {
                return NULL;
            }
        }


        unsigned char getCurrentExtractorMethod(){

            return type;
        }
};

#endif // STILLIMAGEEXTRACTORS_HPP
