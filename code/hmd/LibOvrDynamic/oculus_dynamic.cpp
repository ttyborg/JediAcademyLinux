// Uses SDL for library loading. If you're not using SDL, you can
// replace SDL_LoadObject with dlopen/LoadLibrary and SDL_LoadFunction
// with dlsym/GetProcAddress

#include <SDL.h>
#include "oculus_dynamic.h"
#include <stdlib.h>

void *oculus_library_handle;

#define OVRFUNC(need, rtype, fn, params)        \
rtype(*d_ ## fn) params;
#include "ovr_dynamic_funcs.h"
#undef OVRFUNC

extern ovr_dynamic_load_result oculus_dynamic_load(const char** failed_function) {
    const char* liboculus = getenv("LIBOVR");
    if (!liboculus) {
#ifdef OVR_OS_WIN32
        liboculus = "libovr.dll";
#else
        liboculus = "./libovr.so";
#endif
    }

    oculus_library_handle = SDL_LoadObject(liboculus);
    if (!oculus_library_handle) {
        return OVR_DYNAMIC_RESULT_LIBOVR_COULD_NOT_LOAD;
    }

#define OVRFUNC(need, r, f, p)                                          \
	d_##f = (pfn_##f)SDL_LoadFunction(oculus_library_handle, #f);       \
    if (need && !d_##f) {                                               \
        if (failed_function)                                            \
            *failed_function = #f;                                      \
        return OVR_DYNAMIC_RESULT_LIBOVR_COULD_NOT_LOAD_FUNCTION;       \
    }
#include "ovr_dynamic_funcs.h"
#undef OVRFUNC
    
    return OVR_DYNAMIC_RESULT_SUCCESS;
}
