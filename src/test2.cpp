/*
 * Copyright © 2013 Tuomas Jormola <tj@solitudo.net> <http://solitudo.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * <http://www.apache.org/licenses/LICENSE-2.0>
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Short intro about this program:
 *
 *  `rpi-camera-dump-yuv` records video using the RaspiCam module and dumps the raw
 * YUV planar 4:2:0 ([I420](http://www.fourcc.org/yuv.php#IYUV)) data to `stdout`.
 *
 *     $ ./rpi-camera-dump-yuv >test.yuv
 *
 * `rpi-camera-dump-yuv` uses `camera` and `null_sink` components. Uncompressed
 * YUV planar 4:2:0 ([I420](http://www.fourcc.org/yuv.php#IYUV)) frame data is
 * read from the buffer of `camera` video output port and dumped to stdout and
 * `camera` preview output port is tunneled to `null_sink`.
 *
 * However, the camera is sending a frame divided into multiple buffers. Each
 * buffer contains a slice of the Y, U, and V planes. This means that the plane
 * data is fragmented if printed out just as is. Search for the definition of
 * `OMX_COLOR_FormatYUV420PackedPlanar` in the OpenMAX IL specification for more
 * details. Thus in order to produce valid I420 data to output file, you first
 * have to save the received buffers until the whole frame has been delivered
 * unpacking the plane slices in the process. Then the whole frame can be written
 * to output file.
 *
 * Please see README.mdwn for more detailed description of this
 * OpenMAX IL demos for Raspberry Pi bundle.
 *
 */

#include <queue>
#include <mutex>
#include <cstring>
#include <iostream>

#include <unistd.h>
#include <OMX_Core.h>
#include <OMX_Component.h>
#include <OMX_Video.h>

#ifdef TARGET_RASPBERRY_PI
#include <bcm_host.h>
#endif




#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <bcm_host.h>

#include <interface/vcos/vcos_semaphore.h>
#include <interface/vmcs_host/vchost.h>

#include <IL/OMX_Core.h>
#include <IL/OMX_Component.h>
#include <IL/OMX_Video.h>
#include <IL/OMX_Broadcom.h>

// Dunno where this is originally stolen from...
#define OxMX_INIT_STRUCTURE(a) \
    memset(&(a), 0, sizeof(a)); \
    (a).nSize = sizeof(a); \
    (a).nVersion.nVersion = OMX_VERSION; /* \
    (a).nVersion.s.nVersionMajor = OMX_VERSION_MAJOR; \
    (a).nVersion.s.nVersionMinor = OMX_VERSION_MINOR; \
    (a).nVersion.s.nRevision = OMX_VERSION_REVISION; \
    (a).nVersion.s.nStep = OMX_VERSION_STEP */

template <typename T>
static inline void InitOMXStructure(T& a)
{
    memset(&a, 0, sizeof(T));
    a.nSize = sizeof(T);
    a.nVersion.nVersion = OMX_VERSION;
/*    a.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
    a.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
    a.nVersion.s.nRevision = OMX_VERSION_REVISION;
    a.nVersion.s.nStep = OMX_VERSION_STEP;*/
}


// Ugly, stupid utility functions
static void say(const char* message, ...) {
    va_list args;
    char str[1024];
    memset(str, 0, sizeof(str));
    va_start(args, message);
    vsnprintf(str, sizeof(str) - 1, message, args);
    va_end(args);
    size_t str_len = strnlen(str, sizeof(str));
    if(str[str_len - 1] != '\n') {
        str[str_len] = '\n';
    }
    fputs(str, stderr);
}

static void die(const char* message, ...) {
    va_list args;
    char str[1024];
    memset(str, 0, sizeof(str));
    va_start(args, message);
    vsnprintf(str, sizeof(str), message, args);
    va_end(args);
    say(str);
    exit(1);
}



static void omx_die(OMX_ERRORTYPE error, const char* message, ...) {
    va_list args;
    char str[1024];
    const char *e;
    memset(str, 0, sizeof(str));
    va_start(args, message);
    vsnprintf(str, sizeof(str), message, args);
    va_end(args);
    switch(error) {
        case OMX_ErrorNone:                     e = "no error";                                      break;
        case OMX_ErrorBadParameter:             e = "bad parameter";                                 break;
        case OMX_ErrorIncorrectStateOperation:  e = "invalid state while trying to perform command"; break;
        case OMX_ErrorIncorrectStateTransition: e = "unallowed state transition";                    break;
        case OMX_ErrorInsufficientResources:    e = "insufficient resource";                         break;
        case OMX_ErrorBadPortIndex:             e = "bad port index, i.e. incorrect port";           break;
        case OMX_ErrorHardware:                 e = "hardware error";                                break;
        /* That's all I've encountered during hacking so let's not bother with the rest... */
        default:                                e = "(no description)";
    }
    die("OMX error: %s: 0x%08x %s", str, error, e);
}

// OMX calls this handler for all the events it emits
static OMX_ERRORTYPE event_handler(
        OMX_HANDLETYPE hComponent,
        OMX_PTR pAppData,
        OMX_EVENTTYPE eEvent,
        OMX_U32 nData1,
        OMX_U32 nData2,
        OMX_PTR pEventData) {
    return OMX_ErrorNone;
}

// Called by OMX when the camera component has filled
// the output buffer with captured video data
static OMX_ERRORTYPE fill_output_buffer_done_handler(
        OMX_HANDLETYPE hComponent,
        OMX_PTR pAppData,
        OMX_BUFFERHEADERTYPE* pBuffer) {
    return OMX_ErrorNone;
}

void* on_thread_run(void* ptr)
{
    for (;;) sleep(1);
    return NULL;
}

static void GetHandle(OMX_HANDLETYPE& handle, const char* name, void* self)
{
    OMX_ERRORTYPE       r;
    static OMX_CALLBACKTYPE   cb;

    memset(&cb, 0, sizeof(cb));
    cb.EventHandler   = event_handler; // _OpenMAXCamera::StaticEventHandler  ;
    cb.FillBufferDone = fill_output_buffer_done_handler; // _OpenMAXCamera::StaticFillBufferDone;

    // Create the component
    r = OMX_GetHandle(&handle, const_cast<char*>(name), self, &cb);
    if (r != OMX_ErrorNone)
    {
        omx_die(r, "OMX_GetHandle() failed with code");
    }
}

void GetParameter(OMX_HANDLETYPE handle, OMX_INDEXTYPE index, void* ptr)
{
    OMX_ERRORTYPE r;
std::cerr << "OMX_GetParameter(" << std::hex << uint32_t(handle) << ", " << std::dec << index << ", " << std::hex << uint32_t(ptr) << ")" << std::endl;
    r = OMX_GetParameter(handle, index, ptr);
    if (r != OMX_ErrorNone)
    {
        omx_die(r, "OMX_GetParameter() failed with code");
    }
}


int main2(int argc, char **argv) {

    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    int err = pthread_create(&thread, &attr, (void*(*)(void*))on_thread_run, NULL);
    if (err != 0)
    {
        die("pthread_create() failed");
    }


    bcm_host_init();

    OMX_ERRORTYPE r;

    if((r = OMX_Init()) != OMX_ErrorNone) {
        omx_die(r, "OMX initalization failed");
    }

    // Init context
/*
    VCOS_SEMAPHORE_T handler_lock;
    if(vcos_semaphore_create(&handler_lock, "handler_lock", 1) != VCOS_SUCCESS) {
        die("Failed to create handler lock semaphore");
    }
*/
    // Init component handles
/*
    OMX_CALLBACKTYPE callbacks;
    memset(&callbacks, 0, sizeof(callbacks));
    callbacks.EventHandler   = event_handler;
    callbacks.FillBufferDone = fill_output_buffer_done_handler;

    // Get handle
    char fullname[32];
    OMX_HANDLETYPE camera;
    strcpy(fullname, "OMX.broadcom.camera");
    say("Initializing component %s", fullname);
    if((r = OMX_GetHandle(&camera, fullname, NULL, &callbacks)) != OMX_ErrorNone) {
        omx_die(r, "Failed to get handle for component %s", fullname);
    }
*/
    char fullname[32];
    OMX_HANDLETYPE camera;
    strcpy(fullname, "OMX.broadcom.camera");
    GetHandle(camera, fullname, NULL);

    OMX_PORT_PARAM_TYPE ports;
    InitOMXStructure(ports);
    GetParameter(camera, OMX_IndexParamVideoInit, &ports);
/*
std::cerr << "OMX_GetParameter(" << std::hex << uint32_t(camera) << ", " << std::dec << OMX_IndexParamVideoInit << ", " << std::hex << uint32_t(&ports) << ")" << std::endl;
    if ((r = OMX_GetParameter(camera, OMX_IndexParamVideoInit, &ports)) != OMX_ErrorNone) {
        omx_die(r, "Failed to get port for component %s", fullname);
    }
std::cerr << "Result was " << std::hex << r << std::endl;
*/
    return 0;
}

