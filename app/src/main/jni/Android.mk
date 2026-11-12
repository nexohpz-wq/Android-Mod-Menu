LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := MyLibName

# Busca Main.cpp y usa el comando 'find' para buscar TODOS los .c y .cpp dentro de Dobby
LOCAL_SRC_FILES := Main.cpp \
                   $(subst $(LOCAL_PATH)/,,$(shell find $(LOCAL_PATH)/Dobby -name "*.c" -o -name "*.cpp"))

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/Dobby \
                    $(LOCAL_PATH)/Dobby/include

LOCAL_CPPFLAGS  := -std=c++17 -fexceptions -frtti -Wno-error
LOCAL_CFLAGS    := -Wno-error
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2 -ldl -lm

include $(BUILD_SHARED_LIBRARY)
