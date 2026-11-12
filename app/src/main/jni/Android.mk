LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := MyLibName

# MAGIA: Busca archivos .cpp y .c en las carpetas clave
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/*.cpp)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/*.c)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Menu/*.cpp)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Includes/*.cpp)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/KittyMemory/*.cpp)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Dobby/*.c)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/xDL/*.c)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Menu/ImGui/*.cpp)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Menu/ImGui/backends/*.cpp))

# Eliminamos el archivo problemático de Keystone
LOCAL_SRC_FILES := $(filter-out %MemoryPatch.cpp, $(LOCAL_SRC_FILES))

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/Includes \
                    $(LOCAL_PATH)/Menu \
                    $(LOCAL_PATH)/Menu/ImGui \
                    $(LOCAL_PATH)/Menu/ImGui/backends \
                    $(LOCAL_PATH)/KittyMemory \
                    $(LOCAL_PATH)/xDL \
                    $(LOCAL_PATH)/Dobby

# Banderas para que C++ moderno no llore por el código viejo
LOCAL_CPPFLAGS  := -std=c++17 -fexceptions -frtti -DIMGUI_DEFINE_MATH_OPERATORS -DIMGUI_IMPL_OPENGL_ES2 -Wno-error -Wno-format-security -Wno-non-pod-varargs
LOCAL_CFLAGS    := -Wno-error -Wno-format-security -Wno-non-pod-varargs
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2 -ldl -latomic -lm

include $(BUILD_SHARED_LIBRARY)
