LOCAL_PATH := $(call my-dir)

#Library
include $(CLEAR_VARS)
LOCAL_MODULE := libACE
LOCAL_SRC_FILES := /home/skwak/ACE_wrappers/lib/libACE.so
include $(PREBUILT_SHARED_LIBRARY)

#Library
include $(CLEAR_VARS)
LOCAL_MODULE := libevt
LOCAL_SRC_FILES := /home/skwak/x1/lib/libs/armeabi/libevt.so
include $(PREBUILT_SHARED_LIBRARY)

#Executable
include $(CLEAR_VARS)

LOCAL_MODULE := eSvr 
LOCAL_SRC_FILES := server.cpp \
                   CStreamHandler.cpp \
                   CSender.cpp
LOCAL_LDFLAGS:=-fPIE -pie
LOCAL_SHARED_LIBRARIES := libACE \
                          libevt
LOCAL_LDLIBS += -lz
LOCAL_C_INCLUDES += /home/skwak/ACE_wrappers \
                    /home/skwak/x1/lib/jni
LOCAL_CFLAGS := -DACE_NTRACE=1

include $(BUILD_EXECUTABLE)
