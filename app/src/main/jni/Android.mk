LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := MyLibName
LOCAL_SRC_FILES := src/Main.cpp
include $(BUILD_SHARED_LIBRARY)
