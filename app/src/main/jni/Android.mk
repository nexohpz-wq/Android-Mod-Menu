LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := MyLibName

# Quitamos MemoryPatch.cpp para que no pida la librería Keystone
LOCAL_SRC_FILES := Main.cpp \
                   Menu/Menu.cpp \
                   Menu/Setup.cpp \
                   Menu/Jni.cpp \
                   Includes/Utils.cpp \
                   KittyMemory/KittyMemory.cpp \
                   KittyMemory/KittyUtils.cpp \
                   xDL/xdl.c \
                   Dobby/dobby.c

# Le decimos al compilador dónde están las cabeceras (.h y .hpp)
LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/Includes \
                    $(LOCAL_PATH)/Menu \
                    $(LOCAL_PATH)/KittyMemory \
                    $(LOCAL_PATH)/xDL \
                    $(LOCAL_PATH)/Dobby

LOCAL_CFLAGS    := -Wno-error -Wno-format-security -Wno-non-pod-varargs
LOCAL_CPPFLAGS  := -Wno-error -Wno-format-security -Wno-non-pod-varargs
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2

include $(BUILD_SHARED_LIBRARY)
