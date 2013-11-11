// Copyright (c) 2005 - 2006, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: Sanjay Ghemawat
//

// Heavily refactored for Symbian OS by Accenture.

#ifndef CREGEX_H_
#define CREGXX_H_

#include <e32base.h>
#include "tregexoptions.h"

// Forward declarations
class TRegExArg;
struct real_pcre;
typedef struct real_pcre pcre;
struct pcre_extra;

// Errors

/** Base offset for CRegEx Errors */
const TInt KErrRegExBase 									= -20000;

/** Base offset for RegEx pattern compile errors */
const TInt KErrRegExCompileBase 							= KErrRegExBase;

/** \ at end of pattern */
const TInt KErrRegExCmpBackslashAtEOP 						= KErrRegExCompileBase - 1;
/** \c at end of pattern */
const TInt KErrRegExCmpBackslashCAtEOP 						= KErrRegExCompileBase - 2;
/** unrecognized character follows \ */
const TInt KErrRegExCmpUnrecCharAftBackslash				= KErrRegExCompileBase - 3;
/** numbers out of order in {} quantifier */
const TInt KErrRegExCmpNumsOutOfOrderInBraceQuantifier		= KErrRegExCompileBase - 4;
/** number too big in {} quantifier */
const TInt KErrRegExCmpNumTooBigInBraceQuantifier			= KErrRegExCompileBase - 5;
/** missing terminating ] for character class */
const TInt KErrRegExCmpMissingTermBracketInCharClass		= KErrRegExCompileBase - 6;
/** invalid escape sequence in character class */
const TInt KErrRegExCmpInvalidEscapeSeqInCharClass			= KErrRegExCompileBase - 7;
/** range out of order in character class */
const TInt KErrRegExCmpRangeOutOfOrderInCharClass			= KErrRegExCompileBase - 8;
/** nothing to repeat */
const TInt KErrRegExCmpNothingToRepeat						= KErrRegExCompileBase - 9;
/** operand of unlimited repeat could match the empty string - no longer used */
const TInt KErrRegExCmpUnused01								= KErrRegExCompileBase - 10;
/** internal error: unexpected repeat */
const TInt KErrRegExCmpUnexpectedRepeat						= KErrRegExCompileBase - 11;
/** unrecognized character after (? or (?-\0t */
const TInt KErrRegExCmpUnexpectedCharAftParenthQuest		= KErrRegExCompileBase - 12;
/** POSIX named classes are supported only within a class */
const TInt KErrRegExCmpPosixNamedSupportedWithinClass		= KErrRegExCompileBase - 13;
/** missing ) */
const TInt KErrRegExCmpMissingCloseParenth					= KErrRegExCompileBase - 14;
/** reference to non-existent subpattern */
const TInt KErrRegExCmpRefNonExistSubpattern				= KErrRegExCompileBase - 15;
/** internal error: erroffset passed as NULL */
const TInt KErrRegExCmpErrOffsetNull						= KErrRegExCompileBase - 16;
/** unknown option bit(s) set */
const TInt KErrRegExCmpUnknownOptionBitsSet					= KErrRegExCompileBase - 17;
/** missing ) after comment */
const TInt KErrRegExCmpMissingCloseParenthAftComment		= KErrRegExCompileBase - 18;
/** parentheses nested too deeply - no longer used */
const TInt KErrRegExCmpUnused02								= KErrRegExCompileBase - 19;
/** regular expression is too large */
const TInt KErrRegExCmpExprTooLarge							= KErrRegExCompileBase - 20;
/** failed to get memory */
const TInt KErrRegExCmpFailedGetMemory						= KErrRegExCompileBase - 21;
/** unmatched parentheses */
const TInt KErrRegExCmpUnmatchedParenth						= KErrRegExCompileBase - 22;
/** internal error: code overflow */
const TInt KErrRegExCmpCodeOverflow							= KErrRegExCompileBase - 23;
/** unrecognized character after (?< */
const TInt KErrRegExCmpUnRecogCharAftParenthQuestAngle		= KErrRegExCompileBase - 24;
/** lookbehind assertion is not fixed length*/
const TInt KErrRegExCmpLookbehindAssertNotFixedLen			= KErrRegExCompileBase - 25;
/** malformed number or name after (?( */
const TInt KErrRegExCmpMalformedAftParenthQuestParenth		= KErrRegExCompileBase - 26;
/** conditional group contains more than two branches */
const TInt KErrRegExCmpCondGroupMoreThanTwoBranches			= KErrRegExCompileBase - 27;
/** assertion expected after (?(\0 */
const TInt KErrRegExCmpAssertExpAftParenthQuestParent		= KErrRegExCompileBase - 28;
/** (?R or (?[+-]digits must be followed by ) */
const TInt KErrRegExCmpMustFollowedByCloseParenth			= KErrRegExCompileBase - 29;
/** unknown POSIX class name */
const TInt KErrRegExCmpUnknownPosixClassName				= KErrRegExCompileBase - 30;
/** POSIX collating elements are not supported */
const TInt KErrRegExCmpPosixCollElemsNotSupported			= KErrRegExCompileBase - 31;
/** this version of PCRE is not compiled with PCRE_UTF8 support */
const TInt KErrRegExCmpNotCompiledWithUtf8Support			= KErrRegExCompileBase - 32;
/** spare error - no longer user  */
const TInt KErrRegExCmpUnused03								= KErrRegExCompileBase - 33;
/** character value in \x{...} sequence is too large */
const TInt KErrRegExCmpCharValueInBackslashXSeqTooLarge		= KErrRegExCompileBase - 34;
/** invalid condition (?(0) */
const TInt KErrRegExCmpInvalidCondition						= KErrRegExCompileBase - 35;
/** \C not allowed in lookbehind assertion */
const TInt KErrRegExCmpBackslashCNotAllowedinLookbehind		= KErrRegExCompileBase - 36;
/** PCRE does not support \L, \l, \N, \U, or \u\0 */
const TInt KErrRegExCmpLNUEscapeSeqNotSupported				= KErrRegExCompileBase - 37;
/** number after (?C is > 255 */
const TInt KErrRegExCmpNumAftParenthQuestCIsGreatherThan	= KErrRegExCompileBase - 38;
/** closing ) for (?C expected\ */
const TInt KErrRegExCmpCloseParenthAftParenthQuestCExp		= KErrRegExCompileBase - 39;
/** recursive call could loop indefinitely */
const TInt KErrRegExCmpRecuriveCallLoopIndef				= KErrRegExCompileBase - 40;
/** unrecognized character after (?P */
const TInt KErrRegExCmpUnrecCharaftParenthQuestP			= KErrRegExCompileBase - 41;
/** syntax error in subpattern name (missing terminator) */
const TInt KErrRegExCmpSyntaxInSubpatternName				= KErrRegExCompileBase - 42;
/** two named subpatterns have the same name */
const TInt KErrRegExCmpTwoSubpatternsHaveSameName			= KErrRegExCompileBase - 43;
/** invalid UTF-8 string */
const TInt KErrRegExCmpInvalidUtf8String					= KErrRegExCompileBase - 44;
/** support for \P, \p, and \X has not been compiled */
const TInt KErrRegExCmpSupportForEscapeSeqNotCompiled		= KErrRegExCompileBase - 45;
/** malformed \P or \p sequence */
const TInt KErrRegExCmpMalformedBackslashPSeq				= KErrRegExCompileBase - 46;
/** unknown property name after \P or \p */
const TInt KErrRegExCmpUnknownPropNameAftBackslashPSeq		= KErrRegExCompileBase - 47;
/** subpattern name is too long. Default max = 32 chars. See MAX_NAME_SIZE */
const TInt KErrRegExCmpSubpatternNameTooLong				= KErrRegExCompileBase - 48;
/** subpattern name is too long. Default max = 10000. See MAX_NAME_COUNT */
const TInt KErrRegExCmpTooManyNamesSubpatterns				= KErrRegExCompileBase - 49;
/** repeated subpattern is too long - no longer used */
const TInt KErrRegExCmpUnused04								= KErrRegExCompileBase - 50;
/** octal value is greater than \\377 (not in UTF-8 mode) */
const TInt KErrRegExCmpOctalValueGreatherThan377			= KErrRegExCompileBase - 51;
/** internal error: overran compiling workspace */
const TInt KErrRegExCmpOverranCompilingSpace				= KErrRegExCompileBase - 52;
/** internal error: previously-checked referenced subpattern not found */
const TInt KErrRegExCmpCheckedSubpatternNotFound			= KErrRegExCompileBase - 53;
/** DEFINE group contains more than one branch */
const TInt KErrRegExCmpDefineGroupMoreThanOneBranch			= KErrRegExCompileBase - 54;
/** repeating a DEFINE group is not allowed */
const TInt KErrRegExCmpRepeatingDefineGroupNotAllowed		= KErrRegExCompileBase - 55;
/** inconsistent NEWLINE options */
const TInt KErrRegExCmpInconsistantNewlineOpts				= KErrRegExCompileBase - 56;
/** \g is not followed by a braced, angle-bracketed, or quoted name/number or by a plain number */
const TInt KErrRegExCmpBackslashGNotFollowed				= KErrRegExCompileBase - 57;
/** a numbered reference must not be zero */
const TInt KErrRegExCmpNumberedRefIsZero					= KErrRegExCompileBase - 58;
/** (*VERB) with an argument is not supported */
const TInt KErrRegExCmpAsteriskVerbWithArgNotSupported		= KErrRegExCompileBase - 59;
/** (*VERB) not recognized */
const TInt KErrRegExCmpAsteriskVerbNotRecog					= KErrRegExCompileBase - 60;
/** number is too big */
const TInt KErrRegExCmpNumTooBig							= KErrRegExCompileBase - 61;
/** subpattern name expected */
const TInt KErrRegExCmpSubpatternNameExp					= KErrRegExCompileBase - 62;
/** digit expected after (?+ */
const TInt KErrRegExCmpDigitExpAftParenthQuestPlus			= KErrRegExCompileBase - 63;
/** ] is an invalid data character in JavaScript compatibility mode */
const TInt KErrRegExCloseBracketInvalidInJSCompatMode		= KErrRegExCompileBase - 64;

/** Base offset for RegEx pattern general errors */
const TInt KErrRegExGeneralBase 							= KErrRegExCompileBase - 512;

/** Zero Matches */
const TInt KErrRegExZeroMatches								= KErrRegExGeneralBase - 1;

/** Regular expression has fewer capturing groups than number of args passed in */
const TInt KErrRegExFewerCaptureGroupsThanArgs				= KErrRegExGeneralBase - 2;

/** Failed to parse argument, e.g. the supplied argument does not have enough capacity */
const TInt KErrRegExFailedToParseArg						= KErrRegExGeneralBase - 3;

/** The modifiable descriptor supplied to recieve output does not have a large enough maximum length */
const TInt KErrRegExOutputTooBig							= KErrRegExGeneralBase - 4;

/** Invalid rewrite pattern */
const TInt KErrRegExInvalidRewritePattern					= KErrRegExGeneralBase - 5;

/** Error with one of the backslash substitutions in the supplied rewrite string */
const TInt KErrRegExBadBackslashSubsitution					= KErrRegExGeneralBase - 6;

// Panic Codes
_LIT(KRegExPanic, "CRegEx");

/**
 * Internal CRegEx panics (Debug only)
 */
enum TRegExPanic
	{
	EInvalidMatchResults,
	EUnexpectedRetValFromPcre,
	EVectorTooSmall,
	EInvalidNumArgs
	};

/*
 * Newlines are indicated by a single LF character.
 * @see KNewLineCr
 * @see KNewLineCrLf
 * @see KNewLineAnyCrLf
 * @see KNewLineAny 
 */
static const TInt KNewLineLf = 10;
/*
 * Newlines are indicated by a single CR character.
 * @see KNewLineLf
 * @see KNewLineCrLf
 * @see KNewLineAnyCrLf
 * @see KNewLineAny 
 */
static const TInt KNewLineCr = 13;

/* 
 * Newlines are indicated by the two-character CRLF sequence.
 * @see KNewLineCr
 * @see KNewLineLf 
 * @see KNewLineAnyCrLf
 * @see KNewLineAny
 */
static const TInt KNewLineCrLf = 3338;

/*
 * Newlines are indicated by any of the following:
 * - A single CR character.
 * - A single LF character.
 * - The two-character CRLF sequence. 
 * @see KNewLineLf
 * @see KNewLineCr
 * @see KNewLineCrLf 
 * @see KNewLineAny
 */
static const TInt KNewLineAnyCrLf = -2;

/*
 * Newlines are indicated by any Unicode sequence:
 * - A single CR character.
 * - A single LF character.
 * - The two-character CRLF sequence. 
 * - A single VT character (vertical tab, U+000B).
 * - A single FF character (formfeed, U+000C).
 * - A single NEL character (next line, U+0085).
 * - A single LS character (line separator, U+2028). 
 * - A single PS character (paragraph separator, U+2029).
 * The last two are recognized only in UTF-8 mode. 
 * @see KNewLineLf
 * @see KNewLineCr
 * @see KNewLineCrLf
 * @see KNewLineAnyCrLf
 */
static const TInt KNewLineAny = -1;

/**
 * Symbian C++ interface to the pcre regular-expression library. This class, its
 * supporting classes and most of the following documentation is largely based
 * on or taken from the C++ wrapper included with source distributions of PCRE
 * to which all credit should be given.
 * 
 * CRegEx supports
 * Perl-style regular expressions (with extensions like \d, \w, \s,
 * ...).
 *
 * NOTE: These following examples make liberal use of _L8() purely for clarity
 * and not because it is recommend. In fact, it is strongly discouraged
 * in favour of _LIT8() as per the standard Symbian coding conventions.
 * -----------------------------------------------------------------------
 * REGEXP SYNTAX:
 *
 * This module is part of the pcre library and hence supports its syntax
 * for regular expressions.
 *
 * The syntax is pretty similar to Perl's.  For those not familiar
 * with Perl's regular expressions, here are some examples of the most
 * commonly used extensions:
 *
 *   "hello (\\w+) world"  -- \w matches a "word" character
 *   "version (\\d+)"      -- \d matches a digit
 *   "hello\\s+world"      -- \s matches any whitespace character
 *   "\\b(\\w+)\\b"        -- \b matches empty string at a word boundary
 *   "(?i)hello"           -- (?i) turns on case-insensitive matching
 *
 * -----------------------------------------------------------------------
 * MATCHING INTERFACE:
 *
 * The FullMatchL() operation checks that supplied text matches a
 * supplied pattern exactly.
 *
 * Example: successful match
 * @code
 *    CRegEx* re = CRegEx::NewLC(_L8("h.*o"));
 *    re->FullMatchL(_L8("hello"));
 *    CleanupStack::PopAndDestroy(re);
 * @endcode
 * 
 * Example: unsuccessful match (requires full match):
 * @code
 *    CRegEx* re = CRegEx::NewLC(_L8("e"));
 *    !re->FullMatchL(_L8("hello"));
 *    CleanupStack::PopAndDestroy(re);
 * @endcode
 *
 * -----------------------------------------------------------------------
 * MATCHING WITH SUB-STRING EXTRACTION:
 *
 * You can supply extra pointer arguments to extract matched subpieces.
 *
 * Example: extracts "ruby" into "s" and 1234 into "i"
 * @code
 *    TInt i;
 *    TBuf<4> s;
 *    CRegEx* re = CRegEx::NewLC(_L8("(\\w+):(\\d+)"));
 *    re->FullMatchL(_L8("ruby:1234"), &s, &i);
 *    CleanupStack::PopAndDestroy(re);
 * @endcode
 *
 * Example: does not try to extract any extra sub-patterns
 * @code
 *    re->FullMatchL(_L8("ruby:1234"), &s);
 * @endcode
 * 
 * Example: does not try to extract into NULL
 * @code
 *    re->FullMatchL(_L8("ruby:1234"), NULL, &i);
 * @endcode
 * 
 * Example: integer overflow causes failure
 * @code
 *    !re.FullMatchL(_L8("ruby:1234567891234"), NULL, &i);
 * @endcode
 * 
 * Example: fails because there aren't enough sub-patterns:
 * @code
 *    TBuf<4> s;
 *    CRegEx* re = CRegEx::NewLC(_L8("\\w+:\\d+"));
 *    !re->FullMatchL(_L8("ruby:1234"), &s);
 *    CleanupStack::PopAndDestroy(re);
 * @endcode
 * 
 * Example: fails because string cannot be stored in integer
 * @code
 *    TInt i;
 *    CRegEx* re = CRegEx::NewLC(_L8("(.*)"));
 *    !re->FullMatchL(_L8("ruby"), &i);
 *    CleanupStack::PopAndDestroy(re);
 * @endcode 
 *
 * The provided pointer arguments can be pointers to any scalar numeric
 * type, or one of
 *    TDes8        (matched piece is copied to descriptor)
 *    TPtrC8	   (matched piece is pointed to by)
 *    T            (where "TBool T::ParseFrom(const TDesC8&)" exists)
 *    NULL         (the corresponding matched sub-pattern is not copied)
 *
 * CAVEAT: An optional sub-pattern that does not exist in the matched
 * string is assigned the empty string.  Therefore, the following will
 * return false (because the empty string is not a valid number):
 * @code
 *    TInt number;
 *    CRegEx* re = CRegEx::NewLC(_L8("abc"));
 *    re->FullMatchL(_L8("[a-z]+(\\d+)?"), &number);
 *    CleanupStack::PopAndDestroy(re); 
 * @endcode
 *
 * -----------------------------------------------------------------------
 * DO_MATCH
 *
 * The matching interface supports at most 4 arguments per call.
 * If you need more, consider using the more general interface
 * CRegEx::DoMatchL().
 *
 * -----------------------------------------------------------------------
 * PARTIAL MATCHES
 *
 * You can use the PartialMatchL() operation when you want the pattern
 * to match any substring of the text.
 *
 * Example: simple search for a string:
 * @code
 *    CRegEx* re = CRegEx::NewLC(_L8("ell"));
 *    re->PartialMatchL(_L8("hello"));
 *    CleanupStack::PopAndDestroy(re);
 * @endcode
 * 
 * Example: find first number in a string:
 * @code
 *    TInt number;
 *    CRegEx* re = CRegEx::NewLC(_L8("(\\d+)"));
 *    re->PartialMatchL(_L8("x*100 + 20"), &number);
 *    ASSERT(number == 100);
 *    CleanupStack::PopAndDestroy(re);
 * @endcode
 *
 * -----------------------------------------------------------------------
 * UTF-8 AND THE MATCHING INTERFACE:
 *
 * By default, pattern and text are plain text, one byte per character.
 * The UTF8 flag, passed to the constructor, causes both pattern
 * and string to be treated as UTF-8 text, still a byte stream but
 * potentially multiple bytes per character. In practice, the text
 * is likelier to be UTF-8 than the pattern, but the match returned
 * may depend on the UTF8 flag, so always use it when matching
 * UTF8 text.  E.g., "." will match one byte normally but with UTF8
 * set may match up to three bytes of a multi-byte character.
 *
 * Example:
 * @code
 *    TRegExOptions options;
 *    options.SetUtf8(ETrue);
 *    CRegEx* re = CRegEx::NewLC(utf8Pattern);
 *    re->FullMatchL(utf8String);
 *    CleanupStack::PopAndDestroy(re);
 * @endcode
 * NOTE: The UTF8 option is ignored if libpcre was not compiled with the
 *       SUPPORT_UTF8 macro.
 *
 * -----------------------------------------------------------------------
 * PASSING MODIFIERS TO THE REGULAR EXPRESSION ENGINE
 *
 * SPCRE defines some modifiers to change the behavior of the regular
 * expression engine.
 * The C++ wrapper defines an auxiliary class, TRegExOptions, as a vehicle
 * to pass such modifiers to a CRegEx class.
 *
 * Currently, the following modifiers are supported
 *
 *    modifier              description               Perl corresponding
 *
 *    EPcreCaseless         case insensitive match    /i
 *    EPcreMultiline        multiple lines match      /m
 *    EPcreDotAll           dot matches newlines      /s
 *    EPcreDollarEndOnly    $ matches only at end     N/A
 *    EPcreExtra            strict escape parsing     N/A
 *    EPcreExtended         ignore whitespaces        /x
 *    EPcreUtf8             handles UTF8 chars        built-in
 *    EPcreUngreedy         reverses * and *?         N/A
 *    EPcreNoAutoCapture    disables matching parens  N/A (*)
 *
 * (For a full account on how each modifier works, please check the
 * PCRE API reference manual).
 *
 * (*) Both Perl and PCRE allow non matching parentheses by means of the
 * "?:" modifier within the pattern itself. e.g. (?:ab|cd) does not
 * capture, while (ab|cd) does.
 *
 * For each modifier, there are two member functions whose name is made
 * out of the modifier , without the "EPcre" prefix. For
 * instance, EPcreCaseless is handled by
 *    TBool Caseless(),
 * which returns ETrue if the modifier is set, and
 *    TRegExOptions SetCaseless(TBool),
 * which sets or unsets the modifier.
 *
 * Moreover, PCRE_EXTRA_MATCH_LIMIT can be accessed through the
 * SetMatchLimit() and MatchLimit() member functions.
 * Setting the match limit to a non-zero value will limit the executation of
 * SPCRE to keep it from doing bad things like blowing the stack or taking
 * an eternity to return a result.  A value of 5000 is good enough to stop
 * stack blowup in a 2MB thread stack.  Setting MathLimit to zero will
 * disable match limiting.  Alternately, you can set MatchLimitRecursion()
 * which uses PCRE_EXTRA_MATCH_LIMIT_RECURSION to limit how much SPCRE
 * recurses.  MatchLimit() caps the number of matches pcre does;
 * MatchLimitRecursion() caps the depth of recursion.
 *
 * Normally, to pass one or more modifiers to a CRegEx class, you declare
 * a TRegExOptions object, set the appropriate options, and pass this
 * object to a CRegEx constructor. Example:
 *
 * @code
 *    TRegExOptions opt;
 *    opt.setCaseless(ETrue);
 *    CRegEx* re = CRegEx::NewLC(_L8("HELLO"), opt);
 *    if(re->PartialMatchL(_L8("hello world"))) ...
 * @endcode
 *
 * -----------------------------------------------------------------------
 * SCANNING TEXT INCREMENTALLY
 *
 * The ConsumeL() operation may be useful if you want to repeatedly
 * match regular expressions at the front of a string and skip over
 * them as they match.  This requires use of the "StringPiece" type,
 * which represents a sub-range of a real string.  Like RE, StringPiece
 * is defined in the pcrecpp namespace.
 *
 * Example: read lines of the form "var = value" from a string.
 * @code
 *    TBuf8<KContentLength> contents;
 *    // fill contents somehow
 *    TBuf8<KMaxVarLength> var;
 *    TInt value;
 *    CRegEx* re = CRegEx::NewLC(_L8("(\\w+) = (\\d+)\n"));
 *    while(re->ConsumeL(contents, &var, &value))
 *    	{
 *    	...
 *    	}
 *    CleanupStack::PopAndDestroy(re);
 * @endcode
 * 
 * Each successful call to ConsumeL will set "var/value", and also
 * advance "contents" so it points past the matched text.
 *
 * The FindAndConsumeL() operation is similar to ConsumeL() but does not
 * anchor your match at the beginning of the string.  For example, you
 * could extract all words from a string by repeatedly calling
 * @code
 *    TBuf8<KContentLength> contents;
 *    // fill contents somehow
 *    TBuf8<KMaxWordLength> word;
 *    TInt value;
 *    CRegEx* re = CRegEx::NewLC(_L8("(\\w+)"));
 *    while(re->FindAndConsumeL(contents, &word))
 *    	{
 *    	...
 *    	}
 *    CleanupStack::PopAndDestroy(re);
 * @endcode
 * -----------------------------------------------------------------------
 * PARSING HEX/OCTAL NUMBERS
 *
 * By default, if you pass a pointer to a numeric value, the
 * corresponding text is interpreted as a base-10 number.  You can
 * instead wrap the pointer with a call to one of the operators Hex(),
 * or Octal() to interpret the text in another base.  
 *
 * Example:
 * @code
 *    TInt a, b;
 *    CRegEx* re = CRegEx::NewLC(_L8("(.*) (.*)"));
 *    re->FullMatchL(_L8("100 40"), Hex(&a), Octal(&b));
 *    CleanupStack::PopAndDestroy(re);
 * @endcode
 * will leave 64 in a and b.
 *
 * -----------------------------------------------------------------------
 * REPLACING PARTS OF STRINGS
 *
 * You can replace the first match of aPattern in aString with
 * aRewrite.  Within aRewrite, backslash-escaped digits (\1 to \9)
 * can be used to insert text matching corresponding parenthesized
 * group from the pattern.  \0 in aRewrite refers to the entire
 * matching text.  E.g.,
 * @code
 *    _LIT8(KYabbaDabbaDoo, "yabba dabba doo"); 
 *    TBuf8<20> s;
 *    CRegEx* re = CRegEx::NewLC(_L8("b+");
 *    re->ReplaceL(KYabbaDabbaDoo(), s);
 *    CleanupStack::PopAndDestroy(re);
 * @endcode
 * will leave "s" containing "yada dabba doo".  The result is ETrue if
 * the pattern matches and a replacement occurs, or Efalse otherwise.
 *
 * GlobalReplaceL() is like Replace:(), except that it replaces all
 * occurrences of the pattern in the string with the rewrite.
 * Replacements are not subject to re-matching.  E.g.,
 * @code
 *    _LIT8(KYabbaDabbaDoo, "yabba dabba doo"); 
 *    TBuf8<20> s;
 *    CRegEx* re = CRegEx::NewLC(_L8("b+");
 *    re->GlobalReplaceL(_L8("d"), s);
 *    CleanupStack::PopAndDestroy(re);
 * @endcode
 * will leave "s" containing "yada dada doo".  It returns the number
 * of replacements made.
 *
 * ExtractL() is like Replace(), except that if the pattern matches,
 * aRewrite is copied into aOut (an additional argument) with
 * substitutions.  The non-matching portions of aText are ignored.
 * Returns ETrue if a match occurred and the extraction happened
 * successfully.  If no match occurs, the string is left unaffected.
 */
class CRegEx : public CBase
	{
public:
	// Type of match (TODO: Should be restructured as part of TRegExOptions)
	enum TAnchor
		{
		EUnanchored,		/** No anchoring */
		EAnchorStart,		/** Anchor at start only */
		EAnchorBoth			/** Anchor at start and end */
		};
	
public:
	IMPORT_C static CRegEx* NewL(const TDesC8& aPattern);
	IMPORT_C static CRegEx* NewL(const TDesC8& aPattern, const TRegExOptions& aOptions);
	IMPORT_C static CRegEx* NewLC(const TDesC8& aPattern);
	IMPORT_C static CRegEx* NewLC(const TDesC8& aPattern, const TRegExOptions& aOptions);

	IMPORT_C static CRegEx* NewL(const TDesC16& aPattern, const TRegExOptions& aOptions);
	
	IMPORT_C ~CRegEx();
	
	inline const TDesC8& Pattern() const;
	inline TInt Error() const;

	IMPORT_C TBool FullMatchL(const TDesC8& aText) const;
	
	IMPORT_C TBool FullMatchL(const TDesC8& aText,
			  const TRegExArg& aArg1) const;
	
	IMPORT_C TBool FullMatchL(const TDesC8& aText,
			  const TRegExArg& aArg1,
			  const TRegExArg& aArg2) const;
	
	IMPORT_C TBool FullMatchL(const TDesC8& aText,
			  const TRegExArg& aArg1,
			  const TRegExArg& aArg2,
			  const TRegExArg& aArg3) const;
	
	IMPORT_C TBool FullMatchL(const TDesC8& aText,
			  const TRegExArg& aArg1,
			  const TRegExArg& aArg2,
			  const TRegExArg& aArg3,
			  const TRegExArg& aArg4) const;	
	
	IMPORT_C TBool PartialMatchL(const TDesC8& aText) const;
	
	IMPORT_C TBool PartialMatchL(const TDesC8& aText,
			  const TRegExArg& aArg1) const;
	
	IMPORT_C TBool PartialMatchL(const TDesC8& aText,
			  const TRegExArg& aArg1,
			  const TRegExArg& aArg2) const;
	
	IMPORT_C TBool PartialMatchL(const TDesC8& aText,
			  const TRegExArg& aArg1,
			  const TRegExArg& aArg2,
			  const TRegExArg& aArg3) const;
	
	IMPORT_C TBool PartialMatchL(const TDesC8& aText,
			  const TRegExArg& aArg1,
			  const TRegExArg& aArg2,
			  const TRegExArg& aArg3,
			  const TRegExArg& aArg4) const;
	
	IMPORT_C TBool DoMatchL(const TDesC8& aText,
			  TAnchor aAnchor,
			  TInt&	aConsumed) const;
	
	IMPORT_C TBool DoMatchL(const TDesC8& aText,
			  TAnchor aAnchor,
			  TInt&	aConsumed,
			  const TRegExArg& aArg1) const;
	
	IMPORT_C TBool DoMatchL(const TDesC8& aText,
			  TAnchor aAnchor,
			  TInt&	aConsumed,
			  const TRegExArg& aArg1,
			  const TRegExArg& aArg2) const;
	
	IMPORT_C TBool DoMatchL(const TDesC8& aText,
			  TAnchor aAnchor,
			  TInt&	aConsumed,
			  const TRegExArg& aArg1,
			  const TRegExArg& aArg2,
			  const TRegExArg& aArg3) const;
	
	IMPORT_C TBool DoMatchL(const TDesC8& aText,
			  TAnchor aAnchor,
			  TInt&	aConsumed,
			  const TRegExArg& aArg1,
			  const TRegExArg& aArg2,
			  const TRegExArg& aArg3,
			  const TRegExArg& aArg4) const;
	
	IMPORT_C TBool DoMatchL(const TDesC8& aText,
			  TAnchor aAnchor,
			  TInt&	aConsumed,			
			  const RPointerArray<const TRegExArg>& aArgs) const;
	
	
	IMPORT_C TBool ConsumeL(TDes8& aText) const;
	
	IMPORT_C TBool ConsumeL(TDes8& aText,
			  const TRegExArg& aArg1) const;
	
	IMPORT_C TBool ConsumeL(TDes8& aText,
			  const TRegExArg& aArg1,
			  const TRegExArg& aArg2) const;
	
	IMPORT_C TBool ConsumeL(TDes8& aText,
			  const TRegExArg& aArg1,
			  const TRegExArg& aArg2,
			  const TRegExArg& aArg3) const;
	
	IMPORT_C TBool ConsumeL(TDes8& aText,
			  const TRegExArg& aArg1,
			  const TRegExArg& aArg2,
			  const TRegExArg& aArg3,
			  const TRegExArg& aArg4) const;
	
	IMPORT_C TBool FindAndConsumeL(TDes8& aText) const;
	
	IMPORT_C TBool FindAndConsumeL(TDes8& aText,
			  const TRegExArg& aArg1) const;
	
	IMPORT_C TBool FindAndConsumeL(TDes8& aText,
			  const TRegExArg& aArg1,
			  const TRegExArg& aArg2) const;
	
	IMPORT_C TBool FindAndConsumeL(TDes8& aText,
			  const TRegExArg& aArg1,
			  const TRegExArg& aArg2,
			  const TRegExArg& aArg3) const;
	
	IMPORT_C TBool FindAndConsumeL(TDes8& aText,
			  const TRegExArg& aArg1,
			  const TRegExArg& aArg2,
			  const TRegExArg& aArg3,
			  const TRegExArg& aArg4) const;
	
	IMPORT_C TBool ReplaceL(const TDesC8& aRewrite, TDes8& aString) const;
	
	IMPORT_C TInt GlobalReplaceL(const TDesC8& aRewrite, TDes8& aString) const;
	
	IMPORT_C TBool ExtractL(const TDesC8& aRewrite, const TDesC8& aText, TDes8& aOut) const;

	IMPORT_C static TInt NewlineMode(TInt aOptions);
	
	IMPORT_C static HBufC8* QuoteMetaL(const TDesC8& aUnquoted);

	IMPORT_C TInt NumberOfCapturingGroups() const;
	
	IMPORT_C void Study();	

private:
	CRegEx();
	CRegEx(const TRegExOptions& aOptions);
	
	void ConstructL(const TDesC8& aPattern);
	void ConstructL(const TDesC16& aPattern);
	void CommonConstructL();
	
	pcre* CompileL(TAnchor anchor);
	void Cleanup();

	TInt TryMatch(const TDesC8& aText,
			   TInt aStartPos,
			   TAnchor aAnchor,
			   TInt* aVector,
			   TInt aVectorSize) const;
	

	TBool Rewrite(TDes8& aOut,
			   const TDesC8& aRewrite,
			   const TDesC8& aText,
			   TInt* aVector,
			   TInt aVectorSize,
			   TInt aMatches) const;
	
	TBool DoMatchImpl(const TDesC8& aText,
				   TAnchor aAnchor,
				   TInt& aConsumed,
				   const RPointerArray<const TRegExArg>& aArgs,
				   TInt* aVector,
				   TInt aVectorSize) const;
	

	static void Panic(TRegExPanic aPanic);

	mutable TInt					iErrorCode;		// Error code for the alst unsuccessful operation.
	TInt							iErrorOffset;	// Offset in pattern where error was detected
	HBufC8*							iPattern;		// Regular expression pattern
	TRegExOptions					iOptions;		// Options used to compile RE pattern.
	pcre*							iReFull;		// For full matches
	pcre*							iRePartial;		// For partial matches
	pcre_extra*						iExtraPartial;	// Study Data for iRePartial
	TRegExArg*						iNoArg;			// Default argument
	};
	
#include <cregex.inl>
#endif /* CREGEX_H_ */
