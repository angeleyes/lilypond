#include "warn.hh"
#include <stream.h>

void
error(String s)
{
    cerr <<  _("error: ") << s << "\n";

    exit(1);
}


void
warning(String m)
{
    cerr << _("warning: ") <<m <<endl;

}

void
message(String m)
{
    cerr << m<<endl;
}
