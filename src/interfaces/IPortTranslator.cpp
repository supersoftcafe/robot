
#include <iostream>
#include "../interfaces/IPort.h"
#include "../interfaces/IPortTranslator.h"


PortTranslatorException::PortTranslatorException(const char* message) : what_(message)
{
}

const char* PortTranslatorException::what() const throw()
{
    return what_;
}


IPortTranslator::IPortTranslator()
{
}

IPortTranslator::~IPortTranslator()
{
}

bool IPortTranslator::IsTranslatable(const std::shared_ptr<IPort>& port)
{
    return DoIsTranslatable(port);
}

std::shared_ptr<IPort> IPortTranslator::TranslatePort(const std::shared_ptr<IPort>& port)
{
    if (!port)
    {
        std::cerr << "TranslatePort given empty port reference" << std::endl;
        throw PortTranslatorException("TranslatePort given empty port reference");
    }
    if (!IsTranslatable(port))
    {
        std::cerr << "TranslatePort given incompatible port" << std::endl;
        throw PortTranslatorException("DigitalToAnalogueWrapper given incompatible port");
    }
    return DoTranslatePort(port);
}

