defineReplace(cudaInstalled){
    CUDATEXT = $$system(nvcc --version)
    contains(CUDATEXT, Cuda ){
	return(true)
    } else {
	return(false)
    }
}

defineTest(print) {
  !build_pass:message($$1)
}

CUDA_INSTALLED = $$cudaInstalled()
CUDA_INSTALLED = false

if($$CUDA_INSTALLED) {

win32 {
	SYSTEM_TYPE = 64            # '32' or '64', depending on your system
	CUDA_ARCH = sm_50           # Type of CUDA architecture, for example 'compute_10', 'compute_11', 'sm_10'
	NVCC_OPTIONS = --use_fast_math -Xcompiler
	SYSTEM_NAME = Win64         # Depending on your system either 'Win32', 'x64', or 'Win64'
	NVCC_OPTIONS += -MD
	CUDA_SDK = $$(CUDA_PATH)
	#QMAKE_LIBDIR += $$CUDA_SDK/lib/x64
	INCLUDEPATH += $$CUDA_SDK/include
	message($$INCLUDEPATH)
	LIBS += $$CUDA_SDK/lib/x64/cuda.lib \
	$$CUDA_SDK/lib/x64/cudart.lib
	#QMAKE_LFLAGS_RELEASE += /FORCE:MULTIPLE
}

unix:!macx {
	SYSTEM_TYPE = 64            # '32' or '64', depending on your system
	CUDA_ARCH = sm_50            # Type of CUDA architecture, for example 'compute_10', 'compute_11', 'sm_10'
	NVCC_OPTIONS = --use_fast_math -Xcompiler
	SYSTEM_NAME = unix         # Depending on your system either 'Win32', 'x64', or 'Win64'

	NVCC_OPTIONS += -fpic

	QMAKE_LIBDIR += /usr/lib/x86_64-linux-gnu \
					/usr/lib/nvidia-319
	LIBS += $$CUDA_LIBS
}
macx {
	SYSTEM_TYPE = 64
	CUDA_ARCH = sm_50           # Type of CUDA architecture, for example 'compute_10', 'compute_11', 'sm_10'
	SYSTEM_NAME = unix         # Depending on your system either 'Win32', 'x64', or 'Win64'

	#NVCC_OPTIONS += -fpic

	QMAKE_LIBDIR += /usr/lib/x86_64-linux-gnu \
					/usr/lib/nvidia-319
	LIBS += $$CUDA_LIBS
}
}

#message(Libraries: $$QMAKE_LIBS_WINDOWS)


#message($$PWD)
#message(Cuda includes: $$CUDA_INC)
