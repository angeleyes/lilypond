/*
  pqueue.hh -- declare PQueue_ent and PQueue

  source file of the Flower Library

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#ifndef PQUEUE_HH
#define PQUEUE_HH
#include "array.hh"

template<class K, class T>
struct PQueue_ent
{
  T val;
  K key;
};

template<class K, class T>
int compare (PQueue_ent<K, T> const &e1, PQueue_ent<K, T> const &e2)
{
  return compare (e1.key, e2.key);
}

/**
   Priority queue using a (variable size) in-situ heap.

   Hungarian postfix pq

   TODO: add increase/decrease operations,
   add max () operation
*/
template<class T>
class PQueue
{
  Array<T> heap_array_;
  T &elt (int i)
  {
    return heap_array_[i - 1];
  }
  T const &elt (int i) const
  {
    return heap_array_[i - 1];
  }
public:
  /** acces an heap element.  Careful with this, as changing the
      priority might fuck up the invariants

      @param 1 <= i < size () */
  T &operator [] (int i) { return heap_array_[i]; }
  T operator [] (int i) const { return heap_array_[i]; }
  void OK () const
  {
#ifndef NDEBUG
    for (int i = 2; i <= size (); i++)
      assert (compare (elt (i / 2), elt (i)) <= 0);
#endif
  }
  T front () const { return elt (1); }
  int size () const { return heap_array_.size (); }
  void insert (T v)
  {
    heap_array_.push (v);
    int i = heap_array_.size ();
    int j = i / 2;
    while (j)
      {
	if (compare (elt (j), v) > 0)
	  {
	    elt (i) = elt (j);
	    i = j;
	    j = i / 2;
	  }
	else

	  {
	    break;
	  }
      }
    elt (i) = v;
    OK ();
  }
  T max () const
  {
    //int first_leaf_i = size ();
    T max_t;
    return max_t;
  }
  void delmin ()
  {
    assert (size ());
    T last = heap_array_.top ();

    int mini = 2;
    int lasti = 1;

    while (mini < size ())
      {
	if (compare (elt (mini + 1), elt (mini)) < 0)
	  mini++;
	if (compare (last, elt (mini)) < 0)
	  break;
	elt (lasti) = elt (mini);
	lasti = mini;
	mini *= 2;
      }
    elt (lasti) = last;
    heap_array_.pop ();
    OK ();
  }
  T get ()
  {
    T t = front ();
    delmin ();
    return t;
  }
};

#endif // PQUEUE_HH
