/*
  cluster.hh

  source file of the GNU LilyPond music typesetter

  (c) 2002--2007 Juergen Reuter <reuter@ipd.uka.de>
*/

#ifndef CLUSTER_HH
#define CLUSTER_HH

#include "stencil.hh"
#include "grob-interface.hh"

class Cluster
{
public:
  DECLARE_SCHEME_CALLBACK (print, (SCM));
  DECLARE_GROB_INTERFACE();
};

#endif // CLUSTER_HH

