LOCAL_PATH:= $(call my-dir)  #获取当前目录
include $(CLEAR_VARS)		 #清空环境变量
LOCAL_MODULE_TAGS :=optional
LOCAL_C_INCLUDES := $(KERNEL_HEADERS)
LOCAL_LDLIBS := -llog   #引入库

LOCAL_MODULE:= hello		#so库的名称
LOCAL_SRC_FILES:= hello.cpp
LOCAL_PRELINK_MODULE := false
include $(BUILD_SHARED_LIBRARY)
