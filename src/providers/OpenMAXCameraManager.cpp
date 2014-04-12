
#include <queue>
#include <mutex>
#include <cstring>
#include <iostream>

#include "../providers/OpenMAXCameraManager.h"
#include "../interfaces/ICamera.h"

#include <unistd.h>
#include <OMX_Core.h>
#include <OMX_Component.h>
#include <OMX_Video.h>
#include <OMX_Broadcom.h>

#ifdef TARGET_RASPBERRY_PI
#include <bcm_host.h>
#endif


namespace { // Begin anonymous namespace

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
/*
static const OMX_INDEXTYPE all_port_types[] = {
        OMX_IndexParamAudioInit,
        OMX_IndexParamVideoInit,
        OMX_IndexParamImageInit,
        OMX_IndexParamOtherInit};*/


class _OpenMAXCameraManager;

class _OpenMAXCamera : public ICamera, public std::enable_shared_from_this<_OpenMAXCamera>
{
public:
    _OpenMAXCamera(const char* name);
    virtual ~_OpenMAXCamera();

    virtual void do_set_pixel_width(int value) override;
    virtual int do_pixel_width() override;

    virtual void do_set_pixel_height(int value) override;
    virtual int do_pixel_height() override;

    virtual void do_set_pixel_format(Format value) override;
    virtual Format do_pixel_format() override;

    virtual void do_set_frames_per_second(int value) override;
    virtual int do_frames_per_second() override;

    virtual void do_set_circular_frame_buffer_size(int value) override;
    virtual int do_circular_frame_buffer_size() override;

    virtual void do_set_delegate(std::shared_ptr<ICameraDelegate>& value) override;
    virtual std::shared_ptr<ICameraDelegate> do_delegate() override;

    virtual void do_set_delegate_data(int value) override;
    virtual int do_delegate_data() override;

    virtual void DoStart() override;
    virtual void DoStop() override;

    virtual size_t do_size() override;
    virtual const CameraFrame& do_frame(size_t index) override;


    // void CreateComponent(const char* name, OMX_HANDLETYPE& handle, OMX_U32& video_port_base);
    // void ChangePortStatus(OMX_HANDLETYPE handle, OMX_U32 port_index, OMX_BOOL status);
    static void GetHandle(OMX_HANDLETYPE& handle, const char* name, void* self);
    static void SendCommand(OMX_HANDLETYPE handle, OMX_COMMANDTYPE command, OMX_U32 port_index, void* ptr);
    static void GetParameter(OMX_HANDLETYPE handle, OMX_INDEXTYPE index, void* ptr);
    static void GetComponentVersion(OMX_HANDLETYPE handle, char* name, OMX_VERSIONTYPE& version, OMX_VERSIONTYPE& spec_version, OMX_UUIDTYPE& uuid);

    static OMX_ERRORTYPE StaticEventHandler(
        OMX_IN OMX_HANDLETYPE hComponent,
        OMX_IN OMX_PTR pAppData,
        OMX_IN OMX_EVENTTYPE eEvent,
        OMX_IN OMX_U32 nData1,
        OMX_IN OMX_U32 nData2,
        OMX_IN OMX_PTR pEventData);

    static OMX_ERRORTYPE StaticFillBufferDone(
        OMX_IN OMX_HANDLETYPE hComponent,
        OMX_IN OMX_PTR pAppData,
        OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);

    struct EventData
    {
        OMX_IN OMX_HANDLETYPE hComponent;
        OMX_IN OMX_EVENTTYPE      eEvent;
        OMX_IN OMX_U32            nData1;
        OMX_IN OMX_U32            nData2;
        OMX_IN OMX_PTR        pEventData;
    };

    std::shared_ptr<_OpenMAXCameraManager> camera_manager_;
    std::string                                      name_;
    OMX_HANDLETYPE                          camera_handle_;
    // OMX_U32                                  preview_port_;
    OMX_U32                                    video_port_;
    std::mutex                     global_structure_mutex_;
};

class _OpenMAXCameraManager : public OpenMAXCameraManager, public std::enable_shared_from_this<_OpenMAXCameraManager>
{
public:
    _OpenMAXCameraManager();
    virtual ~_OpenMAXCameraManager();

    virtual std::shared_ptr<ICamera> DoFindCamera(const char* name) override;
    virtual void test() override;
};


OMX_ERRORTYPE _OpenMAXCamera::StaticEventHandler(
        OMX_IN OMX_HANDLETYPE hComponent,
        OMX_IN OMX_PTR pAppData,
        OMX_IN OMX_EVENTTYPE eEvent,
        OMX_IN OMX_U32 nData1,
        OMX_IN OMX_U32 nData2,
        OMX_IN OMX_PTR pEventData)
{
    std::cerr << __func__ << std::endl;
    return OMX_ErrorNone;
}

OMX_ERRORTYPE _OpenMAXCamera::StaticFillBufferDone(
        OMX_IN OMX_HANDLETYPE hComponent,
        OMX_IN OMX_PTR pAppData,
        OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
    std::cerr << __func__ << std::endl;
    return OMX_ErrorNone;
}



void _OpenMAXCamera::GetHandle(OMX_HANDLETYPE& handle, const char* name, void* self)
{
    OMX_ERRORTYPE       r;
    OMX_CALLBACKTYPE   cb;

    memset(&cb, 0, sizeof(cb));
    cb.EventHandler   = _OpenMAXCamera::StaticEventHandler  ;
    cb.FillBufferDone = _OpenMAXCamera::StaticFillBufferDone;

    // Create the component
    r = OMX_GetHandle(&handle, const_cast<char*>(name), self, &cb);
    if (r != OMX_ErrorNone)
    {
        std::cerr << "OMX_GetHandle() failed with code " << std::hex << r << std::endl;
        throw CameraException("OMX_GetHandle() failed");
    }
}

void _OpenMAXCamera::SendCommand(OMX_HANDLETYPE handle, OMX_COMMANDTYPE command, OMX_U32 port_index, void* ptr)
{
    OMX_ERRORTYPE r;
    r = OMX_SendCommand(handle, command, port_index, ptr);
    if (r != OMX_ErrorNone)
    {
        std::cerr << "OMX_SendCommand() failed with code " << std::hex << r << std::endl;
        throw CameraException("OMX_SendCommand() failed");
    }
}

void _OpenMAXCamera::GetParameter(OMX_HANDLETYPE handle, OMX_INDEXTYPE index, void* ptr)
{
    OMX_ERRORTYPE r;
    r = OMX_GetParameter(handle, index, ptr);
    if (r != OMX_ErrorNone)
    {
        std::cerr << "OMX_GetParameter() failed with code " << std::hex << r << std::endl;
        throw CameraException("OMX_GetParameter() failed");
    }
}

void _OpenMAXCamera::GetComponentVersion(OMX_HANDLETYPE handle, char* name, OMX_VERSIONTYPE& version, OMX_VERSIONTYPE& spec_version, OMX_UUIDTYPE& uuid)
{
    OMX_ERRORTYPE r;
    r = OMX_GetComponentVersion(handle, name, &version, &spec_version, &uuid);
    if (r != OMX_ErrorNone)
    {
        std::cerr << "OMX_GetComponentVersion() failed with code " << std::hex << r << std::endl;
        throw CameraException("OMX_GetComponentVersion() failed");
    }
}

/*
void _OpenMAXCamera::ChangePortStatus(OMX_HANDLETYPE handle, OMX_U32 port_index, OMX_BOOL status)
{
    SendCommand(handle, status!=OMX_FALSE?OMX_CommandPortEnable:OMX_CommandPortDisable, port_index, NULL);

    auto portdef = InitOMXStructure<OMX_PARAM_PORTDEFINITIONTYPE>();
    portdef.nPortIndex = port_index;
    for (;;)
    {
        GetParameter(handle, OMX_IndexParamPortDefinition, &portdef);
        if (portdef.bEnabled == status) break;
        usleep(10000);
    }
}
*/
/*
void _OpenMAXCamera::CreateComponent(const char* name, OMX_HANDLETYPE& handle, OMX_U32& video_port_base)
{
    GetHandle(handle, name);

    // Disable all ports, for now
    for (auto port_type : all_port_types)
    {
        OMX_PORT_PARAM_TYPE ports;
        GetParameter(camera_handle_, port_type, &ports);

        if (port_type == OMX_IndexParamVideoInit)
        {
            video_port_base = ports.nStartPortNumber;
        }

        OMX_U32 port_index  = ports.nStartPortNumber;
        OMX_U32 final_index = port_index + ports.nPorts;
        for (; port_index < final_index; ++port_index)
        {
            ChangePortStatus(handle, port_index, OMX_FALSE);
        }
    }
}
*/
_OpenMAXCamera::_OpenMAXCamera(const char* name) : name_(name)
{
    // Create the camera component
    GetHandle(camera_handle_, name, this);

    try
    {
        OMX_UUIDTYPE uid;
        char fetched_name[128];
        OMX_VERSIONTYPE version, spec_version;

        GetComponentVersion(camera_handle_, fetched_name, version, spec_version, uid);
        std::cerr << "GetComponentVersion info:" << std::endl
                  << "    name = " << fetched_name << std::endl
                  << "    uuid = " << uid << std::endl;

        OMX_PORT_PARAM_TYPE ports;

        // Discover the video port and disable the preview port
        InitOMXStructure(ports);
        GetParameter(camera_handle_, OMX_IndexParamVideoInit, &ports);
        SendCommand(camera_handle_, OMX_CommandPortDisable, ports.nStartPortNumber, NULL); // preview_port_ = ports.nStartPortNumber + 0;
        video_port_ = ports.nStartPortNumber + 1;

        // Disable the audio ports
        InitOMXStructure(ports);
        GetParameter(camera_handle_, OMX_IndexParamAudioInit, &ports);
        for (int index = ports.nPorts; --index >= 0; )
            SendCommand(camera_handle_, OMX_CommandPortDisable, ports.nStartPortNumber+index, NULL);

        // Disable the image ports
        InitOMXStructure(ports);
        GetParameter(camera_handle_, OMX_IndexParamImageInit, &ports);
        for (int index = ports.nPorts; --index >= 0; )
            SendCommand(camera_handle_, OMX_CommandPortDisable, ports.nStartPortNumber+index, NULL);

        // Disable the other ports
        InitOMXStructure(ports);
        GetParameter(camera_handle_, OMX_IndexParamOtherInit, &ports);
        for (int index = ports.nPorts; --index >= 0; )
            SendCommand(camera_handle_, OMX_CommandPortDisable, ports.nStartPortNumber+index, NULL);

        // Check that we actually have a camera type, that is video capable.
        OMX_PARAM_SENSORMODETYPE sensor_mode;
        InitOMXStructure(sensor_mode);
        InitOMXStructure(sensor_mode.sFrameSize);
        sensor_mode.sFrameSize.nPortIndex = OMX_ALL;
        sensor_mode.nPortIndex = OMX_ALL;

        // Print some sensor stats, which also ensures that we really have a camera
        GetParameter(camera_handle_, OMX_IndexParamCommonSensorMode, &sensor_mode);

        std::cerr << "Camera info:" << std::endl
                  << "   nPortIndex = " << sensor_mode.nPortIndex << std::endl
                  << "   nFrameRate = " << sensor_mode.nFrameRate << std::endl
                  << "   bOneShot   = " << sensor_mode.bOneShot   << std::endl
                  << "   sFrameSize.nPortIndex = " << sensor_mode.sFrameSize.nPortIndex << std::endl
                  << "   sFrameSize.nWidth     = " << sensor_mode.sFrameSize.nWidth     << std::endl
                  << "   sFrameSize.nHeight    = " << sensor_mode.sFrameSize.nHeight    << std::endl;
    }
    catch (...)
    {
        OMX_FreeHandle(camera_handle_);
        throw;
    }
}

_OpenMAXCamera::~_OpenMAXCamera()
{
    // Safely transition to idle and then unload

    // Might mean blocking
    // r = OMX_FreeHandle(camera_handle_);
}

void _OpenMAXCamera::do_set_pixel_width(int value)
{
}

int _OpenMAXCamera::do_pixel_width()
{
    return -1;
}

void _OpenMAXCamera::do_set_pixel_height(int value)
{
}

int _OpenMAXCamera::do_pixel_height()
{
    return -1;
}

void _OpenMAXCamera::do_set_pixel_format(ICamera::Format value)
{
}

ICamera::Format _OpenMAXCamera::do_pixel_format()
{
    return ICamera::RGB888;
}

void _OpenMAXCamera::do_set_frames_per_second(int value)
{
}

int _OpenMAXCamera::do_frames_per_second()
{
    return -1;
}

void _OpenMAXCamera::do_set_circular_frame_buffer_size(int value)
{
}

int _OpenMAXCamera::do_circular_frame_buffer_size()
{
    return -1;
}

void _OpenMAXCamera::do_set_delegate(std::shared_ptr<ICameraDelegate>& value)
{
}

std::shared_ptr<ICameraDelegate> _OpenMAXCamera::do_delegate()
{
    return std::shared_ptr<ICameraDelegate>();
}

void _OpenMAXCamera::do_set_delegate_data(int value)
{
}

int _OpenMAXCamera::do_delegate_data()
{
    return -1;
}

void _OpenMAXCamera::DoStart()
{
    // Setup

    // Start capture
}

void _OpenMAXCamera::DoStop()
{
    // Return to idle state
}

size_t _OpenMAXCamera::do_size()
{
    return 0;
}

const CameraFrame& _OpenMAXCamera::do_frame(size_t index)
{
    static CameraFrame f;
    return f;
}



_OpenMAXCameraManager::_OpenMAXCameraManager()
{
#ifdef TARGET_RASPBERRY_PI
    std::cerr << "bcm_host_init();" << std::endl;
    bcm_host_init();
#endif

    std::cerr << "OMX_Init();" << std::endl;
    OMX_ERRORTYPE r = OMX_Init();
    if (r != OMX_ErrorNone)
    {
        std::cerr << "OMX_Init() failed with code " << r << std::endl;
        throw CameraManagerException("OMX_Init() failed");
    }
}

_OpenMAXCameraManager::~_OpenMAXCameraManager()
{
    std::cerr << "OMX_DeInit();" << std::endl;
    OMX_Deinit();
}

void _OpenMAXCameraManager::test()
{
    char       name[128];
    int       index;

    index = 0;
    OMX_ERRORTYPE r;
    while ((r = OMX_ComponentNameEnum(name, sizeof(name), index)) != OMX_ErrorNoMore)
    {
        if (r != OMX_ErrorNone)
        {
            throw CameraManagerException("OMX_ComponentNameEnum() failed");
        }
        std::cerr << "Found component[" << index << "] named " << name << std::endl;
        index += 1;
    }
    std::cerr << "Enum done. Found " << index << " components" << std::endl;
}

std::shared_ptr<ICamera> _OpenMAXCameraManager::DoFindCamera(const char* name)
{
    auto ptr = std::make_shared<_OpenMAXCamera>(name);
    ptr->camera_manager_ = shared_from_this();
    return std::static_pointer_cast<ICamera>(ptr);
}


} // End anonymous namespace




std::shared_ptr<OpenMAXCameraManager> OpenMAXCameraManager::Create()
{
    auto self = std::make_shared<_OpenMAXCameraManager>();
    return std::static_pointer_cast<OpenMAXCameraManager>(self);
}

OpenMAXCameraManager::OpenMAXCameraManager()
{
}

OpenMAXCameraManager::~OpenMAXCameraManager()
{
}

