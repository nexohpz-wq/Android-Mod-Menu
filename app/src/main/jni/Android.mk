LOCAL_PATH := $(call my-dir)

# 1. Inyector Zygisk
include $(CLEAR_VARS)
LOCAL_MODULE    := zygisk
LOCAL_SRC_FILES := zygisk.cpp
LOCAL_CPPFLAGS  := -std=c++17 -fexceptions -frtti -Wno-error
LOCAL_LDLIBS    := -llog -ldl
include $(BUILD_SHARED_LIBRARY)

# 2. ESP (Main.cpp)
include $(CLEAR_VARS)
LOCAL_MODULE    := MyLibName
LOCAL_SRC_FILES := Main.cpp \
                   Includes/Utils.cpp \
                   xDL/xdl.c \
                   xDL/xdl_iterate.c \
                   xDL/xdl_linker.c \
                   xDL/xdl_lzma.c \
                   xDL/xdl_util.c \
                   $(call all-c-files-under, Dobby/source) \
                   $(call all-cpp-files-under, Dobby/source)

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/Includes \
                    $(LOCAL_PATH)/xDL \
                    $(LOCAL_PATH)/Dobby \
                    $(LOCAL_PATH)/Dobby/include

LOCAL_CPPFLAGS  := -std=c++17 -fexceptions -frtti -Wno-error
LOCAL_CFLAGS    := -Wno-error
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2 -ldl -lm
include $(BUILD_SHARED_LIBRARY)
