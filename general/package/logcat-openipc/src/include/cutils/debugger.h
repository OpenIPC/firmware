/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __CUTILS_DEBUGGER_H
#define __CUTILS_DEBUGGER_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DEBUGGER32_SOCKET_NAME "android:debuggerd"
#define DEBUGGER64_SOCKET_NAME "android:debuggerd64"

#if defined(__LP64__)
#define DEBUGGER_SOCKET_NAME DEBUGGER64_SOCKET_NAME
#else
#define DEBUGGER_SOCKET_NAME DEBUGGER32_SOCKET_NAME
#endif

typedef enum {
    // dump a crash
    DEBUGGER_ACTION_CRASH,
    // dump a tombstone file
    DEBUGGER_ACTION_DUMP_TOMBSTONE,
    // dump a backtrace only back to the socket
    DEBUGGER_ACTION_DUMP_BACKTRACE,
} debugger_action_t;

typedef struct {
    debugger_action_t action;
    pid_t tid;
    uintptr_t abort_msg_address;
    int32_t original_si_code;
} debugger_msg_t;

#if defined(__LP64__)
// For a 64 bit process to contact the 32 bit debuggerd.
typedef struct {
    debugger_action_t action;
    pid_t tid;
    uint32_t abort_msg_address;
    int32_t original_si_code;
} debugger32_msg_t;
#endif

/* Dumps a process backtrace, registers, and stack to a tombstone file (requires root).
 * Stores the tombstone path in the provided buffer.
 * Returns 0 on success, -1 on error.
 */
int dump_tombstone(pid_t tid, char* pathbuf, size_t pathlen);

/* Dumps a process backtrace only to the specified file (requires root).
 * Returns 0 on success, -1 on error.
 */
int dump_backtrace_to_file(pid_t tid, int fd);

#ifdef __cplusplus
}
#endif

#endif /* __CUTILS_DEBUGGER_H */
