/*
  PROJECT: FlowerSoft C++ library
  FILE   : string-convert.cc

--*/


#include <assert.h>
#include <limits.h>
#include "libc-extension.hh"
#include "string.hh"
#include "string-convert.hh"

/**
   a safe length for stringconversion buffers

   worst case would be %f printing HUGE (or 1/HUGE), which is approx
   2e318, this number would have approx 318 zero's in its string.

   Should enlarge buff dynamically.
   @see
   man 3 snprintf
   */
static const int STRING_BUFFER_LEN=1024;

String
String_convert::bin2hex_str( String bin_str )
{
    String str;
    Byte const* byte_c_l = bin_str.byte_c_l();
    for ( int i = 0; i < bin_str.length_i(); i++ ) {
	str += (char)nibble2hex_byte( *byte_c_l >> 4 );
	str += (char)nibble2hex_byte( *byte_c_l++ );
    }
    return str;
}

int
String_convert::bin2_i( String bin_str )
{
    assert( bin_str.length_i() <= 4 );

    int result_i = 0;
    for ( int i = 0; i < bin_str.length_i(); i++ ) {
	result_i <<= 8;
	result_i += (Byte)bin_str[ i ];
    }
    return result_i;
}

// breendet imp from String
int
String_convert::dec2_i( String dec_str )
{
    if ( !dec_str.length_i() )
    	return 0;

    long l = 0;
    int conv = sscanf( dec_str.ch_c_l(), "%ld", &l );
    assert( conv );

    return (int)l;
}

String
String_convert::i64_str( I64 i64, char const* fmt)
{
    char buffer[STRING_BUFFER_LEN];
    snprintf(buffer, STRING_BUFFER_LEN,
	     (fmt ? fmt : "%Ld"), i64 );     // assume radix 10
    return String(buffer);

}
// breendet imp from String
double
String_convert::dec2_f( String dec_str )
{
    if ( !dec_str.length_i() )
    	return 0;
    double d = 0;
    int conv = sscanf( dec_str.ch_c_l(), "%lf", &d );
    assert( conv );
    return d;
}

int
String_convert::hex2bin_i( String hex_str, String& bin_str_r )
{
    if ( hex_str.length_i() % 2 )
        hex_str = "0" + hex_str;

    bin_str_r = "";
    Byte const* byte_c_l= hex_str.byte_c_l();
    int i = 0;
    while ( i < hex_str.length_i() ) {   
        int high_i = hex2nibble_i( *byte_c_l++ );
        int low_i = hex2nibble_i( *byte_c_l++ );
        if ( high_i < 0 || low_i < 0 )
            return 1; // illegal char
        bin_str_r += String( (char)( high_i << 4 | low_i ), 1 );
        i += 2;
    }
    return 0;
}

String 
String_convert::hex2bin_str( String hex_str )
{
    String str;
//  silly, asserts should alway be "on"!
//    assert( !hex2bin_i( hex_str, str ) );
    int error_i = hex2bin_i( hex_str, str );
    assert( !error_i );
    return str;
}

int 
String_convert::hex2nibble_i( Byte byte )
{
    if ( byte >= '0' && byte <= '9' )
        return byte - '0';
    if ( byte >= 'A' && byte <= 'F' )
        return byte - 'A' + 10;
    if ( byte >= 'a' && byte <= 'f')
        return byte - 'a' + 10;
    return -1;
}

// stupido.  Should use int_str()
String 
String_convert::i2dec_str( int i, int length_i, char ch )
{
    char fill_ch = ch;
    if ( fill_ch)
        fill_ch = '0';

    // ugh
    String dec_str( i );
    
    // ugh
    return String( fill_ch, length_i - dec_str.length_i() ) + dec_str;
}


// stupido.  Should use int_str()
String 
String_convert::u2hex_str( unsigned u, int length_i, char fill_ch )
{
    String str;
    if ( !u )
	str = "0";

#if 1 // both go...
    while ( u ) {
	str = String( (char)( ( u % 16 )["0123456789abcdef"] ) ) + str;
	u /= 16;
    }
#else
    str += int_str( u, "%x" );
#endif

    str = String( fill_ch, length_i - str.length_i() ) + str;
    while ( ( str.length_i() > length_i ) &&  ( str[ 0 ] == 'f' ) )
    	str = str.mid_str( 2, INT_MAX );

    return str;
}

String 
String_convert::i2hex_str( int i, int length_i, char fill_ch )
{
    return u2hex_str( (unsigned)i, length_i, fill_ch );
}

Byte
String_convert::nibble2hex_byte( Byte byte )
{
    if ( ( byte & 0x0f ) <= 9 )
	return ( byte & 0x0f ) + '0';
    else
	return ( byte & 0x0f ) - 10 + 'a';
}
/**
  Convert an integer to a string

  @param
  #fmt# is a printf style format, default assumes "%d" as format. 
  */
String
String_convert::int_str(int i, char const* fmt)
{
    char buffer[STRING_BUFFER_LEN];
    snprintf(buffer, STRING_BUFFER_LEN,
	     (fmt ? fmt : "%d"), i );     // assume radix 10
    return String(buffer);
}

/**
  Convert a double to a string.

  @param #fmt# is a printf style format, default assumes "%lf" as format
 */
String
String_convert::double_str(double f, char const* fmt)
{
    char buf[STRING_BUFFER_LEN]; 

    snprintf(buf, STRING_BUFFER_LEN, fmt ? fmt : "%f", f);
    return buf;
}

/**
  Make a string from a single character.

  @param
    #n# is a repetition count, default value is 1
 */
String
String_convert::char_str(char c, int n)
{
    n = n >= 0 ? n : 0;
    char* ch_p = new char[ n ];
    memset( ch_p, c, n );
    String s((Byte*)ch_p, n);
    delete ch_p;
    return s;
}

String
String_convert::rational_str(Rational r)
{
    char * n = Itoa(r.numerator()); // LEAK????
    
    String s = n;
    if (r.denominator() != 1) {
	char * d = Itoa(r.denominator());
	s +=  String( '/' ) + String(d);
	//delete d;
    }
/*    delete n;
    */
    return s;
}

String
String_convert::pointer_str(void const *l)
{
    I64 i64 = (I64)l;
    return String_convert::i64_str(i64,  "0x%0Lx");
}
