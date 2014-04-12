
#include <iostream>
#include "../interfaces/ICamera.h"


CameraException::CameraException(const char* message) : Exception(message)
{
}


ICameraDelegate::ICameraDelegate()
{
}

ICameraDelegate::~ICameraDelegate()
{
}

void ICameraDelegate::CameraUpdate(ICamera& camera, int data)
{
    DoCameraUpdate(camera, data);
}


ICamera::ICamera()
{
}

ICamera::~ICamera()
{
}

void ICamera::set_pixel_width(int value)
{
    do_set_pixel_width(value);
}

int ICamera::pixel_width()
{
    return do_pixel_width();
}

void ICamera::set_pixel_height(int value)
{
    do_set_pixel_height(value);
}

int ICamera::pixel_height()
{
    return pixel_height();
}

void ICamera::set_pixel_format(ICamera::Format value)
{
    do_set_pixel_format(value);
}

ICamera::Format ICamera::pixel_format()
{
    return do_pixel_format();
}

void ICamera::set_frames_per_second(int value)
{
    do_set_frames_per_second(value);
}

int ICamera::frames_per_second()
{
    return do_frames_per_second();
}

void ICamera::set_circular_frame_buffer_size(int value)
{
    do_set_circular_frame_buffer_size(value);
}

int ICamera::circular_frame_buffer_size()
{
    return do_circular_frame_buffer_size();
}

void ICamera::set_delegate(std::shared_ptr<ICameraDelegate>& value)
{
    do_set_delegate(value);
}

std::shared_ptr<ICameraDelegate> ICamera::delegate()
{
    return do_delegate();
}

void ICamera::set_delegate_data(int value)
{
    do_set_delegate_data(value);
}

int ICamera::delegate_data()
{
    return do_delegate_data();
}

size_t ICamera::size()
{
    return do_size();
}

const CameraFrame& ICamera::frame(size_t index)
{
    if (index >= size())
    {
        std::cerr << "ICamera::frame() index is out of bounds" << std::endl;
        throw CameraException("ICamera::frame() index is out of bounds");
    }
    return do_frame(index);
}




