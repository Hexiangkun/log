#pragma once


class nocopyable
{
private:
    /* data */
public:
    nocopyable(const nocopyable&) = delete;
    nocopyable& operator=(const nocopyable&) = delete;
protected:
    nocopyable() = default;
    ~nocopyable() = default;
};

/*
nocopyable的目的时禁止该类的对象被复制或者赋值
将复制构造函数和复制赋值构造函数声明为delete，从而防止这些函数被调用并阻止对象的复制和赋值操作

将构造函数和析构函数声明为保护函数

通过继承该类，其他类可以实现禁止复制和赋值操作的功能，从而保证程序的正确性和安全性

*/