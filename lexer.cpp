#include <cctype>
#include <cstddef>
#include <iostream>
#include <iomanip>
#include "lexer.hpp"
#include "constants.hpp"


namespace Sass {
  using namespace Constants;

  namespace Prelexer {

    //####################################
    // implement some function that do exist in the standard
    // but those are locale aware which brought some trouble
    // this even seems to improve performance by quite a bit
    //####################################

    const bool is_alpha(const char& chr)
    {
      return unsigned(chr - 'A') <= 'Z' - 'A' ||
             unsigned(chr - 'a') <= 'z' - 'a';
    }

    const bool is_space(const char& chr)
    {
      // adapted the technique from is_alpha
      return chr == ' ' || unsigned(chr - '\t') <= '\r' - '\t';
    }

    const bool is_digit(const char& chr)
    {
      // adapted the technique from is_alpha
      return unsigned(chr - '0') <= '9' - '0';
    }

    const bool is_xdigit(const char& chr)
    {
      // adapted the technique from is_alpha
      return unsigned(chr - '0') <= '9' - '0' ||
             unsigned(chr - 'a') <= 'f' - 'a' ||
             unsigned(chr - 'A') <= 'F' - 'A';
    }

    const bool is_punct(const char& chr)
    {
      // locale independent
      return chr == '.';
    }

    const bool is_alnum(const char& chr)
    {
      return is_alpha(chr) || is_digit(chr);
    }

    // check if char is outside ascii range
    const bool is_unicode(const char& chr)
    {
      // check for unicode range
      return unsigned(chr) > 127;
    }

    // Match word character (look ahead)
    const bool is_character(const char& chr)
    {
      // valid alpha, numeric or unicode char (plus hyphen)
      return is_alnum(chr) || is_unicode(chr) || chr == '-';
    }

    //####################################
    // BASIC CHARACTER MATCHERS
    //####################################

    // create matchers that advance the position
    const char* space(const char* src) { return is_space(*src) ? src + 1 : 0; }
    const char* alpha(const char* src) { return is_alpha(*src) ? src + 1 : 0; }
    const char* unicode(const char* src) { return is_unicode(*src) ? src + 1 : 0; }
    const char* digit(const char* src) { return is_digit(*src) ? src + 1 : 0; }
    const char* xdigit(const char* src) { return is_xdigit(*src) ? src + 1 : 0; }
    const char* alnum(const char* src) { return is_alnum(*src) ? src + 1 : 0; }
    const char* punct(const char* src) { return is_punct(*src) ? src + 1 : 0; }
    const char* character(const char* src) { return is_character(*src) ? src + 1 : 0; }

    // Match multiple ctype characters.
    const char* spaces(const char* src) { return one_plus<space>(src); }
    const char* digits(const char* src) { return one_plus<digit>(src); }

    // Whitespace handling.
    const char* no_spaces(const char* src) { return negate< space >(src); }
    const char* optional_spaces(const char* src) { return zero_plus< space >(src); }

    // Match any single character.
    const char* any_char(const char* src) { return *src ? src + 1 : src; }

    // Match word boundary (zero-width lookahead).
    const char* word_boundary(const char* src) { return is_character(*src) ? 0 : src; }

    // Match linefeed /(?:\n|\r\n?)/
    const char* re_linebreak(const char* src)
    {
      // end of file or unix linefeed return here
      if (*src == 0 || *src == '\n') return src + 1;
      // a carriage return may optionally be followed by a linefeed
      if (*src == '\r') return *(src + 1) == '\n' ? src + 2 : src + 1;
      // no linefeed
      return 0;
    }

    // Assert string boundaries (/\Z|\z|\A/)
    // This is a zero-width positive lookahead
    const char* end_of_line(const char* src)
    {
      // end of file or unix linefeed return here
      return *src == 0 || *src == '\n' || *src == '\r' ? src : 0;
    }

  }
}
