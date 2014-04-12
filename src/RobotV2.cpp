//============================================================================
// Name        : RobotV1.cpp
// Author      : Michael Brown
// Version     :
// Copyright   : It's mine, all mine!!!
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <sstream>
#include <memory>
#include <unistd.h>
#include <cstdlib>

#include "RobotControl.h"

#include "interfaces/ITimer.h"
#include "interfaces/IMainLoop.h"
#include "providers/SelectMainLoop.h"
#include "providers/PollEventQueue.h"
#include "providers/OpenMAXCameraManager.h"
#include "tools/InputReader.h"
#include "tools/System.h"




class Application : public IInputReaderDelegate, public ITimerDelegate, public std::enable_shared_from_this<Application>
{
public:
    Application();
    void Init();

private:
    virtual void DoInputReaderUpdate(InputReader& reader, int data) override;
    virtual void DoTimerUpdate(ITimer& timer, int data) override;

    std::shared_ptr<RobotControl> control_;
    std::shared_ptr<InputReader>   reader_;
    std::shared_ptr<ITimer>         timer_;
};

Application::Application() :
    control_(RobotControl::Create()),
    reader_ (InputReader::Create(0)),
    timer_  (System::CreateTimer( ))
{
}

void Application::Init()
{
    auto self = shared_from_this();
    reader_->set_delegate(std::static_pointer_cast<IInputReaderDelegate>(self));
    timer_ ->set_delegate(std::static_pointer_cast<ITimerDelegate      >(self));
    timer_ ->set_timeout_ms(250);
    timer_ ->set_repeat(true);
    timer_ ->Start();
}

void Application::DoTimerUpdate(ITimer& timer, int data)
{
//    std::cerr << "Time = " << System::CurrentTimeMS() << std::endl;
}

void Application::DoInputReaderUpdate(InputReader& reader, int data)
{
    std::string line;
    while (reader.ReadLine(line))
    {
        if (line.size() == 0)
        {
            System::Exit();
        }
        else
        {
            std::istringstream in(line);

            float l, r;
            in >> r >> l;

            control_->set_speed_left(l);
            control_->set_speed_right(r);
        }
    }
}

int main(int argc, char** argv)
{
    std::cerr << "Setup" << std::endl;

    auto select_main_loop = SelectMainLoop::Create();
    System::set_time(std::static_pointer_cast<ITime>(select_main_loop));
    System::set_main_loop(std::static_pointer_cast<IMainLoop>(select_main_loop));
    System::set_timer_factory(std::static_pointer_cast<ITimerFactory>(select_main_loop));
    System::set_file_watcher_factory(std::static_pointer_cast<IFileWatcherFactory>(select_main_loop));

    auto poll_event_queue = PollEventQueue::Create();
    System::set_event_queue(std::static_pointer_cast<IEventQueue>(poll_event_queue));

    auto app = std::make_shared<Application>();
    app->Init();

    auto cam_man = OpenMAXCameraManager::Create();
    auto cam = cam_man->FindCamera("OMX.broadcom.camera");
//    cam_man->test();

    select_main_loop->Loop();

    return 0;
}
