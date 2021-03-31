//
// Created by blank on 2021/3/22.
//

#ifndef NDKTEST_COMMON_H
#define NDKTEST_COMMON_H

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <android/log.h>
#include <malloc.h>

#include <jni.h>


#ifndef NDEBUG
#define G_DBG
#else
#endif




#ifdef G_DBG
#define LOG_TAG "sdklog-jni"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#define LOGV(...)
#define LOGD(...)
#define LOGI(...)
#define LOGW(...)
#define LOGE(...)
#endif

#endif //NDKTEST_COMMON_H
