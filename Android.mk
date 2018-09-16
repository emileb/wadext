
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := wadext

# WARNING, needs to be no optimisation otherwise crash on arm64
LOCAL_CFLAGS :=  -O0 -Dstrnicmp=strncasecmp -Dstricmp=strcasecmp

LOCAL_SRC_FILES =  convert.cpp fileformat.cpp wadext.cpp wadfile.cpp jni.cpp

LOCAL_LDLIBS :=  -ldl -llog -lz

include $(BUILD_SHARED_LIBRARY)






