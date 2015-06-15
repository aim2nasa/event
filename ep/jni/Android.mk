LOCAL_PATH := $(call my-dir)

#Executable
include $(CLEAR_VARS)

LOCAL_MODULE := ep
LOCAL_SRC_FILES := main.cpp \
                   CEvtPlay.cpp
LOCAL_MODULE_PATH := $(TARGET_OUT_EXECUTABLES)

include $(BUILD_EXECUTABLE)
