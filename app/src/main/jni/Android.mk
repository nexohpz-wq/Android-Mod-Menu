LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := MyLibName

# Busca recursivamente TODOS los .cpp y .c en todas las carpetas (src, Menu, Includes, Dobby, etc)
LOCAL_SRC_FILES := $(patsubst $(LOCAL_PATH)/%,%,$(wildcard $(LOCAL_PATH)/*.cpp) $(wildcard $(LOCAL_PATH)/*.c) $(wildcard $(LOCAL_PATH)/*/*.cpp) $(wildcard $(LOCAL_PATH)/*/*.c) $(wildcard $(LOCAL_PATH)/*/*/*.cpp) $(wildcard $(LOCAL_PATH)/*/*/*.c))

# Le dice al compilador dónde están los archivos .h y .hpp
LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/Includes \
                    $(LOCAL_PATH)/Menu \
                    $(LOCAL_PATH)/Menu/ImGui \
                    $(LOCAL_PATH)/Dobby \
                    $(LOCAL_PATH)/KittyMemory \
                    $(LOCAL_PATH)/xDL

LOCAL_CFLAGS    := -Wno-error -Wno-format-security -Wno-non-pod-varargs
LOCAL_CPPFLAGS  := -Wno-error -Wno-format-security -Wno-non-pod-varargs
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2

include $(BUILD_SHARED_LIBRARY)
