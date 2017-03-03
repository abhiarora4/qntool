#ifndef _FUNCTIONPOINTER_WITH_CONTEXT_H_
#define _FUNCTIONPOINTER_WITH_CONTEXT_H_

#include "safebool.h"
#include <string.h>


// The class is meant to store and call a pointer to a void static or member function that can take a context!
template <typename ContextType>
class FunctionPointerWithContext : public SafeBool<FunctionPointerWithContext<ContextType> > {
public:
    typedef FunctionPointerWithContext<ContextType> *pFunctionPointerWithContext_t;
    typedef const FunctionPointerWithContext<ContextType> *cpFunctionPointerWithContext_t;
    typedef void (*pvoidfcontext_t)(ContextType context);

    
    // Create a instance of FunctionPointerWithContext using a static void function!
    FunctionPointerWithContext(void (*function)(ContextType context) = NULL) :
        _memberFunctionAndPointer(), _caller(NULL), _next(NULL) 
    {
        attach(function);
    }

    // Used to invoke non-static member function of a class
    template<typename T>
    FunctionPointerWithContext(T *object, void (T::*member)(ContextType context)) :
        _memberFunctionAndPointer(), _caller(NULL), _next(NULL) 
    {
        attach(object, member);
    }

    FunctionPointerWithContext(const FunctionPointerWithContext& that) : 
        _memberFunctionAndPointer(that._memberFunctionAndPointer), _caller(that._caller), _next(NULL) 
    {

    }

    FunctionPointerWithContext& operator=(const FunctionPointerWithContext& that) 
    {
        _memberFunctionAndPointer = that._memberFunctionAndPointer;
        _caller = that._caller; 
        _next = NULL;
        return *this;
    }

    
    // Attach a static function
    void attach(void (*function)(ContextType context) = NULL) 
    {
        _function = function;
        _caller = functioncaller;
    }

    // Attach a non-static member function
    template<typename T>
    void attach(T *object, void (T::*member)(ContextType context)) 
    {
        _memberFunctionAndPointer._object = static_cast<void *>(object);
        memcpy(_memberFunctionAndPointer._memberFunction, (char*) &member, sizeof(member));
        _caller = &FunctionPointerWithContext::membercaller<T>;
    }

    // Call static or non-static member function and any chained callbacks will be called!
    void call(ContextType context) const 
    {
        _caller(this, context);
    }

    void operator()(ContextType context) const 
    {
        call(context);
    }

    void clear(void)
    {
        _function = NULL;
        _caller = NULL;
        _memberFunctionAndPointer._object = NULL;
        memset(_memberFunctionAndPointer._memberFunction, 0, sizeof(_memberFunctionAndPointer._memberFunction));
    }

    typedef void (FunctionPointerWithContext::*bool_type)() const;

    // Safe bool workaround
    bool toBool() const 
    {
        return (_function || _memberFunctionAndPointer._object);
    }

    // You can build a chain of callback using it!
    // Calling "call" will call all the callbacks in the chain!
    void chainAsNext(pFunctionPointerWithContext_t next) 
    {
        _next = next;
    }

    pFunctionPointerWithContext_t getNext(void) const 
    {
        return _next;
    }

    pvoidfcontext_t get_function() const 
    {
        return (pvoidfcontext_t)_function;
    }

    friend bool operator==(const FunctionPointerWithContext& lhs, const FunctionPointerWithContext& rhs) 
    {
        return ((rhs._caller == lhs._caller) &&
               (memcmp(
                   &rhs._memberFunctionAndPointer, 
                   &lhs._memberFunctionAndPointer, 
                   sizeof(rhs._memberFunctionAndPointer)
               ) == 0));
    }

private:

    template<typename T>
    static void membercaller(cpFunctionPointerWithContext_t self, ContextType context) 
    {
        if (self->_memberFunctionAndPointer._object == NULL)
            return;

        T *o = static_cast<T *>(self->_memberFunctionAndPointer._object);
        void (T::*m)(ContextType);
        memcpy((char*) &m, self->_memberFunctionAndPointer._memberFunction, sizeof(m));
        (o->*m)(context);
    }

    static void functioncaller(cpFunctionPointerWithContext_t self, ContextType context) 
    {
        if (self->_function)
            self->_function(context);
    }

    struct MemberFunctionAndPtr {
       
        // Forward declaration of a calss and a member function to this class.
        // This will force compiler to use the biggest size and the biggest alignement
        // that a member function can take
        class UndefinedClass;
        typedef void (UndefinedClass::*UndefinedMemberFunction)(ContextType);

        void* _object;
        union {
            char _memberFunction[sizeof(UndefinedMemberFunction)];
            UndefinedMemberFunction _alignment;
        };
    };

    union {
        pvoidfcontext_t _function;      // For static member functions!

        mutable MemberFunctionAndPtr _memberFunctionAndPointer;
    };

    void (*_caller)(const FunctionPointerWithContext*, ContextType);

    pFunctionPointerWithContext_t _next;    // Used to contruct chain of function pointers
};

/**
 * @brief Create a new FunctionPointerWithContext which bind an instance and a  
 * a member function together.
 * @details This little helper is a just here to eliminate the need to write the
 * FunctionPointerWithContext type each time you want to create one by kicking 
 * automatic type deduction of function templates. With this function, it is easy 
 * to write only one entry point for functions which expect a FunctionPointer 
 * in parameters.
 * 
 * @param object to bound with member function
 * @param member The member function called
 * @return a new FunctionPointerWithContext
 */
template<typename T, typename ContextType>
FunctionPointerWithContext<ContextType> makeFunctionPointer(T *object, void (T::*member)(ContextType context)) 
{
    return FunctionPointerWithContext<ContextType>(object, member);
}

#endif // ifndef MBED_FUNCTIONPOINTER_WITH_CONTEXT_H