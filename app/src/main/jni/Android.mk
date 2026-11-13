LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := zygisk
LOCAL_SRC_FILES := zygisk.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_CPPFLAGS  := -std=c++17 -fexceptions -frtti -Wno-error
LOCAL_LDLIBS    := -llog -ldl
include $(BUILD_SHARED_LIBRARY)
