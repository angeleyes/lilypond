/*
  clef.cc -- implement  Clef_engraver

  source file of the GNU LilyPond music typesetter

  (c) 1997 Han-Wen Nienhuys <hanwen@stack.nl>,
  Mats Bengtsson <matsb@s3.kth.se>
*/

#include "bar.hh"
#include "clef-grav.hh"
#include "clef-item.hh"
#include "debug.hh"
#include "command-request.hh"
#include "time-description.hh"

Clef_engraver::Clef_engraver()
{
    clef_p_ = 0;
    clef_req_l_ =0;
    
    set_type("violin");
}

bool
Clef_engraver::set_type(String s)
{
    clef_type_str_  = s;
    if (clef_type_str_ == "violin") {
	c0_position_i_= -2;
    } else if (clef_type_str_ == "alto") {
	c0_position_i_= 4;
    } else if (clef_type_str_ == "tenor") {
	c0_position_i_= 6;
    } else if (clef_type_str_ == "bass") {
	c0_position_i_= 10;
    } else 
	return false;
    
    return true;
}

void
Clef_engraver::fill_staff_info(Staff_info &i)
{
    i.c0_position_i_l_ = &c0_position_i_;
}

void 
Clef_engraver::read_req(Clef_change_req*c_l)
{
    if (!set_type(c_l->clef_str_))
	c_l->error("unknown clef type ");
}
void
Clef_engraver::acknowledge_element(Score_elem_info info)
{
    if (info.elem_l_->name() == Bar::static_name() ) {
	create_clef();
	if ( !clef_req_l_ )
	    clef_p_->default_b_ = true;
    }
}

void
Clef_engraver::do_creation_processing()
{
    create_clef();
    clef_p_->default_b_ = false;
}

bool
Clef_engraver::do_try_request(Request * r_l)
{
    Command_req* creq_l= r_l->command();
    if (!creq_l || !creq_l->clefchange())
	return false;

    clef_req_l_ = creq_l->clefchange();
    
    read_req(clef_req_l_); 
    return true;
}

void 
Clef_engraver::create_clef()
{
    if (!clef_p_) {
	clef_p_ = new Clef_item;
        announce_element(Score_elem_info(clef_p_,clef_req_l_));
    }
    clef_p_->read(*this);
}

void
Clef_engraver::do_process_requests()
{
    if (clef_req_l_) {
	create_clef();
	clef_p_->default_b_ = false;
    }
}

void
Clef_engraver::do_pre_move_processing()
{
    if (!clef_p_)
	return;
    typeset_breakable_item(clef_p_);
    clef_p_ = 0;
}
    
void
Clef_engraver::do_post_move_processing()
{
    clef_req_l_ = 0;
}

void
Clef_engraver::do_removal_processing()
{
    delete clef_p_;
    clef_p_ =0;
}


IMPLEMENT_IS_TYPE_B1(Clef_engraver,Engraver);
ADD_THIS_ENGRAVER(Clef_engraver);
