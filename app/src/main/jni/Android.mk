LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := MyLibName
LOCAL_SRC_FILES := Main.cpp \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/*.cpp)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Includes/*.cpp))
LOCAL_CPPFLAGS  := -std=c++17 -static-libstdc++
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2
include $(BUILD_SHARED_LIBRARY)
