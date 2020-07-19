/*****************************************************************************
**
**  SRELL (std::regex-like library) version 2.730
**
**  Copyright (c) 2012-2020, Nozomu Katoo. All rights reserved.
**
**  Redistribution and use in source and binary forms, with or without
**  modification, are permitted provided that the following conditions are
**  met:
**
**  1. Redistributions of source code must retain the above copyright notice,
**     this list of conditions and the following disclaimer.
**
**  2. Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in the
**     documentation and/or other materials provided with the distribution.
**
**  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
**  IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
**  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
**  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
**  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
**  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
**  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
**  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
**  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
**  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
******************************************************************************
**/

#ifndef SRELL_REGEX_TEMPLATE_LIBRARY
#define SRELL_REGEX_TEMPLATE_LIBRARY

#include <stdexcept>
#include <climits>
#include <cwchar>
#include <string>
#include <locale>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <utility>
#include <vector>
#include <iterator>
#include <memory>
#include <algorithm>

#ifdef __cpp_unicode_characters
  #ifndef SRELL_CPP11_CHAR1632_ENABLED
  #define SRELL_CPP11_CHAR1632_ENABLED
  #endif
#endif
#ifdef __cpp_initializer_lists
#include <initializer_list>
  #ifndef SRELL_CPP11_INITIALIZER_LIST_ENABLED
  #define SRELL_CPP11_INITIALIZER_LIST_ENABLED
  #endif
#endif
#ifdef __cpp_rvalue_references
  #ifndef SRELL_CPP11_MOVE_ENABLED
  #define SRELL_CPP11_MOVE_ENABLED
  #endif
#endif
#ifdef SRELL_CPP11_MOVE_ENABLED
  #if defined(_MSC_VER) && _MSC_VER < 1900
  #define SRELL_NOEXCEPT
  #else
  #define SRELL_NOEXCEPT noexcept
  #endif
#endif
#ifdef __cpp_char8_t
  #ifndef SRELL_CPP20_CHAR8_ENABLED
	#ifdef __cpp_lib_char8_t
	#define SRELL_CPP20_CHAR8_ENABLED 2
	#else
	#define SRELL_CPP20_CHAR8_ENABLED 1
	#endif
  #endif
#endif

#ifdef SRELL_NO_UNICODE_DATA
//  If specified, prevents Unicode data from being output into a
//  resulting binary. Useful when either of icase search or the Unicode
//  property is not needed and want to reduce the size of a binary.
#define SRELL_NO_UNICODE_ICASE
#define SRELL_NO_UNICODE_PROPERTY
#endif

//  This macro may be removed in the future.
#ifdef SRELL_V1_COMPATIBLE
#ifndef SRELL_NO_UNICODE_PROPERTY
#define SRELL_NO_UNICODE_PROPERTY
#endif
#define SRELL_NO_NAMEDCAPTURE
#define SRELL_NO_SINGLELINE
#define SRELL_FIXEDWIDTHLOOKBEHIND
#endif

namespace srell
{
//  ["regex_constants.h" ...

	namespace regex_constants
	{
		enum syntax_option_type
		{
			icase      = 1 << 0,
			nosubs     = 1 << 1,
			optimize   = 1 << 2,
			collate    = 1 << 3,
			ECMAScript = 1 << 4,
			basic      = 1 << 5,
			extended   = 1 << 6,
			awk        = 1 << 7,
			grep       = 1 << 8,
			egrep      = 1 << 9,
			multiline  = 1 << 10,

			//  SRELL's extension.
			dotall     = 1 << 11	//  singleline.
		};

		inline syntax_option_type operator&(const syntax_option_type left, const syntax_option_type right)
		{
			return static_cast<syntax_option_type>(static_cast<int>(left) & static_cast<int>(right));
		}
		inline syntax_option_type operator|(const syntax_option_type left, const syntax_option_type right)
		{
			return static_cast<syntax_option_type>(static_cast<int>(left) | static_cast<int>(right));
		}
		inline syntax_option_type operator^(const syntax_option_type left, const syntax_option_type right)
		{
			return static_cast<syntax_option_type>(static_cast<int>(left) ^ static_cast<int>(right));
		}
		inline syntax_option_type operator~(const syntax_option_type b)
		{
			return static_cast<syntax_option_type>(~static_cast<int>(b));
		}
		inline syntax_option_type &operator&=(syntax_option_type &left, const syntax_option_type right)
		{
			left = left & right;
			return left;
		}
		inline syntax_option_type &operator|=(syntax_option_type &left, const syntax_option_type right)
		{
			left = left | right;
			return left;
		}
		inline syntax_option_type &operator^=(syntax_option_type &left, const syntax_option_type right)
		{
			left = left ^ right;
			return left;
		}
	}
	//  namespace regex_constants

	namespace regex_constants
	{
		enum match_flag_type
		{
			match_default     = 0,
			match_not_bol     = 1 <<  0,
			match_not_eol     = 1 <<  1,
			match_not_bow     = 1 <<  2,
			match_not_eow     = 1 <<  3,
			match_any         = 1 <<  4,
			match_not_null    = 1 <<  5,
			match_continuous  = 1 <<  6,
			match_prev_avail  = 1 <<  7,

			format_default    = 0,
			format_sed        = 1 <<  8,
			format_no_copy    = 1 <<  9,
			format_first_only = 1 << 10,

			//  For internal use.
			match_match_      = 1 << 11
		};

		inline match_flag_type operator&(const match_flag_type left, const match_flag_type right)
		{
			return static_cast<match_flag_type>(static_cast<int>(left) & static_cast<int>(right));
		}
		inline match_flag_type operator|(const match_flag_type left, const match_flag_type right)
		{
			return static_cast<match_flag_type>(static_cast<int>(left) | static_cast<int>(right));
		}
		inline match_flag_type operator^(const match_flag_type left, const match_flag_type right)
		{
			return static_cast<match_flag_type>(static_cast<int>(left) ^ static_cast<int>(right));
		}
		inline match_flag_type operator~(const match_flag_type b)
		{
			return static_cast<match_flag_type>(~static_cast<int>(b));
		}
		inline match_flag_type &operator&=(match_flag_type &left, const match_flag_type right)
		{
			left = left & right;
			return left;
		}
		inline match_flag_type &operator|=(match_flag_type &left, const match_flag_type right)
		{
			left = left | right;
			return left;
		}
		inline match_flag_type &operator^=(match_flag_type &left, const match_flag_type right)
		{
			left = left ^ right;
			return left;
		}
	}
	//  namespace regex_constants

	//  28.5, regex constants:
	namespace regex_constants
	{
//		typedef implementation defined error_type;
		typedef int error_type;

		static const error_type error_collate    = 100;
		static const error_type error_ctype      = 101;
		static const error_type error_escape     = 102;
		static const error_type error_backref    = 103;
		static const error_type error_brack      = 104;
		static const error_type error_paren      = 105;
		static const error_type error_brace      = 106;
		static const error_type error_badbrace   = 107;
		static const error_type error_range      = 108;
		static const error_type error_space      = 109;
		static const error_type error_badrepeat  = 110;
		static const error_type error_complexity = 111;
		static const error_type error_stack      = 112;

		//  SRELL's extension.
		static const error_type error_utf8       = 113;

#if defined(SRELL_FIXEDWIDTHLOOKBEHIND)
		static const error_type error_lookbehind = 200;
#endif
		static const error_type error_internal   = 999;
	}
	//  namespace regex_constants

//  ... "regex_constants.h"]
//  ["regex_error.hpp" ...

//  28.6, class regex_error:
class regex_error : public std::runtime_error
{
public:

	explicit regex_error(const regex_constants::error_type ecode)
		: std::runtime_error("regex_error")	//  added for error C2512.
		, ecode_(ecode)
	{
	}

	regex_constants::error_type code() const
	{
		return ecode_;
	}

private:

	regex_constants::error_type ecode_;
};

//  ... "regex_error.hpp"]
//  ["rei_type.h" ...

	namespace regex_internal
	{

#if defined(SRELL_CPP11_CHAR1632_ENABLED)

		typedef char32_t uchar21;

//#else	//  !defined(SRELL_CPP11_CHAR1632_ENABLED)
#elif defined(UINT_MAX) && UINT_MAX >= 0x1FFFFF

		typedef unsigned int uchar21;

#elif defined(ULONG_MAX) && ULONG_MAX >= 0x1FFFFF

		typedef unsigned long uchar21;

#else
#error could not find a suitable type for 21-bit Unicode integer values.
#endif	//  defined(SRELL_CPP11_CHAR1632_ENABLED)

	}	//  regex_internal

//  ... "rei_type.h"]
//  ["rei_constants.h" ...

	namespace regex_internal
	{
		enum re_state_type
		{
			st_character,               //  0x00
			st_character_class,         //  0x01

			st_epsilon,                 //  0x02

			st_check_counter,           //  0x03
			st_increment_counter,       //  0x04
			st_decrement_counter,       //  0x05
			st_save_and_reset_counter,  //  0x06
			st_restore_counter,         //  0x07

			st_roundbracket_open,       //  0x08
			st_roundbracket_pop,        //  0x09
			st_roundbracket_close,      //  0x0a

			st_repeat_in_push,          //  0x0b
			st_repeat_in_pop,           //  0x0c
			st_check_0_width_repeat,    //  0x0d

			st_backreference,           //  0x0e

			st_lookaround_open,         //  0x0f

			st_lookaround_pop,          //  0x10

			st_bol,                     //  0x11
			st_eol,                     //  0x12
			st_boundary,                //  0x13

			st_success,                 //  0x14

#if !defined(SRELLDBG_NO_NEXTPOS_OPT)
			st_move_nextpos,            //  0x15
#endif

			st_lookaround_close        = st_success,
			st_zero_width_boundary     = st_lookaround_open,
			//st_assertions_boundary     = st_lookaround_open,

//			st_independent_open,
//			st_independent_close,       //  = st_success,
		};
		//  re_state_type

		namespace constants
		{
			static const uchar21 unicode_max_codepoint = 0x10ffff;
			static const uchar21 invalid_u21value = static_cast<uchar21>(~0);
		}
		//  constants

		namespace meta_char
		{
			static const uchar21 mc_exclam = 0x21;	//  '!'
			static const uchar21 mc_dollar = 0x24;	//  '$'
			static const uchar21 mc_rbraop = 0x28;	//  '('
			static const uchar21 mc_rbracl = 0x29;	//  ')'
			static const uchar21 mc_astrsk = 0x2a;	//  '*'
			static const uchar21 mc_plus   = 0x2b;	//  '+'
			static const uchar21 mc_comma  = 0x2c;	//  ','
			static const uchar21 mc_minus  = 0x2d;	//  '-'
			static const uchar21 mc_period = 0x2e;	//  '.'
			static const uchar21 mc_colon  = 0x3a;	//  ':'
			static const uchar21 mc_lt = 0x3c;		//  '<'
			static const uchar21 mc_eq = 0x3d;		//  '='
			static const uchar21 mc_gt = 0x3e;		//  '>'
			static const uchar21 mc_query  = 0x3f;	//  '?'
			static const uchar21 mc_sbraop = 0x5b;	//  '['
			static const uchar21 mc_escape = 0x5c;	//  '\\'
			static const uchar21 mc_sbracl = 0x5d;	//  ']'
			static const uchar21 mc_caret  = 0x5e;	//  '^'
			static const uchar21 mc_cbraop = 0x7b;	//  '{'
			static const uchar21 mc_bar    = 0x7c;	//  '|'
			static const uchar21 mc_cbracl = 0x7d;	//  '}'
		}
		//  meta_char

		namespace char_ctrl
		{
			static const uchar21 cc_nul  = 0x00;	//  '\0'	//0x00:NUL
//			static const uchar21 cc_bel  = 0x07;	//  '\a'	//0x07:BEL
			static const uchar21 cc_bs   = 0x08;	//  '\b'	//0x08:BS
			static const uchar21 cc_htab = 0x09;	//  '\t'	//0x09:HT
			static const uchar21 cc_nl   = 0x0a;	//  '\n'	//0x0a:LF
			static const uchar21 cc_vtab = 0x0b;	//  '\v'	//0x0b:VT
			static const uchar21 cc_ff   = 0x0c;	//  '\f'	//0x0c:FF
			static const uchar21 cc_cr   = 0x0d;	//  '\r'	//0x0d:CR
//			static const uchar21 cc_esc  = 0x1b;	//  '\x1b'	//0x1b:ESC
		}
		//  char_ctrl

		namespace char_alnum
		{
			static const uchar21 ch_0 = 0x30;	//  '0'
			static const uchar21 ch_1 = 0x31;	//  '1'
			static const uchar21 ch_7 = 0x37;	//  '7'
			static const uchar21 ch_8 = 0x38;	//  '8'
			static const uchar21 ch_9 = 0x39;	//  '9'
			static const uchar21 ch_A = 0x41;	//  'A'
			static const uchar21 ch_B = 0x42;	//  'B'
			static const uchar21 ch_D = 0x44;	//  'D'
			static const uchar21 ch_F = 0x46;	//  'F'
//			static const uchar21 ch_G = 0x47;	//  'G'
			static const uchar21 ch_P = 0x50;	//  'P'
			static const uchar21 ch_S = 0x53;	//  'S'
			static const uchar21 ch_W = 0x57;	//  'W'
			static const uchar21 ch_Z = 0x5a;	//  'Z'
			static const uchar21 ch_a = 0x61;	//  'a'
			static const uchar21 ch_b = 0x62;	//  'b'
			static const uchar21 ch_c = 0x63;	//  'c'
			static const uchar21 ch_d = 0x64;	//  'd'
//			static const uchar21 ch_e = 0x65;	//  'e'
			static const uchar21 ch_f = 0x66;	//  'f'
			static const uchar21 ch_k = 0x6b;	//  'k'
			static const uchar21 ch_n = 0x6e;	//  'n'
			static const uchar21 ch_p = 0x70;	//  'p'
			static const uchar21 ch_r = 0x72;	//  'r'
			static const uchar21 ch_s = 0x73;	//  's'
			static const uchar21 ch_t = 0x74;	//  't'
			static const uchar21 ch_u = 0x75;	//  'u'
			static const uchar21 ch_v = 0x76;	//  'v'
			static const uchar21 ch_w = 0x77;	//  'w'
			static const uchar21 ch_x = 0x78;	//  'x'
			static const uchar21 ch_z = 0x7a;	//  'z'
		}
		//  char_alnum

		namespace char_other
		{
			static const uchar21 co_sp    = 0x20;	//  ' '
			static const uchar21 co_amp   = 0x26;	//  '&'
			static const uchar21 co_apos  = 0x27;	//  '\''
			static const uchar21 co_slash = 0x2f;	//  '/'
			static const uchar21 co_ll    = 0x5f;	//  '_'
			static const uchar21 co_grav  = 0x60;	//  '`'
		}
		//  char_other
	}
	//  namespace regex_internal

//  ... "rei_constants.h"]
//  ["rei_utf_traits.hpp" ...

	namespace regex_internal
	{

template <typename charT>
struct utf_traits_core
{
public:

	static const std::size_t maxseqlen = 1;

	static const std::size_t bitsetsize = 0x100;
	static const uchar21 bitsetmask = 0xff;
	static const uchar21 cumask = 0xff;

	//  *iter
	template <typename ForwardIterator>
	static uchar21 codepoint(ForwardIterator begin, const ForwardIterator &/* end */)
	{
		return static_cast<uchar21>(*begin);
		//  Caller is responsible for begin != end.
	}

	//  *iter++
	template <typename ForwardIterator>
	static uchar21 codepoint_inc(ForwardIterator &begin, const ForwardIterator &/* end */)
	{
		return static_cast<uchar21>(*begin++);
		//  Caller is responsible for begin != end.
	}

	//  iter2 = iter; return *--iter2;
	template <typename BidirectionalIterator>
	static uchar21 prevcodepoint(BidirectionalIterator cur, const BidirectionalIterator &/* begin */)
	{
		return static_cast<uchar21>(*--cur);
	}

	//  *--iter
	template <typename BidirectionalIterator>
	static uchar21 dec_codepoint(BidirectionalIterator &cur, const BidirectionalIterator &/* begin */)
	{
		return static_cast<uchar21>(*--cur);
		//  Caller is responsible for cur != begin.
	}

#if !defined(SRELLDBG_NO_BMH)
	template <typename charT2>
	static bool is_trailing(const charT2 /* cu */)
	{
		return false;
	}
#endif	//  !defined(SRELLDBG_NO_BMH)

	static uchar21 to_codeunits(charT out[maxseqlen], uchar21 cp)
	{
		out[0] = static_cast<charT>(cp);
		return 1;
	}

	static uchar21 firstcodeunit(const uchar21 cp)
	{
		return cp;
	}

	template <typename ForwardIterator>
	static bool seek_charboundary(ForwardIterator &begin, const ForwardIterator &end)
	{
		return begin != end;
	}
};	//  utf_traits_core

//  common and utf-32.
template <typename charT>
struct utf_traits : public utf_traits_core<charT>
{
	static const std::size_t bitsetsize = 0x10000;
	static const uchar21 bitsetmask = 0xffff;
	static const uchar21 cumask = 0x1fffff;
};	//  utf_traits

//  utf-8 specific.
template <typename charT>
struct utf8_traits : public utf_traits_core<charT>
{
public:

	//  utf-8 specific.
	static const std::size_t maxseqlen = 4;

	template <typename ForwardIterator>
	static uchar21 codepoint(ForwardIterator begin, const ForwardIterator &end)
	{
//		return codepoint_inc(begin, end);
		uchar21 codepoint = static_cast<uchar21>(*begin);

		if ((codepoint & 0x80) == 0)	//  1 octet.
		{
			return static_cast<uchar21>(codepoint & 0x7f);
		}
		else if (++begin != end)
		{
			if ((codepoint & 0xe0) == 0xc0)	//  2 octets.
			{
				if ((*begin & 0xc0) == 0x80)
					return static_cast<uchar21>(((codepoint << 6) | (*begin & 0x3f)) & 0x7ff);
			}
			else if ((codepoint & 0xf0) == 0xe0)	//  3 octets.
			{
				if ((*begin & 0xc0) == 0x80)
				{
					codepoint = static_cast<uchar21>((codepoint << 6) | (*begin & 0x3f));

					if (++begin != end && (*begin & 0xc0) == 0x80)
						return static_cast<uchar21>(((codepoint << 6) | (*begin & 0x3f)) & 0xffff);
				}
			}
			else if ((codepoint & 0xf8) == 0xf0)	//  4 octets.
			{
				if ((*begin & 0xc0) == 0x80)
				{
					codepoint = static_cast<uchar21>((codepoint << 6) | (*begin & 0x3f));

					if (++begin != end && (*begin & 0xc0) == 0x80)
					{
						codepoint = static_cast<uchar21>((codepoint << 6) | (*begin & 0x3f));

						if (++begin != end && (*begin & 0xc0) == 0x80)
							return static_cast<uchar21>(((codepoint << 6) | (*begin & 0x3f)) & 0x1fffff);
					}
				}
			}
//			else	//  80-bf, f8-ff: invalid.
		}
		return regex_internal::constants::invalid_u21value;	//replacement_character;
	}

	template <typename ForwardIterator>
	static uchar21 codepoint_inc(ForwardIterator &begin, const ForwardIterator &end)
	{
		uchar21 codepoint = static_cast<uchar21>(*begin++);

		if ((codepoint & 0x80) == 0)	//  1 octet.
		{
			return static_cast<uchar21>(codepoint & 0x7f);
		}
		else if (begin != end)
		{
			if ((codepoint & 0xe0) == 0xc0)	//  2 octets.
			{
				//  c080-c1bf: invalid. 00-7F.
				//  c280-dfbf: valid. 080-7FF.

				if ((*begin & 0xc0) == 0x80)	//  1 1111 11 1111
					return static_cast<uchar21>(((codepoint << 6) | (*begin++ & 0x3f)) & 0x7ff);
			}
			else if ((codepoint & 0xf0) == 0xe0)	//  3 octets.
			{
				//  e08080-e09fbf: invalid. 000-7FF.
				//  e0a080-efbfbf: valid. 0800-FFFF.

				if ((*begin & 0xc0) == 0x80)
				{
					codepoint = static_cast<uchar21>((codepoint << 6) | (*begin & 0x3f));

					if (++begin != end && (*begin & 0xc0) == 0x80)	//  1111 11 1111 11 1111
						return static_cast<uchar21>(((codepoint << 6) | (*begin++ & 0x3f)) & 0xffff);
				}
			}
			else if ((codepoint & 0xf8) == 0xf0)	//  4 octets.
			{
				//  f0808080-f08fbfbf: invalid. 0000-FFFF.
				//  f0908080-f3bfbfbf: valid. 10000-FFFFF.
				//  f4808080-f48fbfbf: valid. 100000-10FFFF.
				//  f4908080-f7bfbfbf: invalid. 110000-1FFFFF.

				if ((*begin & 0xc0) == 0x80)
				{
					codepoint = static_cast<uchar21>((codepoint << 6) | (*begin & 0x3f));

					if (++begin != end && (*begin & 0xc0) == 0x80)
					{
						codepoint = static_cast<uchar21>((codepoint << 6) | (*begin & 0x3f));

																		//  100 00 | 1111 11 11|11 11 1111
						if (++begin != end && (*begin & 0xc0) == 0x80)	//  011 11 | 1111 11 11|11 11 1111
							return static_cast<uchar21>(((codepoint << 6) | (*begin++ & 0x3f)) & 0x1fffff);
					}
				}
			}
//			else	//  80-bf, f5-ff: invalid.
		}

		return regex_internal::constants::invalid_u21value;	//replacement_character;
	}

	template <typename BidirectionalIterator>
	static uchar21 prevcodepoint(BidirectionalIterator cur, const BidirectionalIterator &begin)
	{
		if ((*--cur & 0x80) == 0)
			return static_cast<uchar21>(*cur & 0x7f);

		if ((*cur & 0xc0) == 0x80 && cur != begin)
		{
			uchar21 codepoint = static_cast<uchar21>(*cur & 0x3f);

			if ((*--cur & 0xe0) == 0xc0)
				return static_cast<uchar21>((codepoint | (*cur << 6)) & 0x7ff);

			if ((*cur & 0xc0) == 0x80 && cur != begin)
			{
				codepoint = static_cast<uchar21>(codepoint | ((*cur & 0x3f) << 6));

				if ((*--cur & 0xf0) == 0xe0)
					return static_cast<uchar21>((codepoint | (*cur << 12)) & 0xffff);

				if ((*cur & 0xc0) == 0x80 && cur != begin)
				{
					codepoint = static_cast<uchar21>(codepoint | ((*cur & 0x3f) << 12));

					if ((*--cur & 0xf8) == 0xf0)
						return static_cast<uchar21>((codepoint | (*cur << 18)) & 0x1fffff);
				}
			}
		}
		return regex_internal::constants::invalid_u21value;	//replacement_character;
	}

	template <typename BidirectionalIterator>
	static uchar21 dec_codepoint(BidirectionalIterator &cur, const BidirectionalIterator &begin)
	{
		if ((*--cur & 0x80) == 0)
			return static_cast<uchar21>(*cur & 0x7f);

		if ((*cur & 0xc0) == 0x80 && cur != begin)
		{
			uchar21 codepoint = static_cast<uchar21>(*cur & 0x3f);

			if ((*--cur & 0xe0) == 0xc0)
				return static_cast<uchar21>((codepoint | (*cur << 6)) & 0x7ff);

			if ((*cur & 0xc0) == 0x80 && cur != begin)	//  [\x80-\xbf]{2}.
			{
				codepoint = static_cast<uchar21>(codepoint | ((*cur & 0x3f) << 6));

				if ((*--cur & 0xf0) == 0xe0)
					return static_cast<uchar21>((codepoint | (*cur << 12)) & 0xffff);

				if ((*cur & 0xc0) == 0x80 && cur != begin)	//  [\x80-\xbf]{3}.
				{
					codepoint = static_cast<uchar21>(codepoint | ((*cur & 0x3f) << 12));

					if ((*--cur & 0xf8) == 0xf0)
						return static_cast<uchar21>((codepoint | (*cur << 18)) & 0x1fffff);
					//else	//  [\0-\xef\xf8-\xff][\x80-\xbf]{3}.

					//  Sequences [\xc0-\xdf][\x80-\xbf] and [\xe0-\xef][\x80-\xbf]{2} are valid.
					//  To give a chance to them, rewinds cur.
					++cur;
				}
				//else	//  [\0-\x7f\xc0-\xdf\xf0-\xff][\x80-\xbf]{2}.
				++cur;	//  Sequence [\xc0-\xdf][\x80-\xbf] is valid. Rewinds to give a chance to it.
			}
			//else	//  [\0-\x7f\xe0-\xff][\x80-\xbf].
			++cur;	//  Rewinds to give a chance to [\0-\x7f].
		}
		//else	//  [\xc0-\xff].

		return regex_internal::constants::invalid_u21value;	//replacement_character;
	}

#if !defined(SRELLDBG_NO_BMH)
	template <typename charT2>
	static bool is_trailing(const charT2 cu)
	{
		return (cu & 0xc0) == 0x80;
	}
#endif	//  !defined(SRELLDBG_NO_BMH)

	static uchar21 to_codeunits(charT out[maxseqlen], uchar21 cp)
	{
		if (cp < 0x80)
		{
			out[0] = static_cast<charT>(cp);
			return 1;
		}
		else if (cp < 0x800)
		{
			out[0] = static_cast<charT>(((cp >> 6) & 0x1f) | 0xc0);
			out[1] = static_cast<charT>((cp & 0x3f) | 0x80);
			return 2;
		}
		else if (cp < 0x10000)
		{
			out[0] = static_cast<charT>(((cp >> 12) & 0x0f) | 0xe0);
			out[1] = static_cast<charT>(((cp >> 6) & 0x3f) | 0x80);
			out[2] = static_cast<charT>((cp & 0x3f) | 0x80);
			return 3;
		}
//		else	//  if (cp < 0x110000)
		{
			out[0] = static_cast<charT>(((cp >> 18) & 0x07) | 0xf0);
			out[1] = static_cast<charT>(((cp >> 12) & 0x3f) | 0x80);
			out[2] = static_cast<charT>(((cp >> 6) & 0x3f) | 0x80);
			out[3] = static_cast<charT>((cp & 0x3f) | 0x80);
			return 4;
		}
	}

	static uchar21 firstcodeunit(const uchar21 cp)
	{
		if (cp < 0x80)
			return cp;

		if (cp < 0x800)
			return static_cast<uchar21>(((cp >> 6) & 0x1f) | 0xc0);

		if (cp < 0x10000)
			return static_cast<uchar21>(((cp >> 12) & 0x0f) | 0xe0);

		return static_cast<uchar21>(((cp >> 18) & 0x07) | 0xf0);
	}

	template <typename ForwardIterator>
	static bool seek_charboundary(ForwardIterator &begin, const ForwardIterator &end)
	{
		for (; begin != end; ++begin)
		{
//			if ((*begin & 0xc0) != 0x80 && (*begin & 0xf8) != 0xf8)	//  00-7f, c0-f7.
			if ((*begin & 0xc0) != 0x80)	//  00-7f, c0-ff.
				return true;
		}
		return false;
	}
};	//  utf8_traits

//  utf-16 specific.
template <typename charT>
struct utf16_traits : public utf_traits_core<charT>
{
public:

	//  utf-16 specific.
	static const std::size_t maxseqlen = 2;

	static const std::size_t bitsetsize = 0x10000;
	static const uchar21 bitsetmask = 0xffff;
	static const uchar21 cumask = 0xffff;

	template <typename ForwardIterator>
	static uchar21 codepoint(ForwardIterator begin, const ForwardIterator &end)
	{
		const uchar21 codeunit = *begin;

		if ((codeunit & 0xdc00) != 0xd800)
			return static_cast<uchar21>(codeunit & 0xffff);

		if (++begin != end && (*begin & 0xdc00) == 0xdc00)
			return static_cast<uchar21>((((codeunit & 0x3ff) << 10) | (*begin & 0x3ff)) + 0x10000);

		return static_cast<uchar21>(codeunit & 0xffff);
	}

	template <typename ForwardIterator>
	static uchar21 codepoint_inc(ForwardIterator &begin, const ForwardIterator &end)
	{
		const uchar21 codeunit = *begin++;

		if ((codeunit & 0xdc00) != 0xd800)
			return static_cast<uchar21>(codeunit & 0xffff);

		if (begin != end && (*begin & 0xdc00) == 0xdc00)
			return static_cast<uchar21>((((codeunit & 0x3ff) << 10) | (*begin++ & 0x3ff)) + 0x10000);

		return static_cast<uchar21>(codeunit & 0xffff);
	}

	template <typename BidirectionalIterator>
	static uchar21 prevcodepoint(BidirectionalIterator cur, const BidirectionalIterator &begin)
	{
		const uchar21 codeunit = *--cur;

		if ((codeunit & 0xdc00) != 0xdc00 || cur == begin)
			return static_cast<uchar21>(codeunit & 0xffff);

		if ((*--cur & 0xdc00) == 0xd800)
			return static_cast<uchar21>((((*cur & 0x3ff) << 10) | (codeunit & 0x3ff)) + 0x10000);

		return static_cast<uchar21>(codeunit & 0xffff);
	}

	template <typename BidirectionalIterator>
	static uchar21 dec_codepoint(BidirectionalIterator &cur, const BidirectionalIterator &begin)
	{
		const uchar21 codeunit = *--cur;

		if ((codeunit & 0xdc00) != 0xdc00 || cur == begin)
			return static_cast<uchar21>(codeunit & 0xffff);

		if ((*--cur & 0xdc00) == 0xd800)
			return static_cast<uchar21>((((*cur & 0x3ff) << 10) | (codeunit & 0x3ff)) + 0x10000);
		//else	//  (codeunit & 0xdc00) == 0xdc00 && (*cur & 0xdc00) != 0xd800

		++cur;

		return static_cast<uchar21>(codeunit & 0xffff);
	}

#if !defined(SRELLDBG_NO_BMH)
	template <typename charT2>
	static bool is_trailing(const charT2 cu)
	{
		return (cu & 0xdc00) == 0xdc00;
	}
#endif	//  !defined(SRELLDBG_NO_BMH)

	static uchar21 to_codeunits(charT out[maxseqlen], uchar21 cp)
	{
		if (cp < 0x10000)
		{
			out[0] = static_cast<charT>(cp);
			return 1;
		}
//		else	//  if (cp < 0x110000)
		{
			cp -= 0x10000;
			out[0] = static_cast<charT>(((cp >> 10) & 0x3ff) | 0xd800);
			out[1] = static_cast<charT>((cp & 0x3ff) | 0xdc00);
			return 2;
		}
	}

	static uchar21 firstcodeunit(const uchar21 cp)
	{
		if (cp < 0x10000)
			return cp;

		return static_cast<uchar21>((cp >> 10) + 0xd7c0);
			//  aaaaa bbbbcccc ddddeeee -> AA AAbb bbcc/cc dddd eeee where AAAA = aaaaa - 1.
	}

	template <typename ForwardIterator>
	static bool seek_charboundary(ForwardIterator &begin, const ForwardIterator &end)
	{
		for (; begin != end; ++begin)
		{
			if ((*begin & 0xdc00) != 0xdc00)
				return true;
		}
		return false;
	}
};	//  utf16_traits

//  specialisation for char.
template <>
struct utf_traits<char> : public utf_traits_core<char>
{
public:

	template <typename ForwardIterator>
	static uchar21 codepoint(ForwardIterator begin, const ForwardIterator &/* end */)
	{
		return static_cast<uchar21>(static_cast<unsigned char>(*begin));
	}

	template <typename ForwardIterator>
	static uchar21 codepoint_inc(ForwardIterator &begin, const ForwardIterator &/* end */)
	{
		return static_cast<uchar21>(static_cast<unsigned char>(*begin++));
	}

	template <typename BidirectionalIterator>
	static uchar21 prevcodepoint(BidirectionalIterator cur, const BidirectionalIterator &/* begin */)
	{
		return static_cast<uchar21>(static_cast<unsigned char>(*--cur));
	}

	template <typename BidirectionalIterator>
	static uchar21 dec_codepoint(BidirectionalIterator &cur, const BidirectionalIterator &/* begin */)
	{
		return static_cast<uchar21>(static_cast<unsigned char>(*--cur));
	}

#if !defined(SRELLDBG_NO_BMH)
#endif	//  !defined(SRELLDBG_NO_BMH)
};	//  utf_traits<char>

//  specialisation for signed char.
template <>
struct utf_traits<signed char> : public utf_traits<char>
{
};

//  (signed) short, (signed) int, (signed) long, (signed) long long, ...

#if defined(SRELL_CPP11_CHAR1632_ENABLED)
template <>
struct utf_traits<char16_t> : public utf16_traits<char16_t>
{
};
#endif

#if defined(SRELL_CPP20_CHAR8_ENABLED)
template <>
struct utf_traits<char8_t> : public utf8_traits<char8_t>
{
};
#endif

	}	//  regex_internal

//  ... "rei_utf_traits.hpp"]
//  ["regex_traits.hpp" ...

//  28.7, class template regex_traits:
template <class charT>
struct regex_traits
{
public:

	typedef charT char_type;
	typedef std::basic_string<char_type> string_type;
	typedef std::locale locale_type;
//	typedef bitmask_type char_class_type;
	typedef unsigned int char_class_type;

	typedef regex_internal::utf_traits<charT> utf_traits;

public:

//	regex_traits();

	static std::size_t length(const char_type *p)
	{
		return std::char_traits<charT>::length(p);
	}

	charT translate(const charT c) const
	{
		return c;
	}

	charT translate_nocase(const charT c) const
	{
		return c;
	}

	template <class ForwardIterator>
	string_type transform(ForwardIterator first, ForwardIterator last) const
	{
		return string_type(first, last);
	}

	template <class ForwardIterator>
	string_type transform_primary(ForwardIterator first, ForwardIterator last) const
	{
		return string_type(first, last);
	}

	template <class ForwardIterator>
	string_type lookup_collatename(ForwardIterator first, ForwardIterator last) const
	{
		return string_type(first, last);
	}

	template <class ForwardIterator>
	char_class_type lookup_classname(ForwardIterator /* first */, ForwardIterator /* last */, bool /* icase */ = false) const
	{
		return static_cast<char_class_type>(0);
	}

	bool isctype(const charT /* c */, const char_class_type /* f */) const
	{
		return false;
	}

	int value(const charT /* ch */, const int /* radix */) const
	{
		return -1;
	}

	locale_type imbue(const locale_type /* l */)
	{
		return locale_type();
	}

	locale_type getloc() const
	{
		return locale_type();
	}
};	//  regex_traits

template <class charT>
struct u8regex_traits : public regex_traits<charT>
{
	typedef regex_internal::utf8_traits<charT> utf_traits;
};

template <class charT>
struct u16regex_traits : public regex_traits<charT>
{
	typedef regex_internal::utf16_traits<charT> utf_traits;
};

//  ... "regex_traits.hpp"]
//  ["rei_memory.hpp" ...

	namespace regex_internal
	{
/*
 *  Similar to std::basic_string, except for:
 *    a. only allocates memory, does not initialise it.
 *    b. uses realloc() to avoid moving data as much as possible when
 *       resizing an allocated buffer.
 */
template <typename ElemT>
class simple_array
{
public:

	typedef ElemT value_type;
	typedef std::size_t size_type;
	typedef ElemT &reference;
	typedef const ElemT &const_reference;
	typedef ElemT *pointer;
	typedef const ElemT *const_pointer;

	static const size_type npos = static_cast<size_type>(-1);

public:

	simple_array()
		: buffer_(NULL)
		, size_(0)
		, capacity_(0)
	{
	}

	simple_array(const size_type initsize)
		: buffer_(static_cast<pointer>(std::malloc(initsize * sizeof (ElemT))))
		, size_(initsize)
		, capacity_(initsize)
	{
		if (buffer_ == NULL)
		{
			size_ = capacity_ = 0;
			throw std::bad_alloc();
		}
	}

	simple_array(const simple_array &right, size_type pos, size_type size = npos)
	{
		if (pos > right.size_)
			pos = right.size_;

		if (size > right.size_ - pos)
			size = right.size_ - pos;

		buffer_ = static_cast<pointer>(std::malloc(size * sizeof (ElemT)));

		if (buffer_ != NULL)
		{
			for (size_ = 0; size_ < size; ++size_, ++pos)
				buffer_[size_] = right[pos];

			capacity_ = size_;
			return;
		}
		size_ = capacity_ = 0;
		throw std::bad_alloc();
	}

	simple_array(const simple_array &right)
		: buffer_(NULL)
		, size_(0)
		, capacity_(0)
	{
		operator=(right);
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	simple_array(simple_array &&right) SRELL_NOEXCEPT
		: buffer_(right.buffer_)
		, size_(right.size_)
		, capacity_(right.capacity_)
	{
		right.size_ = 0;
		right.capacity_ = 0;
		right.buffer_ = NULL;
	}
#endif

	simple_array &operator=(const simple_array &right)
	{
		if (this != &right)
		{
			resize(right.size_);
			for (size_type i = 0; i < right.size_; ++i)
				buffer_[i] = right.buffer_[i];
		}
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	simple_array &operator=(simple_array &&right) SRELL_NOEXCEPT
	{
		if (this != &right)
		{
			this->size_ = right.size_;
			this->capacity_ = right.capacity_;
			this->buffer_ = right.buffer_;

			right.size_ = 0;
			right.capacity_ = 0;
			right.buffer_ = NULL;
		}
		return *this;
	}
#endif

	~simple_array()
	{
		if (buffer_ != NULL)
			std::free(buffer_);
	}

	size_type size() const
	{
		return size_;
	}

	void clear()
	{
		size_ = 0;
	}

	void resize(const size_type newsize)
	{
		if (newsize > capacity_)
			reserve(newsize);

		size_ = newsize;
	}

	void resize(const size_type newsize, const ElemT &type)
	{
		size_type oldsize = size_;

		resize(newsize);
		for (; oldsize < size_; ++oldsize)
			buffer_[oldsize] = type;
	}

	reference operator[](const size_type pos)
	{
		return buffer_[pos];
	}

	const_reference operator[](const size_type pos) const
	{
		return buffer_[pos];
	}

	void push_back(const_reference n)
	{
		const size_type oldsize = size_;

		if (++size_ > capacity_)
			reserve(size_);

		buffer_[oldsize] = n;
	}

	const_reference back() const
	{
		return buffer_[size_ - 1];
	}

	reference back()
	{
		return buffer_[size_ - 1];
	}

	void pop_back()
	{
		--size_;
	}

	simple_array &operator+=(const simple_array &right)
	{
		return append(right);
	}

	simple_array &append(const size_type size, const ElemT &type)
	{
		resize(size_ + size, type);
		return *this;
	}

	simple_array &append(const simple_array &right)
	{
		size_type oldsize = size_;

		resize(size_ + right.size_);
		for (size_type i = 0; i < right.size_; ++i, ++oldsize)
			buffer_[oldsize] = right.buffer_[i];

		return *this;
	}

	//  For rei_char_class class.
	void erase(const size_type pos)
	{
		if (pos < size_)
		{
			std::memmove(buffer_ + pos, buffer_ + pos + 1, (size_ - pos - 1) * sizeof (ElemT));
			--size_;
		}
	}

	//  For rei_compiler class.
	void insert(const size_type pos, const ElemT &type)
	{
		move_forward(pos, 1);
		buffer_[pos] = type;
	}

	void insert(size_type pos, const simple_array &right)
	{
		move_forward(pos, right.size_);
		for (size_type i = 0; i < right.size_; ++i, ++pos)
			buffer_[pos] = right.buffer_[i];
	}

	simple_array &replace(size_type pos, size_type count, const simple_array &right)
	{
		if (count < right.size_)
			move_forward(pos + count, right.size_ - count);
		else if (count > right.size_)
		{
			const pointer base = buffer_ + pos;

			std::memmove(base + right.size_, base + count, (size_ - pos - count) * sizeof (ElemT));
			size_ -= count - right.size_;
		}

		for (size_type i = 0; i < right.size_; ++pos, ++i)
			buffer_[pos] = right[i];

		return *this;
	}

	size_type find(const value_type c, size_type pos = 0) const
	{
		for (; pos <= size_; ++pos)
			if (buffer_[pos] == c)
				return pos;

		return npos;
	}

	const_pointer data() const
	{
		return buffer_;
	}

	int compare(size_type pos, const size_type count1, const_pointer p, const size_type count2) const
	{
		size_type count = count1 <= count2 ? count1 : count2;

		for (; count; ++pos, ++p, --count)
		{
			const value_type &v = buffer_[pos];
			if (v != *p)
				return v < *p ? -1 : 1;
		}
		return count1 == count2 ? 0 : (count1 < count2 ? -1 : 1);
	}

	size_type max_size() const
	{
		return maxsize_;
	}

	void swap(simple_array &right)
	{
		if (this != &right)
		{
			const pointer tmpbuffer = this->buffer_;
			const size_type tmpsize = this->size_;
			const size_type tmpcapacity = this->capacity_;

			this->buffer_ = right.buffer_;
			this->size_ = right.size_;
			this->capacity_ = right.capacity_;

			right.buffer_ = tmpbuffer;
			right.size_ = tmpsize;
			right.capacity_ = tmpcapacity;
		}
	}

private:

	void reserve(const size_type newsize)
	{
//		if (newsize > capacity_)
		{
			if (newsize <= maxsize_)
			{
//				capacity_ = newsize + (newsize >> 1);	//  newsize * 1.5.
				capacity_ = ((newsize >> 8) + 1) << 8;	//  Round up to a multiple of 256.

				if (capacity_ > maxsize_)
					capacity_ = maxsize_;

				const size_type newsize_in_byte = capacity_ * sizeof (ElemT);
				const pointer oldbuffer = buffer_;

				buffer_ = static_cast<pointer>(std::realloc(buffer_, newsize_in_byte));
				if (buffer_ != NULL)
					return;

				//  Even if realloc() failed, already-existing buffer remains valid.
				std::free(oldbuffer);
//				buffer_ = NULL;
				size_ = capacity_ = 0;
			}
			throw std::bad_alloc();
		}
	}

	void move_forward(const size_type pos, const size_type count)
	{
		size_type oldsize = size_;

		resize(size_ + count);

		const pointer begin = buffer_ + pos;
		pointer src = buffer_ + oldsize;
		pointer dest = src + count;

		while (src > begin)
			*--dest = *--src;
	}

private:

	pointer buffer_;
	size_type size_;
	size_type capacity_;

//	static const size_type maxsize_ = (npos - sizeof (simple_array)) / sizeof (ElemT);
	static const size_type maxsize_ = (npos - sizeof (pointer) - sizeof (size_type) * 2) / sizeof (ElemT) / 2;
};
//  simple_array

	}	//  namespace regex_internal

//  ... "rei_memory.hpp"]
//  ["rei_bitset.hpp" ...

	namespace regex_internal
	{

//  Always uses a heap.
template <const std::size_t Bits>
class bitset
{
private:

	typedef unsigned long Type;

public:

	bitset()
		: buffer_(static_cast<Type *>(std::malloc(size_in_byte)))
	{
		if (buffer_ != NULL)
		{
			reset();
			return;
		}
		throw std::bad_alloc();
	}

	bitset(const bitset &right)
		: buffer_(static_cast<Type *>(std::malloc(size_in_byte)))
	{
		if (buffer_ != NULL)
		{
			operator=(right);
			return;
		}
		throw std::bad_alloc();
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	bitset(bitset &&right) SRELL_NOEXCEPT
		: buffer_(right.buffer_)
	{
		right.buffer_ = NULL;
	}
#endif

	bitset &operator=(const bitset &right)
	{
		if (this != &right)
		{
			for (std::size_t i = 0; i < arraylength_; ++i)
				buffer_[i] = right.buffer_[i];
		}
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	bitset &operator=(bitset &&right) SRELL_NOEXCEPT
	{
		if (this != &right)
		{
			this->buffer_ = right.buffer_;
			right.buffer_ = NULL;
		}
		return *this;
	}
#endif

	~bitset()
	{
		if (buffer_ != NULL)
			std::free(buffer_);
	}

	bitset &reset()
	{
		std::memset(buffer_, 0, size_in_byte);
		return *this;
	}

	bitset &reset(const std::size_t bit)
	{
		buffer_[bit / bits_per_ulong_] &= ~(1 << (bit & bitmask_));
		return *this;
	}

	bitset &set(const std::size_t bit)
	{
		buffer_[bit / bits_per_ulong_] |= (1 << (bit & bitmask_));
		return *this;
	}

#if 0
	bitset &set_range(const std::size_t firstbit, const std::size_t lastbit)
	{
		const std::size_t lastelem = lastbit / bits_per_ulong_;
		std::size_t firstelem = firstbit / bits_per_ulong_;
		const unsigned int lastbitshift = static_cast<unsigned int>(lastbit & bitmask_);
		const unsigned int lastelemmask = ~(~0 << lastbitshift) | (1 << lastbitshift);
		unsigned int ormask = ~0 << (firstbit & bitmask_);

		if (firstelem < lastelem)
		{
			buffer_[firstelem] |= ormask;
			ormask = static_cast<unsigned int>(~0);

			for (++firstelem; firstelem < lastelem; ++firstelem)
				buffer_[firstelem] |= ormask;
		}
		ormask &= lastelemmask;
		buffer_[lastelem] |= ormask;

		return *this;
	}
#endif

	bool test(const std::size_t bit) const
	{
		return (buffer_[bit / bits_per_ulong_] & (1 << (bit & bitmask_))) != 0;
	}

	bool operator[](const std::size_t bit) const
	{
		return (buffer_[bit / bits_per_ulong_] & (1 << (bit & bitmask_))) != 0;
	}

	bitset<Bits> &flip()
	{
		for (std::size_t i = 0; i < arraylength_; ++i)
			buffer_[i] = ~buffer_[i];
		return *this;
	}

	void swap(bitset &right)
	{
		if (this != &right)
		{
			Type *const tmpbuffer = this->buffer_;
			this->buffer_ = right.buffer_;
			right.buffer_ = tmpbuffer;
		}
	}

private:

	static const std::size_t bits_per_ulong_ = CHAR_BIT * sizeof (Type);
	static const std::size_t bitmask_ = bits_per_ulong_ - 1;
	static const std::size_t arraylength_ = (Bits + bitmask_) / bits_per_ulong_;
	static const std::size_t size_in_byte = arraylength_ * sizeof (Type);

	Type *buffer_;
};

	}	//  namespace regex_internal

//  ... "rei_bitset.hpp"]
//  ["rei_ucf.hpp" ...

	namespace regex_internal
	{

#if !defined(SRELL_NO_UNICODE_ICASE)
#include "srell_ucfdata2.h"

		namespace ucf_internal
		{

typedef unicode_casefolding<uchar21, uchar21> ucf_data;

		}	//  namespace ucf_internal
#endif	//  !defined(SRELL_NO_UNICODE_ICASE)

class unicode_case_folding
{
public:

#if !defined(SRELL_NO_UNICODE_ICASE)
	static const uchar21 rev_maxset = ucf_internal::ucf_data::rev_maxset;
#else
	static const uchar21 rev_maxset = 2;
#endif

	static uchar21 do_casefolding(const uchar21 cp)
	{
#if !defined(SRELL_NO_UNICODE_ICASE)
		if (cp <= ucf_internal::ucf_data::ucf_maxcodepoint)
			return cp + ucf_internal::ucf_data::ucf_deltatable[ucf_internal::ucf_data::ucf_segmenttable[cp >> 8] + (cp & 0xff)];
#else
		if (cp >= char_alnum::ch_A && cp <= char_alnum::ch_Z)	//  'A' && 'Z'
			return static_cast<uchar21>(cp - char_alnum::ch_A + char_alnum::ch_a);	//  - 'A' + 'a'
#endif
		return cp;
	}

	static uchar21 casefoldedcharset(uchar21 out[rev_maxset], const uchar21 cp)
	{
#if !defined(SRELL_NO_UNICODE_ICASE)
		uchar21 count = 0;

		if (cp <= ucf_internal::ucf_data::rev_maxcodepoint)
		{
			const uchar21 offset_of_charset = ucf_internal::ucf_data::rev_indextable[ucf_internal::ucf_data::rev_segmenttable[cp >> 8] + (cp & 0xff)];
			const uchar21 *ptr = &ucf_internal::ucf_data::rev_charsettable[offset_of_charset];

			for (; *ptr != cfcharset_eos_ && count < rev_maxset; ++ptr, ++count)
				out[count] = *ptr;
		}
		if (count == 0)
			out[count++] = cp;

		return count;
#else
//		const uchar21 nocase = static_cast<uchar21>(cp & ~0x20);
		const uchar21 nocase = static_cast<uchar21>(cp | 0x20);

		out[0] = cp;
//		if (nocase >= char_alnum::ch_A && nocase <= char_alnum::ch_Z)
		if (nocase >= char_alnum::ch_a && nocase <= char_alnum::ch_z)
		{
			out[1] = static_cast<uchar21>(cp ^ 0x20);
			return 2;
		}
		return 1;
#endif
	}

	unicode_case_folding &operator=(const unicode_case_folding &)
	{
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	unicode_case_folding &operator=(unicode_case_folding &&) SRELL_NOEXCEPT
	{
		return *this;
	}
#endif

	void swap(unicode_case_folding & /* right */)
	{
	}

private:

#if !defined(SRELL_NO_UNICODE_ICASE)
	static const uchar21 cfcharset_eos_ = ucf_internal::ucf_data::eos;
#endif

public:	//  For debug.

	void print_tables() const;
};
//  unicode_case_folding

	}	//  namespace regex_internal

//  ... "rei_ucf.hpp"]
//  ["rei_up.hpp" ...

	namespace regex_internal
	{

#if !defined(SRELL_NO_UNICODE_PROPERTY)
#include "srell_updata.h"

//template <typename PairType>
class unicode_property
{
public:

	typedef regex_internal::uchar21 uchar21;
	typedef unsigned int property_type;
	typedef simple_array<char> pstring;

	static const property_type error_property = static_cast<property_type>(-1);

	unicode_property()
	{
	}

	unicode_property &operator=(const unicode_property &)
	{
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	unicode_property &operator=(unicode_property &&) SRELL_NOEXCEPT
	{
		return *this;
	}
#endif

	static property_type lookup_property(const pstring &name, const pstring &value)
	{
		pname_type ptype = name.size() ? lookup_property_name(name) : updata::ptype::general_category;
		property_type property_number = lookup_property_value(ptype, value);

		if (property_number == updata::unknown && name.size() == 0)
		{
			ptype = updata::ptype::binary;
			property_number = lookup_property_value(ptype, value);
		}

		return property_number != updata::unknown ? property_number : error_property;
	}

	static std::size_t ranges_offset(const property_type property_number)
	{
#if defined(SRELL_UPDATA_VERSION)
		return updata::positiontable[property_number].offset;
#else
		const offset_and_number *const postable = updata::position_table();
		return postable[property_number].offset;
#endif
	}

	static std::size_t number_of_ranges(const property_type property_number)
	{
#if defined(SRELL_UPDATA_VERSION)
		return updata::positiontable[property_number].number_of_pairs;
#else
		const offset_and_number *const postable = updata::position_table();
		return postable[property_number].number_of_pairs;
#endif
	}

	static const uchar21 *ranges_address(const property_type property_number)
	{
#if defined(SRELL_UPDATA_VERSION)
		return &updata::rangetable[ranges_offset(property_number) << 1];
#else
		const uchar21 *const ranges = updata::ranges();
		return &ranges[ranges_offset(property_number) << 1];
#endif
	}

private:

	typedef unsigned int pname_type;
	typedef const char *pname_string_type;

	struct pvalue_type
	{
		pname_type pname;
		pname_string_type csstrings;
		property_type pnumber;
	};

	struct offset_and_number
	{
		std::size_t offset;
		std::size_t number_of_pairs;
	};

	typedef unicode_property_data<property_type,
		pname_type,
		pname_string_type,
		uchar21,
		pvalue_type,
		offset_and_number
		>
		updata;

	static pname_type lookup_property_name(const pstring &name)
	{
#if defined(SRELL_UPDATA_VERSION)
		for (std::size_t pno = 0; *updata::propertynametable[pno]; ++pno)
		{
			if (check_if_included(name, updata::propertynametable[pno]))
				return static_cast<pname_type>(pno);
		}
#else
		const pname_string_type *const pname_table = updata::propertyname_table();

		for (std::size_t pno = 0; *pname_table[pno]; ++pno)
		{
			if (check_if_included(name, pname_table[pno]))
				return static_cast<pname_type>(pno);
		}
#endif
		return updata::ptype::unknown;
	}

	//  Checks if value is included in colon-separated strings.
	static bool check_if_included(const pstring &value, pname_string_type csstrings)
	{
		if (static_cast<uchar21>(*csstrings) != meta_char::mc_astrsk)	//  '*'
		{
			while (*csstrings)
			{
				const pname_string_type begin = csstrings;

				for (; static_cast<uchar21>(*csstrings) != meta_char::mc_colon && static_cast<uchar21>(*csstrings) != char_ctrl::cc_nul; ++csstrings);

				const std::size_t length = csstrings - begin;

				if (static_cast<std::size_t>(value.size()) == length)
					if (value.compare(0, value.size(), begin, length) == 0)
						return true;

				if (static_cast<uchar21>(*csstrings) == meta_char::mc_colon)
					++csstrings;
			}
		}
		return false;
	}

	static property_type lookup_property_value(const pname_type ptype, const pstring &value)
	{
#if defined(SRELL_UPDATA_VERSION)
		for (std::size_t pno = 0; *updata::rangenumbertable[pno].csstrings; ++pno)
		{
			const pvalue_type &pvalue = updata::rangenumbertable[pno];
			if (pvalue.pname == ptype && check_if_included(value, pvalue.csstrings))
				return pvalue.pnumber;
		}
#else
		const pvalue_type *const pvalue_table = updata::rangenumber_table();

		for (std::size_t pno = 0; *pvalue_table[pno].csstrings; ++pno)
		{
			const pvalue_type &pvalue = pvalue_table[pno];
			if (pvalue.pname == ptype && check_if_included(value, pvalue.csstrings))
				return pvalue.pnumber;
		}
#endif
		return updata::unknown;
	}

public:

	static const std::size_t number_of_properties = updata::last_property_number + 1;
	static const property_type gc_Zs = updata::gc_Space_Separator;
	static const property_type gc_Cn = updata::gc_Unassigned;
	static const property_type bp_Assigned = updata::bp_Assigned;

//  UnicodeIDStart::
//  UnicodeIDContinue::
	static const property_type bp_ID_Start = updata::bp_ID_Start;
	static const property_type bp_ID_Continue = updata::bp_ID_Continue;
};
//  unicode_property

#endif	//  !defined(SRELL_NO_UNICODE_PROPERTY)
	}	//  namespace regex_internal

//  ... "rei_up.hpp"]
//  ["rei_char_class.hpp" ...

	namespace regex_internal
	{

struct range_pair	//  , public std::pair<charT, charT>
{
	uchar21 first;
	uchar21 second;

	bool is_range_valid() const
	{
		return first <= second;
	}

	bool operator==(const range_pair &right) const
	{
		return this->first == right.first && this->second == right.second;
	}

	bool operator<(const range_pair &right) const
	{
		return this->second < right.first;	//  This assumes that optimise() has been called.
	}

	void swap(range_pair &right)
	{
		const range_pair tmp = *this;
		*this = right;
		right = tmp;
	}

	bool unify_range(const range_pair &right)
	{
		range_pair &left = *this;

		if (right.first <= left.second || left.second + 1 == right.first)	//  r1 <= l2 || l2+1 == r1
		{
			//  l1 l2+1 < r1 r2 excluded.

			if (left.first <= right.second || right.second + 1 == left.first)	//  l1 <= r2 || r2+1 == l1
			{
				//  r1 r2+1 < l1 l2 excluded.

				if (left.first > right.first)
					left.first = right.first;

				if (left.second < right.second)
					left.second = right.second;

				return true;
			}
		}
		return false;
	}
};
//  range_pair

struct range_pair_helper : public range_pair
{
	range_pair_helper(const uchar21 min, const uchar21 max)
	{
		this->first = min;
		this->second = max;
	}

	range_pair_helper(const uchar21 minmax)
	{
		this->first = minmax;
		this->second = minmax;
	}

#if 0
	range_pair_helper(const range_pair &right)
	{
		this->first = right.first;
		this->second = right.second;
	}
#endif
};
//  range_pair_helper

struct range_pairs	//  : public simple_array<range_pair>
{
public:

	typedef simple_array<range_pair> array_type;
	typedef array_type::size_type size_type;

	range_pairs()
	{
	}

	range_pairs(const range_pairs &rp) : rparray_(rp.rparray_)
	{
	}

	range_pairs &operator=(const range_pairs &rp)
	{
		rparray_.operator=(rp.rparray_);
		return *this;
	}

	range_pairs(const size_type initsize) : rparray_(initsize)
	{
	}

	range_pairs(const range_pairs &right, size_type pos, size_type size)
		: rparray_(right.rparray_, pos, size)
	{
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	range_pairs(range_pairs &&rp) SRELL_NOEXCEPT
		: rparray_(std::move(rp.rparray_))
	{
	}

	range_pairs &operator=(range_pairs &&rp) SRELL_NOEXCEPT
	{
		rparray_.operator=(std::move(rp.rparray_));
		return *this;
	}
#endif

	void clear()
	{
		rparray_.clear();
	}

	size_type size() const
	{
		return rparray_.size();
	}

	const range_pair &operator[](const size_type pos) const
	{
		return rparray_[pos];
	}
	range_pair &operator[](const size_type pos)
	{
		return rparray_[pos];
	}

	void resize(const size_type size)
	{
		rparray_.resize(size);
	}

	void replace(const size_type pos, const size_type count, const range_pairs &right)
	{
		rparray_.replace(pos, count, right.rparray_);
	}

	void swap(range_pairs &right)
	{
		rparray_.swap(right.rparray_);
	}

	void set_solerange(const range_pair &right)
	{
		rparray_.clear();
		rparray_.push_back(right);
	}

	void append_newclass(const range_pairs &right)
	{
		rparray_.append(right.rparray_);
	}

	void append_newpair(const range_pair &right)
	{
		rparray_.push_back(right);
	}

	void join(const range_pair &right)
	{
		size_type pos = 0;

		for (; pos < rparray_.size(); ++pos)
		{
			range_pair &curpair = rparray_[pos];

			if (curpair.unify_range(right))
			{
				for (++pos; pos < rparray_.size();)
				{
					if (curpair.unify_range(rparray_[pos]))
						rparray_.erase(pos);
					else
						break;
				}
				return;
			}
			if (right.second < curpair.first)
				break;
		}
		rparray_.insert(pos, right);
	}

	void merge(const range_pairs &right)
	{
		for (size_type i = 0; i < right.size(); ++i)
			join(right[i]);
	}

#if defined(SRELLDBG_NO_BITSET)
	bool is_included(const uchar21 ch) const
	{
#if 01
		const range_pair *const end = rparray_.data() + rparray_.size();

		for (const range_pair *cur = rparray_.data(); cur != end; ++cur)
		{
			//  This assumes that sort() has been called.
			if (ch <= cur->second)
				return ch >= cur->first;
#else
		for (size_type i = 0; i < rparray_.size(); ++i)
		{
			if (rparray_[i].is_included(ch))
				return true;
#endif
		}
		return false;
	}
#endif	//  defined(SRELLDBG_NO_BITSET)

	bool is_included(const uchar21 pos, uchar21 count, const uchar21 c) const
	{
		const range_pair *base = &rparray_[pos];

		while (count)
		{
			uchar21 mid = count >> 1;
			const range_pair &rp = base[mid];

			if (c >= rp.first)
			{
				if (c <= rp.second)
					return true;

				++mid;
				count -= mid;
				base += mid;
			}
			else
				count = mid;
		}
		return false;
	}

	bool same(uchar21 pos, const uchar21 count, const range_pairs &right) const
	{
		if (count == right.size())
		{
			for (uchar21 i = 0; i < count; ++i, ++pos)
				if (!(rparray_[pos] == right[i]))
					return false;

			return true;
		}
		return false;
	}

	void negation()
	{
		uchar21 begin = 0;
		range_pairs newpairs;

#if 01	//  begin does not pass 0x10ffff.
		for (size_type i = 0;; ++i)
		{
			if (i == rparray_.size())
			{
				newpairs.join(range_pair_helper(begin, constants::unicode_max_codepoint));
				break;
			}
			else
			{
				const range_pair &range = rparray_[i];

				if (begin < range.first)
					newpairs.join(range_pair_helper(begin, range.first - 1));

				if (range.second == constants::unicode_max_codepoint)
					break;

				begin = range.second + 1;
			}
		}

#else	//  begin can pass 0x10ffff, but safe.
		for (size_type i = 0; i < rparray_.size(); ++i)
		{
			const range_pair &range = rparray_[i];

			if (begin < range.first)
				newpairs.push_back(range_pair_helper(begin, range.first - 1));

			begin = range.second + 1;
		}
		if (begin <= constants::unicode_max_codepoint)
			newpairs.push_back(range_pair_helper(begin, constants::unicode_max_codepoint));
#endif
		*this = newpairs;
	}

	bool is_overlap(const range_pairs &right) const
	{
		for (size_type i = 0; i < rparray_.size(); ++i)
		{
			const range_pair &leftrange = rparray_[i];

			for (size_type j = 0; j < right.size(); ++j)
			{
				const range_pair &rightrange = right[j];

				if (rightrange.first <= leftrange.second)	//  Excludes l1 l2 < r1 r2.
					if (leftrange.first <= rightrange.second)	//  Excludes r1 r2 < l1 l2.
						return true;
			}
		}
		return false;
	}

	void load_from_memory(const uchar21 *array, size_type number_of_pairs)
	{
		for (; number_of_pairs; --number_of_pairs, array += 2)
			join(range_pair_helper(array[0], array[1]));
	}

	void make_caseunfoldedcharset()
	{
		uchar21 table[unicode_case_folding::rev_maxset] = {};
		bitset<constants::unicode_max_codepoint + 1> bs;

		for (size_type i = 0; i < rparray_.size(); ++i)
		{
			const range_pair &range = rparray_[i];

			for (uchar21 ucp = range.first; ucp <= range.second; ++ucp)
			{
				const uchar21 setnum = unicode_case_folding::casefoldedcharset(table, ucp);

				for (uchar21 j = 0; j < setnum; ++j)
					bs.set(table[j]);
			}
		}
		load_from_bitset(bs);
	}

	//  For updataout.hpp.
	void remove_range(const range_pair &right)
	{
		for (size_type pos = 0; pos < rparray_.size();)
		{
			range_pair &left = rparray_[pos];

			if (right.first <= left.first && left.first <= right.second)	//  r1 <= l1 <= r2.
			{
				if (left.second > right.second)	//  r1 <= l1 <= r2 < l2.
				{
					left.first = right.second + 1;	//  carry doesn't happen.
					++pos;
				}
				else	//  r1 <= l1 <= l2 <= r2.
					rparray_.erase(pos);
			}
			else if (right.first <= left.second && left.second <= right.second)	//  r1 <= l2 <= r2.
			{
				if (left.first < right.first)	//  l1 < r1 <= l2 <= r2.
				{
					left.second = right.first - 1;
					++pos;
				}
				else	//  r1 <= l1 <= l2 <= r2.
					rparray_.erase(pos);
			}
			else if (left.first < right.first && right.second < left.second)	//  l1 < r1 && r2 < l2.
			{
				range_pair newrange(left);

				left.second = right.first - 1;
				newrange.first = right.second + 1;
				rparray_.insert(++pos, newrange);
				++pos;
			}
			else
				++pos;
		}
	}

//	template <typename ucf>
	uchar21 consists_of_one_character(const bool icase) const
	{
		if (rparray_.size() >= 1)
		{
			uchar21 (*const casefolding_func)(const uchar21) = !icase ? do_nothing : unicode_case_folding::do_casefolding;
			const uchar21 ucp1st = casefolding_func(rparray_[0].first);

			for (size_type no = 0; no < rparray_.size(); ++no)
			{
				const range_pair &cr = rparray_[no];

				for (uchar21 ucp = cr.first;; ++ucp)
				{
					if (ucp1st != casefolding_func(ucp))
						return constants::invalid_u21value;

					if (ucp == cr.second)
						break;
				}
			}
			return ucp1st;
		}
		return constants::invalid_u21value;
	}

private:

	static uchar21 do_nothing(const uchar21 cp)
	{
		return cp;
	}

	template <typename BitSetT>
	void load_from_bitset(const BitSetT &bs)
	{
		uchar21 begin = constants::invalid_u21value;
		range_pairs newranges;

		for (uchar21 ucp = 0;; ++ucp)
		{
#if 01	//  ucp does not pass 0x10ffff.
			if (ucp == constants::unicode_max_codepoint)
			{
				if (bs.test(ucp))
				{
					if (begin == constants::invalid_u21value)
						begin = ucp;
				}
				else
					--ucp;

				if (begin != constants::invalid_u21value)
				{
					newranges.join(range_pair_helper(begin, ucp));
				}
				break;
			}
			else if (!bs.test(ucp))
			{
				if (begin != constants::invalid_u21value)
				{
					newranges.join(range_pair_helper(begin, ucp - 1));
					begin = constants::invalid_u21value;
				}
			}
			else if (begin == constants::invalid_u21value)
				begin = ucp;

#else	//  ucp can pass 0x10ffff, but safe.
			if (ucp > constants::unicode_max_codepoint || !bs.test(ucp))
			{
				if (begin != constants::invalid_u21value)
				{
					newranges.push_back(range_pair_helper(begin, ucp - 1));
					begin = constants::invalid_u21value;
				}
				if (ucp > constants::unicode_max_codepoint)
					break;
			}
			else if (begin == constants::invalid_u21value && bs.test(ucp))
				begin = ucp;
#endif
		}
		rparray_.swap(newranges.rparray_);
	}

	array_type rparray_;

public:	//  For debug.

	void print_pairs(const int, const char *const = NULL, const char *const = NULL) const;
};
//  range_pairs

class re_character_class
{
public:

	enum
	{	//    0       1      2      3     4           5
		newline, dotall, space, digit, word, icase_word,
#if !defined(SRELL_NO_UNICODE_PROPERTY)
		//                     6                        7
		reg_exp_identifier_start, reg_exp_identifier_part,
#endif
		//              6/8
		number_of_predefcls
	};
	static const unsigned int error_property = static_cast<unsigned int>(-1);

#if !defined(SRELL_NO_UNICODE_PROPERTY)
	typedef unicode_property::pstring pstring;
#endif

	re_character_class()
	{
		setup_predefinedclass();
	}

	re_character_class &operator=(const re_character_class &that)
	{
		if (this != &that)
		{
			this->char_class_ = that.char_class_;
			this->char_class_pos_ = that.char_class_pos_;
		}
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	re_character_class &operator=(re_character_class &&that) SRELL_NOEXCEPT
	{
		if (this != &that)
		{
			this->char_class_ = std::move(that.char_class_);
			this->char_class_pos_ = std::move(that.char_class_pos_);
		}
		return *this;
	}
#endif

	bool is_included(const unsigned int class_number, const uchar21 c) const
	{
//		return char_class_.is_included(char_class_pos_[class_number], c);
		const range_pair &rp = char_class_pos_[class_number];

		return char_class_.is_included(rp.first, rp.second, c);
	}

#if !defined(SRELLDBG_NO_CCPOS)
//	bool is_included(const unsigned int pos, const unsigned int len, const uchar21 &c) const
	bool is_included(const uchar21 pos, const uchar21 len, const uchar21 c) const
	{
		return char_class_.is_included(pos, len, c);
	}
#endif

	void setup_icase_word()
	{
		range_pair &icase_pos = char_class_pos_[icase_word];

		if (icase_pos.second == char_class_pos_[word].second)
		{
			range_pairs icasewordclass(char_class_, icase_pos.first, icase_pos.second);

			icasewordclass.make_caseunfoldedcharset();
				//  Includes 017f and 212a so that they and their case-folded
				//  characters 's' and 'k' will be excluded from the character
				//  set that /[\W]/i matches.

			char_class_.replace(icase_pos.first, icase_pos.second, icasewordclass);

			if (icase_pos.second < static_cast<uchar21>(icasewordclass.size()))
			{
				const uchar21 delta = static_cast<uchar21>(icasewordclass.size() - icase_pos.second);

				for (int i = number_of_predefcls; i < static_cast<int>(char_class_pos_.size()); ++i)
					char_class_pos_[i].first += delta;
			}
			icase_pos.second = static_cast<uchar21>(icasewordclass.size());
		}
	}

	void clear()
	{
		char_class_pos_.resize(number_of_predefcls);

		uchar21 basesize = 0;
		for (int i = 0; i < number_of_predefcls; ++i)
			basesize += char_class_pos_[i].second;

		char_class_.resize(basesize);
	}

	unsigned int register_newclass(const range_pairs &rps)
	{
		for (range_pairs::size_type no = 0; no < char_class_pos_.size(); ++no)
		{
			const range_pair &rp = char_class_pos_[no];

			if (char_class_.same(rp.first, rp.second, rps))
				return static_cast<unsigned int>(no);
		}

		append_charclass(rps);
		return static_cast<unsigned int>(char_class_pos_.size() - 1);
	}

	range_pairs operator[](const unsigned int no) const
	{
		const range_pair &ccpos = char_class_pos_[no];
		range_pairs rp(ccpos.second);

		for (uchar21 i = 0; i < ccpos.second; ++i)
			rp[i] = char_class_[ccpos.first + i];

		return rp;
	}

#if !defined(SRELLDBG_NO_CCPOS)
	const range_pair &charclasspos(const unsigned int no) const
	{
		return char_class_pos_[no];
	}
#endif

	void optimise()
	{
	}

#if !defined(SRELL_NO_UNICODE_PROPERTY)

	unsigned int lookup_property(const pstring &pname, const pstring &pvalue, const bool icase)
	{
		const unsigned int property_number = static_cast<unsigned int>(unicode_property::lookup_property(pname, pvalue));

		if (property_number != unicode_property::error_property)
		{
			const unsigned int charclass_number = register_property_as_charclass(property_number, icase);
			return charclass_number;
		}
		return error_property;
	}

#endif

	void swap(re_character_class &right)
	{
		if (this != &right)
		{
			this->char_class_.swap(right.char_class_);
			this->char_class_pos_.swap(right.char_class_pos_);
		}
	}

private:

#if !defined(SRELL_NO_UNICODE_PROPERTY)

	unsigned int register_property_as_charclass(const unsigned int property_number, const bool icase)
	{
		if (property_number == unicode_property::bp_Assigned)
		{
			//  \p{Assigned} == \P{Cn}
			return load_updata_and_register_as_charclass(unicode_property::gc_Cn, false, true);
		}
		return load_updata_and_register_as_charclass(property_number, icase, false);
	}

	unsigned int load_updata_and_register_as_charclass(const unsigned int property_number, const bool icase, const bool negation)
	{
		const uchar21 *const address = unicode_property::ranges_address(property_number);
//		const std::size_t offset = unicode_property::ranges_offset(property_number);
		const std::size_t number = unicode_property::number_of_ranges(property_number);
		range_pairs newranges;

		newranges.load_from_memory(address, number);

		if (icase)
			newranges.make_caseunfoldedcharset();

		if (negation)
			newranges.negation();

		return register_newclass(newranges);
	}

#endif	//  !defined(SRELL_NO_UNICODE_PROPERTY)

	void append_charclass(const range_pairs &rps)
	{
		char_class_pos_.push_back(range_pair_helper(static_cast<uchar21>(char_class_.size()), static_cast<uchar21>(rps.size())));
		char_class_.append_newclass(rps);
	}

//  The production CharacterClassEscape::s  evaluates as follows:
//    Return the set of characters containing the characters that are on the right-hand side of the WhiteSpace or LineTerminator productions.
//  WhiteSpace::<TAB> <VT> <FF> <SP> <NBSP> <ZWNBSP> <USP>
//               0009 000B 000C 0020   00A0     FEFF    Zs
//  LineTerminator::<LF> <CR> <LS> <PS>
//                  000A 000D 2028 2029

	void setup_predefinedclass()
	{
#if !defined(SRELL_NO_UNICODE_PROPERTY)
		const uchar21 *const Zs_address = unicode_property::ranges_address(unicode_property::gc_Zs);
//		const std::size_t Zs_offset = unicode_property::ranges_offset(unicode_property::gc_Zs);
		const std::size_t Zs_number = unicode_property::number_of_ranges(unicode_property::gc_Zs);
#else
		static const uchar21 Zs[] = {
			0x1680, 0x1680, 0x2000, 0x200a,	// 0x2028, 0x2029,
			0x202f, 0x202f, 0x205f, 0x205f, 0x3000, 0x3000
		};
#endif	//  defined(SRELL_NO_UNICODE_PROPERTY)
		static const uchar21 allranges[] = {
			//  dotall.
			0x0000, 0x10ffff,
			//  newline.
			0x0a, 0x0a, 0x0d, 0x0d,	//  \n \r
			//  newline, space.
			0x2028, 0x2029,
			//  space.
			0x09, 0x0d,	//  \t \n \v \f \r
			0x20, 0x20,	//  ' '
			0xa0, 0xa0,	//  <NBSP>
			0xfeff, 0xfeff,	//  <BOM>
			//  digit, word.
			0x30, 0x39,	//  '0'-'9'
			0x41, 0x5a, 0x5f, 0x5f, 0x61, 0x7a	//  'A'-'Z' '_' 'a'-'z'
#if !defined(SRELL_NO_UNICODE_PROPERTY)
			//  reg_exp_identifier_start, reg_exp_identifier_part.
			, 0x24, 0x24, 0x5f, 0x5f, 0x200c, 0x200d	//  '$' '_' <ZWNJ>-<ZWJ>
#endif
		};
		range_pairs ranges;

		//  newline
		ranges.load_from_memory(&allranges[2], 3);
		append_charclass(ranges);

		//  dotall
		ranges.clear();
		ranges.load_from_memory(&allranges[0], 1);
		append_charclass(ranges);

		//  space
		ranges.clear();
		ranges.load_from_memory(&allranges[6], 5);
#if !defined(SRELL_NO_UNICODE_PROPERTY)
		ranges.load_from_memory(Zs_address, Zs_number);
#else
		ranges.load_from_memory(Zs, 5);
#endif
		append_charclass(ranges);

		//  digit
		ranges.clear();
		ranges.load_from_memory(&allranges[16], 1);
		append_charclass(ranges);

		//  word
		ranges.clear();
		ranges.load_from_memory(&allranges[16], 4);
		append_charclass(ranges);

		//  Reservation for icase_word.
		append_charclass(ranges);

#if !defined(SRELL_NO_UNICODE_PROPERTY)
		//  For reg_exp_identifier_start
		ranges.clear();
		{
			const uchar21 *const IDs_address = unicode_property::ranges_address(unicode_property::bp_ID_Start);
			const std::size_t IDs_number = unicode_property::number_of_ranges(unicode_property::bp_ID_Start);
			ranges.load_from_memory(IDs_address, IDs_number);
		}
		ranges.load_from_memory(&allranges[24], 2);
		append_charclass(ranges);

		//  For reg_exp_identifier_part
		ranges.clear();
		{
			const uchar21 *const IDc_address = unicode_property::ranges_address(unicode_property::bp_ID_Continue);
			const std::size_t IDc_number = unicode_property::number_of_ranges(unicode_property::bp_ID_Continue);
			ranges.load_from_memory(IDc_address, IDc_number);
		}
		ranges.load_from_memory(&allranges[24], 3);
		append_charclass(ranges);
#endif
	}

private:

	range_pairs char_class_;
	range_pairs::array_type char_class_pos_;

public:	//  For debug.

	void print_classes(const int) const;
};
//  re_character_class

	}	//  namespace regex_internal

//  ... "rei_char_class.hpp"]
//  ["rei_groupname_mapper.hpp" ...

	namespace regex_internal
	{

#if !defined(SRELL_NO_NAMEDCAPTURE)

template <typename charT, typename numberT>
class groupname_and_backrefnumber_mapper
{
public:

	typedef simple_array<charT> gname_string;
	typedef typename gname_string::size_type size_type;
	static const numberT notfound = static_cast<numberT>(-1);

	groupname_and_backrefnumber_mapper()
	{
	}

	groupname_and_backrefnumber_mapper(const groupname_and_backrefnumber_mapper &right)
		: names_(right.names_), keysize_classno_(right.keysize_classno_)
	{
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	groupname_and_backrefnumber_mapper(groupname_and_backrefnumber_mapper &&right) SRELL_NOEXCEPT
		: names_(std::move(right.names_)), keysize_classno_(std::move(right.keysize_classno_))
	{
	}
#endif

	groupname_and_backrefnumber_mapper &operator=(const groupname_and_backrefnumber_mapper &right)
	{
		if (this != &right)
		{
			names_ = right.names_;
			keysize_classno_ = right.keysize_classno_;
		}
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	groupname_and_backrefnumber_mapper &operator=(groupname_and_backrefnumber_mapper &&right) SRELL_NOEXCEPT
	{
		if (this != &right)
		{
			names_ = std::move(right.names_);
			keysize_classno_ = std::move(right.keysize_classno_);
		}
		return *this;
	}
#endif

	void clear()
	{
		names_.clear();
		keysize_classno_.clear();
	}

	numberT operator[](const gname_string &gname) const
	{
		numberT pos = 0;
		for (std::size_t i = 0; i < static_cast<std::size_t>(keysize_classno_.size()); i += 2)
		{
			const numberT keysize = keysize_classno_[i];

			if (keysize == static_cast<numberT>(gname.size()) && sameseq(pos, gname))
				return keysize_classno_[++i];

			pos += keysize;
		}
		return notfound;
	}

	gname_string operator[](const numberT indexno) const
	{
		numberT pos = 0;
		for (std::size_t i = 0; i < static_cast<std::size_t>(keysize_classno_.size()); ++i)
		{
			const numberT keysize = keysize_classno_[i];
			const numberT classno = keysize_classno_[++i];

			if (classno == indexno)
				return gname_string(names_, pos, keysize);

			pos += keysize;
		}
		return gname_string();
	}

	size_type size() const
	{
		return static_cast<size_type>(keysize_classno_.size() >> 1);
	}

	bool push_back(const gname_string &gname, const numberT class_number)
	{
		const numberT num = operator[](gname);

		if (num == notfound)
		{
			names_.append(gname);
			keysize_classno_.append(1, static_cast<numberT>(gname.size()));
			keysize_classno_.append(1, class_number);
			return true;
		}
		return false;	//  Already exists.
	}

	void swap(groupname_and_backrefnumber_mapper &right)
	{
		this->names_.swap(right.names_);
		keysize_classno_.swap(right.keysize_classno_);
	}

private:

	bool sameseq(size_type pos, const gname_string &gname) const
	{
		for (size_type i = 0; i < gname.size(); ++i, ++pos)
			if (pos >= names_.size() || names_[pos] != gname[i])
				return false;

		return true;
	}

	gname_string names_;
	simple_array<numberT> keysize_classno_;

public:	//  For debug.

	void print_mappings(const int) const;
};
//  groupname_and_backrefnumber_mapper

template <typename charT>
class groupname_mapper : public groupname_and_backrefnumber_mapper<charT, unsigned int>
{
};

#endif	//  !defined(SRELL_NO_NAMEDCAPTURE)

	}	//  namespace regex_internal

//  ... "rei_groupname_mapper.hpp"]
//  ["rei_state.hpp" ...

	namespace regex_internal
	{

struct re_quantifier
{
	static const unsigned int infinity = static_cast<unsigned int>(~0);

	//  atleast and atmost: for check_counter.
	//  offset and length: for charcter_class.
	//  (special case 1) in roundbracket_open and roundbracket_pop atleast and atmost represent
	//    the minimum and maximum bracket numbers respectively inside the brackets itself.
	//  (special case 2) in repeat_in_push and repeat_in_pop atleast and atmost represent the
	//    minimum and maximum bracket numbers respectively inside the repetition.
	union
	{
		unsigned int atleast;
		//  (special case 3: v1) in lookaround_open represents the number of characters to be rewound.
		//  (special case 3: v2) in lookaround_open represents whether lookaheads (0) or lookbehinds (1).
		//  (special case 4) in NFA_states[0] represents the class number of the first character class.
		uchar21 offset;
	};
	union
	{
		unsigned int atmost;
		uchar21 length;
	};

	union
	{
		bool is_greedy;
		unsigned int padding_;
	};

	void reset(const unsigned int len = 1)
	{
		atleast = atmost = len;
		is_greedy = true;
	}

	void set(const unsigned int min, const unsigned int max)
	{
		atleast = min;
		atmost = max;
	}

	void setccpos(const uchar21 o, const uchar21 l)
	{
		offset = o;
		length = l;
	}

	bool is_valid() const
	{
		return atleast <= atmost && atmost > 0;
	}

	void set_infinity()
	{
		atmost = infinity;
	}

	bool is_infinity() const
	{
		return atmost == infinity;
	}

	bool is_same() const
	{
		return atleast == atmost;
	}

	bool is_default() const
	{
		return atleast == 1 && atmost == 1;
	}

	bool is_asterisk() const
	{
		return atleast == 0 && atmost == infinity;
	}
	bool is_plus() const
	{
		return atleast == 1 && atmost == infinity;
	}
	bool is_asterisk_or_plus() const
	{
		return atleast <= 1 && atmost == infinity;
	}

	bool has_simple_equivalence() const
	{
		return (atleast <= 1 && atmost <= 3) || (atleast == 2 && atmost <= 4) || (atleast == atmost && atmost <= 6);
	}

	void multiply(const re_quantifier &q)
	{
		if (atleast != infinity)
		{
			if (q.atleast != infinity)
				atleast *= q.atleast;
			else
				atleast = infinity;
		}

		if (atmost != infinity)
		{
			if (q.atmost != infinity)
				atmost *= q.atmost;
			else
				atmost = infinity;
		}
	}

	void add(const re_quantifier &q)
	{
		if (atleast != infinity)
		{
			if (q.atleast != infinity && (atleast + q.atleast) >= atleast)
				atleast += q.atleast;
			else
				atleast = infinity;
		}

		if (atmost != infinity)
		{
			if (q.atmost != infinity && (atmost + q.atmost) >= atmost)
				atmost += q.atmost;
			else
				atmost = infinity;
		}
	}
};
//  re_quantifier

//template <typename charT>
struct re_state
{
	union
	{
//		charT character;
		uchar21 character;	//  For character.
		unsigned int number;	//  For character_class, brackets, counter, repeat, backreference.
	};

	re_state_type type;

	union
	{
		std::ptrdiff_t next1;
		re_state *next_state1;
		//  Points to the next state.
		//  (Special Case 1) in lookaround_open points to the next of lookaround_close.
		//  (Special Case 2) in lookaround_pop points to the content of brackets instead of lookaround_open.
	};
	union
	{
		std::ptrdiff_t next2;
		re_state *next_state2;
		//  character and character_class: points to another possibility, non-backtracking.
		//  epsilon: points to another possibility, backtracking.
		//  increment_counter, save_and_reset_counter, roundbracket_open, repeat_in_push,
		//    and lookaround_open: points to a restore state, backtracking.
		//  check_counter: complementary to next1 based on quantifier.is_greedy.
		//  (Special Case) roundbracket_close, check_0_width_repeat, and backreference:
		//    points to the next state as an exit after 0 width match.
	};

	re_quantifier quantifier;	//  For check_counter, roundbrackets, repeasts, (?<=...) and (?<!...),
		//  and character_class.

	union
	{
		bool is_not;	//  For \B, (?!...) and (?<!...).
		bool dont_push;	//  For check_counter.
		bool backrefnumber_unresolved;	//  For backreference (used only in compiler).
		bool icase;	//  For [0] only.
		unsigned int padding_;
	};

	//  st_character,               //  0x00
		//  char/number:        character
		//  next1:              gen.
		//  next2:              +1 (exit. used only when '*')
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_character_class,         //  0x01
		//  char/number:        character class number
		//  next1:              gen.
		//  next2:              +1 (exit. used only when '*')
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_epsilon,                 //  0x02
		//  char/number:        -
		//  next1:              gen.
		//  next2:              alt.
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_check_counter,           //  0x03
		//  char/number:        counter number
		//  next1:              greedy: increment_counter or repeat_in_push
		//                      not-greedy: out-of-loop
		//  next2:              complementary to next1
		//  q.atleast:          gen.
		//  q.atmost:           gen.
		//  q.greedy:           gen.
		//  is_not/dont_push:   dont_push (don't push backtracking data)

	//  st_increment_counter,       //  0x04
		//  char/number:        counter number
		//  next1:              +2 (next of decrement_counter, atom)
		//  next2:              +1 (decrement_counter)
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_decrement_counter,       //  0x05
		//  char/number:        counter number
		//  next1:              0 (always treated as "not matched")
		//  next2:              0
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_save_and_reset_counter,  //  0x06
		//  char/number:        counter number
		//  next1:              +2 (check_counter)
		//  next2:              +1 (restore_counter)
		//  quantifiers:        -
		//  is_not/dont_push:   - (was dont_push)

	//  st_restore_counter,         //  0x07
		//  char/number:        counter number
		//  next1:              0 (always treated as "not matched")
		//  next2:              0
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_roundbracket_open,       //  0x08
		//  char/number:        bracket number
		//  next1:              +2 (next of roundbracket_pop, atom)
		//  next2:              +1 (roundbracket_pop)
		//  q.atleast:          min bracket number inside this bracket (i.e., myself's number)
		//  q.atmost:           max bracket number inside this bracket
		//  q.greedy:           -
		//  is_not/dont_push:   - (was dont_push)

	//  st_roundbracket_pop,        //  0x09
		//  char/number:        bracket number
		//  next1:              0 (always treated as "not matched")
		//  next2:              0
		//  q.atleast:          min bracket number inside this bracket (i.e., myself's number)
		//  q.atmost:           max bracket number inside this bracket
		//  q.greedy:           -
		//  is_not/dont_push:   - (was dont_push)

	//  st_roundbracket_close,      //  0x0a
		//  char/number:        bracket number
		//  next1:              gen.
		//  next2:              +1 (exit for 0 width loop)
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_repeat_in_push,          //  0x0b
		//  char/number:        repeat counter
		//  next1:              +2 (next of repeat_in_pop, atom)
		//  next2:              +1 (repeat_in_pop)
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_repeat_in_pop,           //  0x0c
		//  char/number:        repeat counter
		//  next1:              0 (always treated as "not matched")
		//  next2:              0
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_check_0_width_repeat,    //  0x0d
		//  char/number:        repeat counter
		//  next1:              gen. (epsilon or check_counter)
		//  next2:              +1 (exit for 0 width loop)
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_backreference,           //  0x0e
		//  char/number:        bracket number
		//  next1:              gen.
		//  next2:              +1 (exit for 0 width match)
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_lookaround_open,         //  0x0f
		//  char/number:        character class number for nocase-word or icase-word
		//  next1:              next of lookaround_close (to where jumps after lookaround assertion)
		//  next2:              +1 (lookaround_pop)
		//  q.atleast:          <fixed-width> number of chars to be rewound (for (?<=...) (?<!...))
		//                      <variable-width> whether lookbehind or not
		//  q.atmost:           -
		//  q.greedy:           -
		//  is_not/dont_push:   not

	//  st_lookaround_pop,          //  0x10
		//  char/number:        -
		//  next1:              +1 (used by lookaround_open. this type is treated
		//                          as "not unmatched" and does not use this value)
		//  next2:              0
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_bol,                     //  0x11
		//  char/number:        -
		//  next1/next2:        -
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_eol,                     //  0x12
		//  char/number:        -
		//  next1/next2:        -
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_boundary,                //  0x13
		//  char/number:        -
		//  next1/next2:        -
		//  quantifiers:        -
		//  is_not/dont_push:   not

	//  st_success,                 //  0x14
		//  char/number:        -
		//  next1/next2:        -
		//  quantifiers:        -
		//  is_not/dont_push:   -

	//  st_move_nextpos,            //  0x15
		//  char/number:        -
		//  next1/next2:        -
		//  quantifiers:        -
		//  is_not/dont_push:   -

	void reset()
	{
		number = 0;
		type   = st_character;
		next1  = 1;
		next2  = 0;
		is_not = false;
		quantifier.reset();
	}

	bool is_character_or_class() const
	{
		return type == st_character || type == st_character_class;
	}

	bool has_quantifier() const
	{
		//  1. character:  size == 1 && type == character,
		//  2. [...]:      size == 1 && type == character_class,
		//  3. (...):      size == ? && type == roundbracket_open,
		//  4. (?:...):    size == ? && type == epsilon && character == ':',
		//  5. backref:    size == ? && type == backreference,
		//  -- assertions boundary --
		//  6. lookaround: size == ? && type == lookaround,
		//  7. assertion:  size == 0 && type == one of assertions (^, $, \b and \B).
#if !defined(SRELL_ENABLE_GT)
		return type < st_zero_width_boundary;
#else
		//  5.5. independent: size == ? && type == lookaround && character == '>',
		return type < st_zero_width_boundary || (type == st_lookaround_open && character == meta_char::mc_gt);
#endif
	}

	bool is_noncapturinggroup() const
	{
		return type == st_epsilon && character == meta_char::mc_colon;
	}

	bool has_0widthchecker() const
	{
		return type == st_roundbracket_open || type == st_backreference;
	}

	bool is_negcharclass() const
	{
		return type == st_character_class && is_not;
	}
};
//  re_state

template <typename charT>
//struct re_flags
struct re_compiler_state
{
//	bool i;
//	bool m;
//	bool s;

#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND)
	bool back;
#endif

	simple_array<unsigned int> atleast_widths_of_brackets;
#if !defined(SRELL_NO_NAMEDCAPTURE)
	groupname_mapper<charT> unresolved_gnames;
#endif

	void reset(const regex_constants::syntax_option_type & /* flags */)
	{
//		i = (flags & regex_constants::icase) != 0;	//  Case-insensitive.
//		m = (flags & regex_constants::multiline) != 0;
//		s = (flags & regex_constants::dotall) != 0;

#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND)
		back = false;
#endif

		atleast_widths_of_brackets.clear();

#if !defined(SRELL_NO_NAMEDCAPTURE)
		unresolved_gnames.clear();
#endif
	}

	void restore_from(const re_compiler_state &backup)
	{
#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND)
		back = backup.back;
#endif
	}
};
//  re_compiler_state

	}	//  namespace regex_internal

//  ... "rei_state.hpp"]
//  ["rei_search_state.hpp" ...

template <typename BidirectionalIterator>
class sub_match /* : std::pair<BidirectionalIterator, BidirectionalIterator> */;

	namespace regex_internal
	{

//template <typename charT>
struct re_state;

template </* typename charT, */typename BidirectionalIterator>
struct re_search_state_core
{
	const re_state/* <charT> */ *in_NFA_states;
	BidirectionalIterator in_string;
};

template <typename BidirectionalIterator>
struct re_submatch_core
{
	BidirectionalIterator open_at;
	BidirectionalIterator close_at;
};

template <typename BidirectionalIterator>
struct re_submatch_type
{
	re_submatch_core<BidirectionalIterator> core;
	unsigned int counter;
};

template </*typename charT, */typename BidirectionalIterator>
struct re_search_state_types
{
	typedef re_submatch_core<BidirectionalIterator> submatch_core;
	typedef re_submatch_type<BidirectionalIterator> submatch_type;
	typedef unsigned int counter_type;
	typedef BidirectionalIterator position_type;

	typedef std::vector<submatch_type> submatch_array;

	typedef re_search_state_core</*charT, */BidirectionalIterator> search_core_state;

	typedef std::vector<search_core_state> backtracking_array;
	typedef std::vector<submatch_core> capture_array;
	typedef simple_array<counter_type> counter_array;
	typedef std::vector<position_type> repeat_array;
};

template </*typename charT1, */typename charT2>
struct re_search_state_types</*charT1, */const charT2 *>
{
	typedef re_submatch_core<const charT2 *> submatch_core;
	typedef re_submatch_type<const charT2 *> submatch_type;
	typedef unsigned int counter_type;
	typedef const charT2 *position_type;

	typedef simple_array<submatch_type> submatch_array;

	typedef re_search_state_core</*charT1, */const charT2 *> search_core_state;

	typedef simple_array<search_core_state> backtracking_array;
	typedef simple_array<submatch_core> capture_array;
	typedef simple_array<position_type> repeat_array;
	typedef simple_array<counter_type> counter_array;
};
//  re_search_state_types

template </*typename charT, */typename BidirectionalIterator>
class re_search_state : public re_search_state_types</*charT, */BidirectionalIterator>
{
private:

	typedef re_search_state_types</*charT, */BidirectionalIterator> base_type;

public:

	typedef typename base_type::submatch_core submatchcore_type;
	typedef typename base_type::submatch_type submatch_type;
	typedef typename base_type::counter_type counter_type;
	typedef typename base_type::position_type position_type;

	typedef typename base_type::submatch_array submatch_array;

	typedef typename base_type::search_core_state search_core_state;

	typedef typename base_type::backtracking_array backtracking_array;
	typedef typename base_type::capture_array capture_array;
	typedef typename base_type::counter_array counter_array;
	typedef typename base_type::repeat_array repeat_array;

	typedef typename backtracking_array::size_type btstack_size_type;

public:

	struct bottom_state
	{
		btstack_size_type btstack_size;
		typename capture_array::size_type capturestack_size;
		typename counter_array::size_type counterstack_size;
		typename repeat_array::size_type repeatstack_size;

		bottom_state(
			const btstack_size_type bt,
			const typename capture_array::size_type h,
			const typename counter_array::size_type c,
			const typename repeat_array::size_type r)
			: btstack_size(bt)
			, capturestack_size(h)
			, counterstack_size(c)
			, repeatstack_size(r)
		{
		}
	};

public:

	search_core_state nth;

#if !defined(SRELL_NO_LIMIT_COUNTER)
	std::size_t failure_counter;
#endif

	BidirectionalIterator srchend;
	BidirectionalIterator lblim;

	BidirectionalIterator nextpos;

	backtracking_array bt_stack;

	capture_array capture_stack;
	counter_array counter_stack;
	repeat_array repeat_stack;

	submatch_array bracket;
	counter_array counter;
	repeat_array repeat;

	btstack_size_type btstack_size;

	BidirectionalIterator srchbegin;

public:

	void init
	(
		const BidirectionalIterator &begin,
		const BidirectionalIterator &end,
		const BidirectionalIterator &lookbehindlimit,
		const regex_constants::match_flag_type flags
	)
	{
		lblim = lookbehindlimit;
		nextpos = srchbegin = begin;
		srchend = end;
		flags_ = flags;
	}

	void init_for_automaton
	(
		const re_state/*<charT>*/ *const entry,
		unsigned int num_of_submatches,
		const unsigned int num_of_counters,
		const unsigned int num_of_repeats
	)
	{
		entry_state_ = entry;

		bracket.resize(num_of_submatches);
		counter.resize(num_of_counters);
		repeat.resize(num_of_repeats);

		nth.in_string = (flags_ & regex_constants::match_continuous) ? srchbegin : srchend;

		while (num_of_submatches > 1)
		{
			submatch_type &br = bracket[--num_of_submatches];

			br.core.open_at = br.core.close_at = this->srchend;
			br.counter = 0;
			//  15.10.2.9; AtomEscape:
			//  If the regular expression has n or more capturing parentheses
			//  but the nth one is undefined because it hasn't captured anything,
			//  then the backreference always succeeds.

			//  C.f., table 27 and 28 on TR1, table 142 and 143 on C++11.
		}

		clear_stacks();
	}

#if defined(SRELL_NO_LIMIT_COUNTER)
	void reset(/* const BidirectionalIterator &start */)
#else
	void reset(/* const BidirectionalIterator &start, */ const std::size_t limit)
#endif
	{
		nth.in_NFA_states = this->entry_state_;

		bracket[0].core.open_at = nth.in_string;

#if !defined(SRELL_NO_LIMIT_COUNTER)
		failure_counter = limit;
#endif
	}

	bool is_at_lookbehindlimit() const
	{
		return nth.in_string == this->lblim;
	}

	bool is_at_srchend() const
	{
		return nth.in_string == this->srchend;
	}

	bool is_null() const
	{
		return nth.in_string == bracket[0].core.open_at;
	}

//	regex_constants::match_flag_type flags() const
//	{
//		return this->flags_;
//	}

	bool match_not_bol_flag() const
	{
		if (this->flags_ & regex_constants::match_not_bol)
			return true;
		return false;
	}

	bool match_not_eol_flag() const
	{
		if (this->flags_ & regex_constants::match_not_eol)
			return true;
		return false;
	}

	bool match_not_bow_flag() const
	{
		if (this->flags_ & regex_constants::match_not_bow)
			return true;
		return false;
	}

	bool match_not_eow_flag() const
	{
		if (this->flags_ & regex_constants::match_not_eow)
			return true;
		return false;
	}

	bool match_prev_avail_flag() const
	{
		if (this->flags_ & regex_constants::match_prev_avail)
			return true;
		return false;
	}

	bool match_not_null_flag() const
	{
		if (this->flags_ & regex_constants::match_not_null)
			return true;
		return false;
	}

	bool match_continuous_flag() const
	{
		if (this->flags_ & regex_constants::match_continuous)
			return true;
		return false;
	}

	bool match_match_flag() const
	{
		if (this->flags_ & regex_constants::match_match_)
			return true;
		return false;
	}

	bool set_bracket0(const BidirectionalIterator &begin, const BidirectionalIterator &end)
	{
		nth.in_string = begin;
		nextpos = end;
		return true;
	}

	void clear_stacks()
	{
		btstack_size = 0;
		bt_stack.clear();
		capture_stack.clear();
		repeat_stack.clear();
		counter_stack.clear();
	}

	btstack_size_type size() const	//  For debug.
	{
		return bt_stack.size();
	}

	bool is_empty() const	//  For debug.
	{
		if (btstack_size == 0
			&& bt_stack.size() == 0
			&& capture_stack.size() == 0
			&& repeat_stack.size() == 0
			&& counter_stack.size() == 0)
			return true;

		return false;
	}

private:

	/* const */regex_constants::match_flag_type flags_;
	const re_state/* <charT> */ * /* const */entry_state_;
};
//  re_search_state

	}	//  namespace regex_internal

//  ... "rei_search_state.hpp"]
//  ["rei_bmh.hpp" ...

	namespace regex_internal
	{

#if !defined(SRELLDBG_NO_BMH)

template <typename charT, typename utf_traits>
class re_bmh
{
public:

	re_bmh()
	{
	}

	re_bmh(const re_bmh &right)
	{
		operator=(right);
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	re_bmh(re_bmh &&right) SRELL_NOEXCEPT
	{
		operator=(std::move(right));
	}
#endif

	re_bmh &operator=(const re_bmh &that)
	{
		if (this != &that)
		{
			this->u21string_ = that.u21string_;

			this->bmtable_ = that.bmtable_;
			this->repseq_ = that.repseq_;
		}
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	re_bmh &operator=(re_bmh &&that) SRELL_NOEXCEPT
	{
		if (this != &that)
		{
			this->u21string_ = std::move(that.u21string_);

			this->bmtable_ = std::move(that.bmtable_);
			this->repseq_ = std::move(that.repseq_);
		}
		return *this;
	}
#endif

	void clear()
	{
		u21string_.clear();

		bmtable_.clear();
		repseq_.clear();
	}

	void setup(const simple_array<uchar21> &u21s, const bool icase)
	{
		u21string_ = u21s;
		setup_();

		if (!icase)
			setup_for_casesensitive();
		else
			setup_for_icase();
	}

	template <typename RandomAccessIterator>
	bool do_casesensitivesearch(re_search_state<RandomAccessIterator> &sstate, const std::random_access_iterator_tag) const
	{
		RandomAccessIterator begin = sstate.srchbegin;
		const RandomAccessIterator end = sstate.srchend;
		std::size_t offset = static_cast<std::size_t>(repseq_.size() - 1);
		const charT *const relastchar = &repseq_[offset];

		for (; static_cast<std::size_t>(end - begin) > offset;)
		{
			begin += offset;

			if (*begin == *relastchar)
			{
				const charT *re = relastchar;
				RandomAccessIterator tail = begin;

				for (; *--re == *--tail;)
				{
					if (re == repseq_.data())
						return sstate.set_bracket0(tail, ++begin);
				}
			}
			offset = bmtable_[*begin & 0xff];
		}
		return false;
	}

	template <typename BidirectionalIterator>
	bool do_casesensitivesearch(re_search_state<BidirectionalIterator> &sstate, const std::bidirectional_iterator_tag) const
	{
		BidirectionalIterator begin = sstate.srchbegin;
		const BidirectionalIterator end = sstate.srchend;
		std::size_t offset = static_cast<std::size_t>(repseq_.size() - 1);
		const charT *const relastchar = &repseq_[offset];

		for (;;)
		{
			for (; offset; --offset, ++begin)
				if (begin == end)
					return false;

			if (*begin == *relastchar)
			{
				const charT *re = relastchar;
				BidirectionalIterator tail = begin;

				for (; *--re == *--tail;)
				{
					if (re == repseq_.data())
						return sstate.set_bracket0(tail, ++begin);
				}
			}
			offset = bmtable_[*begin & 0xff];
		}
	}

	template <typename RandomAccessIterator>
	bool do_icasesearch(re_search_state<RandomAccessIterator> &sstate, const std::random_access_iterator_tag) const
	{
		const RandomAccessIterator begin = sstate.srchbegin;
		const RandomAccessIterator end = sstate.srchend;
		std::size_t offset = bmtable_[256];
		const uchar21 entrychar = u21string_[u21string_.size() - 1];
		const uchar21 *const re2ndlastchar = &u21string_[u21string_.size() - 2];
		RandomAccessIterator curpos = begin;

		for (; static_cast<std::size_t>(end - curpos) > offset;)
		{
			curpos += offset;

			for (; utf_traits::is_trailing(*curpos);)
				if (++curpos == end)
					return false;

			const uchar21 txtlastchar = utf_traits::codepoint(curpos, end);

			if (txtlastchar == entrychar || unicode_case_folding::do_casefolding(txtlastchar) == entrychar)
			{
				const uchar21 *re = re2ndlastchar;
				RandomAccessIterator tail = curpos;

//				for (; *--re == unicode_case_folding::do_casefolding(utf_traits::dec_codepoint(tail, begin));)
				for (; *re == unicode_case_folding::do_casefolding(utf_traits::dec_codepoint(tail, begin)); --re)
				{
					if (re == u21string_.data())
					{
						utf_traits::codepoint_inc(curpos, end);
						return sstate.set_bracket0(tail, curpos);
					}
					if (tail == begin)
						break;
				}
			}
			offset = bmtable_[txtlastchar & 0xff];
		}
		return false;
	}

	template <typename BidirectionalIterator>
	bool do_icasesearch(re_search_state<BidirectionalIterator> &sstate, const std::bidirectional_iterator_tag) const
	{
		const BidirectionalIterator begin = sstate.srchbegin;
		const BidirectionalIterator end = sstate.srchend;

		if (begin != end)
		{
			std::size_t offset = bmtable_[256];	//static_cast<std::size_t>(u21string_.size() - 1);
			const uchar21 entrychar = u21string_[offset];
			const uchar21 *const re2ndlastchar = &u21string_[offset - 1];
			BidirectionalIterator curpos = begin;

			for (;;)
			{
				for (;;)
				{
					if (++curpos == end)
						return false;
					if (!utf_traits::is_trailing(*curpos))
						if (--offset == 0)
							break;
				}
//				const uchar21 txtlastchar = unicode_case_folding::do_casefolding(utf_traits::codepoint(curpos, end));
				const uchar21 txtlastchar = utf_traits::codepoint(curpos, end);

//				if (txtlastchar == *re2ndlastchar)
//				if (txtlastchar == *re2ndlastchar || unicode_case_folding::do_casefolding(txtlastchar) == *re2ndlastchar)
				if (txtlastchar == entrychar || unicode_case_folding::do_casefolding(txtlastchar) == entrychar)
				{
					const uchar21 *re = re2ndlastchar;
					BidirectionalIterator tail = curpos;

					for (; *re == unicode_case_folding::do_casefolding(utf_traits::dec_codepoint(tail, begin)); --re)
					{
						if (re == u21string_.data())
						{
							utf_traits::codepoint_inc(curpos, end);
							return sstate.set_bracket0(tail, curpos);
						}
						if (tail == begin)
							break;
					}
				}
				offset = bmtable_[txtlastchar & 0xff];
			}
		}
		return false;
	}

private:

	void setup_()
	{
		bmtable_.resize(257);
	}

	void setup_for_casesensitive()
	{
		charT mbstr[utf_traits::maxseqlen];
		const std::size_t u21str_lastcharpos_ = static_cast<std::size_t>(u21string_.size() - 1);

		repseq_.clear();

		for (std::size_t i = 0; i <= u21str_lastcharpos_; ++i)
		{
			const uchar21 seqlen = utf_traits::to_codeunits(mbstr, u21string_[i]);

			for (uchar21 j = 0; j < seqlen; ++j)
				repseq_.push_back(mbstr[j]);
		}

		for (std::size_t i = 0; i < 256; ++i)
			bmtable_[i] = static_cast<std::size_t>(repseq_.size());

		const std::size_t repseq_lastcharpos_ = static_cast<std::size_t>(repseq_.size() - 1);

		for (std::size_t i = 0; i < repseq_lastcharpos_; ++i)
			bmtable_[repseq_[i] & 0xff] = repseq_lastcharpos_ - i;
	}

	void setup_for_icase()
	{
		charT mbstr[utf_traits::maxseqlen];
		uchar21 u21table[unicode_case_folding::rev_maxset];
		const std::size_t u21str_lastcharpos = static_cast<std::size_t>(u21string_.size() - 1);
		simple_array<std::size_t> minlen(u21string_.size());
		std::size_t cu_repseq_lastcharpos = 0;

		for (std::size_t i = 0; i <= u21str_lastcharpos; ++i)
		{
			const uchar21 setnum = unicode_case_folding::casefoldedcharset(u21table, u21string_[i]);
			uchar21 u21c = u21table[0];

			for (uchar21 j = 1; j < setnum; ++j)
				if (u21c > u21table[j])
					u21c = u21table[j];

			if (i < u21str_lastcharpos)
				cu_repseq_lastcharpos += minlen[i] = utf_traits::to_codeunits(mbstr, u21c);
		}

		++cu_repseq_lastcharpos;

		for (std::size_t i = 0; i < 256; ++i)
			bmtable_[i] = cu_repseq_lastcharpos;

		bmtable_[256] = --cu_repseq_lastcharpos;

		for (std::size_t i = 0; i < u21str_lastcharpos; ++i)
		{
			const uchar21 setnum = unicode_case_folding::casefoldedcharset(u21table, u21string_[i]);

			for (uchar21 j = 0; j < setnum; ++j)
				bmtable_[u21table[j] & 0xff] = cu_repseq_lastcharpos;

			cu_repseq_lastcharpos -= minlen[i];
		}
	}

public:	//  For debug.

	void print_table() const;
	void print_seq() const;

private:

	simple_array<uchar21> u21string_;
//	std::size_t bmtable_[256];
	simple_array<std::size_t> bmtable_;
	simple_array<charT> repseq_;
};
//  re_bmh

#endif	//  !defined(SRELLDBG_NO_BMH)
	}	//  namespace regex_internal

//  ... "rei_bmh.hpp"]
//  ["rei_compiler.hpp" ...

	namespace regex_internal
	{

template <typename charT, typename traits>
struct re_object_core
{
protected:

	typedef re_state/*<charT>*/ state_type;
	typedef simple_array<state_type> state_array;

	state_array NFA_states;
	re_character_class character_class;

#if !defined(SRELLDBG_NO_1STCHRCLS)
	#if !defined(SRELLDBG_NO_BITSET)
	bitset<traits::utf_traits::bitsetsize> firstchar_class_bs;
	#else
	range_pairs firstchar_class;
	#endif
#endif

#if !defined(SRELL_NO_LIMIT_COUNTER)
public:

	std::size_t limit_counter;

protected:
#endif

	typedef typename traits::utf_traits utf_traits;

	unsigned int number_of_brackets;
	unsigned int number_of_counters;
	unsigned int number_of_repeats;
	regex_constants::syntax_option_type soflags;

#if !defined(SRELL_NO_NAMEDCAPTURE)
	groupname_mapper<charT> namedcaptures;
	typedef typename groupname_mapper<charT>::gname_string gname_string;
#endif

#if !defined(SRELLDBG_NO_BMH)
	re_bmh<charT, utf_traits> *bmdata;
#endif

#if !defined(SRELL_NO_LIMIT_COUNTER)
private:

	static const std::size_t lcounter_defnum_ = 16777216;

#endif

protected:

	re_object_core()
#if !defined(SRELL_NO_LIMIT_COUNTER)
		: limit_counter(lcounter_defnum_)
	#if !defined(SRELLDBG_NO_BMH)
		, bmdata(NULL)
	#endif
#elif !defined(SRELLDBG_NO_BMH)
		: bmdata(NULL)
#endif
	{
	}

	re_object_core(const re_object_core &right)
#if !defined(SRELLDBG_NO_BMH)
		: bmdata(NULL)
#endif
	{
		operator=(right);
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	re_object_core(re_object_core &&right) SRELL_NOEXCEPT
#if !defined(SRELLDBG_NO_BMH)
		: bmdata(NULL)
#endif
	{
		operator=(std::move(right));
	}
#endif

#if !defined(SRELLDBG_NO_BMH)
	~re_object_core()
	{
		if (bmdata)
			delete bmdata;
	}
#endif

	void reset(const regex_constants::syntax_option_type flags)
	{
		NFA_states.clear();
		character_class.clear();

#if !defined(SRELLDBG_NO_1STCHRCLS)
	#if !defined(SRELLDBG_NO_BITSET)
		firstchar_class_bs.reset();
	#else
		firstchar_class.clear();
	#endif
#endif

#if !defined(SRELL_NO_LIMIT_COUNTER)
		limit_counter = lcounter_defnum_;
#endif

		number_of_brackets = 1;
		number_of_counters = 0;
		number_of_repeats  = 0;
		soflags = flags;	//  regex_constants::ECMAScript;

#if !defined(SRELL_NO_NAMEDCAPTURE)
		namedcaptures.clear();
#endif

#if !defined(SRELLDBG_NO_BMH)
//		bmdata->clear();
		if (bmdata)
			delete bmdata;
		bmdata = NULL;
#endif
	}

	re_object_core &operator=(const re_object_core &that)
	{
		if (this != &that)
		{
			this->NFA_states = that.NFA_states;
			this->character_class = that.character_class;

#if !defined(SRELLDBG_NO_1STCHRCLS)
	#if !defined(SRELLDBG_NO_BITSET)
			this->firstchar_class_bs = that.firstchar_class_bs;
	#else
			this->firstchar_class = that.firstchar_class;
	#endif
#endif

#if !defined(SRELL_NO_LIMIT_COUNTER)
			this->limit_counter = that.limit_counter;
#endif

//			this->utf_traits_inst = that.utf_traits_inst;

			this->number_of_brackets = that.number_of_brackets;
			this->number_of_counters = that.number_of_counters;
			this->number_of_repeats = that.number_of_repeats;
			this->soflags = that.soflags;

#if !defined(SRELL_NO_NAMEDCAPTURE)
			this->namedcaptures = that.namedcaptures;
#endif

#if !defined(SRELLDBG_NO_BMH)
			if (that.bmdata)
			{
				if (this->bmdata)
					*this->bmdata = *that.bmdata;
				else
					this->bmdata = new re_bmh<charT, utf_traits>(*that.bmdata);
			}
			else if (this->bmdata)
			{
				delete this->bmdata;
				this->bmdata = NULL;
			}
#endif

			if (that.NFA_states.size())
				repair_nextstates(&that.NFA_states[0]);
		}
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	re_object_core &operator=(re_object_core &&that) SRELL_NOEXCEPT
	{
		if (this != &that)
		{
			this->NFA_states = std::move(that.NFA_states);
			this->character_class = std::move(that.character_class);

#if !defined(SRELLDBG_NO_1STCHRCLS)
	#if !defined(SRELLDBG_NO_BITSET)
			this->firstchar_class_bs = std::move(that.firstchar_class_bs);
	#else
			this->firstchar_class = std::move(that.firstchar_class);
	#endif
#endif

#if !defined(SRELL_NO_LIMIT_COUNTER)
			this->limit_counter = that.limit_counter;
#endif

			this->number_of_brackets = that.number_of_brackets;
			this->number_of_counters = that.number_of_counters;
			this->number_of_repeats = that.number_of_repeats;
			this->soflags = that.soflags;

#if !defined(SRELL_NO_NAMEDCAPTURE)
			this->namedcaptures = std::move(that.namedcaptures);
#endif

#if !defined(SRELLDBG_NO_BMH)
			if (this->bmdata)
				delete this->bmdata;
			this->bmdata = that.bmdata;
			that.bmdata = NULL;
#endif
		}
		return *this;
	}
#endif	//  defined(SRELL_CPP11_MOVE_ENABLED)

	void swap(re_object_core &right)
	{
		if (this != &right)
		{
			this->NFA_states.swap(right.NFA_states);
			this->character_class.swap(right.character_class);

#if !defined(SRELLDBG_NO_1STCHRCLS)
	#if !defined(SRELLDBG_NO_BITSET)
			this->firstchar_class_bs.swap(right.firstchar_class_bs);
	#else
			this->firstchar_class.swap(right.firstchar_class);
	#endif
#endif

#if !defined(SRELL_NO_LIMIT_COUNTER)
			{
				const std::size_t tmp_limit_counter = this->limit_counter;
				this->limit_counter = right.limit_counter;
				right.limit_counter = tmp_limit_counter;
			}
#endif
//			this->utf_traits_inst.swap(right.utf_traits_inst);

			{
				const unsigned int tmp_numof_brackets = this->number_of_brackets;
				this->number_of_brackets = right.number_of_brackets;
				right.number_of_brackets = tmp_numof_brackets;
			}
			{
				const unsigned int tmp_numof_counters = this->number_of_counters;
				this->number_of_counters = right.number_of_counters;
				right.number_of_counters = tmp_numof_counters;
			}
			{
				const unsigned int tmp_numof_repeats = this->number_of_repeats;
				this->number_of_repeats = right.number_of_repeats;
				right.number_of_repeats = tmp_numof_repeats;
			}
			{
				const regex_constants::syntax_option_type tmp_soflags = this->soflags;
				this->soflags = right.soflags;
				right.soflags = tmp_soflags;
			}

#if !defined(SRELL_NO_NAMEDCAPTURE)
			this->namedcaptures.swap(right.namedcaptures);
#endif

#if !defined(SRELLDBG_NO_BMH)
			{
				re_bmh<charT, utf_traits> *const tmp_bmdata = this->bmdata;
				this->bmdata = right.bmdata;
				right.bmdata = tmp_bmdata;
			}
#endif
		}
	}

	void throw_error(const regex_constants::error_type &e)
	{
//		reset();
		NFA_states.clear();
#if !defined(SRELLDBG_NO_BMH)
		if (bmdata)
			delete bmdata;
		bmdata = NULL;
#endif
		throw regex_error(e);
	}

private:

	void repair_nextstates(const state_type *const oldbase)
	{
		state_type *const newbase = &this->NFA_states[0];

		for (typename state_array::size_type i = 0; i < this->NFA_states.size(); ++i)
		{
			state_type &state = this->NFA_states[i];

			if (state.next_state1)
				state.next_state1 = state.next_state1 - oldbase + newbase;

			if (state.next_state2)
				state.next_state2 = state.next_state2 - oldbase + newbase;
		}
	}
};
//  re_object_core

template <typename charT, typename traits>
class re_compiler : public re_object_core<charT, traits>
{
protected:

	template <typename ForwardIterator>
	bool compile(ForwardIterator begin, const ForwardIterator &end, const regex_constants::syntax_option_type flags /* = regex_constants::ECMAScript */)
	{
		simple_array<uchar21> u21;

		while (begin != end)
		{
			const uchar21 u21c = utf_traits::codepoint_inc(begin, end);
			if (u21c > constants::unicode_max_codepoint)
				this->throw_error(regex_constants::error_utf8);
			u21.push_back(u21c);
		}

		return compile_core(u21.data(), u21.data() + u21.size(), flags);
	}

	bool is_icase() const
	{
		if (this->soflags & regex_constants::icase)
			return true;
		return false;
	}
	bool is_ricase() const
	{
		return /* this->NFA_states.size() && */ this->NFA_states[0].icase == true;
	}

	bool is_multiline() const
	{
		if (this->soflags & regex_constants::multiline)
			return true;
		return false;
	}

	bool is_dotall() const
	{
		return (this->soflags & regex_constants::dotall) ? true : false;
	}

private:

	typedef re_object_core<charT, traits> base_type;
	typedef typename base_type::utf_traits utf_traits;
	typedef typename base_type::state_type state_type;
	typedef typename base_type::state_array state_array;
#if !defined(SRELL_NO_NAMEDCAPTURE)
	typedef typename base_type::gname_string gname_string;
#endif
#if !defined(SRELL_NO_UNICODE_PROPERTY)
	typedef typename re_character_class::pstring pstring;
#endif

	bool compile_core(const uchar21 *begin, const uchar21 *const end, const regex_constants::syntax_option_type flags)
	{
		re_quantifier piececharlen;
		re_compiler_state<charT> cstate;
		state_type atom;

		this->reset(flags);
//		this->soflags = flags;
		cstate.reset(flags);

		atom.reset();
		atom.type = st_epsilon;
		this->NFA_states.push_back(atom);

		if (!make_nfa_states(this->NFA_states, piececharlen, begin, end, cstate))
		{
			return false;
		}

		if (begin != end)
			this->throw_error(regex_constants::error_paren);	//  ')'s are too many.

		if (!check_backreferences(cstate))
			this->throw_error(regex_constants::error_backref);

		if (this->is_icase())
			this->NFA_states[0].icase = check_if_really_needs_icase_search();

#if !defined(SRELLDBG_NO_BMH)
		setup_bmhdata();
#endif

		atom.type = st_success;
		atom.next1 = 0;
		this->NFA_states.push_back(atom);

		optimise();
		relativejump_to_absolutejump();

		return true;
	}

	bool make_nfa_states(state_array &piece, re_quantifier &piececharlen, const uchar21 *&curpos, const uchar21 *const end, re_compiler_state<charT> &cstate)
	{
		typename state_array::size_type prevbranch_end = 0;
		state_type atom;
		state_array branch;
		re_quantifier branchsize;

		piececharlen.reset(0);

		for (;;)
		{
			branch.clear();

			if (!make_branch(branch, branchsize, curpos, end, cstate))
				return false;

			//  For piececharlen.atleast, 0 as the initial value and 0 as an
			//  actual value must be distinguished.
			if (piececharlen.atmost == 0 || piececharlen.atleast > branchsize.atleast)
				piececharlen.atleast = branchsize.atleast;

			if (piececharlen.atmost < branchsize.atmost)
				piececharlen.atmost = branchsize.atmost;

			if (curpos != end && *curpos == meta_char::mc_bar)
			{
				atom.reset();
				atom.character = meta_char::mc_bar;
				atom.type = st_epsilon;
				atom.next2 = static_cast<std::ptrdiff_t>(branch.size()) + 2;
				branch.insert(0, atom);
			}

			if (prevbranch_end)
				piece[prevbranch_end].next1 = static_cast<std::ptrdiff_t>(branch.size()) + 1;

			piece += branch;

				//  end or ')'
			if (curpos == end || *curpos == meta_char::mc_rbracl)
				break;

			//  *curpos == '|'

			prevbranch_end = piece.size();
			atom.reset();
			atom.type = st_epsilon;
			piece.push_back(atom);

			++curpos;
		}
		return true;
	}

	bool make_branch(state_array &branch, re_quantifier &branchsize, const uchar21 *&curpos, const uchar21 *const end, re_compiler_state<charT> &cstate)
	{
		state_array piece;
		state_array piece_with_quantifier;
		re_quantifier quantifier;

		branchsize.reset(0);

		for (;;)
		{
			re_quantifier piececharlen;

			if (curpos == end)
				return true;

			piece.clear();
			piece_with_quantifier.clear();

			switch (*curpos)
			{
//			case char_ctrl::cc_nul:	//  '\0':
			case meta_char::mc_bar:	//  '|':
			case meta_char::mc_rbracl:	//  ')':
				return true;

			default:
				if (!get_atom(piece, piececharlen, curpos, end, cstate))
					return false;
			}

			if (piece.size())
			{
				const state_type &firstatom = piece[0];

				quantifier.reset();	//  quantifier.atleast = quantifier.atmost = 1;

				if (firstatom.has_quantifier())
				{
					if (curpos != end && !get_quantifier(quantifier, curpos, end))
						return false;
				}

				if (piece.size() == 2 && firstatom.is_noncapturinggroup() && piece[1].is_noncapturinggroup())
				{
					//  (?:) alone or followed by a quantifier.
//					piece_with_quantifier += piece;
					;	//  Do nothing.
				}
				else
					combine_piece_with_quantifier(piece_with_quantifier, piece, quantifier, piececharlen);

#if 01
				piececharlen.multiply(quantifier);
				branchsize.add(piececharlen);
#else
				branchsize.atleast += piececharlen.atleast * quantifier.atleast;
				if (!branchsize.is_infinity())
				{
					if (piececharlen.is_infinity() || quantifier.is_infinity())
						branchsize.set_infinity();
					else
						branchsize.atmost += piececharlen.atmost * quantifier.atmost;
				}
#endif

#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND)

				if (!cstate.back)
					branch += piece_with_quantifier;
				else
					branch.insert(0, piece_with_quantifier);
#else
				branch += piece_with_quantifier;
#endif
			}
		}
	}

	bool get_atom(state_array &piece, re_quantifier &atomsize, const uchar21 *&curpos, const uchar21 *const end, re_compiler_state<charT> &cstate)
	{
		state_type atom;

		atom.reset();
		atom.character = *curpos++;

		switch (atom.character)
		{
		case meta_char::mc_rbraop:	//  '(':
			return get_piece_in_roundbrackets(piece, atomsize, curpos, end, cstate);

		case meta_char::mc_sbraop:	//  '[':
			if (!register_character_class(atom, curpos, end, cstate))
				return false;

			break;

		case meta_char::mc_escape:	//  '\\':
			if (!translate_atom_escape(atom, curpos, end, cstate))
				return false;

			break;

		case meta_char::mc_period:	//  '.':
			atom.type = st_character_class;
#if !defined(SRELL_NO_SINGLELINE)
			if (this->is_dotall())
			{
				atom.number = static_cast<unsigned int>(re_character_class::dotall);
			}
			else
#endif
			{
				atom.number = static_cast<unsigned int>(re_character_class::newline);
				atom.is_not = true;
				register_if_negatedcharclass(atom);
			}
			break;

		case meta_char::mc_caret:	//  '^':
			atom.type = st_bol;
			atom.quantifier.reset(0);
//			if (current_flags.m)
//				atom.multiline = true;
			break;

		case meta_char::mc_dollar:	//  '$':
			atom.type = st_eol;
			atom.quantifier.reset(0);
//			if (current_flags.m)
//				atom.multiline = true;
			break;

		case meta_char::mc_astrsk:	//  '*':
		case meta_char::mc_plus:	//  '+':
		case meta_char::mc_query:	//  '?':
		case meta_char::mc_cbraop:	//  '{'
			this->throw_error(regex_constants::error_badrepeat);

		default:;
		}

		if (atom.type == st_character)
		{
			if (this->is_icase())
				atom.character = unicode_case_folding::do_casefolding(atom.character);
		}

		piece.push_back(atom);
		atomsize = atom.quantifier;

		return true;
	}

	//  '('.

	bool get_piece_in_roundbrackets(state_array &piece, re_quantifier &piececharlen, const uchar21 *&curpos, const uchar21 *const end, re_compiler_state<charT> &cstate)
	{
		const re_compiler_state<charT> original_cstate(cstate);
		state_type atom;

		if (curpos == end)
			this->throw_error(regex_constants::error_paren);

		atom.reset();
		atom.type = st_roundbracket_open;

		if (*curpos == meta_char::mc_query)	//  '?'
		{
#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND)
			if (!extended_roundbrackets(piece, atom, ++curpos, end, cstate))
#else
			if (!extended_roundbrackets(piece, atom, ++curpos, end))
#endif
				return false;
		}

		if (atom.type == st_roundbracket_open)
		{
			push_bracket_open(piece, atom);
		}

//		if (curpos == end)
//			this->throw_error(regex_constants::error_paren);

		if (!make_nfa_states(piece, piececharlen, curpos, end, cstate))
			return false;

		//  end or ')'?
		if (curpos == end)
			this->throw_error(regex_constants::error_paren);

		++curpos;

		cstate.restore_from(original_cstate);

		switch (atom.type)
		{
		case st_epsilon:

//			if (piece.size() <= 2)	//  ':' or ':' + one.
			if (piece.size() == 2)	//  ':' + something.
			{
				piece.erase(0);
				return true;
			}

			piece[0].quantifier.atmost = this->number_of_brackets - 1;
			break;

		case st_lookaround_pop:
			{
				state_type &firstatom = piece[0];

#if defined(SRELL_FIXEDWIDTHLOOKBEHIND)
//				if (firstatom.reverse)
				if (firstatom.quantifier.atleast)	//  Marked as lookbehind.
				{
					if (!piececharlen.is_same() || piececharlen.is_infinity())
						this->throw_error(regex_constants::error_lookbehind);

					firstatom.quantifier = piececharlen;
				}
#endif

#if defined(SRELL_ENABLE_GT)
				if (firstatom.character != meta_char::mc_gt)
#endif
					piececharlen.reset(0);

				firstatom.next1 = static_cast<std::ptrdiff_t>(piece.size()) + 1;

				atom.type  = st_lookaround_close;
				atom.next1 = 0;
			}
			break;

		default:
			set_bracket_close(piece, atom, piececharlen, cstate);
		}

		piece.push_back(atom);
		return true;
	}

#if !defined(NO_REVERSE) && !defined(SRELL_FIXEDWIDTHLOOKBEHIND)
	bool extended_roundbrackets(state_array &piece, state_type &atom, const uchar21 *&curpos, const uchar21 *const end, re_compiler_state<charT> &cstate)
#else
	bool extended_roundbrackets(state_array &piece, state_type &atom, const uchar21 *&curpos, const uchar21 *const end)
#endif
	{
#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND)
		bool lookbehind = false;
#endif

		if (curpos == end)
			this->throw_error(regex_constants::error_paren);

		atom.character = *curpos;

		if (atom.character == meta_char::mc_lt)	//  '<'
		{
#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND)
			lookbehind = true;
#endif
			if (++curpos == end)
				this->throw_error(regex_constants::error_paren);

			atom.character = *curpos;

			if (atom.character != meta_char::mc_eq && atom.character != meta_char::mc_exclam)
			{
#if !defined(SRELL_NO_NAMEDCAPTURE)
				return parse_groupname(curpos, end);
#else
				this->throw_error(regex_constants::error_paren);
#endif	//  !defined(SRELL_NO_NAMEDCAPTURE)
			}
		}
		else
			atom.quantifier.atleast = 0;
			//  Sets atleast to 0 for other assertions than lookbehinds. The automaton
			//  checks atleast to know whether lookbehinds or other assertions.

		switch (atom.character)
		{
		case meta_char::mc_colon:
			atom.type = st_epsilon;
			atom.quantifier.atleast = this->number_of_brackets;
			break;

		case meta_char::mc_exclam:	//  '!':
			atom.is_not = true;
			//@fallthrough@

		case meta_char::mc_eq:	//  '=':
#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND)
			cstate.back = lookbehind;
#else
//			atom.reverse = lookbehind;
#endif

#if defined(SRELL_ENABLE_GT)
		case meta_char::mc_gt:
#endif
			atom.type = st_lookaround_open;
			atom.next2 = 1;
			piece.push_back(atom);
			atom.next1 = 1;
			atom.next2 = 0;
			atom.type = st_lookaround_pop;
			break;

		default:
			this->throw_error(regex_constants::error_paren);
		}

		++curpos;
		piece.push_back(atom);
		return true;
	}

	void push_bracket_open(state_array &piece, state_type &atom)
	{
		atom.number = this->number_of_brackets;
		atom.next1  = 2;
		atom.next2  = 1;
		piece.push_back(atom);
		++this->number_of_brackets;

		atom.type  = st_roundbracket_pop;
		atom.next1 = 0;
		atom.next2 = 0;
		piece.push_back(atom);
	}

	void set_bracket_close(state_array &piece, state_type &atom, const re_quantifier &piececharlen, re_compiler_state<charT> &cstate)
	{
//		unsigned int max_bracketno = atom.number;

		atom.type = st_roundbracket_close;
		atom.next1 = 1;
		atom.next2 = 1;
#if 0
		for (typename state_array::size_type i = 0; i < piece.size(); ++i)
		{
			const state_type &state = piece[i];

			if (state.type == st_roundbracket_open && max_bracketno < state.number)
				max_bracketno = state.number;
		}
#endif

		re_quantifier &rb_open = piece[0].quantifier;
		re_quantifier &rb_pop = piece[1].quantifier;

		rb_open.atleast = rb_pop.atleast = atom.number + 1;
		rb_open.atmost = rb_pop.atmost = this->number_of_brackets - 1;	//  max_bracketno;

		if (cstate.atleast_widths_of_brackets.size() < atom.number)
			cstate.atleast_widths_of_brackets.resize(atom.number, 0);

		cstate.atleast_widths_of_brackets[atom.number - 1] = piececharlen.atleast;
	}

	void combine_piece_with_quantifier(state_array &piece_with_quantifier, state_array &piece, const re_quantifier &quantifier, const re_quantifier &piececharlen)
	{
		state_type &firstatom = piece[0];
//		const bool firstpiece_is_roundbracket_open = (firstatom.type == st_roundbracket_open);
		const bool piece_has_0widthchecker = firstatom.has_0widthchecker();
		const bool piece_is_noncapturinggroup_contaning_capturinggroup = firstatom.is_noncapturinggroup() && firstatom.quantifier.is_valid();
		state_type atom;

		if (quantifier.atmost == 0)
			return;

		atom.reset();
		atom.quantifier = quantifier;
		if (firstatom.is_character_or_class())
			atom.character = char_other::co_sp;	//  Marked for nextpos_optimisation3().

		if (quantifier.atmost == 1)
		{
			if (quantifier.atleast == 0)
			{
				atom.type  = st_epsilon;
				atom.next2 = static_cast<std::ptrdiff_t>(piece.size()) + 1;

				if (!quantifier.is_greedy)
				{
					atom.next1 = atom.next2;
					atom.next2 = 1;
				}
				piece_with_quantifier.push_back(atom);
				//      (push)
			}

			if (piece.size() >= 2 && firstatom.type == st_roundbracket_open && piece[1].type == st_roundbracket_pop)
			{
				firstatom.quantifier.atmost = 0u;
				piece[1].quantifier.atmost = 0u;
			}

			piece_with_quantifier += piece;
			return;
		}

		//  atmost >= 2

#if !defined(SRELLDBG_NO_SIMPLEEQUIV)
		//  The counter requires at least 6 states: save, restore, check, inc, dec, atom(s).
		//  A character or charclass quantified by one of these has a simple equivalent representation:
		//  a{0,2}  1.epsilon(2|5), 2.CHorCL(3), 3.epsilon(4|5), 4.CHorCL(5), [5].
		//  a{0,3}  1.epsilon(2|7), 2.CHorCL(3), 3.epsilon(4|7), 4.CHorCL(5), 5.epsilon(6|7), 6.CHorCL(7), [7].
		//  a{1,2}  1.CHorCL(2), 2.epsilon(3|4), 3.CHorCL(4), [4].
		//  a{1,3}  1.CHorCL(2), 2.epsilon(3|6), 3.CHorCL(4), 4.epsilon(5|6), 5.CHorCL(6), [6].
		//  a{2,3}  1.CHorCL(2), 2.CHorCL(3), 3.epsilon(4|5), 4.CHorCL(5), [5].
		//  a{2,4}  1.CHorCL(2), 2.CHorCL(3), 3.epsilon(4|7), 4.CHorCL(5), 5.epsilon(6|7), 6.CHorCL(7), [7].
		if (piece.size() == 1 && firstatom.is_character_or_class() && quantifier.has_simple_equivalence())
		{
			for (unsigned int i = 0; i < quantifier.atleast; ++i)
				piece_with_quantifier += piece;

			atom.type = st_epsilon;
			atom.next2 = (quantifier.atmost - quantifier.atleast) * 2;
			if (!quantifier.is_greedy)
			{
				atom.next1 = atom.next2;
				atom.next2 = 1;
			}
			for (unsigned int i = quantifier.atleast; i < quantifier.atmost; ++i)
			{
				piece_with_quantifier.push_back(atom);
				piece_with_quantifier += piece;
				quantifier.is_greedy ? (atom.next2 -= 2) : (atom.next1 -= 2);
			}
			return;
		}
#endif	//  !defined(SRELLDBG_NO_SIMPLEEQUIV)

		atom.type = st_epsilon;
		if (quantifier.is_asterisk())	//  {0,}
		{
			//  greedy:  1.epsilon(2|4), 2.piece, 3.LAorC0WR(1|0), 4.OutOfLoop.
			//  !greedy: 1.epsilon(4|2), 2.piece, 3.LAorC0WR(1|0), 4.OutOfLoop.
			//  LAorC0WR: LastAtomOfPiece or Check0WidthRepeat.
			//  atom.type points to 1.
		}
		else if (quantifier.is_plus())	//  {1,}
		{
#if !defined(SRELLDBG_NO_ASTERISK_OPT)

			if (piece.size() == 1 && firstatom.is_character_or_class())
			{
				piece_with_quantifier += piece;
				--atom.quantifier.atleast;	//  /.+/ -> /..*/.
			}
			else
#endif
			{
				atom.next1 = 2;
				atom.next2 = 0;
				piece_with_quantifier.push_back(atom);
				//  greedy:  1.epsilon(3), 2.epsilon(3|5), 3.piece, 4.LAorC0WR(2|0), 5.OutOfLoop.
				//  !greedy: 1.epsilon(3), 2.epsilon(5|3), 3.piece, 4.LAorC0WR(2|0), 5.OutOfLoop.
				//  atom.type points to 2.
			}
		}
		else
		{
			atom.number = this->number_of_counters;
			++this->number_of_counters;

			atom.type = st_save_and_reset_counter;
			atom.next1 = 2;
			atom.next2 = 1;
			piece_with_quantifier.push_back(atom);

			atom.type  = st_restore_counter;
			atom.next1 = 0;
			atom.next2 = 0;
			piece_with_quantifier.push_back(atom);
			//  1.save_and_reset_counter(3|2), 2.restore_counter(0|0),

			atom.next1 = 0;
			atom.next2 = 0;
			atom.type = st_decrement_counter;
			piece.insert(0, atom);

			atom.next1 = 2;
			atom.next2 = 1;
			atom.type = st_increment_counter;
			piece.insert(0, atom);

			atom.type = st_check_counter;
			//  greedy:  3.check_counter(4|6), 4.piece, 5.LAorC0WR(3|0), 6.OutOfLoop.
			//  !greedy: 3.check_counter(6|4), 4.piece, 5.LAorC0WR(3|0), 6.OutOfLoop.
			//  4.piece = { 4a.increment_counter(4c|4b), 4b.decrement_counter(0|0), 4c.OriginalPiece }.
		}

		//  atom.type is epsilon or check_counter.
		//  Its "next"s point to piece and OutOfLoop.

//		if (piececharlen.atleast || firstpiece_is_roundbracket_open)
		if (!piece_is_noncapturinggroup_contaning_capturinggroup && (piececharlen.atleast || piece_has_0widthchecker))
		{
			const typename state_array::size_type piece_size = piece.size();
			state_type &lastatom = piece[piece_size - 1];

			lastatom.quantifier = atom.quantifier;
			lastatom.next1 = 0 - static_cast<std::ptrdiff_t>(piece_size);
				//  Points to the one immediately before piece, which will be pushed last in this block.

			//  atom.type has already been set. epsilon or check_counter.
			atom.next1 = 1;
			atom.next2 = static_cast<std::ptrdiff_t>(piece_size) + 1;
			if (!quantifier.is_greedy)
			{
				atom.next1 = atom.next2;
				atom.next2 = 1;
			}
			piece_with_quantifier.push_back(atom);
		}
		else
		{
			//  atom.type has already been set. epsilon or check_counter.
			atom.next1 = 1;
			atom.next2 = static_cast<std::ptrdiff_t>(piece.size()) + 4;	//  To OutOfLoop.
				//  The reason for +3 than above is that push, pop, and check_0_width are added below.
			if (!quantifier.is_greedy)
			{
				atom.next1 = atom.next2;
				atom.next2 = 1;
			}
			piece_with_quantifier.push_back(atom);	//  *1

			atom.number = this->number_of_repeats;
			++this->number_of_repeats;

			if (piece_is_noncapturinggroup_contaning_capturinggroup)
				atom.quantifier = firstatom.quantifier;
			else
				atom.quantifier.set(1, 0);

			atom.type  = st_repeat_in_push;	//  '{':
			atom.next1 = 2;
			atom.next2 = 1;
			piece_with_quantifier.push_back(atom);

			atom.type  = st_repeat_in_pop;
			atom.next1 = 0;
			atom.next2 = 0;
			piece_with_quantifier.push_back(atom);

			atom.type  = st_check_0_width_repeat;
			atom.next1 = 0 - static_cast<std::ptrdiff_t>(piece.size()) - 3;	//  3 for *1, push, and pop.
			atom.next2 = 1;
			piece.push_back(atom);
				//  greedy:  1.epsilon or check_counter(2|6),
				//  !greedy: 1.epsilon or check_counter(6|2),
				//    2.repeat_in_push(4|3), 3.repeat_in_pop(0|0), 4.piece,
				//    5.check_0_width_repeat(1|0), 6.OutOfLoop.
		}
		piece_with_quantifier += piece;
	}

#if !defined(SRELL_NO_NAMEDCAPTURE)
	bool parse_groupname(const uchar21 *&curpos, const uchar21 *const end)
	{
		const gname_string groupname = get_groupname(curpos, end);

		if (!this->namedcaptures.push_back(groupname, this->number_of_brackets))
			this->throw_error(regex_constants::error_backref);

		return true;
	}
#endif	//  !defined(SRELL_NO_NAMEDCAPTURE)

	//  '['.

	bool register_character_class(state_type &atom, const uchar21 *&curpos, const uchar21 *const end, const re_compiler_state<charT> & /* cstate */)
	{
		range_pair code_range;
		range_pairs ranges;
		state_type classatom;

		if (curpos == end)
			this->throw_error(regex_constants::error_brack);

		atom.type = st_character_class;

		if (*curpos == meta_char::mc_caret)	//  '^'
		{
			atom.is_not = true;
			++curpos;
		}

		for (;;)
		{
			if (curpos == end)
				this->throw_error(regex_constants::error_brack);

			if (*curpos == meta_char::mc_sbracl)	//   ']'
				break;

			classatom.reset();

			if (!get_character_in_class(classatom, curpos, end))
				return false;

			if (classatom.type == st_character_class)
			{
				add_predefclass_to_charclass(ranges, classatom);
				continue;
			}

			code_range.first = code_range.second = classatom.character;

			if (curpos == end)
				this->throw_error(regex_constants::error_brack);

			if (*curpos == meta_char::mc_minus)	//  '-'
			{
				++curpos;

				if (curpos == end)
					this->throw_error(regex_constants::error_brack);

				if (*curpos == meta_char::mc_sbracl)
				{
					PUSH_SEPARATELY:
					ranges.join(code_range);
					code_range.first = code_range.second = meta_char::mc_minus;
				}
				else
				{
					if (!get_character_in_class(classatom, curpos, end))
						return false;

					if (classatom.type == st_character_class)
					{
						add_predefclass_to_charclass(ranges, classatom);
						goto PUSH_SEPARATELY;
					}

					code_range.second = classatom.character;

					if (!code_range.is_range_valid())
						this->throw_error(regex_constants::error_range);
				}
			}
			ranges.join(code_range);
		}

		//  *curpos == ']'
		++curpos;
		if (this->is_icase())
			ranges.make_caseunfoldedcharset();

		if (atom.is_not)
		{
			ranges.negation();
			atom.is_not = false;
		}

//		atom.character = this->is_icase() ? ranges.template consists_of_one_character<unicode_case_folding>() : ranges.template consists_of_one_character<nocase_faketraits>();
		atom.character = ranges.consists_of_one_character(this->is_icase());

		if (atom.character != constants::invalid_u21value)
		{
			atom.type = st_character;
			return true;
		}

		atom.number = this->character_class.register_newclass(ranges);

		return true;
	}

	bool get_character_in_class(state_type &atom, const uchar21 *&curpos, const uchar21 *const end /* , const re_compiler_state &cstate */)
	{
		atom.character = *curpos++;

		return atom.character != meta_char::mc_escape	//  '\\'
			|| translate_escseq(atom, curpos, end);
	}

	void add_predefclass_to_charclass(range_pairs &cls, const state_type &classatom)
	{
		range_pairs predefclass = this->character_class[classatom.number];

		if (classatom.is_not)
			predefclass.negation();

		cls.merge(predefclass);
	}

	//  Escape characters which appear both in and out of [] pairs.
	bool translate_escseq(state_type &atom, const uchar21 *&curpos, const uchar21 *const end)
	{
		if (curpos == end)
			this->throw_error(regex_constants::error_escape);

		atom.character = *curpos++;

		switch (atom.character)
		{
		//  Predefined classes.

		case char_alnum::ch_D:	//  'D':
			atom.is_not = true;
			//@fallthrough@

		case char_alnum::ch_d:	//  'd':
			atom.number = static_cast<unsigned int>(re_character_class::digit);	//  \d, \D.
			atom.type = st_character_class;
			break;

		case char_alnum::ch_S:	//  'S':
			atom.is_not = true;
			//@fallthrough@

		case char_alnum::ch_s:	//  's':
			atom.number = static_cast<unsigned int>(re_character_class::space);	//  \s, \S.
			atom.type = st_character_class;
			break;

		case char_alnum::ch_W:	//  'W':
			atom.is_not = true;
			//@fallthrough@

		case char_alnum::ch_w:	//  'w':
			if (this->is_icase())
			{
				this->character_class.setup_icase_word();
				atom.number = static_cast<unsigned int>(re_character_class::icase_word);
			}
			else
				atom.number = static_cast<unsigned int>(re_character_class::word);	//  \w, \W.
			atom.type = st_character_class;
			break;

#if !defined(SRELL_NO_UNICODE_PROPERTY)
		//  Prepared for Unicode properties and script names.
		case char_alnum::ch_P:	//  \P{...}
			atom.is_not = true;
			//@fallthrough@

		case char_alnum::ch_p:	//  \p{...}
			atom.number = get_property_number(curpos, end);
			atom.type = st_character_class;
			break;
#endif	//  !defined(SRELL_NO_UNICODE_PROPERTY)

#if 0
		case char_alnum::ch_a:
			atom.character = char_ctrl::cc_bel;	//  '\a' 0x07:BEL
			break;
#endif

		case char_alnum::ch_b:
			atom.character = char_ctrl::cc_bs;	//  '\b' 0x08:BS
			break;

		case char_alnum::ch_t:
			atom.character = char_ctrl::cc_htab;	//  '\t' 0x09:HT
			break;

		case char_alnum::ch_n:
			atom.character = char_ctrl::cc_nl;	//  '\n' 0x0a:LF
			break;

		case char_alnum::ch_v:
			atom.character = char_ctrl::cc_vtab;	//  '\v' 0x0b:VT
			break;

		case char_alnum::ch_f:
			atom.character = char_ctrl::cc_ff;	//  '\f' 0x0c:FF
			break;

		case char_alnum::ch_r:
			atom.character = char_ctrl::cc_cr;	//  '\r' 0x0d:CR
			break;

#if 0
		case char_alnum::ch_e:	//  \e
			atom.character = char_ctrl::cc_esc;	//  '\x1b' 0x1b:ESC
			break;
#endif

		case char_alnum::ch_c:	//  \cX
			if (curpos != end)
			{
//				atom.character = static_cast<uchar21>(utf_traits().codepoint_inc(curpos, end) & 0x1f);	//  *curpos++
				atom.character = static_cast<uchar21>(*curpos | 0x20);

				if (atom.character >= char_alnum::ch_a && atom.character <= char_alnum::ch_z)
					atom.character = static_cast<uchar21>(*curpos++ & 0x1f);
				else
				{
					this->throw_error(regex_constants::error_escape);	//  Strict.
//					atom.character = char_alnum::ch_c;	//  Loose.
				}
			}
			break;

		case char_alnum::ch_0:
			atom.character = char_ctrl::cc_nul;	//  '\0' 0x00:NUL
			break;

		case char_alnum::ch_u:	//  \uhhhh, \u{h~hhhhhh}
			atom.character = parse_escape_u(curpos, end);
			break;

		case char_alnum::ch_x:	//  \xhh
			atom.character = translate_numbers(curpos, end, 16, 2, 2, 0xff, false);
			break;

		//  SyntaxCharacter, '/', and '-'.
		case meta_char::mc_caret:	//  '^'
		case meta_char::mc_dollar:	//  '$'
		case meta_char::mc_escape:	//  '\\'
		case meta_char::mc_period:	//  '.'
		case meta_char::mc_astrsk:	//  '*'
		case meta_char::mc_plus:	//  '+'
		case meta_char::mc_query:	//  '?'
		case meta_char::mc_rbraop:	//  '('
		case meta_char::mc_rbracl:	//  ')'
		case meta_char::mc_sbraop:	//  '['
		case meta_char::mc_sbracl:	//  ']'
		case meta_char::mc_cbraop:	//  '{'
		case meta_char::mc_cbracl:	//  '}'
		case meta_char::mc_bar:		//  '|'
		case char_other::co_slash:	//  '/'
		case meta_char::mc_minus:	//  '-' allowed only in charclass.
			break;

		default:
			atom.character = constants::invalid_u21value;
		}

		if (atom.character == constants::invalid_u21value)
			this->throw_error(regex_constants::error_escape);

		return true;
	}

	uchar21 parse_escape_u(const uchar21 *&curpos, const uchar21 *const end) const
	{
		uchar21 ucp;

		if (curpos == end)
			return constants::invalid_u21value;

		if (*curpos == meta_char::mc_cbraop)
//			ucp = translate_numbers(++curpos, end, 16, 1, 6, constants::unicode_max_codepoint, true);
			ucp = translate_numbers(++curpos, end, 16, 1, 0, constants::unicode_max_codepoint, true);
		else
		{
			ucp = translate_numbers(curpos, end, 16, 4, 4, 0xffff, false);

			if (ucp >= 0xd800 && ucp <= 0xdbff)
			{
				const uchar21 * prefetch = curpos;

				if (prefetch != end && *prefetch == meta_char::mc_escape && ++prefetch != end && *prefetch == char_alnum::ch_u)
				{
					const uchar21 nextucp = translate_numbers(++prefetch, end, 16, 4, 4, 0xffff, false);

					if (nextucp >= 0xdc00 && nextucp <= 0xdfff)
					{
						curpos = prefetch;
						ucp = (((ucp << 10) & 0xffc00) | (nextucp & 0x3ff)) + 0x10000;
					}
				}
			}
		}
		return ucp;
	}

#if !defined(SRELL_NO_UNICODE_PROPERTY)
	unsigned int get_property_number(const uchar21 *&curpos, const uchar21 *const end)
	{
		if (curpos == end || *curpos != meta_char::mc_cbraop)	//  '{'
			this->throw_error(regex_constants::error_escape);

		pstring pname;
		pstring pvalue(get_property_name_or_value(++curpos, end));

		if (!pvalue.size())
			this->throw_error(regex_constants::error_escape);

		if (static_cast<uchar21>(pvalue[pvalue.size() - 1]) != char_other::co_sp)	//  ' ', not marked as value.
		{
			if (curpos == end)
				this->throw_error(regex_constants::error_escape);

			if (*curpos == meta_char::mc_eq)	//  '='
			{
				pname = pvalue;
				pvalue = get_property_name_or_value(++curpos, end);
				if (!pvalue.size())
					this->throw_error(regex_constants::error_escape);
			}
		}

		if (curpos == end || *curpos != meta_char::mc_cbracl)	//  '}'
			this->throw_error(regex_constants::error_escape);

		if (static_cast<uchar21>(pvalue[pvalue.size() - 1]) == char_other::co_sp)	//  ' ', marked as value.
			pvalue.resize(pvalue.size() - 1);

		++curpos;

		const unsigned int class_number = this->character_class.lookup_property(pname, pvalue, this->is_icase());

		if (class_number == re_character_class::error_property)
			this->throw_error(regex_constants::error_escape);

		return class_number;
	}

	pstring get_property_name_or_value(const uchar21 *&curpos, const uchar21 *const end) const
	{
		pstring name_or_value;
		bool number_found = false;

		for (;; ++curpos)
		{
			if (curpos == end)
				break;

			const uchar21 curchar = *curpos;

			if (curchar >= char_alnum::ch_A && curchar <= char_alnum::ch_Z)
				;
			else if (curchar >= char_alnum::ch_a && curchar <= char_alnum::ch_z)
				;
			else if (curchar == char_other::co_ll)	//  '_'
				;
			else if (curchar >= char_alnum::ch_0 && curchar <= char_alnum::ch_9)
				number_found = true;
			else
				break;

			name_or_value.append(1, static_cast<typename pstring::value_type>(curchar));
		}
		if (number_found)
			name_or_value.append(1, char_other::co_sp);	//  ' '

		return name_or_value;
	}
#endif	//  !defined(SRELL_NO_UNICODE_PROPERTY)

	//  Escape characters which do not appear in [] pairs.

	bool translate_atom_escape(state_type &atom, const uchar21 *&curpos, const uchar21 *const end, /* const */ re_compiler_state<charT> &cstate)
	{
		if (curpos == end)
			this->throw_error(regex_constants::error_escape);

		atom.character = *curpos;

		switch (atom.character)
		{
		case meta_char::mc_minus:	//  '-'
			this->throw_error(regex_constants::error_escape);
			//@fallthrough@

		case char_alnum::ch_B:	//  'B':
			atom.is_not = true;
			//@fallthrough@

		case char_alnum::ch_b:	//  'b':
			atom.type   = st_boundary;	//  \b, \B.
			atom.quantifier.reset(0);
//			atom.number = 0;
			if (this->is_icase())
			{
				this->character_class.setup_icase_word();
				atom.number = static_cast<unsigned int>(re_character_class::icase_word);
			}
			else
				atom.number = static_cast<unsigned int>(re_character_class::word);	//  \w, \W.
			break;

//		case char_alnum::ch_A:	//  'A':
//			atom.type   = st_bol;	//  '\A'
//		case char_alnum::ch_Z:	//  'Z':
//			atom.type   = st_eol;	//  '\Z'
//		case char_alnum::ch_z:	//  'z':
//			atom.type   = st_eol;	//  '\z'

		//  Backreferences.

#if !defined(SRELL_NO_NAMEDCAPTURE)
		//  Prepared for named captures.
		case char_alnum::ch_k:	//  'k':
			return parse_backreference_name(atom, curpos, end, cstate);	//  \k.
#endif

		default:

			if (atom.character >= char_alnum::ch_1 && atom.character <= char_alnum::ch_9)	//  \1, \9.
				return parse_backreference_number(atom, curpos, end, cstate);

			return translate_escseq(atom, curpos, end) && register_if_negatedcharclass(atom);
		}

		++curpos;
		return true;
	}

	bool register_if_negatedcharclass(state_type &atom)
	{
		if (atom.is_negcharclass())
		{
			range_pairs ranges;

			add_predefclass_to_charclass(ranges, atom);
			atom.number = this->character_class.register_newclass(ranges);
			atom.is_not = false;
		}
		return true;
	}

	bool parse_backreference_number(state_type &atom, const uchar21 *&curpos, const uchar21 *const end, const re_compiler_state<charT> &cstate)
	{
		atom.number = static_cast<unsigned int>(translate_numbers(curpos, end, 10, 0, 0, 0, false));

		if (atom.number == static_cast<unsigned int>(constants::invalid_u21value))	//  ~0))
			this->throw_error(regex_constants::error_escape);
//		else if (atom.number >= this->number_of_brackets)
//			this->throw_error(regex_constants::error_backref);

		atom.backrefnumber_unresolved = false;

		return backreference_postprocess(atom, cstate);
	}

	bool backreference_postprocess(state_type &atom, const re_compiler_state<charT> & /* cstate */) const
	{
		atom.next2 = 1;
		atom.type = st_backreference;

//		atom.quantifier.atleast = cstate.atleast_widths_of_brackets[atom.number - 1];
			//  Moved to check_backreferences().

		return true;
	}

#if !defined(SRELL_NO_NAMEDCAPTURE)
	bool parse_backreference_name(state_type &atom, const uchar21 *&curpos, const uchar21 *const end, re_compiler_state<charT> &cstate)
	{
		if (++curpos == end || *curpos != meta_char::mc_lt)
			this->throw_error(regex_constants::error_escape);

		const gname_string groupname = get_groupname(++curpos, end);

		atom.number = this->namedcaptures[groupname];

		if (atom.number != groupname_mapper<charT>::notfound)
			atom.backrefnumber_unresolved = false;
		else
		{
			atom.backrefnumber_unresolved = true;
			atom.number = static_cast<unsigned int>(cstate.unresolved_gnames.size());
			cstate.unresolved_gnames.push_back(groupname, atom.number);
		}

		return backreference_postprocess(atom, cstate);
	}

	gname_string get_groupname(const uchar21 *&curpos, const uchar21 *const end)
	{
		charT mbstr[utf_traits::maxseqlen];
		gname_string groupname;

		for (;;)
		{
			if (curpos == end)
				this->throw_error(regex_constants::error_escape);

			uchar21 curchar = *curpos++;

			if (curchar == meta_char::mc_gt)	//  '>'
				break;

			if (curchar == meta_char::mc_escape && curpos != end && *curpos == char_alnum::ch_u)	//  '\\', 'u'.
				curchar = parse_escape_u(++curpos, end);

#if defined(SRELL_NO_UNICODE_PROPERTY)
			if (curchar != meta_char::mc_escape)
#else
			if (this->character_class.is_included(groupname.size() ? re_character_class::reg_exp_identifier_part : re_character_class::reg_exp_identifier_start, curchar))
#endif
				;	//  OK.
			else
				curchar = constants::invalid_u21value;

			if (curchar == constants::invalid_u21value)
				this->throw_error(regex_constants::error_escape);

			const uchar21 seqlen = utf_traits::to_codeunits(mbstr, curchar);
			for (uchar21 i = 0; i < seqlen; ++i)
				groupname.append(1, mbstr[i]);
		}
		if (!groupname.size())
			this->throw_error(regex_constants::error_escape);

		return groupname;
	}
#endif	//  !defined(SRELL_NO_NAMEDCAPTURE)

	bool get_quantifier(re_quantifier &quantifier, const uchar21 *&curpos, const uchar21 *const end)
	{
		switch (*curpos)
		{
		case meta_char::mc_astrsk:	//  '*':
			--quantifier.atleast;
			//@fallthrough@

		case meta_char::mc_plus:	//  '+':
			quantifier.set_infinity();
			break;

		case meta_char::mc_query:	//  '?':
			--quantifier.atleast;
			break;

		case meta_char::mc_cbraop:	//  '{':
			if (!get_brace_with_quantifier(quantifier, curpos, end))
				return false;
			break;

		default:
			return true;
		}

		if (++curpos != end && *curpos == meta_char::mc_query)	//  '?'
		{
			quantifier.is_greedy = false;
			++curpos;
		}
		return true;
	}

	bool get_brace_with_quantifier(re_quantifier &quantifier, const uchar21 *&curpos, const uchar21 *const end)
	{
		++curpos;

		quantifier.atleast = static_cast<unsigned int>(translate_numbers(curpos, end, 10, 1, 0, 0, false));
		if (quantifier.atleast == static_cast<unsigned int>(constants::invalid_u21value))
			this->throw_error(regex_constants::error_brace);

		quantifier.atmost = quantifier.atleast;

		if (curpos == end)
			this->throw_error(regex_constants::error_brace);

		if (*curpos == meta_char::mc_comma)	//  ','
		{
			++curpos;

			quantifier.atmost = static_cast<unsigned int>(translate_numbers(curpos, end, 10, 1, 0, 0, false));

			if (quantifier.atmost == static_cast<unsigned int>(constants::invalid_u21value))
				quantifier.set_infinity();

			if (!quantifier.is_valid())
				this->throw_error(regex_constants::error_badbrace);
		}

		if (curpos == end || *curpos != meta_char::mc_cbracl)	//  '}'
			this->throw_error(regex_constants::error_brace);

		//  *curpos == '}'

		return true;
	}

	uchar21 translate_numbers(const uchar21 *&curpos, const uchar21 *const end, const int radix, const std::size_t minsize, const std::size_t maxsize, const uchar21 maxcodepoint, const bool needs_closecurlybracket) const
	{
		uchar21 univalue = 0;
		int num;

		for (std::size_t count = 0; !maxsize || count < maxsize; ++curpos, ++count)
		{
			if (curpos == end || (num = tonumber(*curpos, radix)) == -1)
			{
				if (count >= minsize)
					break;	//  OK.

				return constants::invalid_u21value;
			}
			univalue *= radix;
			univalue += num;
		}

		if (needs_closecurlybracket)
		{
			if (curpos == end || *curpos != meta_char::mc_cbracl)
				return constants::invalid_u21value;

			++curpos;
		}
		if (!maxcodepoint || univalue <= maxcodepoint)
			return univalue;

		return constants::invalid_u21value;
	}

	int tonumber(const uchar21 ch, const int radix) const
	{
		if ((ch >= char_alnum::ch_0 && ch <= char_alnum::ch_7) || (radix >= 10 && (ch == char_alnum::ch_8 || ch == char_alnum::ch_9)))
			return static_cast<int>(ch - char_alnum::ch_0);

		if (radix == 16)
		{
			if (ch >= char_alnum::ch_a && ch <= char_alnum::ch_f)
				return static_cast<int>(ch - char_alnum::ch_a + 10);

			if (ch >= char_alnum::ch_A && ch <= char_alnum::ch_F)
				return static_cast<int>(ch - char_alnum::ch_A + 10);
		}
		return -1;
	}

	bool check_backreferences(const re_compiler_state<charT> &cstate)
	{
		for (typename state_array::size_type backrefpos = 0; backrefpos < this->NFA_states.size(); ++backrefpos)
		{
			state_type &brs = this->NFA_states[backrefpos];

			if (brs.type == st_backreference)
			{
				unsigned int &backrefno = brs.number;

#if !defined(SRELL_NO_NAMEDCAPTURE)
				if (brs.backrefnumber_unresolved)
				{
					if (backrefno >= cstate.unresolved_gnames.size())
						return false;	//  Internal error.

					brs.number = this->namedcaptures[cstate.unresolved_gnames[backrefno]];

					if (backrefno == groupname_mapper<charT>::notfound)
						return false;

					brs.backrefnumber_unresolved = false;
				}
#endif

				for (typename state_array::size_type roundbracket_closepos = 0;; ++roundbracket_closepos)
				{
					if (roundbracket_closepos < this->NFA_states.size())
					{
						const state_type &rbcs = this->NFA_states[roundbracket_closepos];

						if (rbcs.type == st_roundbracket_close && rbcs.number == backrefno)
						{
							if (roundbracket_closepos < backrefpos)
								brs.quantifier.atleast = cstate.atleast_widths_of_brackets[backrefno - 1];
							else
							{
								brs.type = st_epsilon;
								brs.next2 = 0;
							}
							break;
						}
					}
					else
						return false;
				}
			}
		}
		return true;
	}

#if !defined(SRELLDBG_NO_1STCHRCLS)

	void create_firstchar_class()
	{
#if !defined(SRELLDBG_NO_BITSET)
		range_pairs fcc;
#else
		range_pairs &fcc = this->firstchar_class;
#endif

		const bool canbe0length = gather_nextchars(fcc, static_cast<typename state_array::size_type>(this->NFA_states[0].next1), 0u, false);

		if (canbe0length)
		{
			fcc.set_solerange(range_pair_helper(0, constants::unicode_max_codepoint));
			//  Expressions would consist of assertions only, such as /^$/.
			//  We cannot but accept every codepoint.
		}
		else if (this->is_ricase())
//#if !defined(SRELLDBG_NO_BITSET)
			fcc.make_caseunfoldedcharset();
//#else
//			make_charcls_nocase(fcc);
//#endif

#if !defined(SRELLDBG_NO_BITSET)
		this->NFA_states[0].quantifier.atleast = this->character_class.register_newclass(fcc);

		set_bitset_table(fcc);
#endif
	}

#if !defined(SRELLDBG_NO_BITSET)
	void set_bitset_table(const range_pairs &fcc)
	{
		for (typename range_pairs::size_type i = 0; i < fcc.size(); ++i)
		{
			const range_pair &range = fcc[i];

			for (uchar21 ucp = range.first; ucp <= constants::unicode_max_codepoint; ++ucp)
			{
				this->firstchar_class_bs.set(utf_traits::firstcodeunit(ucp) & utf_traits::bitsetmask);

				if (ucp == range.second)
					break;
			}
		}
	}
#endif	//  !defined(SRELLDBG_NO_BITSET)
#endif	//  !defined(SRELLDBG_NO_1STCHRCLS)

	bool gather_nextchars(range_pairs &nextcharclass, typename state_array::size_type pos, simple_array<bool> &checked, const unsigned int bracket_number, const bool subsequent) const
	{
		bool canbe0length = false;

		for (;;)
		{
			const state_type &state = this->NFA_states[pos];

			if (checked[pos])
				break;

			checked[pos] = true;

			if (state.next2
					&& (state.type != st_check_counter /* || subsequent */ || !state.quantifier.is_greedy || state.quantifier.atleast == 0)
					&& (state.type != st_increment_counter)
					&& (state.type != st_save_and_reset_counter)
					&& (state.type != st_roundbracket_open)
					&& (state.type != st_roundbracket_close || state.number != bracket_number)
					&& (state.type != st_repeat_in_push)
					&& (state.type != st_backreference || (state.quantifier.atleast == 0 && state.next1 != state.next2))
					&& (state.type != st_lookaround_open))
				if (gather_nextchars(nextcharclass, pos + state.next2, checked, bracket_number, subsequent))
					canbe0length = true;

			switch (state.type)
			{
			case st_character:
				nextcharclass.join(range_pair_helper(state.character));
				return canbe0length;

			case st_character_class:
				nextcharclass.merge(this->character_class[state.number]);
				return canbe0length;

			case st_backreference:
				{
					const typename state_array::size_type nextpos = find_next1_of_bracketopen(state.number);

					const bool length0 = gather_nextchars(nextcharclass, nextpos, state.number, subsequent);

					if (!length0)
						return canbe0length;
				}
				break;

			case st_bol:
			case st_eol:
				if (subsequent && is_multiline())
					nextcharclass.merge(this->character_class[re_character_class::newline]);

				break;

			case st_boundary:
				if (subsequent)
					nextcharclass.set_solerange(range_pair_helper(0, constants::unicode_max_codepoint));

				break;

			case st_lookaround_open:
//				if (!state.is_not && !state.reverse)
				if (!state.is_not && state.quantifier.atleast == 0)
				{
					gather_nextchars(nextcharclass, pos + 2, checked, 0u, subsequent);
				}
				else if (subsequent)
					nextcharclass.set_solerange(range_pair_helper(0, constants::unicode_max_codepoint));

				break;

			case st_roundbracket_close:
				if (/* bracket_number == 0 || */ state.number != bracket_number)
					break;
				//@fallthrough@

			case st_success:	//  == st_lookaround_close.
				return true;

			case st_check_counter:
				if (!state.quantifier.is_greedy && state.quantifier.atleast >= 1)
					return canbe0length;
				//@fallthrough@

			default:;
			}

			if (state.next1)
				pos += state.next1;
			else
				break;
		}
		return canbe0length;
	}

	bool gather_nextchars(range_pairs &nextcharclass, const typename state_array::size_type pos, const unsigned int bracket_number, const bool subsequent) const
	{
		simple_array<bool> checked;

		checked.resize(this->NFA_states.size(), false);
		return gather_nextchars(nextcharclass, pos, checked, bracket_number, subsequent);
	}

	typename state_array::size_type find_next1_of_bracketopen(const unsigned int bracketno) const
	{
		for (typename state_array::size_type no = 0; no < this->NFA_states.size(); ++no)
		{
			const state_type &state = this->NFA_states[no];

			if (state.type == st_roundbracket_open && state.number == bracketno)
				return no + state.next1;
		}
		return 0;
	}

	void relativejump_to_absolutejump()
	{
		for (typename state_array::size_type pos = 0; pos < this->NFA_states.size(); ++pos)
		{
			state_type &state = this->NFA_states[pos];

#if !defined(SRELLDBG_NO_ASTERISK_OPT)
			if (state.next1 || state.type == st_character || state.type == st_character_class)
#else
			if (state.next1)
#endif
				state.next_state1 = &this->NFA_states[pos + state.next1];
			else
				state.next_state1 = NULL;

			if (state.next2)
				state.next_state2 = &this->NFA_states[pos + state.next2];
			else
				state.next_state2 = NULL;
		}
	}

	void optimise()
	{
#if !defined(SRELLDBG_NO_ASTERISK_OPT)
		asterisk_optimisation();
#endif

#if !defined(SRELLDBG_NO_NEXTPOS_OPT)
		if (!check_if_backref_presents(0, 0))
		{
	#if defined(SRELLTEST_NEXTPOS_OPT2)
			nextpos_optimisation2();
	#endif
	#if !defined(SRELLDBG_NO_NEXTPOS_OPT3) && !defined(SRELLDBG_NO_ASTERISK_OPT)
			nextpos_optimisation1_3();
	#endif
		}
#endif

#if !defined(SRELLDBG_NO_BRANCH_OPT)
		branch_optimisation();
#endif

#if !defined(SRELLDBG_NO_1STCHRCLS)
		create_firstchar_class();
#endif

#if !defined(SRELLDBG_NO_SKIP_EPSILON)
		skip_epsilon();
#endif

#if !defined(SRELLDBG_NO_CCPOS)
		set_charclass_posinfo();
#endif
	}

#if !defined(SRELLDBG_NO_SKIP_EPSILON)

	void skip_epsilon()
	{
		for (typename state_array::size_type pos = 0; pos < this->NFA_states.size(); ++pos)
		{
			state_type &state = this->NFA_states[pos];

			if (state.next1)
				state.next1 = static_cast<std::ptrdiff_t>(skip_nonbranch_epsilon(pos + state.next1) - pos);

			if (state.next2)
				state.next2 = static_cast<std::ptrdiff_t>(skip_nonbranch_epsilon(pos + state.next2) - pos);
		}
	}

	typename state_array::size_type skip_nonbranch_epsilon(typename state_array::size_type pos) const
	{
		for (;;)
		{
			const state_type &state = this->NFA_states[pos];

			if (state.type == st_epsilon && state.next2 == 0)
			{
				pos += state.next1;
				continue;
			}
			break;
		}
		return pos;
	}

#endif

#if !defined(SRELLDBG_NO_ASTERISK_OPT)

	void asterisk_optimisation()
	{
		for (typename state_array::size_type cur = 0; cur < this->NFA_states.size(); ++cur)
		{
			state_type &curstate = this->NFA_states[cur];

			switch (curstate.type)
			{
			case st_character:
			case st_character_class:
				if (curstate.quantifier.is_asterisk())
				{
					if (is_exclusive_sequence(cur))
					{
						state_type &prevstate = this->NFA_states[cur - 1];

						prevstate.next1 = 1;
						prevstate.next2 = 0;
//						prevstate.quantifier.is_greedy = true;
//						curstate.quantifier.is_greedy = true;
						curstate.next1 = 0;
						curstate.next2 = 1;
					}
				}
				break;

#if !defined(SRELLDBG_NO_SPLIT_COUNTER)
				//  check_counter, increment_counter, decrement_counter, char_or_class?
			case st_check_counter:
				if (!curstate.quantifier.is_same() && is_exclusive_sequence(cur + 3))
					split_counter(cur);
				break;
#endif

			default:;
			}
		}
	}

	bool is_exclusive_sequence(const typename state_array::size_type cur) const
	{
		const state_type &curstate = this->NFA_states[cur];
		range_pairs curchar_class;
		range_pairs nextchar_class;

		if (curstate.type == st_character)
		{
			curchar_class.join(range_pair_helper(curstate.character));
		}
		else if (curstate.type == st_character_class)
		{
			curchar_class = this->character_class[curstate.number];
			if (curchar_class.size() == 0)	//  Means [], which always makes matching fail.
				return true;	//  For preventing the automaton from pushing bt data.
		}
		else
		{
			return false;
		}

		const bool canbe0length = gather_nextchars(nextchar_class, cur + 1, 0u, true);

		if (nextchar_class.size() && !curchar_class.is_overlap(nextchar_class))
		{
			return !canbe0length || curstate.quantifier.is_greedy;
		}
		else if (nextchar_class.size() == 0 && (!canbe0length || only_success_left(cur + 1)))
		{
			//  (size() == 0 && !canbe0length) means [].
			return curstate.quantifier.is_greedy;
		}

		return false;
	}

	bool only_success_left(typename state_array::size_type pos) const
	{
		for (;;)
		{
			const state_type &state = this->NFA_states[pos];

			switch (state.type)
			{
			case st_success:
				return true;

			case st_roundbracket_close:
			case st_backreference:
				if (state.next2 != 0 && state.next1 != state.next2)
					return false;
				break;

			case st_epsilon:
				if (state.next2 != 0 && !only_success_left(pos + state.next2))
					return false;
				break;

			case st_roundbracket_open:
				break;	//  /a*()/

			default:
				return false;
			}
			if (state.next1)
				pos += state.next1;
			else
				return false;
		}
	}
#endif	//  !defined(SRELLDBG_NO_ASTERISK_OPT)

	void insert_at(const typename state_array::size_type pos, const std::ptrdiff_t len)
	{
		state_type newstate;

		for (typename state_array::size_type cur = 0; cur < pos; ++cur)
		{
			state_type &state = this->NFA_states[cur];

			if (state.next1 && (cur + state.next1) >= pos)
				state.next1 += len;

			if (state.next2 && (cur + state.next2) >= pos)
				state.next2 += len;
		}

		for (typename state_array::size_type cur = pos; cur < this->NFA_states.size(); ++cur)
		{
			state_type &state = this->NFA_states[cur];

			if ((cur + state.next1) < pos)
				state.next1 -= len;

			if ((cur + state.next2) < pos)
				state.next2 -= len;
		}

		newstate.reset();
		newstate.type = st_epsilon;
		for (std::ptrdiff_t count = 0; count < len; ++count)
			this->NFA_states.insert(pos, newstate);
	}

#if !defined(SRELLDBG_NO_ASTERISK_OPT)

	void split_counter(typename state_array::size_type &cur)
	{
		insert_at(cur + 1, 4);
			//  check_counter, epsilon x 4, increment_counter, decrement_counter, char_or_class
		state_type &cur_chkcnt = this->NFA_states[cur];	//  check_counter.
		state_type &new_inccnt = this->NFA_states[cur + 1];	//  Copy of increment_counter (cur_inccnt).
		state_type &new_deccnt = this->NFA_states[cur + 2];	//  Copy of decrement_counter (cur_deccnt).
		state_type &new_chorcl = this->NFA_states[cur + 3];	//  Copy of char or class (cur_chorcl).
		state_type &new_chkcnt = this->NFA_states[cur + 4];	//  Copy of check_counter (cur).
		state_type &cur_inccnt = this->NFA_states[cur + 5];	//  increment_counter.
		state_type &cur_deccnt = this->NFA_states[cur + 6];	//  decrement_counter.
		state_type &cur_chorcl = this->NFA_states[cur + 7];	//  character or class.

		cur_chorcl.next1 += 4;
//		cur_chorcl.next2 = 0;

		cur_chkcnt.next1 = 1;
		cur_chkcnt.next2 = 4;
		cur_chkcnt.quantifier.is_greedy = true;

		new_chkcnt = cur_chkcnt;

		if (new_chkcnt.quantifier.atleast <= 4)
		{
			if (new_chkcnt.quantifier.atleast > 0)
			{
				cur_chkcnt = cur_chorcl;
				cur_chkcnt.next1 = 1;
//				cur_chkcnt.next2 = 0;
				cur_chkcnt.quantifier.reset();

				if (new_chkcnt.quantifier.atleast > 1)
				{
					new_inccnt = cur_chkcnt;
					if (new_chkcnt.quantifier.atleast > 2)
					{
						new_deccnt = cur_chkcnt;
						if (new_chkcnt.quantifier.atleast > 3)
							new_chorcl = cur_chkcnt;
					}
				}
			}
			else
			{
				cur_chkcnt.reset();
				cur_chkcnt.type = st_epsilon;
			}

			if (!new_chkcnt.quantifier.is_infinity())
				new_chkcnt.quantifier.atmost -= new_chkcnt.quantifier.atleast;

			new_chkcnt.quantifier.atleast = 0;
		}
		else
		{
			cur_chkcnt.quantifier.atmost = cur_chkcnt.quantifier.atleast;

			new_inccnt = cur_inccnt;	//  increment_counter.
			new_deccnt = cur_deccnt;	//  decrement_counter.
			new_chorcl = cur_chorcl;	//  character or character_class.
		}
		new_chkcnt.dont_push = true;	//  Disables backtracking.
		cur_chorcl.next2 = 1;
		cur += 4;
	}

#endif	//  !defined(SRELLDBG_NO_ASTERISK_OPT)

	bool check_if_backref_presents(typename state_array::size_type begin /* = 0 */, const unsigned int number /* = 0 */) const
	{
		for (; begin < this->NFA_states.size(); ++begin)
		{
			const state_type &state = this->NFA_states[begin];

			if (state.type == st_backreference && (number == 0 || state.number == number))
				return true;
		}
		return false;
	}

#if !defined(SRELLDBG_NO_NEXTPOS_OPT)
#if defined(SRELLTEST_NEXTPOS_OPT2)
	void nextpos_optimisation2()
	{
		range_pairs firstchar_class;
		typename state_array::size_type cur = gather_if_char_or_charclass(firstchar_class, 1);

		if (cur == 0 || firstchar_class.size() == 0)
			return;

		for (++cur; cur < this->NFA_states.size(); ++cur)
		{
			state_type &curstate = this->NFA_states[cur];
			range_pairs nextchar_class;

			if (curstate.type == st_character)
			{
				nextchar_class.join(range_pair_helper(curstate.character));
			}
			else if (curstate.type == st_character_class)
			{
				nextchar_class = this->character_class[curstate.number];
			}
			else
				break;

			if (nextchar_class.size() == 0 || firstchar_class.is_overlap(nextchar_class))
				break;

			insert_at(++cur, 1);
			{
				state_type &newstate = this->NFA_states[cur];

				newstate.type = st_move_nextpos;
				--curstate.next1;
			}
		}
	}
#endif	//  defined(SRELLTEST_NEXTPOS_OPT2)

#if !defined(SRELLDBG_NO_NEXTPOS_OPT3) && !defined(SRELLDBG_NO_ASTERISK_OPT)
	void nextpos_optimisation1_3()
	{
		typename state_array::size_type cur = 1;
		const state_type *prev = NULL;

		for (;;)
		{
			const state_type &curstate = this->NFA_states[cur];

			switch (curstate.type)
			{
			case st_character:
			case st_character_class:
				if (prev)
				{
					if (curstate.type == st_character && prev->character != curstate.character)
						break;
					else if (/* curstate.type == st_character_class && */ prev->number != curstate.number)
						break;
				}

				if (curstate.quantifier.is_asterisk_or_plus())
					goto INS_MNP;

				if (!curstate.quantifier.is_default())
					break;

				prev = &curstate;	//  is_default().
				cur += curstate.next1;
				continue;

			case st_epsilon:
				if (curstate.next2 != 0 && curstate.character != char_other::co_sp)
					return;
				cur += curstate.next1;
				continue;

			default:
				break;
			}
			break;
		}
		return;

		INS_MNP:

#if 01
		if (this->NFA_states[++cur].type != st_success)
		{
			insert_at(cur, 1);
			state_type &newstate = this->NFA_states[cur];
			state_type &curstate = this->NFA_states[cur - 1];

			newstate.type = st_move_nextpos;

			if (curstate.next1 == 0)
			{
				newstate.next1 = curstate.next2 - 1;
				curstate.next2 = 1;
			}
			else
			{
				newstate.next1 = -2;
				curstate.next1 = 1;
			}
#else
		if (this->NFA_states[cur + 1].type != st_success)
		{
			insert_at(cur, 1);
			state_type &newstate = this->NFA_states[cur];
			state_type &curstate = this->NFA_states[cur + 1];

			newstate.type = st_move_nextpos;

			if (curstate.next1 == 0)
			{
				newstate.next1 = curstate.next2 + 1;
				curstate.next2 = -1;
			}
			else
			{
				newstate.next1 = -1;
				++curstate.next1;
			}
#endif
		}
	}
#endif	//  !defined(SRELLDBG_NO_NEXTPOS_OPT3) && !defined(SRELLDBG_NO_ASTERISK_OPT)
#endif	//  !defined(SRELLDBG_NO_NEXTPOS_OPT)

#if !defined(SRELLDBG_NO_BRANCH_OPT) || defined(SRELLTEST_NEXTPOS_OPT2)
	typename state_array::size_type gather_if_char_or_charclass(range_pairs &charclass, typename state_array::size_type pos) const
	{
		for (; pos < this->NFA_states.size();)
		{
			const state_type &curstate = this->NFA_states[pos];

			if (curstate.type == st_character && curstate.next2 == 0)
			{
				charclass.join(range_pair_helper(curstate.character));
				return pos;
			}
			else if (curstate.type == st_character_class && curstate.next2 == 0)
			{
				charclass = this->character_class[curstate.number];
				return pos;
			}
			else if (curstate.type == st_epsilon && curstate.next2 == 0)
			{
			}
			else
				break;

			pos += curstate.next1;
		}
		return 0;
	}
#endif	//  !defined(SRELLDBG_NO_BRANCH_OPT) || defined(SRELLTEST_NEXTPOS_OPT2)

#if !defined(SRELLDBG_NO_BRANCH_OPT)
	void branch_optimisation()
	{
		for (typename state_array::size_type pos = 0; pos < this->NFA_states.size(); ++pos)
		{
			const state_type &state = this->NFA_states[pos];

			if (state.type == st_epsilon)
			{
				if (state.next2 && state.character == meta_char::mc_bar)
				{
					{
						range_pairs nextcharclass1;
						const typename state_array::size_type nextcharpos = gather_if_char_or_charclass(nextcharclass1, pos + state.next1);

						if (nextcharpos)
						{
							range_pairs nextcharclass2;

							gather_nextchars(nextcharclass2, pos + state.next2, 0u /* bracket_number */, true);

							if (!nextcharclass1.is_overlap(nextcharclass2))
							{
								state_type &branch = this->NFA_states[pos];
								state_type &next1 = this->NFA_states[nextcharpos];

								next1.next2 = pos + branch.next2 - nextcharpos;
								branch.next2 = 0;
							}
						}
					}
				}
			}
#if 0
			else if (state.type == st_roundbracket_open)
			{
				pos = branch_optimisation(pos + 1, state.number);
			}
			else if (state.type == st_roundbracket_close && state.number == bracket_number)
			{
				return pos;
			}
#endif
		}
//		return pos;
	}
#endif	//  !defined(SRELLDBG_NO_BRANCH_OPT)

	bool check_if_really_needs_icase_search()
	{
		uchar21 u21chars[unicode_case_folding::rev_maxset];

		for (typename state_array::size_type i = 0; i < this->NFA_states.size(); ++i)
		{
			const state_type &state = this->NFA_states[i];

			if (state.type == st_character)
			{
				if (unicode_case_folding::casefoldedcharset(u21chars, state.character) > 1)
					return true;
			}
			else if (state.type == st_backreference)
				return true;
		}
//		this->soflags &= ~regex_constants::icase;
		return false;
	}

#if !defined(SRELLDBG_NO_BMH)
	void setup_bmhdata()
	{
		simple_array<uchar21> u21s;

		for (typename state_array::size_type i = 1; i < this->NFA_states.size(); ++i)
		{
			const state_type &state = this->NFA_states[i];

			if (state.type == st_character)
				u21s.push_back(state.character);
			else
			{
				u21s.clear();
				break;
			}
		}

		if (u21s.size() > 1)
//		if ((u21s.size() > 1 && !this->is_ricase()) || (u21s.size() > 2 && this->is_ricase()))
		{
			if (this->bmdata)
				this->bmdata->clear();
			else
				this->bmdata = new re_bmh<charT, utf_traits>;

			this->bmdata->setup(u21s, this->is_ricase());
			return /* false */;
		}

		if (this->bmdata)
			delete this->bmdata;
		this->bmdata = NULL;
//		return true;
	}
#endif	//  !defined(SRELLDBG_NO_BMH)

#if !defined(SRELLDBG_NO_CCPOS)
	void set_charclass_posinfo()
	{
		for (typename state_array::size_type i = 1; i < this->NFA_states.size(); ++i)
		{
			state_type &state = this->NFA_states[i];

			if (state.type == st_character_class)
			{
				const range_pair &posinfo = this->character_class.charclasspos(state.number);
				state.quantifier.setccpos(posinfo.first, posinfo.second);
			}
		}
	}
#endif	//  !defined(SRELLDBG_NO_CCPOS)

public:	//  For debug.

	void print_NFA_states(const int) const;
};
//  re_compiler

	}	//  namespace regex_internal

//  ... "rei_compiler.hpp"]
//  ["regex_sub_match.hpp" ...

//  28.9, class template sub_match:
template <class BidirectionalIterator>
class sub_match : public std::pair<BidirectionalIterator, BidirectionalIterator>
{
public:

	typedef typename std::iterator_traits<BidirectionalIterator>::value_type value_type;
	typedef typename std::iterator_traits<BidirectionalIterator>::difference_type difference_type;
	typedef BidirectionalIterator iterator;
	typedef std::basic_string<value_type> string_type;

	bool matched;

//	constexpr sub_match();	//  C++11.

	sub_match() : matched(false)
	{
	}

	difference_type length() const
	{
		return matched ? std::distance(this->first, this->second) : 0;
	}

	operator string_type() const
	{
		return matched ? string_type(this->first, this->second) : string_type();
	}

	string_type str() const
	{
		return matched ? string_type(this->first, this->second) : string_type();
	}

	int compare(const sub_match &s) const
	{
		return str().compare(s.str());
	}

	int compare(const string_type &s) const
	{
		return str().compare(s);
	}

	int compare(const value_type *const s) const
	{
		return str().compare(s);
	}
};

//  28.9.2, sub_match non-member operators:
//  [7.9.2] sub_match non-member operators

//  Compares sub_match & with sub_match &.
template <class BiIter>
bool operator==(const sub_match<BiIter> &lhs, const sub_match<BiIter> &rhs)
{
	return lhs.compare(rhs) == 0;	//  1
}

template <class BiIter>
bool operator!=(const sub_match<BiIter> &lhs, const sub_match<BiIter> &rhs)
{
	return lhs.compare(rhs) != 0;	//  2
}

template <class BiIter>
bool operator<(const sub_match<BiIter> &lhs, const sub_match<BiIter> &rhs)
{
	return lhs.compare(rhs) < 0;	//  3
}

template <class BiIter>
bool operator<=(const sub_match<BiIter> &lhs, const sub_match<BiIter> &rhs)
{
	return lhs.compare(rhs) <= 0;	//  4
}

template <class BiIter>
bool operator>=(const sub_match<BiIter> &lhs, const sub_match<BiIter> &rhs)
{
	return lhs.compare(rhs) >= 0;	//  5
}

template <class BiIter>
bool operator>(const sub_match<BiIter> &lhs, const sub_match<BiIter> &rhs)
{
	return lhs.compare(rhs) > 0;	//  6
}

//  Compares basic_string & with sub_match &.
template <class BiIter, class ST, class SA>
bool operator==(
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs.compare(lhs.c_str()) == 0;	//  7
}

template <class BiIter, class ST, class SA>
bool operator!=(
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(lhs == rhs);	//  8
}

template <class BiIter, class ST, class SA>
bool operator<(
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs.compare(lhs.c_str()) > 0;	//  9
}

template <class BiIter, class ST, class SA>
bool operator>(
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs < lhs;	//  10
}

template <class BiIter, class ST, class SA>
bool operator>=(
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(lhs < rhs);	//  11
}

template <class BiIter, class ST, class SA>
bool operator<=(
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(rhs < lhs);	//  12
}

//  Compares sub_match & with basic_string &.
template <class BiIter, class ST, class SA>
bool operator==(
	const sub_match<BiIter> &lhs,
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &rhs
)
{
	return lhs.compare(rhs.c_str()) == 0;	//  13
}

template <class BiIter, class ST, class SA>
bool operator!=(
	const sub_match<BiIter> &lhs,
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &rhs
)
{
	return !(lhs == rhs);	//  14
}

template <class BiIter, class ST, class SA>
bool operator<(
	const sub_match<BiIter> &lhs,
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &rhs
)
{
	return lhs.compare(rhs.c_str()) < 0;	//  15
}

template <class BiIter, class ST, class SA>
bool operator>(
	const sub_match<BiIter> &lhs,
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &rhs
)
{
	return rhs < lhs;	//  16
}

template <class BiIter, class ST, class SA>
bool operator>=(
	const sub_match<BiIter> &lhs,
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &rhs
)
{
	return !(lhs < rhs);	//  17
}

template <class BiIter, class ST, class SA>
bool operator<=(
	const sub_match<BiIter> &lhs,
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &rhs
)
{
	return !(rhs < lhs);	//  18
}

//  Compares iterator_traits::value_type * with sub_match &.
template <class BiIter>
bool operator==(
	typename std::iterator_traits<BiIter>::value_type const *lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs.compare(lhs) == 0;	//  19
}

template <class BiIter>
bool operator!=(
	typename std::iterator_traits<BiIter>::value_type const *lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(lhs == rhs);	//  20
}

template <class BiIter>
bool operator<(
	typename std::iterator_traits<BiIter>::value_type const *lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs.compare(lhs) > 0;	//  21
}

template <class BiIter>
bool operator>(
	typename std::iterator_traits<BiIter>::value_type const *lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs < lhs;	//  22
}

template <class BiIter>
bool operator>=(
	typename std::iterator_traits<BiIter>::value_type const *lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(lhs < rhs);	//  23
}

template <class BiIter>
bool operator<=(
	typename std::iterator_traits<BiIter>::value_type const *lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(rhs < lhs);	//  24
}

//  Compares sub_match & with iterator_traits::value_type *.
template <class BiIter>
bool operator==(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const *rhs
)
{
	return lhs.compare(rhs) == 0;	//  25
}

template <class BiIter>
bool operator!=(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const *rhs
)
{
	return !(lhs == rhs);	//  26
}

template <class BiIter>
bool operator<(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const *rhs
)
{
	return lhs.compare(rhs) < 0;	//  27
}

template <class BiIter>
bool operator>(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const *rhs
)
{
	return rhs < lhs;	//  28
}

template <class BiIter>
bool operator>=(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const *rhs
)
{
	return !(lhs < rhs);	//  29
}

template <class BiIter>
bool operator<=(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const *rhs
)
{
	return !(rhs < lhs);	//  30
}

//  Compares iterator_traits::value_type & with sub_match &.
template <class BiIter>
bool operator==(
	typename std::iterator_traits<BiIter>::value_type const &lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs.compare(typename sub_match<BiIter>::string_type(1, lhs)) == 0;	//  31
}

template <class BiIter>
bool operator!=(
	typename std::iterator_traits<BiIter>::value_type const &lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(lhs == rhs);	//  32
}

template <class BiIter>
bool operator<(
	typename std::iterator_traits<BiIter>::value_type const &lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs.compare(typename sub_match<BiIter>::string_type(1, lhs)) > 0;	//  33
}

template <class BiIter>
bool operator>(
	typename std::iterator_traits<BiIter>::value_type const &lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs < lhs;	//  34
}

template <class BiIter>
bool operator>=(
	typename std::iterator_traits<BiIter>::value_type const &lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(lhs < rhs);	//  35
}

template <class BiIter>
bool operator<=(
	typename std::iterator_traits<BiIter>::value_type const &lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(rhs < lhs);	//  36
}

//  Compares sub_match & with iterator_traits::value_type &.
template <class BiIter>
bool operator==(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const &rhs
)
{
	return lhs.compare(typename sub_match<BiIter>::string_type(1, rhs)) == 0;	//  37
}

template <class BiIter>
bool operator!=(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const &rhs
)
{
	return !(lhs == rhs);	//  38
}

template <class BiIter>
bool operator<(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const &rhs
)
{
	return lhs.compare(typename sub_match<BiIter>::string_type(1, rhs)) < 0;	//  39
}

template <class BiIter>
bool operator>(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const &rhs
)
{
	return rhs < lhs;	//  40
}

template <class BiIter>
bool operator>=(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const &rhs
)
{
	return !(lhs < rhs);	//  41
}

template <class BiIter>
bool operator<=(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const &rhs
)
{
	return !(rhs < lhs);	//  42
}

template <class charT, class ST, class BiIter>
std::basic_ostream<charT, ST> &operator<<(std::basic_ostream<charT, ST> &os, const sub_match<BiIter> &m)
{
	return (os << m.str());
}

typedef sub_match<const char *> csub_match;
typedef sub_match<const wchar_t *> wcsub_match;
typedef sub_match<std::string::const_iterator> ssub_match;
typedef sub_match<std::wstring::const_iterator> wssub_match;

#if defined(SRELL_CPP11_CHAR1632_ENABLED)
	typedef sub_match<const char16_t *> u16csub_match;
	typedef sub_match<const char32_t *> u32csub_match;
	typedef sub_match<std::u16string::const_iterator> u16ssub_match;
	typedef sub_match<std::u32string::const_iterator> u32ssub_match;
#endif

#if defined(SRELL_CPP20_CHAR8_ENABLED)
	typedef sub_match<const char8_t *> u8csub_match;
#endif
#if defined(SRELL_CPP20_CHAR8_ENABLED) && SRELL_CPP20_CHAR8_ENABLED >= 2
	typedef sub_match<std::u8string::const_iterator> u8ssub_match;
#endif

typedef csub_match u8ccsub_match;
typedef ssub_match u8cssub_match;
#if !defined(SRELL_CPP20_CHAR8_ENABLED)
	typedef u8ccsub_match u8csub_match;
#endif
#if !defined(SRELL_CPP20_CHAR8_ENABLED) || SRELL_CPP20_CHAR8_ENABLED < 2
	typedef u8cssub_match u8ssub_match;
#endif

#if defined(WCHAR_MAX)
	#if WCHAR_MAX >= 0x10ffff
		typedef wcsub_match u32wcsub_match;
		typedef wssub_match u32wssub_match;
	#elif WCHAR_MAX >= 0xffff
		typedef wcsub_match u16wcsub_match;
		typedef wssub_match u16wssub_match;
	#endif
#endif

//  ... "regex_sub_match.hpp"]
//  ["regex_match_results.hpp" ...

//  28.10, class template match_results:
template <class BidirectionalIterator, class Allocator = std::allocator<sub_match<BidirectionalIterator> > >
class match_results
{
public:

	typedef sub_match<BidirectionalIterator> value_type;
	typedef const value_type & const_reference;
	typedef const_reference reference;
//	typedef implementation defined const_iterator;
	typedef typename std::vector<value_type, Allocator>::const_iterator const_iterator;
	typedef const_iterator iterator;
	typedef typename std::iterator_traits<BidirectionalIterator>::difference_type difference_type;

#if defined(__cplusplus) && __cplusplus >= 201103L
	typedef typename std::allocator_traits<Allocator>::size_type size_type;
#else
	typedef typename Allocator::size_type size_type;	//  TR1.
#endif

	typedef Allocator allocator_type;
	typedef typename std::iterator_traits<BidirectionalIterator>::value_type char_type;
	typedef std::basic_string<char_type> string_type;

public:

	//  28.10.1, construct/copy/destroy:
	//  [7.10.1] construct/copy/destroy
	explicit match_results(const Allocator &a = Allocator()) : ready_(false), sub_matches_(a)
	{
	}

	match_results(const match_results &m)
	{
		operator=(m);
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	match_results(match_results &&m) SRELL_NOEXCEPT
	{
		operator=(std::move(m));
	}
#endif

	match_results &operator=(const match_results &m)
	{
		if (this != &m)
		{
//			this->sstate_ = m.sstate_;
			this->ready_ = m.ready_;
			this->sub_matches_ = m.sub_matches_;
			this->prefix_ = m.prefix_;
			this->suffix_ = m.suffix_;
			this->base_ = m.base_;
#if !defined(SRELL_NO_NAMEDCAPTURE)
			this->gnames_ = m.gnames_;
#endif
		}
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	match_results &operator=(match_results &&m) SRELL_NOEXCEPT
	{
		if (this != &m)
		{
//			this->sstate_ = std::move(m.sstate_);
			this->ready_ = m.ready_;
			this->sub_matches_ = std::move(m.sub_matches_);
			this->prefix_ = std::move(m.prefix_);
			this->suffix_ = std::move(m.suffix_);
			this->base_ = m.base_;
#if !defined(SRELL_NO_NAMEDCAPTURE)
			this->gnames_ = std::move(m.gnames_);
#endif
		}
		return *this;
	}
#endif

//	~match_results();

	//  28.10.2, state:
	bool ready() const
	{
		return ready_;
	}

	//  28.10.3, size:
	//  [7.10.2] size
	size_type size() const
	{
		return sub_matches_.size();
	}

	size_type max_size() const
	{
		return sub_matches_.max_size();
//		return static_cast<size_type>(~0) / sizeof (value_type);
	}

	bool empty() const
	{
		return size() == 0;
	}

	//  28.10.4, element access:
	//  [7.10.3] element access
	difference_type length(const size_type sub = 0) const
	{
		return (*this)[sub].length();
	}

	difference_type position(const size_type sub = 0) const
	{
		const_reference ref = (*this)[sub];

		return std::distance(base_, ref.first);
	}

	string_type str(const size_type sub = 0) const
	{
		return string_type((*this)[sub]);
	}

	const_reference operator[](const size_type n) const
	{
		return sub_matches_[n];
	}

#if !defined(SRELL_NO_NAMEDCAPTURE)

	//  Helpers for overload resolution of the integer literal 0 of signed types.
	template <typename IntegerType>
	difference_type length(const IntegerType zero) const
	{
		return length(static_cast<size_type>(zero));
	}
	template <typename IntegerType>
	difference_type position(const IntegerType zero) const
	{
		return position(static_cast<size_type>(zero));
	}
	template <typename IntegerType>
	string_type str(const IntegerType zero) const
	{
		return str(static_cast<size_type>(zero));
	}
	template <typename IntegerType>
	const_reference operator[](const IntegerType zero) const
	{
		return operator[](static_cast<size_type>(zero));
	}

	difference_type length(const string_type &sub) const
	{
		return (*this)[sub].length();
	}

	difference_type position(const string_type &sub) const
	{
		const_reference ref = (*this)[sub];

		return std::distance(base_, ref.first);
	}

	string_type str(const string_type &sub) const
	{
		return string_type((*this)[sub]);
	}

	const_reference operator[](const string_type &sub) const
	{
		return sub_matches_[lookup_and_check_backref_number(sub.c_str(), sub.c_str() + sub.size())];
	}

	difference_type length(const char_type *sub) const
	{
		return (*this)[sub].length();
	}

	difference_type position(const char_type *sub) const
	{
		const_reference ref = (*this)[sub];

		return std::distance(base_, ref.first);
	}

	string_type str(const char_type *sub) const
	{
		return string_type((*this)[sub]);
	}

	const_reference operator[](const char_type *sub) const
	{
		return sub_matches_[lookup_and_check_backref_number(sub, sub + std::char_traits<char_type>::length(sub))];
	}

#endif	//  !defined(SRELL_NO_NAMEDCAPTURE)

	const_reference prefix() const
	{
		return prefix_;
	}

	const_reference suffix() const
	{
		return suffix_;
	}

	const_iterator begin() const
	{
		return sub_matches_.begin();
	}

	const_iterator end() const
	{
		return sub_matches_.end();
	}

	const_iterator cbegin() const
	{
		return sub_matches_.begin();
	}

	const_iterator cend() const
	{
		return sub_matches_.end();
	}

	//  28.10.5, format:
	//  [7.10.4] format
	template <class OutputIter>
	OutputIter format(
		OutputIter out,
		const char_type *fmt_first,
		const char_type *const fmt_last,
		regex_constants::match_flag_type /* flags */ = regex_constants::format_default
	) const
	{
		if (this->ready() && !this->empty())
		{
#if !defined(SRELL_NO_NAMEDCAPTURE)
			const bool no_groupnames = gnames_.size() == 0;
#endif
			const value_type &m0 = (*this)[0];

			while (fmt_first != fmt_last)
			{
				if (*fmt_first != static_cast<char_type>(regex_internal::meta_char::mc_dollar))	//  '$'
				{
					*out++ = *fmt_first++;
				}
				else
				{
					++fmt_first;
					if (fmt_first == fmt_last)
					{
						*out++ = regex_internal::meta_char::mc_dollar;	//  '$';
					}
					else if (*fmt_first == static_cast<char_type>(regex_internal::char_other::co_amp))	//  '&', $&
					{
						out = std::copy(m0.first, m0.second, out);
						++fmt_first;
					}
					else if (*fmt_first == static_cast<char_type>(regex_internal::char_other::co_grav))	//  '`', $`, prefix.
					{
						out = std::copy(this->prefix().first, this->prefix().second, out);
						++fmt_first;
					}
					else if (*fmt_first == static_cast<char_type>(regex_internal::char_other::co_apos))	//  '\'', $', suffix.
					{
						out = std::copy(this->suffix().first, this->suffix().second, out);
						++fmt_first;
					}
#if !defined(SRELL_NO_NAMEDCAPTURE)
					else if (*fmt_first == static_cast<char_type>(regex_internal::meta_char::mc_lt) && !no_groupnames)	//  '<', $<
					{
						const char_type *const current_backup = fmt_first;
						bool replaced = false;

						if (++fmt_first == fmt_last)
							;	//  Do nothing.
						else
						{
							const char_type *const name_begin = fmt_first;

							for (;; ++fmt_first)
							{
								if (*fmt_first == static_cast<char_type>(regex_internal::meta_char::mc_gt))
								{
									const unsigned int backref_number = lookup_backref_number(name_begin, fmt_first);

									if (backref_number != regex_internal::groupname_mapper<char_type>::notfound)
									{
										const value_type &mn = (*this)[backref_number];

										if (mn.matched)
											out = std::copy(mn.first, mn.second, out);
//										replaced = true;
									}
									replaced = true;
									++fmt_first;
									break;
								}
								if (fmt_first == fmt_last)
									break;
							}
						}
						if (!replaced)
						{
							fmt_first = current_backup;
							*out++ = regex_internal::meta_char::mc_dollar;	//  '$';
						}
					}
#endif	//  !defined(SRELL_NO_NAMEDCAPTURE)
					else
					{
						const char_type *const backup_pos = fmt_first;
						size_type backref_number = 0;

						if (fmt_first != fmt_last && *fmt_first >= static_cast<char_type>(regex_internal::char_alnum::ch_0) && *fmt_first <= static_cast<char_type>(regex_internal::char_alnum::ch_9))	//  '0'-'9'
						{
							backref_number += *fmt_first - regex_internal::char_alnum::ch_0;	//  '0';

							if (++fmt_first != fmt_last && *fmt_first >= static_cast<char_type>(regex_internal::char_alnum::ch_0) && *fmt_first <= static_cast<char_type>(regex_internal::char_alnum::ch_9))	//  '0'-'9'
							{
								backref_number *= 10;
								backref_number += *fmt_first - regex_internal::char_alnum::ch_0;	//  '0';
								++fmt_first;
							}
						}

						if (backref_number && backref_number < this->size())
						{
							const value_type &mn = (*this)[backref_number];

							if (mn.matched)
								out = std::copy(mn.first, mn.second, out);
						}
						else
						{
							*out++ = regex_internal::meta_char::mc_dollar;	//  '$';

							fmt_first = backup_pos;
							if (*fmt_first == static_cast<char_type>(regex_internal::meta_char::mc_dollar))
								++fmt_first;
						}
					}
				}
			}
		}
		return out;
	}

	template <class OutputIter, class ST, class SA>
	OutputIter format(
		OutputIter out,
		const std::basic_string<char_type, ST, SA> &fmt,
		regex_constants::match_flag_type flags = regex_constants::format_default
	) const
	{
		return format(out, fmt.data(), fmt.data() + fmt.size(), flags);
	}

	template <class ST, class SA>
	std::basic_string<char_type, ST, SA> format(
		const string_type &fmt,
		regex_constants::match_flag_type flags = regex_constants::format_default
	) const
	{
		std::basic_string<char_type, ST, SA> result;

//		format(std::back_insert_iterator<string_type>(result), fmt, flags);
		format(std::back_inserter(result), fmt, flags);
		return result;
	}

	string_type format(const char_type *fmt, regex_constants::match_flag_type flags = regex_constants::format_default) const
	{
		string_type result;

		format(std::back_inserter(result), fmt, fmt + std::char_traits<char_type>::length(fmt), flags);
		return result;
	}

	//  28.10.6, allocator:
	//  [7.10.5] allocator
	allocator_type get_allocator() const
	{
		return allocator_type();
	}

	//  28.10.7, swap:
	//  [7.10.6] swap
	void swap(match_results &that)
	{
		const match_results tmp(that);
		that = *this;
		*this = tmp;
	}

public:	//  For internal.

	typedef match_results<BidirectionalIterator> match_results_type;
	typedef typename match_results_type::size_type match_results_size_type;
	typedef typename regex_internal::re_search_state</*charT, */BidirectionalIterator> search_state_type;

	search_state_type sstate_;

	void clear_()
	{
		ready_ = false;
		sub_matches_.clear();
//		prefix_.matched = false;
//		suffix_.matched = false;
#if !defined(SRELL_NO_NAMEDCAPTURE)
		gnames_.clear();
#endif
	}

//	template <typename charT>
#if !defined(SRELL_NO_NAMEDCAPTURE)
	bool set_match_results_(const regex_internal::groupname_mapper<char_type> &gnames)
#else
	bool set_match_results_()
#endif
	{
		sub_matches_.resize(sstate_.bracket.size());
//		value_type &m0 = sub_matches_[0];

		sub_matches_[0].matched = true;

		for (unsigned int i = 1; i < static_cast<unsigned int>(sstate_.bracket.size()); ++i)
		{
			const typename search_state_type::submatch_type &br = sstate_.bracket[i];
			value_type &sm = sub_matches_[i];

			sm.first = br.core.open_at;
			sm.second = br.core.close_at;
			sm.matched = br.counter != 0;
		}

		base_ = sstate_.lblim;
		prefix_.first = sstate_.srchbegin;
		prefix_.second = sub_matches_[0].first = sstate_.bracket[0].core.open_at;
		suffix_.first = sub_matches_[0].second = sstate_.nth.in_string;
		suffix_.second = sstate_.srchend;

		prefix_.matched = prefix_.first != prefix_.second;	//  The spec says prefix().first != prefix().second
		suffix_.matched = suffix_.first != suffix_.second;	//  The spec says suffix().first != suffix().second

#if !defined(SRELL_NO_NAMEDCAPTURE)
		gnames_ = gnames;
#endif
		ready_ = true;
		return true;
	}

	bool set_match_results_bmh_()
	{
		sub_matches_.resize(1);
//		value_type &m0 = sub_matches_[0];

		sub_matches_[0].matched = true;

		base_ = sstate_.lblim;
		prefix_.first = sstate_.srchbegin;
		prefix_.second = sub_matches_[0].first = sstate_.nth.in_string;
		suffix_.first = sub_matches_[0].second = sstate_.nextpos;
		suffix_.second = sstate_.srchend;

		prefix_.matched = prefix_.first != prefix_.second;
		suffix_.matched = suffix_.first != suffix_.second;

		ready_ = true;
		return true;
	}

	void set_prefix_first_(const BidirectionalIterator &pf)
	{
		prefix_.first = pf;
	}

	bool mark_as_failed_()
	{
		ready_ = true;	//  30.11.2 and 3: Postconditions: m.ready() == true in all cases.
		return false;
	}

private:

#if !defined(SRELL_NO_NAMEDCAPTURE)

	unsigned int lookup_backref_number(const char_type *begin, const char_type *const end) const
	{
		typename regex_internal::groupname_mapper<char_type>::gname_string key(end - begin);

		for (std::size_t i = 0; begin != end; ++begin, ++i)
			key[i] = *begin;

		return gnames_[key];
	}

	unsigned int lookup_and_check_backref_number(const char_type *begin, const char_type *const end) const
	{
		const unsigned int backrefno = lookup_backref_number(begin, end);

		if (backrefno == regex_internal::groupname_mapper<char_type>::notfound)
			throw regex_error(regex_constants::error_backref);

		return backrefno;
	}

#endif	//  !defined(SRELL_NO_NAMEDCAPTURE)

public:	//  For debug.

	template <typename BasicRegexT>
	void print_sub_matches(const BasicRegexT &, const int) const;
	void print_addresses(const value_type &, const char *const) const;

private:

	typedef std::vector<value_type, Allocator> sub_match_array;

	bool ready_;
	sub_match_array sub_matches_;
	value_type prefix_;
	value_type suffix_;
	BidirectionalIterator base_;

#if !defined(SRELL_NO_NAMEDCAPTURE)
	regex_internal::groupname_mapper<char_type> gnames_;
#endif
};

//  28.10.7, match_results swap:
//  [7.10.6] match_results swap
template <class BidirectionalIterator, class Allocator>
void swap(
	match_results<BidirectionalIterator, Allocator> &m1,
	match_results<BidirectionalIterator, Allocator> &m2
)
{
	m1.swap(m2);
}

//  28.10.8, match_results comparisons
template <class BidirectionalIterator, class Allocator>
bool operator==(
	const match_results<BidirectionalIterator, Allocator> &m1,
	const match_results<BidirectionalIterator, Allocator> &m2
)
{
	if (!m1.ready() && !m2.ready())
		return true;

	if (m1.ready() && m2.ready())
	{
		if (m1.empty() && m2.empty())
			return true;

		if (!m1.empty() && !m2.empty())
		{
			return m1.prefix() == m2.prefix() && m1.size() == m2.size() && std::equal(m1.begin(), m1.end(), m2.begin()) && m1.suffix() == m2.suffix();
		}
	}
	return false;
}

template <class BidirectionalIterator, class Allocator>
bool operator!=(
	const match_results<BidirectionalIterator, Allocator> &m1,
	const match_results<BidirectionalIterator, Allocator> &m2
)
{
	return !(m1 == m2);
}

typedef match_results<const char *> cmatch;
typedef match_results<const wchar_t *> wcmatch;
typedef match_results<std::string::const_iterator> smatch;
typedef match_results<std::wstring::const_iterator> wsmatch;

#if defined(SRELL_CPP11_CHAR1632_ENABLED)
	typedef match_results<const char16_t *> u16cmatch;
	typedef match_results<const char32_t *> u32cmatch;
	typedef match_results<std::u16string::const_iterator> u16smatch;
	typedef match_results<std::u32string::const_iterator> u32smatch;
#endif

#if defined(SRELL_CPP20_CHAR8_ENABLED)
	typedef match_results<const char8_t *> u8cmatch;
#endif
#if defined(SRELL_CPP20_CHAR8_ENABLED) && SRELL_CPP20_CHAR8_ENABLED >= 2
	typedef match_results<std::u8string::const_iterator> u8smatch;
#endif

typedef cmatch u8ccmatch;
typedef smatch u8csmatch;
#if !defined(SRELL_CPP20_CHAR8_ENABLED)
	typedef u8ccmatch u8cmatch;
#endif
#if !defined(SRELL_CPP20_CHAR8_ENABLED) || SRELL_CPP20_CHAR8_ENABLED < 2
	typedef u8csmatch u8smatch;
#endif

#if defined(WCHAR_MAX)
	#if WCHAR_MAX >= 0x10ffff
		typedef wcmatch u32wcmatch;
		typedef wsmatch u32wsmatch;
	#elif WCHAR_MAX >= 0xffff
		typedef wcmatch u16wcmatch;
		typedef wsmatch u16wsmatch;
	#endif
#endif

//  ... "regex_match_results.hpp"]
//  ["rei_algorithm.hpp" ...

	namespace regex_internal
	{

template <typename charT, typename traits>
class regex_object : public re_compiler<charT, traits>
{
public:

	template <typename BidirectionalIterator>
	bool search
	(
		const BidirectionalIterator &begin,
		const BidirectionalIterator &end,
		const BidirectionalIterator &lookbehind_limit,
		match_results<BidirectionalIterator> &results,
		const regex_constants::match_flag_type flags /* = regex_constants::match_default */
	) const
	{
		results.clear_();

//		results.sstate_.template init<utf_traits>(begin, end, lookbehind_limit, flags);
		results.sstate_.init(begin, end, lookbehind_limit, flags);

#if !defined(SRELLDBG_NO_BMH)
		if (this->bmdata && !results.sstate_.match_continuous_flag())
		{
			if (!this->is_ricase() ? this->bmdata->do_casesensitivesearch(results.sstate_, typename std::iterator_traits<BidirectionalIterator>::iterator_category()) : this->bmdata->do_icasesearch(results.sstate_, typename std::iterator_traits<BidirectionalIterator>::iterator_category()))
				return results.set_match_results_bmh_();
		}
		else
#endif
		if (this->NFA_states.size())
		{
			results.sstate_.init_for_automaton(this->NFA_states[0].next_state1, this->number_of_brackets, this->number_of_counters, this->number_of_repeats);

			if (!this->is_ricase() ? do_search<false>(results) : do_search<true>(results))
			{
#if !defined(SRELL_NO_NAMEDCAPTURE)
				return results.set_match_results_(this->namedcaptures);
#else
				return results.set_match_results_();
#endif
			}
		}
		return results.mark_as_failed_();
	}

private:

	typedef typename traits::utf_traits utf_traits;

	template <const bool icase, typename BidirectionalIterator>
	bool do_search
	(
		match_results<BidirectionalIterator> &results
	) const
	{
		re_search_state</*charT, */BidirectionalIterator> &sstate = results.sstate_;
		const BidirectionalIterator searchend = sstate.nth.in_string;

		for (;;)
		{
			const bool final = sstate.nextpos == searchend;

			sstate.nth.in_string = sstate.nextpos;

			if (!final)
			{

#ifdef SRELLDBG_NO_1STCHRCLS
				utf_traits::codepoint_inc(sstate.nextpos, sstate.srchend);
#else
				{
	#if !defined(SRELLDBG_NO_BITSET)
					if (!this->firstchar_class_bs.test((*sstate.nextpos++) & utf_traits::bitsetmask))
	#else
					const uchar21 firstchar = utf_traits::codepoint_inc(sstate.nextpos, sstate.srchend);

					if (!this->firstchar_class.is_included(firstchar))
	#endif
						continue;
				}
#endif
			}
			//  Even when final == true, we have to try for such expressions
			//  as "" =~ /^$/ or "..." =~ /$/.

#if defined(SRELL_NO_LIMIT_COUNTER)
			sstate.reset(/* first */);
#else
			sstate.reset(/* first, */ this->limit_counter);
#endif
			if (run_automaton<icase, false>(sstate /* , false */))
				return true;

			if (final)
				break;
		}
		return false;
	}

	template <typename T, const bool>
	struct casehelper
	{
		static T canonicalise(const T t)
		{
			return t;
		}
	};

	template <typename T>
	struct casehelper<T, true>
	{
		static T canonicalise(const T t)
		{
			return unicode_case_folding::do_casefolding(t);
		}
	};

	template <const bool icase, const bool reverse, typename BidirectionalIterator>
	bool run_automaton
	(
//		match_results<BidirectionalIterator> &results,
		re_search_state</*charT, */BidirectionalIterator> &sstate
//		, const bool is_recursive /* = false */
	) const
	{
		typedef casehelper<uchar21, icase> casehelper_type;
		typedef typename re_object_core<charT, traits>::state_type state_type;
		typedef re_search_state</*charT, */BidirectionalIterator> ss_type;
		typedef typename ss_type::search_core_state scstate_type;
		typedef typename ss_type::submatch_type submatch_type;
		typedef typename ss_type::submatchcore_type submatchcore_type;
		typedef typename ss_type::counter_type counter_type;
		typedef typename ss_type::position_type position_type;
		bool is_matched;

		goto START;

		JUDGE:
		if (is_matched)
		{
			MATCHED:
//			sstate.nth.in_NFA_states = current_NFA.next_state1;
			sstate.nth.in_NFA_states = sstate.nth.in_NFA_states->next_state1;
		}
		else
		{
			NOT_MATCHED:

#if !defined(SRELL_NO_LIMIT_COUNTER)
			if (--sstate.failure_counter)
			{
#endif
				if (sstate.bt_stack.size() > sstate.btstack_size)
				{
					sstate.nth = sstate.bt_stack.back();
					sstate.bt_stack.pop_back();

					sstate.nth.in_NFA_states = sstate.nth.in_NFA_states->next_state2;
//					continue;
				}
				else
				{
					return false;
				}
#if !defined(SRELL_NO_LIMIT_COUNTER)
			}
			else
				throw regex_error(regex_constants::error_complexity);
#endif
		}

		START:
		for (;;)
		{
			const state_type &current_NFA = *sstate.nth.in_NFA_states;

			switch (current_NFA.type)
			{
			case st_character:

#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(push)
#pragma warning(disable:4127)
#endif
				if (!reverse)
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(pop)
#endif
				{
					if (!sstate.is_at_srchend())
					{
#if !defined(SRELLDBG_NO_ASTERISK_OPT)
						const BidirectionalIterator prevpos = sstate.nth.in_string;
#endif
						const uchar21 uchar = utf_traits::codepoint_inc(sstate.nth.in_string, sstate.srchend);

						if (current_NFA.character == casehelper_type::canonicalise(uchar))
							goto MATCHED;

#if !defined(SRELLDBG_NO_ASTERISK_OPT)
						if (current_NFA.next_state2)
						{
							sstate.nth.in_string = prevpos;
							sstate.nth.in_NFA_states = current_NFA.next_state2;
							continue;
						}
#endif
					}
#if !defined(SRELLDBG_NO_ASTERISK_OPT)
					else if (current_NFA.next_state2)
					{
						sstate.nth.in_NFA_states = current_NFA.next_state2;
						continue;
					}
#endif
				}
				else	//  reverse == true.
				{
					if (!sstate.is_at_lookbehindlimit())
					{
#if !defined(SRELLDBG_NO_ASTERISK_OPT)
						const BidirectionalIterator prevpos = sstate.nth.in_string;
#endif
						const uchar21 uchar = utf_traits::dec_codepoint(sstate.nth.in_string, sstate.lblim);

						if (current_NFA.character == casehelper_type::canonicalise(uchar))
							goto MATCHED;

#if !defined(SRELLDBG_NO_ASTERISK_OPT)
						if (current_NFA.next_state2)
						{
							sstate.nth.in_string = prevpos;
							sstate.nth.in_NFA_states = current_NFA.next_state2;
							continue;
						}
#endif
					}
#if !defined(SRELLDBG_NO_ASTERISK_OPT)
					else if (current_NFA.next_state2)
					{
						sstate.nth.in_NFA_states = current_NFA.next_state2;
						continue;
					}
#endif
				}
				goto NOT_MATCHED;

			case st_character_class:

#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(push)
#pragma warning(disable:4127)
#endif
				if (!reverse)
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(pop)
#endif
				{
					if (!sstate.is_at_srchend())
					{
#if !defined(SRELLDBG_NO_ASTERISK_OPT)
						const BidirectionalIterator prevpos = sstate.nth.in_string;
#endif
						const uchar21 uchar = utf_traits::codepoint_inc(sstate.nth.in_string, sstate.srchend);

#if !defined(SRELLDBG_NO_CCPOS)
						if (this->character_class.is_included(current_NFA.quantifier.offset, current_NFA.quantifier.length, uchar))
#else
						if (this->character_class.is_included(current_NFA.number, uchar))
#endif
							goto MATCHED;

#if !defined(SRELLDBG_NO_ASTERISK_OPT)
						if (current_NFA.next_state2)
						{
							sstate.nth.in_string = prevpos;
							sstate.nth.in_NFA_states = current_NFA.next_state2;
							continue;
						}
#endif
					}
#if !defined(SRELLDBG_NO_ASTERISK_OPT)
					else if (current_NFA.next_state2)
					{
						sstate.nth.in_NFA_states = current_NFA.next_state2;
						continue;
					}
#endif
				}
				else	//  reverse == true.
				{
					if (!sstate.is_at_lookbehindlimit())
					{
#if !defined(SRELLDBG_NO_ASTERISK_OPT)
						const BidirectionalIterator prevpos = sstate.nth.in_string;
#endif
						const uchar21 uchar = utf_traits::dec_codepoint(sstate.nth.in_string, sstate.lblim);

#if !defined(SRELLDBG_NO_CCPOS)
						if (this->character_class.is_included(current_NFA.quantifier.offset, current_NFA.quantifier.length, uchar))
#else
						if (this->character_class.is_included(current_NFA.number, uchar))
#endif
							goto MATCHED;

#if !defined(SRELLDBG_NO_ASTERISK_OPT)
						if (current_NFA.next_state2)
						{
							sstate.nth.in_string = prevpos;
							sstate.nth.in_NFA_states = current_NFA.next_state2;
							continue;
						}
#endif
					}
#if !defined(SRELLDBG_NO_ASTERISK_OPT)
					else if (current_NFA.next_state2)
					{
						sstate.nth.in_NFA_states = current_NFA.next_state2;
						continue;
					}
#endif
				}
				goto NOT_MATCHED;

			case st_epsilon:

#if defined(SRELLDBG_NO_SKIP_EPSILON)
				if (current_NFA.next_state2)
#endif
				{
					sstate.bt_stack.push_back(sstate.nth);	//	sstate.push();
				}

				sstate.nth.in_NFA_states = current_NFA.next_state1;
				continue;

			default:
				switch (current_NFA.type)
				{

			case st_check_counter:
				{
					const unsigned int counter = sstate.counter[current_NFA.number];

					if (counter < current_NFA.quantifier.atmost || current_NFA.quantifier.is_infinity())
					{
						if (counter >= current_NFA.quantifier.atleast)
						{
#if !defined(SRELLDBG_NO_ASTERISK_OPT) && !defined(SRELLDBG_NO_SPLIT_COUNTER)
							if (!current_NFA.dont_push)
#endif
								sstate.bt_stack.push_back(sstate.nth);
							sstate.nth.in_NFA_states = current_NFA.next_state1;
						}
						else
						{
							sstate.nth.in_NFA_states
								= current_NFA.quantifier.is_greedy
								? current_NFA.next_state1
								: current_NFA.next_state2;
						}
					}
					else
					{
						sstate.nth.in_NFA_states
							= current_NFA.quantifier.is_greedy
							? current_NFA.next_state2
							: current_NFA.next_state1;
					}
				}
				continue;

			case st_increment_counter:
				sstate.bt_stack.push_back(sstate.nth);
				++sstate.counter[current_NFA.number];
				goto MATCHED;

			case st_decrement_counter:
				--sstate.counter[current_NFA.number];
				goto NOT_MATCHED;

			case st_save_and_reset_counter:
				{
					counter_type &c = sstate.counter[current_NFA.number];

					sstate.counter_stack.push_back(c);
					sstate.bt_stack.push_back(sstate.nth);
					c = 0;
				}
				goto MATCHED;

			case st_restore_counter:
				sstate.counter[current_NFA.number] = sstate.counter_stack.back();
				sstate.counter_stack.pop_back();
				goto NOT_MATCHED;

			case st_roundbracket_open:	//  '(':
				{
					submatch_type &bracket = sstate.bracket[current_NFA.number];

					sstate.capture_stack.push_back(bracket.core);

#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(push)
#pragma warning(disable:4127)
#endif
					if (!reverse)
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(pop)
#endif
					{
						bracket.core.open_at = sstate.nth.in_string;
					}
					else
						bracket.core.close_at = sstate.nth.in_string;

					++bracket.counter;

					for (unsigned int brno = current_NFA.quantifier.atleast; brno <= current_NFA.quantifier.atmost; ++brno)
					{
						submatch_type &inner_bracket = sstate.bracket[brno];

						sstate.capture_stack.push_back(inner_bracket.core);
						sstate.counter_stack.push_back(inner_bracket.counter);
						inner_bracket.core.open_at = inner_bracket.core.close_at = sstate.srchend;
						inner_bracket.counter = 0;
						//  ECMAScript spec (3-5.1) 15.10.2.5, NOTE 3.
						//  ECMAScript 2018 (ES9) 21.2.2.5.1, Note 3.
					}

					sstate.bt_stack.push_back(sstate.nth);
				}
				goto MATCHED;

			case st_roundbracket_pop:	//  '/':
				{
					for (unsigned int brno = current_NFA.quantifier.atmost; brno >= current_NFA.quantifier.atleast; --brno)
					{
						submatch_type &inner_bracket = sstate.bracket[brno];

						inner_bracket.counter = sstate.counter_stack.back();
						inner_bracket.core = sstate.capture_stack.back();
						sstate.counter_stack.pop_back();
						sstate.capture_stack.pop_back();
					}

					submatch_type &bracket = sstate.bracket[current_NFA.number];

					bracket.core = sstate.capture_stack.back();
					sstate.capture_stack.pop_back();
					--bracket.counter;
				}
				goto NOT_MATCHED;

			case st_roundbracket_close:	//  ')':
				{
					submatch_type &bracket = sstate.bracket[current_NFA.number];
					submatchcore_type &brc = bracket.core;

					if ((!reverse ? brc.open_at : brc.close_at) != sstate.nth.in_string)
					{
						sstate.nth.in_NFA_states = current_NFA.next_state1;
					}
					else	//  0 width match, breaks from the loop.
					{
						if (current_NFA.next_state1->type != st_check_counter)
						{
							if (bracket.counter > 1)
								goto NOT_MATCHED;	//  ECMAScript spec 15.10.2.5, note 4.

							sstate.nth.in_NFA_states = current_NFA.next_state2;
								//  Accepts 0 width match and exits.
						}
						else
						{
							//  A pair with check_counter.
							const counter_type counter = sstate.counter[current_NFA.next_state1->number];

							if (counter > current_NFA.quantifier.atleast)
								goto NOT_MATCHED;	//  Takes a captured string in the previous loop.

							sstate.nth.in_NFA_states = current_NFA.next_state1;
								//  Accepts 0 width match and continues.
						}
					}
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(push)
#pragma warning(disable:4127)
#endif
					if (!reverse)
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(pop)
#endif
					{
						brc.close_at = sstate.nth.in_string;
					}
					else	//  reverse == true.
					{
						brc.open_at  = sstate.nth.in_string;
					}
				}
				continue;

			case st_repeat_in_push:
				{
					position_type &r = sstate.repeat[current_NFA.number];

					sstate.repeat_stack.push_back(r);
					r = sstate.nth.in_string;

					for (unsigned int brno = current_NFA.quantifier.atleast; brno <= current_NFA.quantifier.atmost; ++brno)
					{
						submatch_type &inner_bracket = sstate.bracket[brno];

						sstate.capture_stack.push_back(inner_bracket.core);
						sstate.counter_stack.push_back(inner_bracket.counter);
						inner_bracket.core.open_at = inner_bracket.core.close_at = sstate.srchend;
						inner_bracket.counter = 0;
						//  ECMAScript 2019 (ES10) 21.2.2.5.1, Note 3.
					}
					sstate.bt_stack.push_back(sstate.nth);
				}
				goto MATCHED;

			case st_repeat_in_pop:
				for (unsigned int brno = current_NFA.quantifier.atmost; brno >= current_NFA.quantifier.atleast; --brno)
				{
					submatch_type &inner_bracket = sstate.bracket[brno];

					inner_bracket.counter = sstate.counter_stack.back();
					inner_bracket.core = sstate.capture_stack.back();
					sstate.counter_stack.pop_back();
					sstate.capture_stack.pop_back();
				}

				sstate.repeat[current_NFA.number] = sstate.repeat_stack.back();
				sstate.repeat_stack.pop_back();
				goto NOT_MATCHED;

			case st_check_0_width_repeat:
				if (sstate.nth.in_string != sstate.repeat[current_NFA.number])
					goto MATCHED;

				sstate.nth.in_NFA_states = current_NFA.next_state2;
				continue;

			case st_backreference:	//  '\\':
				{
					const submatch_type &bracket = sstate.bracket[current_NFA.number];

					if (!bracket.counter)	//  Undefined.
					{
						ESCAPE_FROM_ZERO_WIDTH_MATCH:
						sstate.nth.in_NFA_states = current_NFA.next_state2;
						continue;
					}
					else
					{
						const submatchcore_type &brc = bracket.core;

						if (brc.open_at == brc.close_at)
						{
							goto ESCAPE_FROM_ZERO_WIDTH_MATCH;
						}
						else
						{
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(push)
#pragma warning(disable:4127)
#endif
							if (!reverse)
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(pop)
#endif
							{
								for (BidirectionalIterator backrefpos = brc.open_at; backrefpos != brc.close_at;)
								{
									if (!sstate.is_at_srchend())
									{
										const uchar21 uchartxt = utf_traits::codepoint_inc(sstate.nth.in_string, sstate.srchend);
										const uchar21 ucharref = utf_traits::codepoint_inc(backrefpos, brc.close_at);

										if (casehelper_type::canonicalise(uchartxt) == casehelper_type::canonicalise(ucharref))
											continue;
									}
									goto NOT_MATCHED;
								}
							}
							else	//  reverse == true.
							{
								for (BidirectionalIterator backrefpos = brc.close_at; backrefpos != brc.open_at;)
								{
									if (!sstate.is_at_lookbehindlimit())
									{
										const uchar21 uchartxt = utf_traits::dec_codepoint(sstate.nth.in_string, sstate.lblim);
										const uchar21 ucharref = utf_traits::dec_codepoint(backrefpos, brc.open_at);

										if (casehelper_type::canonicalise(uchartxt) == casehelper_type::canonicalise(ucharref))
											continue;
									}
									goto NOT_MATCHED;
								}
							}
						}
					}
				}
				goto MATCHED;

			case st_lookaround_open:
				{
					for (unsigned int i = 1; i < this->number_of_brackets; ++i)
					{
						const submatch_type &sm = sstate.bracket[i];
						sstate.capture_stack.push_back(sm.core);
						sstate.counter_stack.push_back(sm.counter);
					}

					for (unsigned int i = 0; i < this->number_of_counters; ++i)
						sstate.counter_stack.push_back(sstate.counter[i]);

					for (unsigned int i = 0; i < this->number_of_repeats; ++i)
						sstate.repeat_stack.push_back(sstate.repeat[i]);

					const typename ss_type::bottom_state backup_bottom(sstate.btstack_size, sstate.capture_stack.size(), sstate.counter_stack.size(), sstate.repeat_stack.size());

					sstate.bt_stack.push_back(sstate.nth);
					sstate.btstack_size = sstate.bt_stack.size();

#if defined(SRELL_FIXEDWIDTHLOOKBEHIND)

//					if (current_NFA.reverse)
					{
						for (unsigned int i = 0; i < current_NFA.quantifier.atleast; ++i)
						{
							if (!sstate.is_at_lookbehindlimit())
							{
								utf_traits::dec_codepoint(sstate.nth.in_string, sstate.lblim);
								continue;
							}
							is_matched = false;
							goto AFTER_LOOKAROUND;
						}
					}
#endif
					++sstate.nth.in_NFA_states;	//  Moves to lookaround_pop,
					sstate.nth.in_NFA_states = sstate.nth.in_NFA_states->next_state1;
						//  Due to shortage of "next_state"s of lookaround_open!

#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND)
					is_matched = current_NFA.quantifier.atleast == 0 ? run_automaton<icase, false>(sstate /* , true */) : run_automaton<icase, true>(sstate /* , true */);
#else
					is_matched = run_automaton<icase, false>(sstate /* , true */);
#endif

#if defined(SRELL_FIXEDWIDTHLOOKBEHIND)
					AFTER_LOOKAROUND:	//  DO NOT USE current_NFA IN THIS BLOCK.
#endif
					{
						const scstate_type &lookaround_open_pair = sstate.bt_stack[sstate.btstack_size - 1];

						sstate.nth.in_NFA_states = lookaround_open_pair.in_NFA_states;

#if defined(SRELL_ENABLE_GT)
						if (sstate.nth.in_NFA_states->character != meta_char::mc_gt)	//  '>'
#endif
						{
							sstate.nth.in_string = lookaround_open_pair.in_string;
						}
						sstate.bt_stack.resize(sstate.btstack_size);

						sstate.btstack_size = backup_bottom.btstack_size;
						sstate.capture_stack.resize(backup_bottom.capturestack_size);
						sstate.counter_stack.resize(backup_bottom.counterstack_size);
						sstate.repeat_stack.resize(backup_bottom.repeatstack_size);

						is_matched ^= sstate.nth.in_NFA_states->is_not;
//						is_matched ^= (sstate.nth.in_NFA_states->character == meta_char::mc_exclam);
					}
				}
				goto JUDGE;

			case st_lookaround_pop:
				for (unsigned int i = this->number_of_repeats; i;)
				{
					sstate.repeat[--i] = sstate.repeat_stack.back();
					sstate.repeat_stack.pop_back();
				}

				for (unsigned int i = this->number_of_counters; i;)
				{
					sstate.counter[--i] = sstate.counter_stack.back();
					sstate.counter_stack.pop_back();
				}

				for (unsigned int i = this->number_of_brackets; i > 1;)
				{
					submatch_type &sm = sstate.bracket[--i];

					sm.counter = sstate.counter_stack.back();
					sm.core = sstate.capture_stack.back();
					sstate.counter_stack.pop_back();
					sstate.capture_stack.pop_back();
				}
				goto NOT_MATCHED;

			case st_bol:	//  '^':
				if (sstate.is_at_lookbehindlimit() && !sstate.match_prev_avail_flag())
				{
					if (!sstate.match_not_bol_flag())
						goto MATCHED;
				}
					//  !sstate.is_at_lookbehindlimit() || sstate.match_prev_avail_flag()
				else if (this->is_multiline())
				{
					const uchar21 prevchar = utf_traits::prevcodepoint(sstate.nth.in_string, sstate.lblim);

					if (this->character_class.is_included(re_character_class::newline, prevchar))
						goto MATCHED;
				}
				goto NOT_MATCHED;

			case st_eol:	//  '$':
				if (sstate.is_at_srchend())
				{
					if (!sstate.match_not_eol_flag())
						goto MATCHED;
				}
				else if (this->is_multiline())
				{
					const uchar21 nextchar = utf_traits::codepoint(sstate.nth.in_string, sstate.srchend);

					if (this->character_class.is_included(re_character_class::newline, nextchar))
						goto MATCHED;
				}
				goto NOT_MATCHED;

			case st_boundary:	//  '\b' '\B'
				is_matched = current_NFA.is_not;
//				is_matched = current_NFA.character == char_alnum::ch_B;

				//  First, suppose the previous character is not \w but \W.

				if (sstate.is_at_srchend())
				{
					if (sstate.match_not_eow_flag())
						is_matched = !is_matched;
				}
				else if (this->character_class.is_included(current_NFA.number, utf_traits::codepoint(sstate.nth.in_string, sstate.srchend)))
				{
					is_matched = !is_matched;
				}
				//      \W/last     \w
				//  \b  false       true
				//  \B  true        false

				//  Second, if the actual previous character is \w, inverse is_matched.

				if (sstate.is_at_lookbehindlimit() && !sstate.match_prev_avail_flag())
				{
					if (sstate.match_not_bow_flag())
						is_matched = !is_matched;
				}
					//  !sstate.is_at_lookbehindlimit() || sstate.match_prev_avail_flag()
				else if (this->character_class.is_included(current_NFA.number, utf_traits::prevcodepoint(sstate.nth.in_string, sstate.lblim)))
				{
					is_matched = !is_matched;
				}
				//  \b                          \B
				//  pre cur \W/last \w          pre cur \W/last \w
				//  \W/base false   true        \W/base true    false
				//  \w      true    false       \w      false   true

				goto JUDGE;

			case st_success:	//  == lookaround_close.
//				if (is_recursive)
				if (sstate.btstack_size)
					return true;

				if
				(
					(!sstate.match_not_null_flag() || !sstate.is_null())
					&&
					(!sstate.match_match_flag() || sstate.is_at_srchend())
				)
					return true;

				goto NOT_MATCHED;

#if !defined(SRELLDBG_NO_NEXTPOS_OPT)
			case st_move_nextpos:
#if !defined(SRELLDBG_NO_1STCHRCLS) && !defined(SRELLDBG_NO_BITSET)
				sstate.nextpos = sstate.nth.in_string;
				if (!sstate.is_at_srchend())
					++sstate.nextpos;
#else	//  defined(SRELLDBG_NO_1STCHRCLS) || defined(SRELLDBG_NO_BITSET)
				if (sstate.nth.in_string != sstate.bracket[0].core.open_at)
				{
					sstate.nextpos = sstate.nth.in_string;
					if (!sstate.is_at_srchend())
						utf_traits::codepoint_inc(sstate.nextpos, sstate.srchend);
				}
#endif
				goto MATCHED;
#endif

			default:
				throw regex_error(regex_constants::error_internal);

				}
			}
		}
	}
};
//  regex_object

	}	//  namespace regex_internal

//  ... "rei_algorithm.hpp"]
//  ["basic_regex.hpp" ...

//  28.8, class template basic_regex:
template <class charT, class traits = regex_traits<charT> >
class basic_regex : public regex_internal::regex_object<charT, traits>
{
public:

	//  Types:
	typedef charT value_type;
	typedef traits traits_type;
	typedef typename traits::string_type string_type;
	typedef regex_constants::syntax_option_type flag_type;
	typedef typename traits::locale_type locale_type;

	//  28.8.1, constants:
	//  [7.8.1] constants
	static const regex_constants::syntax_option_type icase = regex_constants::icase;
	static const regex_constants::syntax_option_type nosubs = regex_constants::nosubs;
	static const regex_constants::syntax_option_type optimize = regex_constants::optimize;
	static const regex_constants::syntax_option_type collate = regex_constants::collate;
	static const regex_constants::syntax_option_type ECMAScript = regex_constants::ECMAScript;
	static const regex_constants::syntax_option_type basic = regex_constants::basic;
	static const regex_constants::syntax_option_type extended = regex_constants::extended;
	static const regex_constants::syntax_option_type awk = regex_constants::awk;
	static const regex_constants::syntax_option_type grep = regex_constants::grep;
	static const regex_constants::syntax_option_type egrep = regex_constants::egrep;
	static const regex_constants::syntax_option_type multiline = regex_constants::multiline;

	static const regex_constants::syntax_option_type dotall = regex_constants::dotall;

	//  28.8.2, construct/copy/destroy:
	//  [7.8.2] construct/copy/destroy
	basic_regex()
	{
	}

	explicit basic_regex(const charT *const p, const flag_type f = regex_constants::ECMAScript)
	{
		assign(p, p + std::char_traits<charT>::length(p), f);
	}

	basic_regex(const charT *const p, const std::size_t len, const flag_type f = regex_constants::ECMAScript)
	{
		assign(p, p + len, f);
	}

	basic_regex(const basic_regex &e)
	{
		assign(e);
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	basic_regex(basic_regex &&e) SRELL_NOEXCEPT
	{
		assign(std::move(e));
	}
#endif

	template <class ST, class SA>
	explicit basic_regex(const std::basic_string<charT, ST, SA> &p, const flag_type f = regex_constants::ECMAScript)
	{
		assign(p, f);
	}

	template <class ForwardIterator>
	basic_regex(ForwardIterator first, ForwardIterator last, const flag_type f = regex_constants::ECMAScript)
	{
		assign(first, last, f);
	}

#if defined(SRELL_CPP11_INITIALIZER_LIST_ENABLED)
	basic_regex(std::initializer_list<charT> il, const flag_type f = regex_constants::ECMAScript)
	{
		assign(il, f);
	}
#endif

//	~basic_regex();

	basic_regex &operator=(const basic_regex &right)
	{
		return assign(right);
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	basic_regex &operator=(basic_regex &&e) SRELL_NOEXCEPT
	{
		return assign(std::move(e));
	}
#endif

	basic_regex &operator=(const charT *const ptr)
	{
		return assign(ptr);
	}

#if defined(SRELL_CPP11_INITIALIZER_LIST_ENABLED)
	basic_regex &operator=(std::initializer_list<charT> il)
	{
		return assign(il.begin(), il.end());
	}
#endif

	template <class ST, class SA>
	basic_regex &operator=(const std::basic_string<charT, ST, SA> &p)
	{
		return assign(p);
	}

	//  28.8.3, assign:
	//  [7.8.3] assign
	basic_regex &assign(const basic_regex &right)
	{
		regex_internal::re_object_core<charT, traits>::operator=(right);
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	basic_regex &assign(basic_regex &&right) SRELL_NOEXCEPT
	{
		regex_internal::re_object_core<charT, traits>::operator=(std::move(right));
		return *this;
	}
#endif

	basic_regex &assign(const charT *const ptr, const flag_type f = regex_constants::ECMAScript)
	{
		this->compile(ptr, ptr + std::char_traits<charT>::length(ptr), f);
		return *this;
	}

	basic_regex &assign(const charT *const p, std::size_t len, const flag_type f = regex_constants::ECMAScript)
	{
		this->compile(p, p + len, f);
		return *this;
	}

	template <class string_traits, class A>
	basic_regex &assign(const std::basic_string<charT, string_traits, A> &s, const flag_type f = regex_constants::ECMAScript)
	{
		this->compile(s.c_str(), s.c_str() + s.size(), f);
		return *this;
	}

	template <class InputIterator>
	basic_regex &assign(InputIterator first, InputIterator last, const flag_type f = regex_constants::ECMAScript)
	{
		return assign(string_type(first, last), f);
	}

#if defined(SRELL_CPP11_INITIALIZER_LIST_ENABLED)
	basic_regex &assign(std::initializer_list<charT> il, const flag_type f = regex_constants::ECMAScript)
	{
		return assign(il.begin(), il.end(), f);
	}
#endif

	//  28.8.4, const operations:
	//  [7.8.4] const operations
	unsigned mark_count() const
	{
		return this->number_of_brackets - 1;
	}

	flag_type flags() const
	{
		return this->soflags;
	}

	//  28.8.5, locale:
	//  [7.8.5] locale
	locale_type imbue(locale_type loc)
	{
		return this->traits_inst.imbue(loc);
	}

	locale_type getloc() const
	{
		return this->traits_inst.getloc();
	}

	//  28.8.6, swap:
	//  [7.8.6] swap
	void swap(basic_regex &e)
	{
		regex_internal::re_object_core<charT, traits>::swap(e);
	}
};

//  28.8.6, basic_regex swap:
template <class charT, class traits>
void swap(basic_regex<charT, traits> &lhs, basic_regex<charT, traits> &rhs)
{
	lhs.swap(rhs);
}

typedef basic_regex<char> regex;
typedef basic_regex<wchar_t> wregex;

#if defined(WCHAR_MAX)
	#if WCHAR_MAX >= 0x10ffff
		typedef wregex u32wregex;
	#elif WCHAR_MAX >= 0xffff
		typedef basic_regex<wchar_t, u16regex_traits<wchar_t> > u16wregex;
	#endif
#endif

#if defined(SRELL_CPP20_CHAR8_ENABLED)
	typedef basic_regex<char8_t, u8regex_traits<char8_t> > u8regex;
#endif

typedef basic_regex<char, u8regex_traits<char> > u8cregex;
#if !defined(SRELL_CPP20_CHAR8_ENABLED)
	typedef u8cregex u8regex;
#endif

#if defined(SRELL_CPP11_CHAR1632_ENABLED)
	typedef basic_regex<char16_t, u16regex_traits<char16_t> > u16regex;
	typedef basic_regex<char32_t> u32regex;
#endif

//  ... "basic_regex.hpp"]
//  ["regex_iterator.hpp" ...

//  28.12.1, class template regex_iterator:
template <class BidirectionalIterator, class charT = typename std::iterator_traits<BidirectionalIterator>::value_type, class traits = regex_traits<charT> >
class regex_iterator
{
public:

	typedef basic_regex<charT, traits> regex_type;
	typedef match_results<BidirectionalIterator> value_type;
	typedef std::ptrdiff_t difference_type;
	typedef const value_type * pointer;
	typedef const value_type & reference;
	typedef std::forward_iterator_tag iterator_category;

	regex_iterator()
	{
		//  28.12.1.1: Constructs an end-of-sequence iterator.
	}

	regex_iterator(
		const BidirectionalIterator a,
		const BidirectionalIterator b,
		const regex_type &re,
		const regex_constants::match_flag_type m = regex_constants::match_default)
		: begin(a), end(b), pregex(&re), flags(m)
	{
		regex_search(begin, end, begin, match, *pregex, flags);
			//  28.12.1.1: If this call returns false the constructor
			//    sets *this to the end-of-sequence iterator.
	}

	regex_iterator(const regex_iterator &that)
	{
		operator=(that);
	}

	regex_iterator &operator=(const regex_iterator &that)
	{
		if (this != &that)
		{
			this->begin = that.begin;
			this->end = that.end;
			this->pregex = that.pregex;
			this->flags = that.flags;
			this->match = that.match;
		}
		return *this;
	}

	bool operator==(const regex_iterator &right) const
	{
		//  It is probably safe to assume that match.size() == 0 means
		//  end-of-sequence, because it happens only when 1) never tried
		//  regex_search, or 2) regex_search returned false.

		if (this->match.size() == 0 || right.match.size() == 0)
			return this->match.size() == right.match.size();

		return
			this->begin == right.begin
			&&
			this->end == right.end
			&&
			this->pregex == right.pregex
			&&
			this->flags == right.flags
			&&
			this->match[0] == right.match[0];
	}

	bool operator!=(const regex_iterator &right) const
	{
		return !(*this == right);
	}

	const value_type &operator*() const
	{
		return match;
	}

	const value_type *operator->() const
	{
		return &match;
	}

	regex_iterator &operator++()
	{
		if (this->match.size())
		{
			BidirectionalIterator start = match[0].second;

			if (match[0].first == start)	//  The iterator holds a 0-length match.
			{
				if (start == end)
				{
					match.clear_();
					//    28.12.1.4.2: If the iterator holds a zero-length match and
					//  start == end the operator sets *this to the end-ofsequence
					//  iterator and returns *this.
				}
				else
				{
					//    28.12.1.4.3: Otherwise, if the iterator holds a zero-length match
					//  the operator calls regex_search(start, end, match, *pregex, flags
					//  | regex_constants::match_not_null | regex_constants::match_continuous).
					//  If the call returns true the operator returns *this. [Cont...]

					if (!regex_search(start, end, begin, match, *pregex, flags | regex_constants::match_not_null | regex_constants::match_continuous))
					{
						const BidirectionalIterator prevend = start;

						//  [...Cont] Otherwise the operator increments start and continues
						//  as if the most recent match was not a zero-length match.
//						++start;
						utf_traits::codepoint_inc(start, end);

						flags |= regex_constants::match_prev_avail;

						if (regex_search(start, end, begin, match, *pregex, flags))
						{
							//    28.12.1.4.5-6: In all cases in which the call to regex_search
							//  returns true, match.prefix().first shall be equal to the previous
							//  value of match[0].second, ... match[i].position() shall return
							//  distance(begin, match[i].first).
							//    This means that match[i].position() gives the offset from the
							//  beginning of the target sequence, which is often not the same as
							//  the offset from the sequence passed in the call to regex_search.
							//
							//  To satisfy this:
							match.set_prefix_first_(prevend);
						}
					}
				}
			}
			else
			{
				//    28.12.1.4.4: If the most recent match was not a zero-length match,
				//  the operator sets flags to flags | regex_constants::match_prev_avail
				//  and calls regex_search(start, end, match, *pregex, flags). [Cont...]
				flags |= regex_constants::match_prev_avail;

				regex_search(start, end, begin, match, *pregex, flags);
				//  [...Cont] If the call returns false the iterator sets *this to
				//  the end-of-sequence iterator. The iterator then returns *this.
				//
				//    28.12.1.4.5-6: In all cases in which the call to regex_search
				//  returns true, match.prefix().first shall be equal to the previous
				//  value of match[0].second, ... match[i].position() shall return
				//  distance(begin, match[i].first).
				//    This means that match[i].position() gives the offset from the
				//  beginning of the target sequence, which is often not the same as
				//  the offset from the sequence passed in the call to regex_search.
				//
				//  These should already be done in regex_search.
			}
		}
		return *this;
	}

	regex_iterator operator++(int)
	{
		const regex_iterator tmp = *this;
		++(*this);
		return tmp;
	}

private:

	BidirectionalIterator                begin;
	BidirectionalIterator                end;
	const regex_type                    *pregex;
	regex_constants::match_flag_type     flags;
	match_results<BidirectionalIterator> match;

	typedef typename traits::utf_traits utf_traits;
};

typedef regex_iterator<const char *> cregex_iterator;
typedef regex_iterator<const wchar_t *> wcregex_iterator;
typedef regex_iterator<std::string::const_iterator> sregex_iterator;
typedef regex_iterator<std::wstring::const_iterator> wsregex_iterator;

#if defined(SRELL_CPP11_CHAR1632_ENABLED)
	typedef regex_iterator<const char16_t *> u16cregex_iterator;
	typedef regex_iterator<const char32_t *> u32cregex_iterator;
	typedef regex_iterator<std::u16string::const_iterator> u16sregex_iterator;
	typedef regex_iterator<std::u32string::const_iterator> u32sregex_iterator;
#endif

#if defined(SRELL_CPP20_CHAR8_ENABLED)
	typedef regex_iterator<const char8_t *> u8cregex_iterator;
#endif
#if defined(SRELL_CPP20_CHAR8_ENABLED) && SRELL_CPP20_CHAR8_ENABLED >= 2
	typedef regex_iterator<std::u8string::const_iterator> u8sregex_iterator;
#endif

typedef regex_iterator<const char *, std::iterator_traits<const char *>::value_type, u8regex_traits<std::iterator_traits<const char *>::value_type> > u8ccregex_iterator;
typedef regex_iterator<std::string::const_iterator, std::iterator_traits<std::string::const_iterator>::value_type, u8regex_traits<std::iterator_traits<std::string::const_iterator>::value_type> > u8csregex_iterator;
#if !defined(SRELL_CPP20_CHAR8_ENABLED)
	typedef u8ccregex_iterator u8cregex_iterator;
#endif
#if !defined(SRELL_CPP20_CHAR8_ENABLED) || SRELL_CPP20_CHAR8_ENABLED < 2
	typedef u8csregex_iterator u8sregex_iterator;
#endif

#if defined(WCHAR_MAX)
	#if WCHAR_MAX >= 0x10ffff
		typedef wcregex_iterator u32wcregex_iterator;
		typedef wsregex_iterator u32wsregex_iterator;
	#elif WCHAR_MAX >= 0xffff
		typedef regex_iterator<const wchar_t *, std::iterator_traits<const wchar_t *>::value_type, u16regex_traits<std::iterator_traits<const wchar_t *>::value_type> > u16wcregex_iterator;
		typedef regex_iterator<std::wstring::const_iterator, std::iterator_traits<std::wstring::const_iterator>::value_type, u16regex_traits<std::iterator_traits<std::wstring::const_iterator>::value_type> > u16wsregex_iterator;
	#endif
#endif

//  ... "regex_iterator.hpp"]
//  ["regex_algorithm.hpp" ...

//  28.11.2, function template regex_match:
//  [7.11.2] Function template regex_match
template <class BidirectionalIterator, class Allocator, class charT, class traits>
bool regex_match(
	const BidirectionalIterator first,
	const BidirectionalIterator last,
	match_results<BidirectionalIterator, Allocator> &m,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return e.search(first, last, first, m, flags | regex_constants::match_continuous | regex_constants::match_match_);
}

template <class BidirectionalIterator, class charT, class traits>
bool regex_match(
	const BidirectionalIterator first,
	const BidirectionalIterator last,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
//  4 Effects: Behaves "as if" by constructing an instance of
//  match_results<BidirectionalIterator> what, and then returning the
//  result of regex_match(first, last, what, e, flags).

	match_results<BidirectionalIterator> what;

	return regex_match(first, last, what, e, flags);
}

template <class charT, class Allocator, class traits>
bool regex_match(
	const charT *const str,
	match_results<const charT *, Allocator> &m,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return regex_match(str, str + std::char_traits<charT>::length(str), m, e, flags);
}

template <class ST, class SA, class Allocator, class charT, class traits>
bool regex_match(
	const std::basic_string<charT, ST, SA> &s,
	match_results<typename std::basic_string<charT, ST, SA>::const_iterator, Allocator> &m,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return regex_match(s.begin(), s.end(), m, e, flags);
}

template <class charT, class traits>
bool regex_match(
	const charT *const str,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return regex_match(str, str + std::char_traits<charT>::length(str), e, flags);
}

template <class ST, class SA, class charT, class traits>
bool regex_match(
	const std::basic_string<charT, ST, SA> &s,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return regex_match(s.begin(), s.end(), e, flags);
}

template <class BidirectionalIterator, class Allocator, class charT, class traits>
bool regex_search(
	const BidirectionalIterator first,
	const BidirectionalIterator last,
	const BidirectionalIterator lookbehind_limit,
	match_results<BidirectionalIterator, Allocator> &m,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return e.search(first, last, lookbehind_limit, m, flags);
}

template <class BidirectionalIterator, class charT, class traits>
bool regex_search(
	const BidirectionalIterator first,
	const BidirectionalIterator last,
	const BidirectionalIterator lookbehind_limit,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
//  6 Effects: Behaves "as if" by constructing an object what of type
//  match_results<iterator> and then returning the result of
//  regex_search(first, last, what, e, flags).

	match_results<BidirectionalIterator> what;
	return regex_search(first, last, lookbehind_limit, what, e, flags);
}

//  28.11.3, function template regex_search:
//  7.11.3 regex_search [tr.re.alg.search]
template <class BidirectionalIterator, class Allocator, class charT, class traits>
bool regex_search(
	const BidirectionalIterator first,
	const BidirectionalIterator last,
	match_results<BidirectionalIterator, Allocator> &m,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return e.search(first, last, first, m, flags);
}

template <class BidirectionalIterator, class charT, class traits>
bool regex_search(
	const BidirectionalIterator first,
	const BidirectionalIterator last,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
//  6 Effects: Behaves "as if" by constructing an object what of type
//  match_results<iterator> and then returning the result of
//  regex_search(first, last, what, e, flags).

	match_results<BidirectionalIterator> what;
	return regex_search(first, last, what, e, flags);
}

template <class charT, class Allocator, class traits>
bool regex_search(
	const charT *const str,
	match_results<const charT *, Allocator> &m,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return regex_search(str, str + std::char_traits<charT>::length(str), m, e, flags);
}

template <class charT, class traits>
bool regex_search(
	const charT *const str,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return regex_search(str, str + std::char_traits<charT>::length(str), e, flags);
}

template <class ST, class SA, class charT, class traits>
bool regex_search(
	const std::basic_string<charT, ST, SA> &s,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return regex_search(s.begin(), s.end(), e, flags);
}

template <class ST, class SA, class Allocator, class charT, class traits>
bool regex_search(
	const std::basic_string<charT, ST, SA> &s,
	match_results<typename std::basic_string<charT, ST, SA>::const_iterator, Allocator> &m,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return regex_search(s.begin(), s.end(), m, e, flags);
}

//  28.11.4, function template regex_replace:
//  [7.11.4] Function template regex_replace
template <class OutputIterator, class BidirectionalIterator, class traits, class charT, class ST, class SA>
OutputIterator regex_replace(
	OutputIterator out,
	const BidirectionalIterator first,
	const BidirectionalIterator last,
	const basic_regex<charT, traits> &e,
	const std::basic_string<charT, ST, SA> &fmt,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	typedef regex_iterator<BidirectionalIterator, charT, traits> iterator_type;

	const bool do_copy = !(flags & regex_constants::format_no_copy);
	const iterator_type eos;
	iterator_type i(first, last, e, flags);
	typename iterator_type::value_type::value_type last_m_suffix;

	last_m_suffix.first = first;
	last_m_suffix.second = last;

	for (; i != eos; ++i)
	{
		if (do_copy)
			out = std::copy(i->prefix().first, i->prefix().second, out);

		out = i->format(out, fmt, flags);
		last_m_suffix = i->suffix();

		if (flags & regex_constants::format_first_only)
			break;
	}

	if (do_copy)
		out = std::copy(last_m_suffix.first, last_m_suffix.second, out);

	return out;
}

template <class OutputIterator, class BidirectionalIterator, class traits, class charT>
OutputIterator regex_replace(
	OutputIterator out,
	const BidirectionalIterator first,
	const BidirectionalIterator last,
	const basic_regex<charT, traits> &e,
	const charT *const fmt,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	//  Strictly speaking, this should be implemented as a version different
	//  from the above with changing the line i->format(out, fmt, flags) to
	//  i->format(out, fmt, fmt + char_traits<charT>::length(fmt), flags).

	const std::basic_string<charT> fs(fmt, fmt + std::char_traits<charT>::length(fmt));

	return regex_replace(out, first, last, e, fs, flags);
}

template <class traits, class charT, class ST, class SA, class FST, class FSA>
std::basic_string<charT, ST, SA> regex_replace(
	const std::basic_string<charT, ST, SA> &s,
	const basic_regex<charT, traits> &e,
	const std::basic_string<charT, FST, FSA> &fmt,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	std::basic_string<charT, ST, SA> result;

	regex_replace(std::back_inserter(result), s.begin(), s.end(), e, fmt, flags);
	return result;
}

template <class traits, class charT, class ST, class SA>
std::basic_string<charT, ST, SA> regex_replace(
	const std::basic_string<charT, ST, SA> &s,
	const basic_regex<charT, traits> &e,
	const charT *const fmt,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	std::basic_string<charT, ST, SA> result;

	regex_replace(std::back_inserter(result), s.begin(), s.end(), e, fmt, flags);
	return result;
}

template <class traits, class charT, class ST, class SA>
std::basic_string<charT> regex_replace(
	const charT *const s,
	const basic_regex<charT, traits> &e,
	const std::basic_string<charT, ST, SA> &fmt,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	std::basic_string<charT> result;

	regex_replace(std::back_inserter(result), s, s + std::char_traits<charT>::length(s), e, fmt, flags);
	return result;
}

template <class traits, class charT>
std::basic_string<charT> regex_replace(
	const charT *const s,
	const basic_regex<charT, traits> &e,
	const charT *const fmt,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	std::basic_string<charT> result;

	regex_replace(std::back_inserter(result), s, s + std::char_traits<charT>::length(s), e, fmt, flags);
	return result;
}

//  ... "regex_algorithm.hpp"]
//  ["regex_token_iterator.hpp" ...

//  28.12.2, class template regex_token_iterator:
template <class BidirectionalIterator, class charT = typename std::iterator_traits<BidirectionalIterator>::value_type, class traits = regex_traits<charT> >
class regex_token_iterator
{
public:

	typedef basic_regex<charT, traits> regex_type;
	typedef sub_match<BidirectionalIterator> value_type;
	typedef std::ptrdiff_t difference_type;
	typedef const value_type * pointer;
	typedef const value_type & reference;
	typedef std::forward_iterator_tag iterator_category;

	regex_token_iterator() : result(NULL)
	{
		//  Constructs the end-of-sequence iterator.
	}

	regex_token_iterator(
		const BidirectionalIterator a,
		const BidirectionalIterator b,
		const regex_type &re,
		int submatch = 0,
		regex_constants::match_flag_type m = regex_constants::match_default
	) : position(a, b, re, m), result(NULL), subs(1, submatch)
	{
		post_constructor(a, b);
	}

	regex_token_iterator(
		const BidirectionalIterator a,
		const BidirectionalIterator b,
		const regex_type &re,
		const std::vector<int> &submatches,
		regex_constants::match_flag_type m = regex_constants::match_default
	) : position(a, b, re, m), result(NULL), subs(submatches)
	{
		post_constructor(a, b);
	}

#if defined(SRELL_CPP11_INITIALIZER_LIST_ENABLED)
	regex_token_iterator(
		const BidirectionalIterator a,
		const BidirectionalIterator b,
		const regex_type &re,
		std::initializer_list<int> submatches,
		regex_constants::match_flag_type m = regex_constants::match_default
	) : position(a, b, re, m), result(NULL), subs(submatches)
	{
		post_constructor(a, b);
	}
#endif

	template <std::size_t N>	//  Was R in TR1.
	regex_token_iterator(
		const BidirectionalIterator a,
		const BidirectionalIterator b,
		const regex_type &re,
		const int (&submatches)[N],
		regex_constants::match_flag_type m = regex_constants::match_default
	) : position(a, b, re, m), result(NULL), subs(submatches, submatches + N)
	{
		post_constructor(a, b);
	}

	regex_token_iterator(const regex_token_iterator &that)
	{
		operator=(that);
	}

	regex_token_iterator &operator=(const regex_token_iterator &that)
	{
		if (this != &that)
		{
			this->position = that.position;
			this->result = that.result;
			this->suffix = that.suffix;
			this->N = that.N;
			this->subs = that.subs;
		}
		return *this;
	}

	bool operator==(const regex_token_iterator &right)
	{
		if (this->result == NULL || right.result == NULL)
			return this->result == right.result;

		if (this->result == &this->suffix || right.result == &right.suffix)
			return this->suffix == right.suffix;

		return
			this->position == right.position
			&&
			this->N == right.N
			&&
			this->subs == right.subs;
	}

	bool operator!=(const regex_token_iterator &right)
	{
		return !(*this == right);
	}

	const value_type &operator*()
	{
		return *result;
	}

	const value_type *operator->()
	{
		return result;
	}

	regex_token_iterator &operator++()
	{
		position_iterator prev(position);
		position_iterator eos_iterator;

		if (result != NULL)
			//  To avoid inifinite loop. The specification does not require, though.
		{
			if (result == &suffix)
			{
				result = NULL;	//  end-of-sequence.
			}
			else
			{
				++this->N;
				for (;;)
				{
					if (this->N < subs.size())
					{
						result = subs[this->N] != -1 ? &((*position)[subs[this->N]]) : &((*position).prefix());
						break;
					}

					this->N = 0;
					++position;

					if (position == eos_iterator)
					{
						if (this->N < subs.size() && prev->suffix().length() && minus1_in_subs())
						{
							suffix = prev->suffix();
							result = &suffix;
						}
						else
						{
							result = NULL;
						}
						break;
					}
				}
			}
		}
		return *this;
	}

	regex_token_iterator operator++(int)
	{
		const regex_token_iterator tmp(*this);
		++(*this);
		return tmp;
	}

private:

	void post_constructor(const BidirectionalIterator a, const BidirectionalIterator b)
	{
		position_iterator eos_iterator;

		this->N = 0;

		if (position != eos_iterator && subs.size())
		{
			result = subs[this->N] != -1 ? &((*position)[subs[this->N]]) : &((*position).prefix());
		}
		else if (minus1_in_subs())	//  end-of-sequence.
		{
			suffix.first   = a;
			suffix.second  = b;
			suffix.matched = a != b;
			//  28.1.2.7: In a suffix iterator the member result holds a pointer
			//  to the data member suffix, the value of the member suffix.match is true,

			if (suffix.matched)
				result = &suffix;
			else
				result = NULL;	//  Means end-of-sequence.
		}
	}

	bool minus1_in_subs() const
	{
		for (std::size_t i = 0; i < subs.size(); ++i)
			if (subs[i] == -1)
				return true;

		return false;
	}

private:

	typedef regex_iterator<BidirectionalIterator, charT, traits> position_iterator;
	position_iterator position;
	const value_type *result;
	value_type suffix;
	std::size_t N;
	std::vector<int> subs;
};

typedef regex_token_iterator<const char *> cregex_token_iterator;
typedef regex_token_iterator<const wchar_t *> wcregex_token_iterator;
typedef regex_token_iterator<std::string::const_iterator> sregex_token_iterator;
typedef regex_token_iterator<std::wstring::const_iterator> wsregex_token_iterator;

#if defined(SRELL_CPP11_CHAR1632_ENABLED)
	typedef regex_token_iterator<const char16_t *> u16cregex_token_iterator;
	typedef regex_token_iterator<const char32_t *> u32cregex_token_iterator;
	typedef regex_token_iterator<std::u16string::const_iterator> u16sregex_token_iterator;
	typedef regex_token_iterator<std::u32string::const_iterator> u32sregex_token_iterator;
#endif

#if defined(SRELL_CPP20_CHAR8_ENABLED)
	typedef regex_token_iterator<const char8_t *> u8cregex_token_iterator;
#endif
#if defined(SRELL_CPP20_CHAR8_ENABLED) && SRELL_CPP20_CHAR8_ENABLED >= 2
	typedef regex_token_iterator<std::u8string::const_iterator> u8sregex_token_iterator;
#endif

typedef regex_token_iterator<const char *, std::iterator_traits<const char *>::value_type, u8regex_traits<std::iterator_traits<const char *>::value_type> > u8ccregex_token_iterator;
typedef regex_token_iterator<std::string::const_iterator, std::iterator_traits<std::string::const_iterator>::value_type, u8regex_traits<std::iterator_traits<std::string::const_iterator>::value_type> > u8csregex_token_iterator;
#if !defined(SRELL_CPP20_CHAR8_ENABLED)
	typedef u8ccregex_token_iterator u8cregex_token_iterator;
#endif
#if !defined(SRELL_CPP20_CHAR8_ENABLED) || SRELL_CPP20_CHAR8_ENABLED < 2
	typedef u8csregex_token_iterator u8sregex_token_iterator;
#endif

#if defined(WCHAR_MAX)
	#if WCHAR_MAX >= 0x10ffff
		typedef wcregex_token_iterator u32wcregex_token_iterator;
		typedef wsregex_token_iterator u32wsregex_token_iterator;
	#elif WCHAR_MAX >= 0xffff
		typedef regex_token_iterator<const wchar_t *, std::iterator_traits<const wchar_t *>::value_type, u16regex_traits<std::iterator_traits<const wchar_t *>::value_type> > u16wcregex_token_iterator;
		typedef regex_token_iterator<std::wstring::const_iterator, std::iterator_traits<std::wstring::const_iterator>::value_type, u16regex_traits<std::iterator_traits<std::wstring::const_iterator>::value_type> > u16wsregex_token_iterator;
	#endif
#endif

//  ... "regex_token_iterator.hpp"]

}		//  namespace srell

#ifdef SRELL_NOEXCEPT
#undef SRELL_NOEXCEPT
#endif

#ifdef SRELL_CPP20_CHAR8_ENABLED
#undef SRELL_CPP20_CHAR8_ENABLED
#endif

#ifdef SRELL_CPP11_CHAR1632_ENABLED
#undef SRELL_CPP11_CHAR1632_ENABLED
#endif

#ifdef SRELL_CPP11_INITIALIZER_LIST_ENABLED
#undef SRELL_CPP11_INITIALIZER_LIST_ENABLED
#endif

#ifdef SRELL_CPP11_MOVE_ENABLED
#undef SRELL_CPP11_MOVE_ENABLED
#endif

#endif	//  SRELL_REGEX_TEMPLATE_LIBRARY
