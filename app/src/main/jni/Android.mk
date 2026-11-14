LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := MyLibName

LOCAL_SRC_FILES := Main.cpp \
                   Includes/Utils.cpp \
                   xDL/xdl.c \
                   xDL/xdl_iterate.c \
                   xDL/xdl_linker.c \
                   xDL/xdl_lzma.c \
                   xDL/xdl_util.c \
                   $(call all-cpp-files-under, Dobby/source) \
                   $(call all-c-files-under, Dobby/source) \
                   $(call all-cpp-files-under, ImGui) \
                   $(call all-cpp-files-under, Menu/ImGui)

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/Includes \
                    $(LOCAL_PATH)/xDL \
                    $(LOCAL_PATH)/Dobby \
                    $(LOCAL_PATH)/Dobby/include \
                    $(LOCAL_PATH)/ImGui \
                    $(LOCAL_PATH)/ImGui/backends \
                    $(LOCAL_PATH)/Menu/ImGui \
                    $(LOCAL_PATH)/Menu/ImGui/backends

LOCAL_CPPFLAGS  := -std=c++17 -fexceptions -frtti -Wno-error -DIMGUI_DEFINE_MATH_OPERATORS -DIMGUI_IMPL_OPENGL_ES2
LOCAL_CFLAGS    := -Wno-error
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2 -ldl -lm

include $(BUILD_SHARED_LIBRARY)
