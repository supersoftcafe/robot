
#ifndef __INPUTREADER_H
#define __INPUTREADER_H

#include <memory>
#include <string>
#include <exception>


class InputReaderException : std::exception
{
public:
    InputReaderException(const char* message);

    virtual const char* what() const throw();

private:
    const char* what_;
};


class InputReader;
class IInputReaderDelegate
{
public:
    void InputReaderUpdate(InputReader& reader, int data);

protected:
    IInputReaderDelegate();
    virtual ~IInputReaderDelegate();

private:
    virtual void DoInputReaderUpdate(InputReader& reader, int data) = 0;
};


class InputReader
{
public:
    static std::shared_ptr<InputReader> Create(int fd = 0);

    void set_delegate(const std::shared_ptr<IInputReaderDelegate>& value);
    std::shared_ptr<IInputReaderDelegate> delegate();

    void set_delegate_data(int value);
    int delegate_data();

    bool ReadLine(std::string& str);

protected:
    InputReader();
    virtual ~InputReader();

private:
    virtual void do_set_delegate(const std::shared_ptr<IInputReaderDelegate>& value) = 0;
    virtual std::shared_ptr<IInputReaderDelegate> do_delegate() = 0;

    virtual void do_set_delegate_data(int value) = 0;
    virtual int do_delegate_data() = 0;

    virtual bool DoReadLine(std::string& str) = 0;
};

#endif // __INPUTREADER_H

