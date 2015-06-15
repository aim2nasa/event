LOCAL_PATH := $(call my-dir)

#Library
include $(CLEAR_VARS)
LOCAL_MODULE := libevt
LOCAL_SRC_FILES := /home/skwak/x1/lib/libs/armeabi/libevt.so
include $(PREBUILT_SHARED_LIBRARY)

#Executable
include $(CLEAR_VARS)

LOCAL_MODULE := ep
LOCAL_SRC_FILES := main.cpp
LOCAL_MODULE_PATH := $(TARGET_OUT_EXECUTABLES)
LOCAL_C_INCLUDES += /home/skwak/x1/lib/jni
LOCAL_SHARED_LIBRARIES := libevt

include $(BUILD_EXECUTABLE)
