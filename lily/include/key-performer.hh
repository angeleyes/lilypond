/*
  key-performer.hh -- declare Key_performer

  source file of the GNU LilyPond music typesetter

  (c) 1997 Jan Nieuwenhuizen <jan@digicash.com>
*/

#ifndef KEY_PERFOMER_HH
#define KEY_PERFOMER_HH

#include "lily-proto.hh"
#include "performer.hh"

class Key_performer : public Performer {
public:
    DECLARE_MY_RUNTIME_TYPEINFO;
    Key_performer();
    ~Key_performer();

protected:
    void do_print() const;
    virtual bool do_try_request( Request* req_l );
    virtual void process_requests();

private:
    Key_change_req* key_req_l_;
};

#endif // KEY_PERFOMER_HH
