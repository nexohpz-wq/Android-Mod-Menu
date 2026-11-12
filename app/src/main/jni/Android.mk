LOCAL_PATH := $(call my-dir)

# 1. Usamos la librería estática precompilada que ya está en tu repositorio
include $(CLEAR_VARS)
LOCAL_MODULE    := dobby
LOCAL_SRC_FILES := Dobby/$(TARGET_ARCH_ABI)/libdobby.a
include $(PREBUILT_STATIC_LIBRARY)

# 2. Compilamos nuestro módulo principal
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
                    $(LOCAL_PATH)/Dobby

LOCAL_CPPFLAGS  := -std=c++17 -fexceptions -frtti -Wno-error
LOCAL_CFLAGS    := -Wno-error
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2 -ldl -lm

# 3. Enlazamos la librería de Dobby
LOCAL_STATIC_LIBRARIES := dobby

include $(BUILD_SHARED_LIBRARY)
