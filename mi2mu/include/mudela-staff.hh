//
// mudela-staff.hh -- declare mudela_staff
//
// copyright 1997 Jan Nieuwenhuizen <jan@digicash.com>

#ifndef MUDELA_STAFF_HH
#define MUDELA_STAFF_HH

#include "mi2mu-proto.hh"
#include "proto.hh"
#include "plist.hh"
#include "string.hh"

/// (mudela_staff)
class Mudela_staff {
public:
    Mudela_staff (int number_i, String copyright_str, String track_name_str, String instrument_str);
    ~Mudela_staff();

    void add_item (Mudela_item* mudela_item_p);
    void eat_voice (Link_list<Mudela_item*>& items);
    String id_str();
    String name_str();
    void output (Mudela_stream& mudela_stream_r);
    void process();
    void set_meter (int num_i, int den_i, int clocks_i, int count_32_i);
    void set_tempo (int useconds_i);

    String copyright_str_;
    String instrument_str_;
    String name_str_;
    Mudela_meter* mudela_meter_p_;
    Mudela_tempo* mudela_tempo_p_;
    int number_i_;

private:
    void output_mudela_begin_bar (Mudela_stream& mudela_stream_r, Moment now_mom, int bar_i);
#if 0
    void output_mudela_rest (Mudela_stream& mudela_stream_r, Moment begin_mom, Moment end_mom);
    void output_mudela_rest_remain (Mudela_stream& mudela_stream_r, Moment mom);
#endif

    Pointer_list<Mudela_voice*> mudela_voice_p_list_;
    Pointer_list<Mudela_item*> mudela_item_p_list_;
};

#endif // MUDELA_STAFF_HH

