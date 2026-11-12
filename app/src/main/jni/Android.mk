LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := MyLibName

# Compilamos Main.cpp y buscamos TODOS los archivos .c y .cpp dentro de la carpeta Dobby
LOCAL_SRC_FILES := Main.cpp \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Dobby/*.c)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Dobby/*.cpp)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Dobby/*/*.c)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Dobby/*/*.cpp))

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/Dobby

LOCAL_CPPFLAGS  := -std=c++17 -fexceptions -frtti -Wno-error
LOCAL_CFLAGS    := -Wno-error
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2 -ldl -lm

include $(BUILD_SHARED_LIBRARY)
