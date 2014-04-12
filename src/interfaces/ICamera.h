
#ifndef __ICAMERA_H
#define __ICAMERA_H

#include <memory>
#include <cstdint>

#include "../tools/Exception.h"
#include "../interfaces/IGenericService.h"


class CameraException : public Exception
{
public:
    CameraException(const char* message);
};


struct CameraBuffer
{
    uint16_t scaled_width;
    uint16_t scaled_height;
    uint16_t bytes_per_row;
    uint8_t  bytes_per_pixel;
    uint8_t  spare;
    uint8_t* data;
};


struct CameraFrame
{
public:
    uint32_t sequence;
    uint16_t pixel_width;
    uint16_t pixel_height;
    uint8_t  pixel_format;
    uint8_t  buffer_count;
    uint8_t  spare[2];
    CameraBuffer buffers[3];
};

class ICamera;
class ICameraDelegate
{
public:
    void CameraUpdate(ICamera& camera, int data);

protected:
    ICameraDelegate();
    virtual ~ICameraDelegate();

private:
    virtual void DoCameraUpdate(ICamera& camera, int data) = 0;
};


class ICamera : public IGenericService
{
public:
    enum Format {YUV420, RGB888};

    void set_pixel_width(int value);
    int pixel_width();

    void set_pixel_height(int value);
    int pixel_height();

    void set_pixel_format(Format value);
    Format pixel_format();

    void set_frames_per_second(int value);
    int frames_per_second();

    void set_circular_frame_buffer_size(int value);
    int circular_frame_buffer_size();

    void set_delegate(std::shared_ptr<ICameraDelegate>& value);
    std::shared_ptr<ICameraDelegate> delegate();

    void set_delegate_data(int value);
    int delegate_data();

    // The following methods should only be used after a call to Start

    size_t             size();
    const CameraFrame& frame(size_t index);

    const CameraFrame& operator [] (size_t index) {return frame(index); }

protected:
    ICamera();
    virtual ~ICamera();

private:
    virtual void do_set_pixel_width(int value) = 0;
    virtual int do_pixel_width() = 0;

    virtual void do_set_pixel_height(int value) = 0;
    virtual int do_pixel_height() = 0;

    virtual void do_set_pixel_format(Format value) = 0;
    virtual Format do_pixel_format() = 0;

    virtual void do_set_frames_per_second(int value) = 0;
    virtual int do_frames_per_second() = 0;

    virtual void do_set_circular_frame_buffer_size(int value) = 0;
    virtual int do_circular_frame_buffer_size() = 0;

    virtual void do_set_delegate(std::shared_ptr<ICameraDelegate>& value) = 0;
    virtual std::shared_ptr<ICameraDelegate> do_delegate() = 0;

    virtual void do_set_delegate_data(int value) = 0;
    virtual int do_delegate_data() = 0;

    virtual size_t do_size() = 0;
    virtual const CameraFrame& do_frame(size_t index) = 0;
};


#endif // __ICAMERA_H


