/*
  key.hh -- declare Key, Octave_key

  (c) 1996,97 Han-Wen Nienhuys
*/

#ifndef KEY_HH
#define KEY_HH

#include "varray.hh"
#include "scalar.hh"

/// administration of current key in one octave.
class Octave_key {

public:
    Array<int> accidental_i_arr_;
 
    Key();
    void set(int i, int acc);
    int acc(int i)const { return accidental_i_arr_[i]; }
};

/// administration of accidentals
class Key
{
    /** for each octave a key. Has to be private since octave 0 isn't member 0.
     */
    Array<Octave_key> octaves;
public:
    bool multi_octave_b_;
    
    Octave_key&oct(int);
    Octave_key  oct(int) const;
    void set(int name, int acc);
    void set(int oct, int name, int acc);
    Key();
};

#endif // KEY_HH


