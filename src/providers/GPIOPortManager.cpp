
#include <list>
#include <memory>
#include <atomic>
#include <cstring>
#include <iostream>

#include <unistd.h>
#include <bcm2835.h>
#include <pthread.h>
#include <sys/timerfd.h>

#include "../interfaces/IPort.h"
#include "../interfaces/IFileWatcher.h"
#include "../interfaces/IEventQueue.h"
#include "../providers/GPIOPortManager.h"
#include "../tools/System.h"
#include "../tools/Pipe.h"


namespace {

static const long FAST_DELAY =  1000000;
static const long SLOW_DELAY = 20000000;

#define PWM_PATTERNS_COUNT  (sizeof(PWM_PATTERNS)/sizeof(PWM_PATTERNS[0]))
static const uint32_t PWM_PATTERNS[] = {
        0b00000000000000000000000000000000, //     0.00     0/32   0
        0b10000000000000000000000000000000, //              1/32
        0b10000000000000001000000000000000, //              2/32
        0b10000000000100000000001000000000, //              3/32
        0b10000000100000001000000010000000, //     0.125    4/32   1/8
        0b10000010000010000001000001000000, //              5/32
        0b10000100001000001000010000100000, //              6/32
        0b10001000010001000010001000010000, //              7/32
        0b10001000100010001000100010001000, //     0.25     8/32   1/4
        0b10010001001000100100010010001000, //              9/32
        0b10001001001001001000100100100100, //             10/32
        0b10100100100100100100100100100100, //             11/32
        0b10010010100100101001001010010010, //     0.375   12/32   3/8
        0b10100101001010010100101001010010, //             13/32
        0b10101010010100101010100101001010, //             14/32
        0b10101010101001010101010100101010, //             15/32
        0b10101010101010101010101010101010, //     0.50    16/32   1/2
        0b11010101010101010110101010101010, //             17/32
        0b11010101011010101101010101101010, //             18/32
        0b11010110101101011010110101101010, //             19/32
        0b11011010110110101101101011011010, //     0.625   20/32   5/8
        0b11011011011011011011011011011010, //             21/32
        0b11101101101101101110110110110110, //             22/32
        0b11101101110110111011011101110110, //             23/32
        0b11101110111011101110111011101110, //     0.75    24/32   3/4
        0b11110111011110111011110111101110, //             25/32
        0b11110111110111101111101111011110, //             26/32
        0b11111011111101111101111110111110, //             27/32
        0b11111110111111101111111011111110, //     0.875   28/32   7/8
        0b11111111110111111111101111111110, //             29/32
        0b11111111111111101111111111111110, //             30/32
        0b11111111111111111111111111111110, //             31/32
        0b11111111111111111111111111111111};//     1.00    32/32   1

static const uint8_t    GPIO_PORTS[] = {7,8,9,10,11};

class _GPIOPort;

struct port_bits_t
{
    // Writable by the calling thread only
    uint32_t     pwm_output_bits; // Bit pattern used when outputting a PWM or constant signal
    uint8_t   request_input_mode; // Calling thread wants the GPIO pin to be in input mode
    uint8_t spare1,spare2,spare3;

    // Writable by the pwm thread only
    uint32_t     poll_input_bits; // Each poll of GPIO pin rotates by 1, and sets bit 0
    uint8_t    last_output_state; // The last GPIO pin state set during a PWM iteration
    uint8_t    shift_pwm_counter; // Which is the next bit from 'pwm_output_bits' to use
    uint8_t       has_input_mode; // Is the GPIO pin currently in output mode?
    uint8_t               spare4;
};

class _GPIOPortManager : public GPIOPortManager, public std::enable_shared_from_this<_GPIOPortManager>
{
public:
    _GPIOPortManager();
    virtual ~_GPIOPortManager();

    void start_thread();

    void thread_run();
    virtual std::shared_ptr<IPort> DoFindPort(const char* name) override;

    std::list<_GPIOPort> ports_;
    pthread_t           thread_;
    port_bits_t      port_bits_[32]  __attribute__ ((aligned (8)));
    volatile bool exit_request_;
};

class _GPIOPort : public IPort, public IEvent
{
public:
    _GPIOPort(_GPIOPortManager& manager, uint8_t port_number);
    virtual ~_GPIOPort();

    _GPIOPortManager&             manager_;
    std::weak_ptr<IPortDelegate> delegate_;
    int                     delegate_data_;
    char                             name_[15];
    uint8_t                   port_number_;

    virtual const char* do_name() override;

    virtual void do_set_delegate(const std::shared_ptr<IPortDelegate>& value) override;
    virtual std::shared_ptr<IPortDelegate> do_delegate() override;

    virtual void  do_set_delegate_data(int value) override;
    virtual int do_delegate_data() override;

    virtual void do_set_writable(bool value) override;
    virtual bool do_writable() override;

    virtual void do_set_binary_level(bool value) override;
    virtual bool do_binary_level() override;

    virtual void do_set_analogue_level(float value) override;
    virtual float do_analogue_level() override;

    virtual void DoRun() override;
};

_GPIOPort::_GPIOPort(_GPIOPortManager& manager, uint8_t port_number) : manager_(manager), delegate_data_(0), port_number_(port_number)
{
    sprintf(name_, "gpio/%d", port_number_);
    bcm2835_gpio_fsel(port_number_, BCM2835_GPIO_FSEL_OUTP);
    manager_.port_bits_[port_number_].request_input_mode = true;
}

_GPIOPort::~_GPIOPort()
{
}

const char* _GPIOPort::do_name()
{
    return name_;
}

void _GPIOPort::do_set_delegate(const std::shared_ptr<IPortDelegate>& value)
{
    delegate_ = value;
}

std::shared_ptr<IPortDelegate> _GPIOPort::do_delegate()
{
    return delegate_.lock();
}

void  _GPIOPort::do_set_delegate_data(int value)
{
    delegate_data_ = value;
}

int _GPIOPort::do_delegate_data()
{
    return delegate_data_;
}

void _GPIOPort::do_set_writable(bool value)
{
    manager_.port_bits_[port_number_].request_input_mode = value;
    manager_.port_bits_[port_number_].poll_input_bits = 0;
    // bcm2835_gpio_fsel(port_number_, writable_ ? BCM2835_GPIO_FSEL_OUTP : BCM2835_GPIO_FSEL_INPT);
}

bool _GPIOPort::do_writable()
{
    return manager_.port_bits_[port_number_].request_input_mode;
}

void _GPIOPort::do_set_binary_level(bool value)
{
    manager_.port_bits_[port_number_].pwm_output_bits = value ? 0xffffffff : 0;
}

bool _GPIOPort::do_binary_level()
{
    port_bits_t& p = manager_.port_bits_[port_number_];
    return (p.request_input_mode ? p.poll_input_bits : p.pwm_output_bits) != 0;
}

void _GPIOPort::do_set_analogue_level(float value)
{
    int ivalue = value * PWM_PATTERNS_COUNT;
    if (ivalue < 0)
        ivalue = 0;
    else if (ivalue >= int(PWM_PATTERNS_COUNT))
        ivalue = PWM_PATTERNS_COUNT-1;

    manager_.port_bits_[port_number_].pwm_output_bits = PWM_PATTERNS[ivalue];
}

float _GPIOPort::do_analogue_level()
{
    port_bits_t& p = manager_.port_bits_[port_number_];
    return __builtin_popcount(p.request_input_mode ? p.poll_input_bits : p.pwm_output_bits) / float(PWM_PATTERNS_COUNT-1);
}


void _GPIOPort::DoRun()
{
    auto delegate = delegate_.lock();
    if (delegate)
    {
        delegate->OnPortUpdate(*this, delegate_data_);
    }
}


_GPIOPortManager::_GPIOPortManager() : exit_request_(false)
{
    memset(port_bits_, 0, sizeof(port_bits_));

    if (!bcm2835_init())
    {
        std::cerr << "bcm2835_init() failed" << std::endl;
        throw PortManagerException("bcm2835_init() failed");
    }

    for (int index = sizeof(GPIO_PORTS)/sizeof(GPIO_PORTS[0]); --index >= 0; )
    {
        ports_.emplace_front(*this, GPIO_PORTS[index]);
    }
}

static void* on_thread_run(_GPIOPortManager* pm)
{
    pm->thread_run();
    return (void*)0;
}

void _GPIOPortManager::start_thread()
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    int err = pthread_create(&thread_, &attr, (void*(*)(void*))on_thread_run, this);
    if (err != 0)
    {
        std::cerr << "pthread_create() failed" << std::endl;
        throw PortManagerException("pthread_create() failed");
    }
}

_GPIOPortManager::~_GPIOPortManager()
{
    exit_request_ = true;
    pthread_join(thread_, NULL);
}

std::shared_ptr<IPort> _GPIOPortManager::DoFindPort(const char* name)
{
    for (_GPIOPort& port : ports_)
    {
        if (::strcmp(port.name_, name) == 0)
        {
            // Share reference count with parent _GPIOPortManager
            return std::shared_ptr<IPort>(shared_from_this(), &port);
        }
    }
    return std::shared_ptr<IPort>();
}

void _GPIOPortManager::thread_run()
{
    bool   using_slow_timeout = false;
    bool request_slow_timeout = true;

    int fd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);
    if (fd == -1)
    {
        std::cerr << "timerfd_create() failed with " << std::strerror(errno) << std::endl;
        return;
    }

    while (!exit_request_)
    {
        if (using_slow_timeout != request_slow_timeout)
        {
            std::cerr << "Using " << (request_slow_timeout ? "slow" : "fast") << " timeout" << std::endl;
            using_slow_timeout = request_slow_timeout;
            long delay = request_slow_timeout ? SLOW_DELAY : FAST_DELAY;
            struct itimerspec ts = { {0, delay}, {0, delay} };
            if (timerfd_settime(fd, 0, &ts, NULL) == -1)
            {
                std::cerr << "timerfd_settime() failed with " << std::strerror(errno) << std::endl;
                break;
            }
        }

        uint64_t timeouts;
        if (read(fd, &timeouts, sizeof(timeouts)) != sizeof(timeouts))
        {
            std::cerr << "Timer read failed with " << std::strerror(errno) << std::endl;
            break;
        }

        request_slow_timeout = true;

        uint32_t write_mask = 0;
        uint32_t write_bits = 0;
        for (int port = sizeof(port_bits_)/sizeof(port_bits_[0]); --port >= 0; )
        {
            port_bits_t& p = port_bits_[port];
            request_slow_timeout &= p.pwm_output_bits == 0 || p.pwm_output_bits == 0xffffffff;
            uint8_t state = (p.pwm_output_bits >> (p.shift_pwm_counter = (p.shift_pwm_counter-1)&31)) & 1;
            if (state != p.last_output_state)
            {
                write_mask |=          1      << port;
                write_bits |= uint32_t(state) << port;
                p.last_output_state = state;
// bcm2835_gpio_write(port, (p.last_output_state = state) ? HIGH : LOW);
            }
        }
        if (write_mask)
        {
            bcm2835_gpio_write_mask(write_bits, write_mask);
        }
    }

    close(fd);
}

std::atomic<unsigned> created_count;
} // End anonymouse namespace


GPIOPortManager::GPIOPortManager()
{
}

GPIOPortManager::~GPIOPortManager()
{
    created_count --;
}

std::shared_ptr<GPIOPortManager> GPIOPortManager::Create()
{
    unsigned old_value = created_count.fetch_add(1);
    if (old_value > 0)
    {
        created_count --;
        std::cerr << "Attempt to create more than one GPIOPortManager" << std::endl;
        throw PortManagerException("Attempt to create more than one GPIOPortManager");
    }
    try
    {
        auto self = std::make_shared<_GPIOPortManager>();
        self->start_thread(); // Must be done outside of constructor
        return std::static_pointer_cast<GPIOPortManager>(self);
    }
    catch (...)
    {
        created_count --;
        throw;
    }
}

