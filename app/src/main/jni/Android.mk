LOCAL_PATH := $(call my-dir)

# 1. Cargar la librería Dobby precompilada que ya viene en la plantilla
include $(CLEAR_VARS)
LOCAL_MODULE    := dobby
LOCAL_SRC_FILES := Include/Dobby/$(TARGET_ARCH_ABI)/libdobby.a
include $(PREBUILT_STATIC_LIBRARY)

# 2. Inyector Zygisk
include $(CLEAR_VARS)
LOCAL_MODULE    := zygisk
LOCAL_SRC_FILES := zygisk.cpp
LOCAL_CPPFLAGS  := -std=c++17 -fexceptions -frtti -Wno-error
LOCAL_LDLIBS    := -llog -ldl
include $(BUILD_SHARED_LIBRARY)

# 3. ESP (Main.cpp + xDL)
include $(CLEAR_VARS)
LOCAL_MODULE    := MyLibName
LOCAL_SRC_FILES := Main.cpp \
                   Includes/Utils.cpp \
                   xDL/xdl.c \
                   xDL/xdl_iterate.c \
                   xDL/xdl_linker.c \
                   xDL/xdl_lzma.c \
                   xDL/xdl_util.c

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/Includes \
                    $(LOCAL_PATH)/xDL \
                    $(LOCAL_PATH)/Include \
                    $(LOCAL_PATH)/Include/Dobby

LOCAL_CPPFLAGS  := -std=c++17 -fexceptions -frtti -Wno-error
LOCAL_CFLAGS    := -Wno-error
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2 -ldl -lm

# Enlazamos la librería de Dobby
LOCAL_STATIC_LIBRARIES := dobby

include $(BUILD_SHARED_LIBRARY)
