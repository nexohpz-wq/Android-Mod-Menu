LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := MyLibName

LOCAL_SRC_FILES := $(patsubst $(LOCAL_PATH)/%,%,$(shell find $(LOCAL_PATH) -type f \( -name "*.cpp" -o -name "*.c" \)))
LOCAL_SRC_FILES := $(filter-out %MemoryPatch.cpp, $(LOCAL_SRC_FILES))

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/Includes \
                    $(LOCAL_PATH)/Menu \
                    $(LOCAL_PATH)/Menu/ImGui \
                    $(LOCAL_PATH)/KittyMemory \
                    $(LOCAL_PATH)/xDL \
                    $(LOCAL_PATH)/Dobby

LOCAL_CPPFLAGS  := -std=c++17 -fexceptions -frtti -Wno-error -Wno-format-security -Wno-non-pod-varargs
LOCAL_CFLAGS    := -Wno-error -Wno-format-security -Wno-non-pod-varargs
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2 -ldl -lm

include $(BUILD_SHARED_LIBRARY)
