/*
  paper-book.cc -- implement Paper_book

  source file of the GNU LilyPond music typesetter

  (c) 2004 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include "ly-module.hh"
#include "main.hh"
#include "output-def.hh"
#include "paper-book.hh"
#include "paper-outputter.hh"
#include "paper-score.hh"
#include "paper-system.hh"
#include "stencil.hh"
#include "warn.hh"

#include "ly-smobs.icc"

Paper_book::Paper_book ()
{
  pages_ = SCM_BOOL_F;
  lines_ = SCM_BOOL_F;
  header_ = SCM_EOL;
  
  bookpaper_ = 0;
  smobify_self ();
}

Paper_book::~Paper_book ()
{
}

IMPLEMENT_DEFAULT_EQUAL_P (Paper_book);
IMPLEMENT_SMOBS (Paper_book)
IMPLEMENT_TYPE_P (Paper_book, "ly:paper-book?")

SCM
Paper_book::mark_smob (SCM smob)
{
  Paper_book *b = (Paper_book*) SCM_CELL_WORD_1 (smob);
  for (int i = 0; i < b->score_lines_.size (); i++)
    b->score_lines_[i].gc_mark ();

  if (b->bookpaper_)
    scm_gc_mark (b->bookpaper_->self_scm ());
  scm_gc_mark (b->header_);
  scm_gc_mark (b->pages_);
  return b->lines_;
}

int
Paper_book::print_smob (SCM smob, SCM port, scm_print_state*)
{
  Paper_book *b = (Paper_book*) ly_cdr (smob);
     
  scm_puts ("#<", port);
  scm_puts (classname (b), port);
  scm_puts (" ", port);
  //scm_puts (b->, port);
  scm_puts (">", port);
  return 1;
}

Array<String>
split_string (String s, char c)
{
  Array<String> rv; 
  while (s.length ())
    {
      int i = s.index (c);
      
      if (i == 0)
	{
	  s = s.nomid_string (0, 1);
	  continue;
	}
      
      if (i < 0)
	i = s.length () ;

      rv.push (s.cut_string (0, i));
      s = s.nomid_string (0, i);
    }

  return rv;
}

SCM
dump_fields ()
{
  SCM fields = SCM_EOL;
  for (int i = dump_header_fieldnames_global.size (); i--; )
    fields
      = scm_cons (ly_symbol2scm (dump_header_fieldnames_global[i].to_str0 ()),
		  fields);
  return fields;
}

LY_DEFINE (ly_output_formats, "ly:output-formats",
	   0, 0, 0, (),
	   "Formats passed to --format as a list of strings, "
	   "used for the output.")
{
  Array<String> output_formats = split_string (output_format_global, ',');

  SCM lst = SCM_EOL;
  int output_formats_count = output_formats.size ();
  for (int i = 0; i < output_formats_count; i ++)
    lst = scm_cons (scm_makfrom0str (output_formats[i].to_str0 ()), lst);
  
  return lst; 
}

void
Paper_book::output (String outname)
{
  if (!score_lines_.size ())
    return;

  /* Generate all stencils to trigger font loads.  */
  pages ();
  
  SCM formats = ly_output_formats ();
  for (SCM s = formats; ly_c_pair_p (s); s = ly_cdr (s)) 
    {
      String format = ly_scm2string (ly_car (s));
      Paper_outputter *out = get_paper_outputter (outname + "." + format,
						  format);
  
      SCM scopes = SCM_EOL;
      if (ly_c_module_p (header_))
	scopes = scm_cons (header_, scopes);
  
      String mod_nm = "scm framework-" + format;
      
      SCM mod = scm_c_resolve_module (mod_nm.to_str0 ());
      SCM func = scm_c_module_lookup (mod, "output-framework");

      func = scm_variable_ref (func);
      scm_apply_0 (func, scm_list_n (out->self_scm (),
				     self_scm (),
				     scopes,
				     dump_fields (),
				     scm_makfrom0str (outname.to_str0 ()),
				     SCM_UNDEFINED));

      scm_gc_unprotect_object (out->self_scm ());
      progress_indication ("\n");
    }
}

void
Paper_book::classic_output (String outname)
{
  /* Generate all stencils to trigger font loads.  */
  lines ();

  // ugh code dup
  SCM scopes = SCM_EOL;
  if (ly_c_module_p (header_))
    scopes = scm_cons (header_, scopes);

  if (ly_c_module_p (score_lines_[0].header_))
    scopes = scm_cons (score_lines_[0].header_, scopes);
  //end ugh

  Array<String> output_formats = split_string (output_format_global, ',');

  for (int i = 0; i < output_formats.size (); i++)
    {
      String format = output_formats[i];
      String mod_nm = "scm framework-" + format;
      
      SCM mod = scm_c_resolve_module (mod_nm.to_str0 ());
      SCM func = scm_c_module_lookup (mod, "output-classic-framework");

      func = scm_variable_ref (func);
      
      Paper_outputter *out = get_paper_outputter (outname + "." + format,
						  format);

      scm_apply_0 (func, scm_list_5 (out->self_scm (), self_scm (), scopes,
				     dump_fields (),
				     scm_makfrom0str (outname.to_str0 ())));

      scm_gc_unprotect_object (out->self_scm ());
      progress_indication ("\n");
    }
}

LY_DEFINE (ly_paper_book_pages, "ly:paper-book-pages",
	  1, 0, 0, (SCM pb),
	  "Return pages in book PB.")
{
  return unsmob_paper_book(pb)->pages ();
}

LY_DEFINE (ly_paper_book_scopes, "ly:paper-book-scopes",
	  1, 0, 0, (SCM book),
	  "Return pages in paper book @var{book}.")
{
  Paper_book *pb = unsmob_paper_book(book);
  SCM_ASSERT_TYPE(pb, book, SCM_ARG1, __FUNCTION__, "Paper_book");
  
  SCM scopes = SCM_EOL;
  if (ly_c_module_p (pb->header_))
    scopes = scm_cons (pb->header_, scopes);
  
  return scopes;
}

LY_DEFINE (ly_paper_book_lines, "ly:paper-book-lines",
	   1, 0, 0, (SCM pb),
	   "Return lines in book PB.")
{
  return unsmob_paper_book (pb)->lines ();
}

LY_DEFINE (ly_paper_book_book_paper, "ly:paper-book-book-paper",
	  1, 0, 0, (SCM pb),
	  "Return pages in book PB.")
{
  return unsmob_paper_book (pb)->bookpaper_->self_scm ();
}

/* TODO: resurrect more complex user-tweaks for titling?  */
Stencil
Paper_book::book_title ()
{
  SCM title_func = bookpaper_->lookup_variable (ly_symbol2scm ("book-title"));
  Stencil title;

  SCM scopes = SCM_EOL;
  if (ly_c_module_p (header_))
    scopes = scm_cons (header_, scopes);

 
  SCM tit = SCM_EOL;
  if (ly_c_procedure_p (title_func))
    tit = scm_call_2 (title_func,
		     bookpaper_->self_scm (),
		     scopes);

  if (unsmob_stencil (tit))
    title = *unsmob_stencil (tit);

  if (!title.is_empty ())
    title.align_to (Y_AXIS, UP);
  
  return title;
}

Stencil
Paper_book::score_title (int i)
{
  SCM title_func = bookpaper_->lookup_variable (ly_symbol2scm ("score-title"));

  Stencil title;

  // ugh code dup
  SCM scopes = SCM_EOL;
  if (ly_c_module_p (header_))
    scopes = scm_cons (header_, scopes);

  if (ly_c_module_p (score_lines_[i].header_))
    scopes = scm_cons (score_lines_[i].header_, scopes);
  //end ugh

  SCM tit = SCM_EOL;
  if (ly_c_procedure_p (title_func))
    tit = scm_call_2 (title_func,
		     bookpaper_->self_scm (),
		     scopes);

  if (unsmob_stencil (tit))
    title = *unsmob_stencil (tit);

  if (!title.is_empty ())
    title.align_to (Y_AXIS, UP);
  
  return title;
}
  
SCM
Paper_book::lines ()
{
  if (lines_ != SCM_BOOL_F)
    return lines_;

  lines_ = SCM_EOL;
  Stencil title = book_title ();

  if (!title.is_empty ())
    {
      Paper_system *pl = new Paper_system (title, true);
      lines_ = scm_cons (pl->self_scm (), lines_);
      scm_gc_unprotect_object (pl->self_scm ());
    }
  
  int score_count = score_lines_.size ();
  for (int i = 0; i < score_count; i++)
    {
      Stencil title = score_title (i);      
      if (!title.is_empty ())
	{
	  Paper_system *pl = new Paper_system (title, true);
	  lines_ = scm_cons (pl->self_scm (), lines_);
	  scm_gc_unprotect_object (pl->self_scm ());
  	}
      
      if (scm_vector_p (score_lines_[i].lines_) == SCM_BOOL_T)
	{
	  // guh.	  
	  SCM line_list = scm_vector_to_list (score_lines_[i].lines_);

	  line_list = scm_reverse (line_list);
	  lines_ = scm_append (scm_list_2 (line_list, lines_));
	}
    }
  
  lines_ = scm_reverse (lines_);
  
  int i = 0;
  Paper_system *last = 0;
  for (SCM s = lines_; s != SCM_EOL; s = ly_cdr (s))
    {
      Paper_system * p = unsmob_paper_line (ly_car (s));
      p->number_ = ++i;

      if (last && last->is_title ())
	// ugh, hardcoded.	
	p->penalty_ = 10000;
      last = p;
    }
  
  return lines_;
}

SCM
Paper_book::pages ()
{
  if (SCM_BOOL_F != pages_)
    return pages_;

  pages_ = SCM_EOL;
  Output_def *paper = bookpaper_;
  SCM proc = paper->c_variable ("page-breaking");
  pages_ = scm_apply_0 (proc, scm_list_2 (lines (), self_scm ()));
  return pages_;
}


/****************************************************************/

Score_lines::Score_lines ()
{
  lines_ = SCM_EOL;
  header_ = SCM_EOL;
}

void
Score_lines::gc_mark ()
{
  scm_gc_mark (lines_);
  scm_gc_mark (header_);
}

