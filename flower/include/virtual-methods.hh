/*
  virtual-methods.hh -- declare 

  source file of the Flower Library

  (c) 1997 Han-Wen Nienhuys <hanwen@stack.nl>
*/


#ifndef VIRTUAL_METHODS_HH
#define VIRTUAL_METHODS_HH

/** a macro to declare the classes name as a static and virtual function.
  The static_name() can *not* be inlined (this might have the effect that 
  s->name() != S::static_name (). Overlapping strings need not be merged in C++
 */
#define DECLARE_MY_RUNTIME_TYPEINFO	\
static char const *static_name();\
static bool static_is_type_b (const char*s);\
virtual bool is_type_b (const char *s) const { return static_is_type_b (s); } \
virtual char const *name() const{ return static_name (); } \
int a_stupid_nonexistent_function_to_allow_the_semicolon_come_out()

#define IMPLEMENT_STATIC_NAME(c)\
    char const *c::static_name() { return #c; } 



#define VIRTUAL_COPY_CONS(T, R)\
  virtual R *clone() const { return  new T(*this); } \
  int  yet_another_stupid_function_to_allow_semicolon()


#define DECLARE_VIRTUAL_COPY_CONS(T,R)\
      virtual R *clone() const
#define IMPLEMENT_VIRTUAL_COPY_CONS(T,R)\
	R *T::clone() const { return  new T(*this); } \
    
#define IMPLEMENT_IS_TYPE_B(D) 							   \
    IMPLEMENT_STATIC_NAME(D)\
  bool D::static_is_type_b (const char *s)					   \
{ 										   \
    return s == static_name();							   \
}										   
										   
#define IMPLEMENT_IS_TYPE_B1(D,B) 						   \
	IMPLEMENT_STATIC_NAME(D)\
  bool D::static_is_type_b (const char *s)						   \
{ 										   \
    return s == static_name() || B::static_is_type_b (s);				   \
}										   
#define IMPLEMENT_IS_TYPE_B2(D, BA, BB) 						   \
	IMPLEMENT_STATIC_NAME(D)\
  bool D::static_is_type_b (const char *s)						   \
{ 										   \
    return s == static_name() || BA::static_is_type_b (s) || BB::static_is_type_b (s); \
}

#endif 
