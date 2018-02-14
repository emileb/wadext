
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := wadext

LOCAL_CFLAGS :=  -O2 -Dstrnicmp=strncasecmp -Dstricmp=strcasecmp

LOCAL_SRC_FILES =  convert.cpp fileformat.cpp wadext.cpp wadfile.cpp jni.cpp

LOCAL_LDLIBS :=  -ldl -llog -lz

include $(BUILD_SHARED_LIBRARY)






