LOCAL_PATH := $(call my-dir)

#Library
include $(CLEAR_VARS)
LOCAL_MODULE := liber
LOCAL_SRC_FILES := CEvtRec.cpp 
include $(BUILD_SHARED_LIBRARY)

#Executable
include $(CLEAR_VARS)

LOCAL_MODULE := ge
LOCAL_SRC_FILES := main.cpp
LOCAL_MODULE_PATH := $(TARGET_OUT_EXECUTABLES)
LOCAL_C_INCLUDES += /home/skwak/android-ndk-r8e/platforms/android-14/arch-arm/usr/include \
                    /home/skwak/x1/ge/jni
LOCAL_SHARED_LIBRARIES := liber

include $(BUILD_EXECUTABLE)
