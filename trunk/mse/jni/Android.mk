LOCAL_PATH := $(call my-dir)

#Executable
include $(CLEAR_VARS)

LOCAL_MODULE := mse
LOCAL_SRC_FILES := main.cpp
LOCAL_LDFLAGS:=-fPIE -pie

include $(BUILD_EXECUTABLE)
