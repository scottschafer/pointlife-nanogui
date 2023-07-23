#if USE_GPU

  extern easycl::CLKernel * gKernel;

  #define AND
  #define PARAM_SEPARATOR

  #define BEGIN_CALL_FUNCTION(FUNCTION_NAME, COUNT) \
    gKernel = getKernel_##FUNCTION_NAME();

  #define ADD_IN_PARAM(x) \
    gKernel->in(x);

  #define ADD_OUT_PARAM(x) \
    gKernel->in(x);

//    gKernel->out(x);

  #define ADD_INOUT_PARAM(x) \
    gKernel->in(x);

//    gKernel->inout(x);

  #define END_CALL_FUNCTION(GLOBAL_WORKSIZE, WORKGROUP_SIZE) \
    gKernel->run_1d((int)GLOBAL_WORKSIZE, (int)WORKGROUP_SIZE);

  #define SET_LAST_PARAM

#else
  #define AND ,
  #define PARAM_SEPARATOR ,

  #define BEGIN_CALL_FUNCTION(FUNCTION_NAME, COUNT) \
    for (globalId = 0; globalId < COUNT; globalId++) { \
      FUNCTION_NAME(

  #define ADD_IN_PARAM(x) x

  #define ADD_OUT_PARAM(x) x

  #define ADD_INOUT_PARAM(x) x

  #define END_CALL_FUNCTION(GLOBAL_WORKSIZE, WORKGROUP_SIZE) \
    ); }

#endif
