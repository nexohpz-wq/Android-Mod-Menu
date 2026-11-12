LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := MyLibName

# Compila Main.cpp y busca DENTRO de la carpeta Dobby recursivamente
LOCAL_SRC_FILES := Main.cpp \
                   $(subst $(LOCAL_PATH)/,,$(call all-c-files-under, Dobby)) \
                   $(subst $(LOCAL_PATH)/,,$(call all-cpp-files-under, Dobby))

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/Dobby

LOCAL_CPPFLAGS  := -std=c++17 -fexceptions -frtti -Wno-error
LOCAL_CFLAGS    := -Wno-error
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2 -ldl -lm

include $(BUILD_SHARED_LIBRARY)
