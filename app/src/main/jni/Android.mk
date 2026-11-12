LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := MyLibName
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/*.cpp)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/*.cpp)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/**/*.cpp)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/**/*.c))
LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv2
include $(BUILD_SHARED_LIBRARY)
