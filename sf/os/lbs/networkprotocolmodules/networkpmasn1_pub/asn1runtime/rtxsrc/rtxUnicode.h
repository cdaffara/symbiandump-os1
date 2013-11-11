/**
 * @file rtxUnicode.h
 * This is an open source header file derived from the libxml2 
 * project.  It defines UNICODE data types and macros. See the header 
 * file for further details.
 */
/*
The software in this module is open source software that has been 
derived from various sources.  All applicable copyright notices are 
contained within.  

NOTE: the copyright notice below applies only to source and header files 
that include this notice.  It does not apply to other Objective Systems 
software with different attached notices.

***
Except where otherwise noted in the source code (trio files, hash.c and list.c)
covered by a similar licence but with different Copyright notices:

 Copyright (C) 1998-2002 Daniel Veillard.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is fur-
nished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FIT-
NESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
DANIEL VEILLARD BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CON-
NECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of Daniel Veillard shall not
be used in advertising or otherwise to promote the sale, use or other deal-
ings in this Software without prior written authorization from him.
*/

/**
@file
@internalTechnology
*/

#ifndef _RTX_UNICODE_H_
#define _RTX_UNICODE_H_

#include <stdio.h>
#include "rtxsrc/rtxContext.h"

/************************************************************************
 *									*
 * UNICODE version of the macros.      					*
 *									*
 ************************************************************************/
/*
 * IS_CHAR:
 * @param c:  a UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 * [2] Char ::= \#x9 | \#xA | \#xD | [\#x20-\#xD7FF] | [\#xE000-\#xFFFD]
 *                  | [\#x10000-\#x10FFFF]
 * any Unicode character, excluding the surrogate blocks, FFFE, and FFFF.
 */
#define OS_IS_CHAR(c)							\
    ((((c) >= 0x20) && ((c) <= 0xD7FF)) ||				\
     ((c) == 0x09) || ((c) == 0x0A) || ((c) == 0x0D) ||			\
     (((c) >= 0xE000) && ((c) <= 0xFFFD)) ||				\
     (((c) >= 0x10000) && ((c) <= 0x10FFFF)))

/*
 * IS_BLANK:
 * @param c:  a UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 * [3] S ::= (\#x20 | \#x9 | \#xD | \#xA)+
 */
#define OS_IS_BLANK(c) \
(((c) == 0x20) || ((c) == 0x09) || ((c) == 0xA) || ((c) == 0x0D))

/*
 * IS_BASECHAR:
 * @param c:  a UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 * [85] BaseChar ::= ... long list see REC ...
 */
#define OS_IS_BASECHAR(c) rtxUCSIsBaseChar(c)

/*
 * IS_DIGIT:
 * @param c:  a UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 * [88] Digit ::= ... long list see REC ...
 */
#define OS_IS_DIGIT(c) rtxUCSIsDigit(c)

/*
 * IS_COMBINING:
 * @param c:  a UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 * [87] CombiningChar ::= ... long list see REC ...
 */
#define OS_IS_COMBINING(c) rtxUCSIsCombining(c)

/*
 * IS_EXTENDER:
 * @param c:  a UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 *
 * [89] Extender ::= \#x00B7 | \#x02D0 | \#x02D1 | \#x0387 | \#x0640 |
 *                   \#x0E46 | \#x0EC6 | \#x3005 | [\#x3031-\#x3035] |
 *                   [\#x309D-\#x309E] | [\#x30FC-\#x30FE]
 */
#define OS_IS_EXTENDER(c) rtxUCSIsExtender(c)

/*
 * IS_IDEOGRAPHIC:
 * @c:  an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 *
 * [86] Ideographic ::= [\#x4E00-\#x9FA5] | \#x3007 | [\#x3021-\#x3029]
 */
#define OS_IS_IDEOGRAPHIC(c) rtxUCSIsIdeographic(c)

/*
 * IS_LETTER:
 * @c:  an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 *
 * [84] Letter ::= BaseChar | Ideographic 
 */
#define OS_IS_LETTER(c) (IS_BASECHAR(c) || IS_IDEOGRAPHIC(c))


/*
 * IS_PUBIDCHAR:
 * @c:  an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 *
 * [13] PubidChar ::= \#x20 | \#xD | \#xA | [a-zA-Z0-9] | [-'()+,./:=?;!*\#\@\$_%]
 */
#define OS_IS_PUBIDCHAR(c)	xmlIsPubidChar(c)

#ifdef __cplusplus
extern "C" {
#endif

/* Unicode string functions */

EXTERNRT OSBOOL rtxUCSStrCmp (const OSUNICHAR* text1, 
                               const OSUNICHAR* text2);

EXTERNRT OSBOOL rtxUCSStrCmpAsc (const OSUNICHAR* text1, 
                                  const char* text2);

EXTERNRT size_t rtxUCSStrLen (const OSUNICHAR* text);

EXTERNRT void rtxUCSStrPrint (const OSUNICHAR* text);

EXTERNRT void rtxUCSStrPrintFile (FILE* fp, const OSUNICHAR* text);

EXTERNRT char* rtxUCSToCStr (OSCTXT* pctxt, const OSUNICHAR* text);

EXTERNRT long rtxUCSToUTF8 (OSCTXT* pctxt, const OSUNICHAR* inbuf, 
                             size_t inlen, OSOCTET* outbuf, size_t outbufsiz);

/**
 * This function converts a null-terminated Unicode string into a 
 * UTF-8 string.  Memory is allocated for the output string using the 
 * built-in memory management functions.
 *
 * @param pctxt         Pointer to context structure.
 * @param inbuf         Null-terminated Unicode string to convert.
 * @return              Converted UTF-8 character string.
 */
EXTERNRT const OSUTF8CHAR* 
rtxUCSToDynUTF8 (OSCTXT* pctxt, const OSUNICHAR* inbuf);

/**
 * rtxUCSIsChar:
 * @param c:  an unicode character (int)
 *
 * Check whether the character is allowed by the production
 * [2] Char ::= \#x9 | \#xA | \#xD | [\#x20-\#xD7FF] | [\#xE000-\#xFFFD]
 *                  | [\#x10000-\#x10FFFF]
 * any Unicode character, excluding the surrogate blocks, FFFE, and FFFF.
 * Also available as a macro IS_CHAR()
 *
 * Returns 0 if not, non-zero otherwise
 */
EXTERNRT OSBOOL rtxUCSIsChar (OS32BITCHAR c); 

/**
 * rtxUCSIsBlank:
 * @param c:  a UNICODE character (int)
 *
 * Check whether the character is allowed by the production
 * [3] S ::= (\#x20 | \#x9 | \#xD | \#xA)+
 * Also available as a macro IS_BLANK()
 *
 * Returns 0 if not, non-zero otherwise
 */
EXTERNRT OSBOOL rtxUCSIsBlank (OS32BITCHAR c);

/**
 * rtxUCSIsBaseChar:
 * @param c:  an unicode character (int)
 *
 * Check whether the character is allowed by the production
 * [85] BaseChar ::= ... long list see REC ...
 *
 * Returns 0 if not, non-zero otherwise
 */
EXTERNRT OSBOOL rtxUCSIsBaseChar (OS32BITCHAR c); 

/**
 * rtxUCSIsDigit:
 * @param c:  an unicode character (int)
 *
 * Check whether the character is allowed by the production
 * [88] Digit ::= ... long list see REC ...
 *
 * Returns 0 if not, non-zero otherwise
 */
EXTERNRT OSBOOL rtxUCSIsDigit (OS32BITCHAR c); 

/**
 * rtxUCSIsCombining:
 * @param c:  an unicode character (int)
 *
 * Check whether the character is allowed by the production
 * [87] CombiningChar ::= ... long list see REC ...
 *
 * Returns 0 if not, non-zero otherwise
 */
EXTERNRT OSBOOL rtxUCSIsCombining (OS32BITCHAR c); 

/**
 * rtxUCSIsExtender:
 * @param c:  an unicode character (int)
 *
 * Check whether the character is allowed by the production
 * [89] Extender ::= \#x00B7 | \#x02D0 | \#x02D1 | \#x0387 | \#x0640 |
 *                   \#x0E46 | \#x0EC6 | \#x3005 | [\#x3031-\#x3035] |
 *                   [\#x309D-\#x309E] | [\#x30FC-\#x30FE]
 *
 * Returns 0 if not, non-zero otherwise
 */
EXTERNRT OSBOOL rtxUCSIsExtender (OS32BITCHAR c); 

/**
 * rtxUCSIsIdeographic:
 * @param c:  an unicode character (int)
 *
 * Check whether the character is allowed by the production
 * [86] Ideographic ::= [\#x4E00-\#x9FA5] | \#x3007 | [\#x3021-\#x3029]
 *
 * Returns 0 if not, non-zero otherwise
 */
EXTERNRT OSBOOL rtxUCSIsIdeographic (OS32BITCHAR c); 

/**
 * rtxUCSIsLetter:
 * @param c:  an unicode character (int)
 *
 * Check whether the character is allowed by the production
 * [84] Letter ::= BaseChar | Ideographic
 *
 * Returns 0 if not, non-zero otherwise
 */
EXTERNRT OSBOOL rtxUCSIsLetter (OS32BITCHAR c); 

/**
 * rtxUCSIsPubidChar:
 * @param c:  an unicode character (int)
 *
 * Check whether the character is allowed by the production
 * [13] PubidChar ::= \#x20 | \#xD | \#xA | [a-zA-Z0-9] | [-'()+,./:=?;!*\#\@\$_%]
 *
 * Returns 0 if not, non-zero otherwise
 */
EXTERNRT OSBOOL rtxUCSIsPubidChar (OS32BITCHAR c);

EXTERNRT OSBOOL rtxUCSIsAlphabeticPresentationForms (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsArabic (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsArabicPresentationFormsA (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsArabicPresentationFormsB (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsArmenian (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsArrows (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsBasicLatin (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsBengali (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsBlockElements (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsBopomofo (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsBopomofoExtended (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsBoxDrawing (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsBraillePatterns (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsByzantineMusicalSymbols (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCJKCompatibility (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCJKCompatibilityForms (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCJKCompatibilityIdeographs (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCJKCompatibilityIdeographsSupplement (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCJKRadicalsSupplement (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCJKSymbolsandPunctuation (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCJKUnifiedIdeographs (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCJKUnifiedIdeographsExtensionA (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCJKUnifiedIdeographsExtensionB (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCherokee (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCombiningDiacriticalMarks (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCombiningHalfMarks (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCombiningMarksforSymbols (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsControlPictures (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCurrencySymbols (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCyrillic (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsDeseret (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsDevanagari (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsDingbats (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsEnclosedAlphanumerics (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsEnclosedCJKLettersandMonths (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsEthiopic (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsGeneralPunctuation (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsGeometricShapes (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsGeorgian (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsGothic (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsGreek (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsGreekExtended (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsGujarati (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsGurmukhi (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsHalfwidthandFullwidthForms (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsHangulCompatibilityJamo (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsHangulJamo (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsHangulSyllables (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsHebrew (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsHighPrivateUseSurrogates (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsHighSurrogates (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsHiragana (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsIPAExtensions (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsIdeographicDescriptionCharacters (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsKanbun (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsKangxiRadicals (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsKannada (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsKatakana (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsKhmer (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsLao (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsLatin1Supplement (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsLatinExtendedA (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsLatinExtendedB (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsLatinExtendedAdditional (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsLetterlikeSymbols (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsLowSurrogates (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsMalayalam (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsMathematicalAlphanumericSymbols (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsMathematicalOperators (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsMiscellaneousSymbols (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsMiscellaneousTechnical (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsMongolian (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsMusicalSymbols (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsMyanmar (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsNumberForms (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsOgham (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsOldItalic (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsOpticalCharacterRecognition (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsOriya (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsPrivateUse (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsRunic (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsSinhala (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsSmallFormVariants (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsSpacingModifierLetters (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsSpecials (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsSuperscriptsandSubscripts (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsSyriac (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsTags (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsTamil (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsTelugu (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsThaana (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsThai (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsTibetan (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsUnifiedCanadianAboriginalSyllabics (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsYiRadicals (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsYiSyllables (OS32BITCHAR code) ;

EXTERNRT int rtxUCSIsBlock (OS32BITCHAR code, const char *block) ;

EXTERNRT OSBOOL rtxUCSIsCatC (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatCc (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatCf (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatCo (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatCs (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatL (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatLl (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatLm (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatLo (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatLt (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatLu (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatM (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatMc (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatMe (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatMn (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatN (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatNd (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatNl (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatNo (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatP (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatPc (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatPd (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatPe (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatPf (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatPi (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatPo (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatPs (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatS (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatSc (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatSk (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatSm (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatSo (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatZ (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatZl (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatZp (OS32BITCHAR code) ;

EXTERNRT OSBOOL rtxUCSIsCatZs (OS32BITCHAR code) ;

EXTERNRT int rtxUCSIsCat (OS32BITCHAR code, const char *cat) ;

#ifdef __cplusplus
}
#endif

#endif
