// link.inl -*-c++-*-
#ifndef LINK_INL
#define LINK_INL
#include <assert.h>
template<class T>
inline
void
Link<T>::OK() const
{
    if (previous_) {
	assert(previous_->next_ == this);
    }
    if (next_) {
	assert(next_->previous_ == this);
    }
}

template<class T>
inline
Link<T>::Link( const T& thing ) : 
    thing_( thing )
{
    previous_ = next_ = 0;
}

template<class T>
inline
Link<T>::Link( Link<T>* previous, Link<T>* next, const T& thing ) : 
    thing_( thing )
{
    previous_ = previous;
    next_ = next;
}

template<class T>
inline
Link<T>*
Link<T>::next()
{
    return next_;
}

template<class T>
inline
Link<T>*
Link<T>::previous()
{
    return previous_;
}

template<class T>
inline
void
Link<T>::add( const T& thing )
{
    
    Link<T>* l = new Link<T>( this, next_, thing );
    if ( next_ )
        next_->previous_ = l;
    next_ = l;
}

template<class T>
inline void
Link<T>::insert( const T& thing )
{
    //    Link<T>* l = new Link<T>( next_, this, thing );
				// bugfix hwn 16/9/96
    Link<T>* l = new Link<T>( previous_, this, thing );
    if ( previous_ )
        previous_->next_ = l;
    previous_ = l;
}

/*
    don't forget to adjust #l#'s top_ and bottom_.
    */
template<class T>
inline void
Link<T>::remove(List<T> &l)
{
    if ( previous_ ) 
        previous_->next_ = next_;
    else 
	l.top_ = next_;

    if ( next_ )
        next_->previous_ = previous_;
    else
	l.bottom_ = previous_;
}

template<class T>
inline
T&
Link<T>::thing()
{
    return thing_;
}
#endif
