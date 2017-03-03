#ifndef _SAFE_BOOL_H_
#define _SAFE_BOOL_H_


/* Safe bool idiom, see : http://www.artima.com/cppsource/safebool.html */

namespace SafeBool_ {

class SafeBoolBase {

  template<typename>
  friend class SafeBool;

protected:
 
    typedef void (SafeBoolBase::*BoolType_t)() const;

    void invalidTag() const;

    void trueTag() const {}
};

}

template <typename T>
class SafeBool : public SafeBool_::SafeBoolBase {
public:

    operator BoolType_t() const 
    {
        return (static_cast<const T*>(this))->toBool()
            ? &SafeBool<T>::trueTag : 0;
    }
};


template <typename T, typename U>
void operator==(const SafeBool<T>& lhs,const SafeBool<U>& rhs) 
{
    lhs.invalidTag();
    // return false;
}


template <typename T,typename U>
void operator!=(const SafeBool<T>& lhs,const SafeBool<U>& rhs) 
{
    lhs.invalidTag();
    // return false;
}

/*
#include <iostream>
class Example : public SafeBool<Example> {
public:

    bool toBool() const
    {
        return true;
    }

};

int main()
{
    Example ex;

    if (ex)
        std :: cout << "True" << std :: endl;
    else
        std :: cout << "False" << std :: endl;
    return 0;
}
*/

#endif // _SAFE_BOOL_H_