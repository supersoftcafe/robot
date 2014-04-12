
#ifndef __IPORTTRANSLATOR_H
#define __IPORTTRANSLATOR_H

#include <memory>
#include <exception>

class PortTranslatorException : public std::exception
{
public:
	PortTranslatorException(const char* message);

	virtual const char* what() const throw();

private:
	const char* what_;
};

class IPort;
class IPortTranslator
{
public:
	bool IsTranslatable(const std::shared_ptr<IPort>& port);
	std::shared_ptr<IPort> TranslatePort(const std::shared_ptr<IPort>& port);

protected:
	IPortTranslator();
	virtual ~IPortTranslator();

private:
	virtual bool DoIsTranslatable(const std::shared_ptr<IPort>&) = 0;
	virtual std::shared_ptr<IPort> DoTranslatePort(const std::shared_ptr<IPort>&) = 0;
};


#endif /* __IPORTTRANSLATOR_H */
