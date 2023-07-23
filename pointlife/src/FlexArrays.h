//
//  FlexArrays.h
//  ClusterBotzGPU
//
//  Created by Scott Schafer on 5/31/17.
//  Copyright © 2017 Scott Schafer. All rights reserved.
//

#ifndef FlexArrays_h
#define FlexArrays_h

#include <vector>
#include "../EasyCL/EasyCL.h"

using namespace std;
using namespace easycl;

class CLIntWrapperEx : public CLIntWrapper {
  int flags;

public:
    CLIntWrapperEx(int N, int *_hostarray, EasyCL *easycl) : CLIntWrapper(N,_hostarray,easycl) {
      flags = CL_MEM_READ_WRITE;
    }

    void setFlags(int flags) { this->flags = flags; }
  
  virtual void createOnDevice() {
    if(onDevice) {
        throw std::runtime_error("createOnDevice(): already on device");
    }
    devicearray = clCreateBuffer(*(cl->context), flags, getElementSize() * N, 0, &error);
    cl->checkError(error);
    onDevice = true;
    deviceDirty = false;
  }
};

class CLFloatWrapperEx : public CLFloatWrapper {
  int flags;

public:

  CLFloatWrapperEx(int N, float *_hostarray, EasyCL *easycl) : CLFloatWrapper(N,_hostarray,easycl) {
    flags = CL_MEM_READ_WRITE;
  }

  void setFlags(int flags) { this->flags = flags; }

  virtual void createOnDevice() {
    if(onDevice) {
        throw std::runtime_error("createOnDevice(): already on device");
    }
    devicearray = clCreateBuffer(*(cl->context), flags, getElementSize() * N, 0, &error);
    cl->checkError(error);
    onDevice = true;
    deviceDirty = false;
  }
  
  void copyToHostEx() {
    if(!onDevice) {
        throw std::runtime_error("copyToHost(): not on device");
    }
    //cl->finish();
  
    cl_event event = NULL;
    size_t sizeCopy = getElementSize() * N;
    error = clEnqueueReadBuffer(*(cl->queue), devicearray, CL_TRUE, 0, sizeCopy, getHostArray(), 0, NULL, NULL);
    return;

  
    error = clEnqueueReadBuffer(*(cl->queue), devicearray, CL_TRUE, 0, getElementSize() * N, getHostArray(), 0, NULL, &event);
    cl->checkError(error);
    cl_int err = clWaitForEvents(1, &event);
    clReleaseEvent(event);
    if (err != CL_SUCCESS) {
        //throw std::runtime_error("wait for event on copytohost failed with " + easycl::toString(err) );
    }
    deviceDirty = false;
  }
  
};

class BaseArray {
public:
  virtual void copyToDevice() = 0;
  virtual void copyFromDevice() = 0;
  
  virtual void appendNegativeOne() = 0;
  
  virtual size_t size() = 0;
};

class IntArray : public BaseArray {
  int flags;
public:
  IntArray(int flags = CL_MEM_READ_WRITE) { _data = NULL; this->flags = flags; }
  void setFlags(int flags) { this->flags = flags; }
  size_t size() { return a.size(); }
  int & operator[](size_t i) { return a[i]; }
  int * data() { return a.data(); }
  void appendNegativeOne() { a.push_back(-1); }
  void push_back(int v) { a.push_back(v); }
  void resize(size_t v) { a.resize(v); }
  
  operator CLIntWrapper *() { return this->get(); }
  operator int *() { return this->data(); }
  
  void copyToDevice() {
#if USE_GPU
    get()->copyToDevice();
#endif
  }
  
  void copyFromDevice() {
#if USE_GPU
    get()->copyToHost();
#endif    
  }

  CLIntWrapper * get();
  
private:
  CLIntWrapperEx * _data;
  vector<int> a;
};

class FloatArray : public BaseArray {
  int flags;
public:
  FloatArray(int flags = CL_MEM_READ_WRITE) { _data = NULL; this->flags = flags; }
  void setFlags(int flags) { this->flags = flags; }
  void copyFrom(FloatArray & src);
  size_t size() { return a.size(); }
  float & operator[](size_t i) { return a[i]; }
  float * data() { return a.data(); }
  void appendNegativeOne() { a.push_back(-1); }
  void push_back(float v) { a.push_back(v); }
  void resize(size_t v) { a.resize(v); }
  void set(float v) {
    for (size_t i = 0; i < a.size(); i++) {
      a[i] = v;
    }
  }
  operator CLFloatWrapper *() { return this->get(); }
  operator float *() { return this->data(); }
  
  bool inRange(void * ptr) {
    long start = long(a.data());
    long end = start + a.size() * sizeof(float);
    long lptr = long(ptr);
    return (lptr >= start && lptr < end);
  }

  void copyToDevice() {
#if USE_GPU
    get()->copyToDevice();
#endif    
  }
  
  void copyFromDeviceEx() {
#if USE_GPU
    ((CLFloatWrapperEx*)_data)->copyToHostEx();
#endif    
  }
  
  void copyFromDevice() {
#if USE_GPU
    get()->copyToHost();
#endif    
  }
  
  CLFloatWrapper * get();

private:
  CLFloatWrapper * _data;
  vector<float> a;
};

#endif /* FlexArrays_h */
