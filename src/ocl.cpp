#include "ocl.h"
cl_platform_id ocl_platform = NULL;
cl_device_id ocl_device = NULL;
cl_bool ocl_dev_is_ver12 = CL_FALSE;
cl_context ocl_ctxt = NULL;
cl_command_queue ocl_cmd_q = NULL;
cl_int ocl_err = CL_SUCCESS;

void CL_CALLBACK
pfn_notify(const char *msg, const void *data0, size_t sz, void *data1) {
  fprintf(stderr, "FATAL ERROR: %s\n", msg);
}

void compute_init(void) {
        cprintf(L"$c*`begin$? compute setup\n");
  cl_uint num_platforms = 0;
  ocl_err = clGetPlatformIDs(0, NULL, &num_platforms);
  if (!num_platforms) {
    cprintf(L"$y*FATAL WARNING$?: no opencl platforms found: %d\n", ocl_err);
    return;
  }

  printf("found %d platform%s on system\n", num_platforms,
         (num_platforms > 1 ? "s" : ""));

  cl_platform_id *platforms =
      (cl_platform_id *)malloc(sizeof(cl_platform_id) * num_platforms);
  ocl_err = clGetPlatformIDs(num_platforms, platforms, NULL);
  if (ocl_err) {
    fprintf(stderr, "error: failed to allocate platforms: %d\n", ocl_err);
    exit(0);
  }

  uint32_t platform_idx = 0;
  for (; platform_idx < num_platforms; ++platform_idx) {
    cl_char *platform_name =
        get_info(platforms[platform_idx], CL_PLATFORM_NAME);
    cl_char *platform_vendor =
        get_info(platforms[platform_idx], CL_PLATFORM_VENDOR);
    cprintf(L"platform[ $c*%d%$? ]: $c*%s$? by $c*%s$?\n", (int)platform_idx,
            (const char*)platform_name, (const char*)platform_vendor);
    free(platform_name);
    platform_name = NULL;
    free(platform_vendor);
    platform_vendor = NULL;

    cl_device_id *devices = NULL;

    cl_uint device_count = 0;
    ocl_err = clGetDeviceIDs(platforms[platform_idx], CL_DEVICE_TYPE_ALL, 0,
                             NULL, &device_count);
    if (ocl_err) {
      cprintf<CPF_STDE>(L"$r*FATAL ERROR$?: failed to query device ids: %d\n",
                        ocl_err);
      abort();
    }

    if (!device_count)
      continue;

    devices = (cl_device_id *)malloc(sizeof(cl_device_id) * device_count);
    ocl_err = clGetDeviceIDs(platforms[platform_idx], CL_DEVICE_TYPE_ALL,
                             device_count, devices, NULL);
    if (ocl_err) {
      cprintf<CPF_STDE>(L"$r*FATAL ERROR$?: failed to allocate device: %d\n",
                        ocl_err);
      abort();
    }

    uint32_t device_idx = 0;
    for (; device_idx < device_count; ++device_idx) {
      cl_char *name = get_info(devices[device_idx], CL_DEVICE_NAME);
      cprintf(L"device[$c*%d$?]: %s\n", (int)device_idx, (const char*)name);
      if (!ocl_device) {
        cprintf(L"$m*default device!\n");
        ocl_device = devices[device_idx];
        cl_char *ver = get_info(devices[device_idx], CL_DEVICE_VERSION);
        std::string s = (const char *)ver;
        size_t p = s.find(".");
        if (p != s.npos) {
          const char cp[] = { s[p + 1] };
          if (atoi(cp) > 1)
            ocl_dev_is_ver12 = true;
        }
        free(ver);
        ver = NULL;
      }
      free(name);
      name = NULL;
    }
  }

  if (!ocl_device) {
    cprintf<CPF_STDE>(L"$r*FATAL ERROR$?: no device found!\n");
    abort();
  }

  cl_platform_id *p = get_info<cl_platform_id>(ocl_device, CL_DEVICE_PLATFORM);
  ocl_platform = *p;

  free(p);
  p = NULL;
  free(platforms);
  platforms = NULL;

  cl_context_properties ctxt_props[] = { CL_CONTEXT_PLATFORM,
                                         (cl_context_properties)ocl_platform,
                                         0 };
  ocl_ctxt =
      clCreateContext(ctxt_props, 1, &ocl_device, pfn_notify, NULL, &ocl_err);
  if (ocl_ctxt == NULL || ocl_err) {
    cprintf<CPF_STDE>(L"$r*FATAL ERROR$?: failed to create context: %d\n",
                      ocl_err);
    abort();
  }

  /*
  cl_command_queue_properties q_props[] = {
    CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, CL_QUEUE_PROFILING_ENABLE, 0
  };*/
  ocl_cmd_q = clCreateCommandQueue(ocl_ctxt, ocl_device, 0, &ocl_err);
  if (ocl_cmd_q == NULL || ocl_err) {
    cprintf<CPF_STDE>(L"$r*FATAL ERROR$?: failed to create command queue: %d\n",
                      ocl_err);
    abort();
  }

  cprintf(L"compute setup $g*success$?`!\n");
}

void compute_teardown(void) {
  if (ocl_cmd_q != NULL) {
    ocl_err = clReleaseCommandQueue(ocl_cmd_q);
    if (ocl_err) {
      cprintf<CPF_STDE>(
          L"$r*FATAL ERROR$?: failed to release cmd queue. aborting: %d\n",
          ocl_err);
      abort();
    }
  }

  if (ocl_ctxt != NULL) {
    ocl_err = clReleaseContext(ocl_ctxt);
    if (ocl_err) {
      cprintf<CPF_STDE>(
          L"$r*FATAL ERROR$?: failed to release context. aborting: %d\n",
          ocl_err);
      abort();
    }
  }

  if (ocl_device && ocl_dev_is_ver12) {
    ocl_err = clReleaseDevice(ocl_device);
    if (ocl_err) {
      cprintf<CPF_STDE>(
          L"$r*FATAL ERROR$?: failed to release device. aborting: %d\n",
          ocl_err);
      abort();
    }
  }
}
