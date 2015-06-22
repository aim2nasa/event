LOCAL_PATH := $(call my-dir)

#Library
include $(CLEAR_VARS)
LOCAL_MODULE := libACE
LOCAL_SRC_FILES := /home/skwak/ACE_wrappers/lib/libACE.so
include $(PREBUILT_SHARED_LIBRARY)

#Library
include $(CLEAR_VARS)
LOCAL_MODULE := libclasf
LOCAL_SRC_FILES := CClassifier.cpp
LOCAL_SHARED_LIBRARIES := libACE
LOCAL_C_INCLUDES += /home/skwak/ACE_wrappers
LOCAL_CFLAGS := -DDLLEXPORT= 
include $(BUILD_SHARED_LIBRARY)
