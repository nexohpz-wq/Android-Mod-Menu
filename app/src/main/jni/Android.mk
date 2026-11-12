LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := MyLibName

# Solo compilamos nuestro Main, Utils, xDL y Dobby. Ignoramos el menú.
LOCAL_SRC_FILES := Main.cpp \
                   Includes/Utils.cpp

LOCAL_SRC_FILES += $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/xDL/*.c))

# Buscamos Dobby en todas sus subcarpetas posibles
LOCAL_SRC_FILES += $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Dobby/*.c)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Dobby/*.cpp)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Dobby/source/*.c)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Dobby/source/*.cpp)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Dobby/builtin-plugin/*.c)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Dobby/builtin-plugin/*.cpp))

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/Includes \
                    $(LOCAL_PATH)/xDL \
                    $(LOCAL_PATH)/Dobby \
                    $(LOCAL_PATH)/Dobby/include

LOCAL_CPPFLAGS  := -std=c++17 -fexceptions -frtti -Wno-error
LOCAL_CFLAGS    := -Wno-error
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2 -ldl -lm

include $(BUILD_SHARED_LIBRARY)
