//
// Created by Wang Haibing on 3/26/21.
//

#if !defined __STDC_VERSION__ || __STDC_VERSION__ < 199901L
#error "Please use a C99 compliant toolchain."
#endif

#pragma once
#ifndef GIMKIT_GKLOG_H
#define GIMKIT_GKLOG_H

#include <stddef.h>
#include <stdarg.h>

#define GKLOG_LEVEL_VERBOSE 1
#define GKLOG_LEVEL_DEBUG 2
#define GKLOG_LEVEL_INFO 3
#define GKLOG_LEVEL_WARN 4
#define GKLOG_LEVEL_ERROR 5
#define GKLOG_LEVEL_FATAL 6
#define GKLOG_LEVEL_SILENT 7

#ifndef GKLOG_TAG
#define GKLOG_TAG __FILE__
#endif

#ifndef GKLOG_LEVEL
#define GKLOG_LEVEL GKLOG_LEVEL_WARN
#endif

#define GKLOG_NOOP (void) 0

static inline const char *wsi_basename(const char *pathName, size_t bytes) {
    const char *file = pathName;
    file += bytes;

    // bytes = strlen(pathName) + 1, move one byte back to point to the '\0x00'
    // If we don't do this '-1', file MAY point to arbitrary character, and in
    // case this character is '/' or '\\', the while loop will do nothing, so file
    // MAY NOT be a null-terminated C string and weired things happen, e.g., stack
    // overflow.
    // Found this when using this code in Emscripten, firefox hangs when there's no
    // this '-1' op.
    file--; 
    while (*file != '/' && *file != '\\' && file > pathName) file--;
    if (file > pathName) file++;
    return file;
}

#if defined (__APPLE__)
#include <os/log.h>
#define GKLOG_TYPE_VERBOSE  OS_LOG_TYPE_DEFAULT
#define GKLOG_TYPE_DEBUG    OS_LOG_TYPE_INFO
#define GKLOG_TYPE_INFO     OS_LOG_TYPE_DEBUG
#define GKLOG_TYPE_WARN     OS_LOG_TYPE_DEBUG
#define GKLOG_TYPE_ERROR    OS_LOG_TYPE_ERROR
#define GKLOG_TYPE_FATAL    OS_LOG_TYPE_FAULT
#define LOG_PRINT(level, fmt, ...) \
    os_log_with_type(OS_LOG_DEFAULT, level, "(%s:%u): " fmt, wsi_basename(__FILE__, sizeof(__FILE__)), __LINE__, ##__VA_ARGS__)
#elif defined(ANDROID) || defined(__ANDROID__)
#include <android/log.h>
#define GKLOG_TYPE_VERBOSE  ANDROID_LOG_VERBOSE
#define GKLOG_TYPE_DEBUG    ANDROID_LOG_DEBUG
#define GKLOG_TYPE_INFO     ANDROID_LOG_INFO
#define GKLOG_TYPE_WARN     ANDROID_LOG_WARN
#define GKLOG_TYPE_ERROR    ANDROID_LOG_ERROR
#define GKLOG_TYPE_FATAL    ANDROID_LOG_FATAL
#define LOG_PRINT(level,fmt,...) \
    __android_log_print(level, GKLOG_TAG, "(%s:%04u): " fmt, wsi_basename(__FILE__, sizeof(__FILE__)), __LINE__, ##__VA_ARGS__)
#elif defined (__EMSCRIPTEN__)
#include "JsLog.h"
#define GKLOG_TYPE_VERBOSE  JS_LOG_VERBOSE
#define GKLOG_TYPE_DEBUG    JS_LOG_DEBUG
#define GKLOG_TYPE_INFO     JS_LOG_INFO
#define GKLOG_TYPE_WARN     JS_LOG_WARN
#define GKLOG_TYPE_ERROR    JS_LOG_ERROR
#define GKLOG_TYPE_FATAL    JS_LOG_FATAL
#define LOG_PRINT(level,fmt,...) \
    js_log_with_level(level, GKLOG_TAG "(%s:%04u): " fmt, wsi_basename(__FILE__, sizeof(__FILE__)), __LINE__, ##__VA_ARGS__)
#else
#warn "Unknown platform, just use fprintf."
#define GKLOG_TYPE_VERBOSE  0
#define GKLOG_TYPE_DEBUG    0
#define GKLOG_TYPE_INFO     0
#define GKLOG_TYPE_WARN     0
#define GKLOG_TYPE_ERROR    0
#define GKLOG_TYPE_FATAL    0
#include <stdio.h>
#define LOG_PRINT(level, fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#endif

#if GKLOG_LEVEL_VERBOSE >= GKLOG_LEVEL
    #define LOGV(fmt,...) LOG_PRINT(GKLOG_TYPE_VERBOSE, fmt, ##__VA_ARGS__)
#else
    #define LOGV(...) GKLOG_NOOP
#endif

#if GKLOG_LEVEL_DEBUG >= GKLOG_LEVEL
    #define LOGD(fmt,...) LOG_PRINT(GKLOG_TYPE_DEBUG, fmt, ##__VA_ARGS__)
#else
    #define LOGD(...) GKLOG_NOOP
#endif

#if GKLOG_LEVEL_INFO >= GKLOG_LEVEL
    #define LOGI(fmt,...) LOG_PRINT(GKLOG_TYPE_INFO, fmt, ##__VA_ARGS__)
#else
    #define LOGI(...) GKLOG_NOOP
#endif

#if GKLOG_LEVEL_WARN >= GKLOG_LEVEL
    #define LOGW(fmt,...) LOG_PRINT(GKLOG_TYPE_WARN, fmt, ##__VA_ARGS__)
#else
    #define LOGW(...) GKLOG_NOOP
#endif

#if GKLOG_LEVEL_ERROR >= GKLOG_LEVEL
    #define LOGE(fmt,...) LOG_PRINT(GKLOG_TYPE_ERROR, fmt, ##__VA_ARGS__)
#else
    #define LOGE(...) GKLOG_NOOP
#endif

#if GKLOG_LEVEL_FATAL >= GKLOG_LEVEL
    #define LOGF(fmt,...) LOG_PRINT(GKLOG_TYPE_FATAL, fmt, ##__VA_ARGS__)
#else
    #define LOGF(...) GKLOG_NOOP
#endif

#if GKLOG_LEVEL_FATAL >= GKLOG_LEVEL
    #define SOFT_ASSERT(expression, fmt, ...) \
        (void)(!(expression) ? LOG_PRINT(GKLOG_TYPE_FATAL, #expression fmt, ##__VA_ARGS__) : GKLOG_NOOP)

    #define SOFT_ASSERT_EQUAL(e1, e2) \
        (void)((p1 != p2) ? LOG_PRINT(GKLOG_TYPE_FATAL, "Assertion failed, "#e1"(0x%X) != "#e2"(0x%x)", e1, e2) : GKLOG_NOOP)

    #define SOFT_ASSERT_NOT_EQUAL(e1, e2) (void)((e1 == e2) ? LOG_PRINT(GKLOG_TYPE_FATAL, "Assertion failed, "#e1"(0x%X) == "#e2"(0x%x)", e1, e2) : GKLOG_NOOP)

    #define SOFT_ASSERT_NOT_NULL(e) (void)((!((void*)(e))) ? LOG_PRINT(GKLOG_TYPE_FATAL, "Assertion failed, "#e"(%p) is NULL", e) : GKLOG_NOOP)

    #define EXPECT_NOT_NULL(e, r)                                                                       \
        do {                                                                                            \
            if (!e) {                                                                                   \
                LOG_PRINT(GKLOG_TYPE_FATAL, "Expect "#e" is NOT NULL, failed with: %p. Abort!", e);   \
                return (r);                                                                             \
            }                                                                                           \
        } while(0)

    #define EXPECT_GREATER_THAN(e1, e2, r)                                                              \
        do {                                                                                            \
            if ((e2) >= (e1)) {                                                                          \
                LOG_PRINT(GKLOG_TYPE_FATAL, "Expect "#e1"(%zu) > "#e2"(%zu), failed. Abort!", e1, e2);    \
                return (r);                                                                             \
            }                                                                                           \
        } while(0)

#else
    #define SOFT_ASSERT(...) GKLOG_NOOP
    #define SOFT_ASSERT_EQUAL(...) GKLOG_NOOP
    #define SOFT_ASSERT_NOT_EQUAL(...) GKLOG_NOOP
    #define SOFT_ASSERT_NOT_NULL(...) GKLOG_NOOP
    #define EXPECT_NOT_NULL(...) GKLOG_NOOP
    #define EXPECT_GREATER_THAN(...) GKLOG_NOOP
#endif

#if GKLOG_LEVEL_SILENT >= GKLOG_LEVEL
/* Nothing To Do Here */
#endif

#endif //GIMKIT_GKLOG_H
