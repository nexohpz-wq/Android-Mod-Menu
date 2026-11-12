LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := MyLibName

LOCAL_SRC_FILES := $(patsubst $(LOCAL_PATH)/%,%,$(wildcard $(LOCAL_PATH)/*.cpp) $(wildcard $(LOCAL_PATH)/*.c) $(wildcard $(LOCAL_PATH)/*/*.cpp) $(wildcard $(LOCAL_PATH)/*/*.c) $(wildcard $(LOCAL_PATH)/*/*/*.cpp) $(wildcard $(LOCAL_PATH)/*/*/*.c))

LOCAL_C_INCLUDES := $(LOCAL_PATH)

LOCAL_CFLAGS    := -Wno-error -Wno-format-security -Wno-non-pod-varargs
LOCAL_CPPFLAGS  := -Wno-error -Wno-format-security -Wno-non-pod-varargs
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2

include $(BUILD_SHARED_LIBRARY)
