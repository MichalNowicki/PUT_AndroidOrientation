LOCAL_PATH := $(call my-dir)

# Extended Kalman Filter library
include $(CLEAR_VARS)

LOCAL_MODULE    := AHRSModule
LOCAL_SRC_FILES := EKF/EKF.cpp AHRSModuleExport.cpp 
LOCAL_LDLIBS +=  -llog -ldl

include $(BUILD_SHARED_LIBRARY)