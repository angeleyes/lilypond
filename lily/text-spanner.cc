/*
  text-spanner.cc -- implement Text_spanner

  source file of the GNU LilyPond music typesetter

  (c) 1997 Han-Wen Nienhuys <hanwen@stack.nl>
*/

#include "molecule.hh"
#include "boxes.hh"
#include "text-spanner.hh"
#include "text-def.hh"
#include "debug.hh"
#include "paper-def.hh"
#include "symbol.hh"


void
Text_spanner::set_support(Directional_spanner*d)
{
    if (support_span_l_)
	remove_dependency(support_span_l_);
    
    support_span_l_ =d;
    add_dependency(d);
}

Text_spanner::Text_spanner()
{
    spec_p_ = 0;
    support_span_l_ = 0;
}


IMPLEMENT_IS_TYPE_B1(Text_spanner,Spanner);

void
Text_spanner::do_print() const
{
    spec_p_->print();
}

void
Text_spanner::do_post_processing()
{
    text_off_ = support_span_l_->center() +
	Offset(0,support_span_l_->dir_i_ * paper()->internote_f() * 4); // todo
}

Molecule*
Text_spanner::brew_molecule_p() const
{
    Atom tsym (spec_p_->get_atom(paper(),0));
    tsym.translate(text_off_);

    Molecule*output = new Molecule;
    output->add( tsym );
    return output;
}

void
Text_spanner::do_pre_processing()
{
    right_col_l_ = support_span_l_->right_col_l_;
    left_col_l_ = support_span_l_->left_col_l_;
    assert(left_col_l_ && right_col_l_);
}

Interval
Text_spanner::height()const
{
    return brew_molecule_p()->extent().y;
}

void
Text_spanner::do_substitute_dependency(Score_elem* o, Score_elem*n)
{
    if (support_span_l_ == o) 
	support_span_l_ = (Directional_spanner*) (n?n->spanner():0);
}


Text_spanner::~Text_spanner()
{
    delete spec_p_;
}

Text_spanner::Text_spanner(Text_spanner const&s)
    : Spanner( s)
{
    support_span_l_ = s.support_span_l_;
    spec_p_ = s.spec_p_? s.spec_p_->clone() : 0;
    text_off_ = s.text_off_;
}
