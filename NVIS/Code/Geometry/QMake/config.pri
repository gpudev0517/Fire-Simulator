if($$CUDA_INSTALLED) {
    DEFINES += CUDA_AVAILABLE
    LIBS += -lGVDB
}
