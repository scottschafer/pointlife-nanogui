#include "constants.h"
#include "../EasyCL/EasyCL.h"
#include "FlexArrays.h"

extern EasyCL * cl;

inline CLIntWrapperEx * wrap(vector<int> & v) {
  return new CLIntWrapperEx((int)v.size(), v.data(), cl);
}

inline CLFloatWrapperEx * wrap(vector<float> & v) {
  return new CLFloatWrapperEx((int)v.size(), v.data(), cl);
  //return cl->wrap((int)v.size(), v.data());
}

CLIntWrapper * IntArray :: get() {
  if (! _data) {
    CLIntWrapperEx * w = wrap(a);
    w->setFlags(flags);
    _data = wrap(a);
  }
  _data->setFlags(0);
  
  return _data;
}

CLFloatWrapper * FloatArray :: get() {
  if (! _data) {
    CLFloatWrapperEx * w = wrap(a);
    w->setFlags(flags);
    _data = w;
  }
  
  return _data;
}

void FloatArray :: copyFrom(FloatArray & src) {
  if (a.size() == 0) {
    a.assign(src.a.begin(), src.a.end());
    copyToDevice();
  }
  
  CLFloatWrapper * destW = get();
  CLFloatWrapper * srcW = src.get();
  
  srcW->copyTo(destW);
  //destW->copyFrom(srcW->get());
}
