LOCAL_PATH := $(call my-dir)

# 1. Inyector Zygisk
include $(CLEAR_VARS)
LOCAL_MODULE    := zygisk
LOCAL_SRC_FILES := zygisk.cpp
LOCAL_CPPFLAGS  := -std=c++17 -fexceptions -frtti -Wno-error
LOCAL_LDLIBS    := -llog -ldl
include $(BUILD_SHARED_LIBRARY)

# 2. ESP (Main.cpp + Dobby + xDL)
include $(CLEAR_VARS)
LOCAL_MODULE    := MyLibName

# Lista exacta de los archivos de Dobby para ARM64
LOCAL_SRC_FILES := Main.cpp \
                   Includes/Utils.cpp \
                   xDL/xdl.c \
                   xDL/xdl_iterate.c \
                   xDL/xdl_linker.c \
                   xDL/xdl_lzma.c \
                   xDL/xdl_util.c \
                   Dobby/source/Backend/UserMode/UnifiedInterface/platform-posix.cc \
                   Dobby/source/Backend/UserMode/PlatformUtil/ProcessRuntimeUtility.cc \
                   Dobby/source/Backend/UserMode/Thread/PlatformThread.cc \
                   Dobby/source/Backend/UserMode/Memory/CodePatchTool.cc \
                   Dobby/source/Backend/UserMode/Memory/ExecMemoryManager.cc \
                   Dobby/source/Core/Assembler/assembler.cc \
                   Dobby/source/Core/Assembler/assembler-arm64.cc \
                   Dobby/source/Core/Codegen/codegen-arm64.cc \
                   Dobby/source/Core/Interceptor.cc \
                   Dobby/source/Core/InterceptEntry.cc \
                   Dobby/source/Core/dobby.cpp \
                   Dobby/source/InstructionRelocation/arm64/InstructionRelocationARM64.cc \
                   Dobby/source/MemoryAllocator/AssemblyCodeBuilder.cc \
                   Dobby/source/MemoryAllocator/CodeBuffer/CodeBufferBase.cc \
                   Dobby/source/MemoryAllocator/MemoryArena.cc \
                   Dobby/source/TrampolineBridge/ClosureTrampolineBridge/closure-bridge-arm64.cc \
                   Dobby/source/TrampolineBridge/Trampoline/arm64/trampoline-arm64.cc \
                   Dobby/source/InterceptRouting/RoutingPlugin.cc \
                   Dobby/source/InterceptRouting/Routing/FunctionInlineReplace/FunctionInlineReplaceExport.cc

# LE DECIMOS AL COMPILADOR DÓNDE ESTÁN LAS CABECERAS (Aquí estaba el error)
LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/Includes \
                    $(LOCAL_PATH)/xDL \
                    $(LOCAL_PATH)/Dobby \
                    $(LOCAL_PATH)/Dobby/include \
                    $(LOCAL_PATH)/Dobby/source \
                    $(LOCAL_PATH)/Dobby/source/Platform \
                    $(LOCAL_PATH)/Dobby/source/Platform/posix \
                    $(LOCAL_PATH)/Dobby/source/Platform/android

LOCAL_CPPFLAGS  := -std=c++17 -fexceptions -frtti -Wno-error
LOCAL_CFLAGS    := -Wno-error
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2 -ldl -lm

include $(BUILD_SHARED_LIBRARY)
