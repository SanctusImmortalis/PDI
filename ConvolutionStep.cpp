#include <cstdlib>
#include <cmath>
#include "include/ConvolutionStep.h"

void RGBtoYStep::run(){
  ConvolutionDataSet* ds = (ConvolutionDataSet*) (this->dataSet);
  if(this->isSetup && ds!=NULL){
    if(ds->rgbsource->getWidth() != ds->ysource->getWidth() || ds->rgbsource->getHeight() != ds->ysource->getHeight()){
      ds->result = CONVERSION_FAIL;
      return;
    }
    bool res = true;
    for(int i=0;i<(ds->rgbsource->getWidth());i++){
      for(int j=0;j<(ds->rgbsource->getHeight());j++){
        res &= this->func(i, j, 0);
      }
    }
    if(!res) ds->result = CONVERSION_FAIL;
  }
  this->nextStep->run();
}

bool RGBtoYStep::func(int x, int y, int z){
  ConvolutionDataSet* ds = (ConvolutionDataSet*) (this->dataSet);
  RGBPixel pix = ds->rgbsource->getPixel(x, y);
  double newpix = 0.299f * (((double) pix.r) / 255.0f) + 0.587f * (((double) pix.g) / 255.0f) + 0.114f * (((double) pix.b) / 255.0f);
  ds->ysource->setPixel(newpix, x, y);
  return true;
}

void ConvolutionStep::run(){
  ConvolutionDataSet* ds = (ConvolutionDataSet*) (this->dataSet);
  if(this->isSetup && ds!=NULL){
    if(ds->rgbsource->getWidth() != ds->rgbdest->getWidth() || ds->rgbsource->getHeight() != ds->rgbdest->getHeight() || ds->ysource->getWidth() != ds->ydest->getWidth() || ds->ysource->getHeight() != ds->ydest->getHeight()){
      ds->result = CONVOLUTION_FAIL;
      return;
    }
    ds->midx = ds->mask->getWidth() / 2;
    ds->midy = ds->mask->getHeight() / 2;
    bool res = true;
    for(int i=0;i<(ds->rgbsource->getWidth());i++){
      for(int j=0;j<(ds->rgbsource->getHeight());j++){
        res &= this->func(i, j, 0);
      }
    }
    if(!res) ds->result = CONVOLUTION_FAIL;
  }
}

bool ConvolutionStep::func(int x, int y, int z){
  ConvolutionDataSet* ds = (ConvolutionDataSet*) (this->dataSet);
  RGBPixel rgbres;
  rgbres.r = 0;
  rgbres.g = 0;
  rgbres.b = 0;
  int iaux;
  double yres = 0.0f;
  double daux;
  int i = - (ds->midx);
  int j = - (ds->midy);
  int ilast = i + ds->mask->getWidth();
  int jlast = j + ds->mask->getHeight();
  for(i = - (ds->midx);i<ilast;i++){
    for(j = - (ds->midy);j<jlast;j++){
      RGBPixel rgbpix = ds->rgbsource->getPixel(x - i, y - j);
      iaux = (int) (rgbpix.r * ds->mask->getPixel(i + (ds->midx), j + (ds->midy)));
      rgbres.r += iaux;
      iaux = (int) (rgbpix.g * ds->mask->getPixel(i + (ds->midx), j + (ds->midy)));
      rgbres.g += iaux;
      iaux = (int) (rgbpix.b * ds->mask->getPixel(i + (ds->midx), j + (ds->midy)));
      rgbres.b += iaux;
      daux = ds->ysource->getPixel(x-i, y-j) * ds->mask->getPixel(i + (ds->midx), j + (ds->midy));
      yres += daux;
    }
  }
  rgbres.r += ds->rgboffset;
  rgbres.g += ds->rgboffset;
  rgbres.b += ds->rgboffset;
  yres += ds->yoffset;
  ds->rgbdest->setPixel(rgbres, x, y);
  ds->ydest->setPixel(clampY(yres), x, y);
  return true;
}
