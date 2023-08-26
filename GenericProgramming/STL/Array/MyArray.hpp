#pragma once

template<class T,int n>
class Array
{
public:
    Array();
    ~Array();
    int size();
    bool get(int index, T& elem);
    bool set(T data, int index);
    T& operator[](int index);
private:
    T *m_pt;
    int m_len;
};

template<class T,int n>
Array<T,n>::Array()
{
    this->m_pt = new T[n];
    if(nullptr == this->m_pt)
    {
        return;
    }
    this->m_len = n;
}

template<class T,int n>
Array<T,n>::~Array()
{
    if(nullptr != this->m_pt)
    {
        delete []this->m_pt;
    }
}

template<class T,int n>
int Array<T,n>::size()
{
    return this->m_len;
}

template<class T,int n>
bool Array<T,n>::get(int index, T& elem)
{
    if(index < 0 || index >= this->m_len)
    {
        return false;
    }

    elem = *(this->m_pt + index);

    return true;
}

template<class T,int n>
bool Array<T,n>::set(T data, int index)
{
    if(index < 0 || index >= this->m_len)
    {
        return false;
    }

    *(this->m_pt + index) = data;

    return true; 
}

template<class T,int n>
T& Array<T,n>::operator[](int index)
{
    return *(this->m_pt + index);
}