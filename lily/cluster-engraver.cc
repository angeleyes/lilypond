/*
  cluster-engraver.cc -- implement Cluster_engraver

  (c) 2002--2003 Juergen Reuter <reuter@ipd.uka.de>

  Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "engraver.hh"
#include "item.hh"
#include "spanner.hh"
#include "note-head.hh"
#include "note-column.hh"
#include "group-interface.hh"

class Cluster_spanner_engraver : public Engraver
{

protected:
  TRANSLATOR_DECLARATIONS(Cluster_spanner_engraver);
  virtual bool try_music (Music *);
  virtual void process_music ();  
  virtual void acknowledge_grob (Grob_info);
  virtual void stop_translation_timestep ();
  virtual void finalize ();
private:
  Link_array<Music> cluster_notes_;
  Item* beacon_;

  void typeset_grobs ();
  Spanner *spanner_;
  Spanner * finished_spanner_ ;
};

Cluster_spanner_engraver::Cluster_spanner_engraver ()
{
  spanner_ = 0;
  finished_spanner_ = 0;
  beacon_ = 0;
}

void
Cluster_spanner_engraver::finalize ()
{
  typeset_grobs ();
  finished_spanner_ = spanner_;
  spanner_ = 0;
  typeset_grobs ();
}

void
Cluster_spanner_engraver::typeset_grobs ()
{
  if (finished_spanner_ )
    {
      typeset_grob (finished_spanner_);
      finished_spanner_ = 0;
    }

  if (beacon_)
    {
      typeset_grob (beacon_);
      beacon_ = 0;
    }
}

bool
Cluster_spanner_engraver::try_music (Music *m)
{
  if (m->is_mus_type ("abort-event"))
    {
      if (spanner_)
	{
	  spanner_->suicide ();
	  spanner_ = 0;
	}
    }
  else if (m->is_mus_type ("cluster-note-event"))
    {
      cluster_notes_.push (m);
      return true;
    }
  return false;
}

void
Cluster_spanner_engraver::process_music ()
{
  if (cluster_notes_.size())
    {
      SCM c0scm = get_property ("centralCPosition");

      int c0 =  gh_number_p (c0scm) ? gh_scm2int (c0scm) : 0;
      int pmax = INT_MIN;
      int pmin = INT_MAX;
      
      for (int i = 0; i <cluster_notes_.size (); i++)
	{
	  Pitch *pit =unsmob_pitch (cluster_notes_[i]->get_mus_property ("pitch"));

	  int p =( pit ? pit->steps () : 0) + c0;

	  pmax = pmax >? p;
	  pmin = pmin <? p;
	}
      
      beacon_ = new Item (get_property ("ClusterSpannerBeacon"));
      beacon_->set_grob_property ("positions",
				  scm_cons (gh_int2scm (pmin),
					    gh_int2scm (pmax)));
      announce_grob (beacon_, cluster_notes_[0]->self_scm ());
    }

  if (beacon_ && !spanner_)
    {    
      spanner_ = new Spanner (get_property ("ClusterSpanner"));
      announce_grob (spanner_, cluster_notes_[0]->self_scm ());
    }
  
  if (beacon_ && spanner_)
    {
      add_bound_item (spanner_, beacon_);
      Pointer_group_interface::add_grob (spanner_, ly_symbol2scm ("columns"), beacon_);
    }
}


void
Cluster_spanner_engraver::stop_translation_timestep ()
{
  typeset_grobs ();

  cluster_notes_.clear();
  
}

void
Cluster_spanner_engraver::acknowledge_grob (Grob_info info)
{
  if (!beacon_ && Note_column::has_interface (info.grob_))
    {
      finished_spanner_ = spanner_;
      spanner_ = 0;
    }
}

ENTER_DESCRIPTION(Cluster_spanner_engraver,
/* descr */	"Engraves a cluster using Spanner notation ",
/* creats*/	"ClusterSpanner ClusterSpannerBeacon",
/* accepts */	"cluster-note-event abort-event",
/* acks  */	"note-column-interface",
/* reads */	"",
/* write */	"");

