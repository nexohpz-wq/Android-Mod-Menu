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
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Dobby/source/*.cc)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Dobby/source/Backend/UserMode/**/*.cc)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Dobby/source/Core/**/*.cc)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Dobby/source/InstructionRelocation/**/*.cc)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Dobby/source/MemoryAllocator/**/*.cc)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Dobby/source/TrampolineBridge/**/*.cc)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/ImGui/*.cpp)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/ImGui/backends/imgui_impl_opengl3.cpp))

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/Includes \
                    $(LOCAL_PATH)/xDL \
                    $(LOCAL_PATH)/Dobby \
                    $(LOCAL_PATH)/Dobby/include \
                    $(LOCAL_PATH)/ImGui \
                    $(LOCAL_PATH)/ImGui/backends

LOCAL_CPPFLAGS  := -std=c++17 -fexceptions -frtti -Wno-error
LOCAL_CFLAGS    := -Wno-error
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2 -ldl -lm

include $(BUILD_SHARED_LIBRARY)
