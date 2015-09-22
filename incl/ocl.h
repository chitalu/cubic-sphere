#ifndef __OCL_H__
#define __OCL_H__

#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <cprintf/cprintf.hpp>

extern cl_platform_id ocl_platform;
extern cl_device_id ocl_device;
extern cl_bool ocl_dev_is_ver12;
extern cl_context ocl_ctxt;
extern cl_command_queue ocl_cmd_q;
extern cl_int ocl_err;

struct compute_work_t {
  const size_t dims;
  size_t global_sz[3];
  size_t local_sz[3];
  cl_mem device_buf;
  float host_data;
  const uint32_t data_count;
};

#define CL_STATUS_(errmsg)                                                     \
  do {                                                                         \
    if (ocl_err != CL_SUCCESS) {                                           \
      cprintf<CPF_STDE>("abort: $y*%d$? : %s", ocl_err, errmsg);           \
      exit(EXIT_FAILURE);                                                 \
    }                                                                          \
  } while (0);

#define CL_GET_INFO_FUNC(obj_type, obj_info_type, clFunc)                      \
  template <typename T = cl_char>                                              \
  T *get_info(obj_type obj, obj_info_type _what) {                             \
    size_t size;                                                               \
    T *info = nullptr;                                                         \
    ocl_err = clFunc(obj, _what, 0, nullptr, &size);                       \
    CL_STATUS_("#1 failed to get " #obj_type "info");                          \
                                                                               \
    info = (T *)malloc(sizeof(T) * size);                                      \
    ocl_err = clFunc(obj, _what, size, info, 0);                           \
    CL_STATUS_("#2 failed to get " #obj_type "info");                          \
                                                                               \
    return info;                                                               \
  }

CL_GET_INFO_FUNC(cl_device_id, cl_device_info, clGetDeviceInfo);
CL_GET_INFO_FUNC(cl_platform_id, cl_platform_info, clGetPlatformInfo);
CL_GET_INFO_FUNC(cl_mem, cl_mem_info, clGetMemObjectInfo);

extern void compute_init(void);
extern void compute_teardown(void);

#endif
