LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := MyLibName
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/*.cpp)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/*.cpp)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/**/*.cpp)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/src/**/*.c)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Includes/*.cpp)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Menu/*.cpp))

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/Includes \
                    $(LOCAL_PATH)/Menu \
                    $(LOCAL_PATH)/src

LOCAL_CFLAGS    := -Wno-error -Wno-format-security -Wno-non-pod-varargs -Wno-c++17-extensions
LOCAL_CPPFLAGS  := -Wno-error -Wno-format-security -Wno-non-pod-varargs -Wno-c++17-extensions
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2
include $(BUILD_SHARED_LIBRARY)
