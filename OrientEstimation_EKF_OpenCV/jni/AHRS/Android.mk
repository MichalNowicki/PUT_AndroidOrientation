LOCAL_PATH := $(call my-dir)

# Extended Kalman Filter library
include $(CLEAR_VARS)
include C:/DiamentowyGrant/OpenCV-2.4.10-android-sdk/sdk/native/jni/OpenCV.mk

LOCAL_MODULE    := AHRSModule
LOCAL_SRC_FILES := EKF/EKF.cpp AHRSModuleExport.cpp 
LOCAL_LDLIBS +=  -llog -ldl

include $(BUILD_SHARED_LIBRARY)