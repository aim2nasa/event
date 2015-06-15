LOCAL_PATH := $(call my-dir)

#Library
include $(CLEAR_VARS)
LOCAL_MODULE := libevt
LOCAL_SRC_FILES := CEvtRec.cpp 
include $(BUILD_SHARED_LIBRARY)
