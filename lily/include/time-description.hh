/*
  moment.hh -- part of GNU LilyPond

  (c) 1996,97 Han-Wen Nienhuys
*/

#ifndef tdes_HH
#define tdes_HH

#include "moment.hh"
#include "lily-proto.hh"
#include "varray.hh"

/// full info on where we are
struct Time_description {
    Moment when_;

    /// found an error so far?
    bool error_b_ ;

    /// if true, no bars needed, no reduction of whole_in_measure
    bool cadenza_b_;
    
    /// current measure info
    Moment whole_per_measure_;

    /// where am i 
    Moment whole_in_measure_;

    /// how long is one beat?
    Moment one_beat_;

    /// idem
    int bars_i_;

    /* *************** */
    void set_cadenza (bool);
    void OK() const;
    Time_description();
    void add (Moment dt);
    bool allow_meter_change_b();
    String str()const;
    void print() const;
    void setpartial (Moment p);
    String try_set_partial_str (Moment)const;
    Moment barleft()const;
    Moment next_bar_moment()const;
    void set_meter (int,int);
    static int compare (const Time_description&, const Time_description&);
};

#include "compare.hh"


INSTANTIATE_COMPARE(Time_description&,Time_description::compare);


void
process_timing_reqs (Time_description &time_, 
		    Rhythmic_grouping *default_grouping,
		    Array<Timing_req*> const& timing_req_l_arr);
#endif // Time_description_HH

