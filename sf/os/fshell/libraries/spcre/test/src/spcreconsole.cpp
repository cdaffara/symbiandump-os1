// spcreconsole.cpp
//
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


#include <e32base.h>
#include <e32std.h>
#include <e32math.h>
#include <e32cons.h>			// Console
#include <bacline.h>
#include <cregex.h>
#include <tregexarg.h>
#include <tregexoptions.h>

//  Constants
_LIT(KTextConsoleTitle, "Console");
_LIT(KTextFailed, " failed, leave code = %d\n");
_LIT(KTextPressAnyKey, "[press any key]\n");
//  Global Variables
LOCAL_D CConsoleBase* console; // write all messages to this
TBool verboseMode = EFalse;

//  Local Functions

// CHECK dies with a fatal error if condition is not true.  It is *not*
// controlled by NDEBUG, so the check will be executed regardless of
// compilation mode.  Therefore, it is safe to do things like:
//    CHECK_EQ(fp->Write(x), 4)
#define CHECK(condition)								\
	{													\
	if (!(condition))									\
		{                                   			\
		_LIT(KCondition, #condition);					\
		console->Printf(_L("%d: Check failed: %S\n"),	\
						__LINE__, &KCondition());		\
		User::Leave(KErrGeneral);						\
		}												\
	}

#define CHECK_EQ(a, b)   CHECK(a == b)

static void Timing1L(TInt aNumIters)
	{
	console->Printf(_L("Test Timing 1, %d iterations.\n"), aNumIters);
	
	// Same pattern lots of times
	_LIT8(KPattern, "ruby:\\d+");
	_LIT8(KString,"ruby:1234");
	CRegEx* pattern = CRegEx::NewLC(KPattern());

	for (TInt j = aNumIters; j > 0; j--)
		{
		CHECK(pattern->FullMatchL(KString()));
		}
	CleanupStack::PopAndDestroy(pattern);
	}

static void Timing2L(TInt aNumIters)
	{
	console->Printf(_L("Test Timing 2, %d iterations.\n"), aNumIters);
	// Same pattern lots of times
	_LIT8(KPattern, "ruby:(\\d+)");
	_LIT8(KString,"ruby:1234");
	CRegEx* pattern = CRegEx::NewLC(KPattern());

	TInt i;
	for (TInt j = aNumIters; j > 0; j--)
		{
		CHECK(pattern->FullMatchL(KString(), &i));
		CHECK_EQ(i, 1234);
		}
	CleanupStack::PopAndDestroy(pattern);
	}

static void Timing3L(TInt aNumIters)
	{
	console->Printf(_L("Test Timing 3, %d iterations.\n"), aNumIters);
	_LIT8(KLine, "this is another line\n");
	HBufC8* text = HBufC8::NewLC(KLine().Length() * aNumIters);
	TPtr8 pText = text->Des();

	for (int j = aNumIters; j > 0; j--)
		{
		pText.Append(KLine());
		}

	CRegEx* lineMatcher = CRegEx::NewLC(_L8(".*\n"));

	TInt counter = 0;
	while (lineMatcher->ConsumeL(pText))
		{
		counter++;
		}
	CleanupStack::PopAndDestroy(2, text);
	CHECK_EQ(counter, aNumIters);
	console->Printf(_L("Matched %d lines\n"), counter);
	}


static void RadixTestsL()
	{
	console->Write(_L("Testing hex\n"));

	#define CHECK_HEX(type, value) 										\
		{																\
		type v;															\
		CRegEx* re = CRegEx::NewLC(_L8("([0-9a-fA-F]+)[uUlL]*"));		\
		CHECK(re->FullMatchL(_L8(#value), Hex(&v)));					\
		CHECK_EQ(v, 0x ## value);										\
		CleanupStack::PopAndDestroy(re);								\
		re = NULL;														\
		}

	CHECK_HEX(TInt64,     2baddeadbeefLL);
	CHECK_HEX(TUint8,     abU);
	CHECK_HEX(TUint16,    2badU);
	CHECK_HEX(TUint32,    deadbeefUL);
	CHECK_HEX(TUint,      cafebabeU);
	#undef CHECK_HEX

	console->Write(_L("Testing octal\n"));

	#define CHECK_OCTAL(type, value) 									\
		{																\
		type v;															\
		CRegEx* re = CRegEx::NewLC(_L8("([0-7]+)[uUlL]*"));				\
		CHECK(re->FullMatchL(_L8(#value), Octal(&v)));					\
		CHECK_EQ(v, 0 ## value);										\
		CleanupStack::PopAndDestroy(re);								\
		re = NULL;														\
		}

	CHECK_OCTAL(TInt64,		777777777777LL);
	CHECK_OCTAL(TUint8,		177U);
	CHECK_OCTAL(TUint16,	177777U);
	CHECK_OCTAL(TUint32,	3777777777UL);
	CHECK_OCTAL(TUint,		3777777777U);
	#undef CHECK_OCTAL

	console->Write(_L("Testing decimal\n"));
	
	#define CHECK_DECIMAL(type, value) 									\
		{																\
		type v;															\
		CRegEx* re = CRegEx::NewLC(_L8("(-?[0-9]+)[uUlL]*"));			\
		CHECK(re->FullMatchL(_L8(#value), &v));							\
		CHECK_EQ(v, value);												\
		CleanupStack::PopAndDestroy(re);								\
		re = NULL;														\
		}	
	
	CHECK_DECIMAL(TInt8,	-1);
	CHECK_DECIMAL(TUint8,	1U);
	CHECK_DECIMAL(TInt16,	-9999);
	CHECK_DECIMAL(TUint16,	9999U);
	CHECK_DECIMAL(TInt32,	-123456L);
	CHECK_DECIMAL(TUint32,	123456U);
	CHECK_DECIMAL(TInt64,	1234567890123456789LL);
	CHECK_DECIMAL(TInt,		-1234567890);
	CHECK_DECIMAL(TUint,	1234567890U);
	#undef CHECK_DECIMAL
}

static void TestReplaceL()
	{
	console->Write(_L("Testing ReplaceL\n"));

	struct ReplaceTest
		{
		TBuf8<32>	iRegExp;
		TBuf8<32>	iRewrite;
		TBuf8<64>	iOriginal;
		TBuf8<64>	iSingle;
		TBuf8<64>	iGlobal;
		TInt		iGlobalCount; // the expected return value from ReplaceAll
		};
	
	static const ReplaceTest tests[] =
	{
		// Test 1
		{
		_L8("(qu|[b-df-hj-np-tv-z]*)([a-z]+)"),
		_L8("\\2\\1ay"),
		_L8("the quick brown fox jumps over the lazy dogs."),
		_L8("ethay quick brown fox jumps over the lazy dogs."),
		_L8("ethay ickquay ownbray oxfay umpsjay overay ethay azylay ogsday."),
		9
		},
		// Test 2
		{
		_L8("\\w+"),
		_L8("\\0-NOSPAM"),
		_L8("paul.haahr@google.com"),
		_L8("paul-NOSPAM.haahr@google.com"),
		_L8("paul-NOSPAM.haahr-NOSPAM@google-NOSPAM.com-NOSPAM"),
		4
		},
		// Test 3
		{_L8("^"),
		_L8("(START)"),
		_L8("foo"),
		_L8("(START)foo"),
		_L8("(START)foo"),
		1
		},
		// Test 4
		{
		_L8("^"),
		_L8("(START)"),
		_L8(""),
		_L8("(START)"),
		_L8("(START)"),
		1
		},
		// Test 5
		{
		_L8("$"),
		_L8("(END)"),
		_L8(""),
		_L8("(END)"),
		_L8("(END)"),
		1
		},
		// Test 6
		{
		_L8("b"),
		_L8("bb"),
		_L8("ababababab"),
		_L8("abbabababab"),
		_L8("abbabbabbabbabb"),
		5
		},
		// Test 7
		{
		_L8("b"),
		_L8("bb"),
		_L8("bbbbbb"),
		_L8("bbbbbbb"),
		_L8("bbbbbbbbbbbb"),
		6
		},
		// Test 8
		{_L8("b+"),
		_L8("bb"),
		_L8("bbbbbb"),
		_L8("bb"),
		_L8("bb"),
		1
		},
		// Test 9
		{
		_L8("b*"),
		_L8("bb"),
		_L8("bbbbbb"),
		_L8("bb"),
		_L8("bb"),
		1
		},
		// Test 10
		{
		_L8("b*"),
		_L8("bb"),
		_L8("aaaaa"),
		_L8("bbaaaaa"),
		_L8("bbabbabbabbabbabb"),
		6
		},
		// Test 11
		{
		_L8("b*"),
		_L8("bb"),
		_L8("aa\naa\n"),
		_L8("bbaa\naa\n"),
		_L8("bbabbabb\nbbabbabb\nbb"),
		7
		},
		// Test 12
		{
		_L8("b*"),
		_L8("bb"),
		_L8("aa\raa\r"),
		_L8("bbaa\raa\r"),
		_L8("bbabbabb\rbbabbabb\rbb"),
		7
		},
		// Test 13
		{
		_L8("b*"),
		_L8("bb"),
		_L8("aa\r\naa\r\n"),
		_L8("bbaa\r\naa\r\n"),
		_L8("bbabbabb\r\nbbabbabb\r\nbb"),
		7
		},
#ifdef SUPPORT_UTF8
		// Test 14
		{	
		_L8("b*"),
		_L8("bb"),
		_L8("\xE3\x83\x9B\xE3\x83\xBC\xE3\x83\xA0\xE3\x81\xB8"),   // utf8
		_L8("bb\xE3\x83\x9B\xE3\x83\xBC\xE3\x83\xA0\xE3\x81\xB8"),
		_L8("bb\xE3\x83\x9B""bb""\xE3\x83\xBC""bb""\xE3\x83\xA0""bb""\xE3\x81\xB8""bb"),
		5
		},
		// Test 15
		{
		_L8("b*"),
		_L8("bb"),
		_L8("\xE3\x83\x9B\r\n\xE3\x83\xBC\r\xE3\x83\xA0\n\xE3\x81\xB8\r\n"),   // utf8
		_L8("bb\xE3\x83\x9B\r\n\xE3\x83\xBC\r\xE3\x83\xA0\n\xE3\x81\xB8\r\n"),
		_L8("bb\xE3\x83\x9B""bb\r\nbb""\xE3\x83\xBC""bb\rbb""\xE3\x83\xA0""bb\nbb""\xE3\x81\xB8""bb\r\nbb"),
		9
		},
#endif
		{
		KNullDesC8(),
		KNullDesC8(),
		KNullDesC8(),
		KNullDesC8(),
		KNullDesC8(),
		-1
		}
	};

#ifdef SUPPORT_UTF8
	const TBool supportUtf8 = ETrue;
#else
	const TBool supportUtf8 = EFalse;
#endif
  
	CRegEx* re = NULL;
	TRegExOptions options = TRegExOptions(EPcreNewlineCrLf);
	options.SetUtf8(supportUtf8);	
	TInt i =0;
	for (const ReplaceTest* t = tests; t->iGlobalCount > -1; ++t)
		{		
		console->Printf(_L("Replace Test %d\n"), ++i);
		re = CRegEx::NewLC(t->iRegExp, options);
		CHECK_EQ(re->Error(), KErrNone);
		TBuf8<64> one(t->iOriginal);
		CHECK(re->ReplaceL(t->iRewrite, one));
		CHECK_EQ(one, t->iSingle);
		TBuf8<64> all(t->iOriginal);
		const TInt replaceCount = re->GlobalReplaceL(t->iRewrite, all);
		CHECK_EQ(all, t->iGlobal);
		CHECK_EQ(replaceCount, t->iGlobalCount);		
		CleanupStack::PopAndDestroy(re);
		re = NULL;
		}

	// One final test: test \r\n replacement when we're not in CRLF mode
	{
	TRegExOptions options2 = TRegExOptions(EPcreNewlineCr);
	options2.SetUtf8(supportUtf8);
	re = CRegEx::NewLC(_L8("b*"), options2);
	CHECK_EQ(re->Error(), KErrNone);
	TBuf8<32> all(_L8("aa\r\naa\r\n"));
	CHECK_EQ(re->GlobalReplaceL(_L8("bb"), all), 9);
	CHECK_EQ(all, _L8("bbabbabb\rbb\nbbabbabb\rbb\nbb"));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	
	}
	{
	TRegExOptions options3 = TRegExOptions(EPcreNewlineLf);
	options3.SetUtf8(supportUtf8);
	re = CRegEx::NewLC(_L8("b*"), options3);
	CHECK_EQ(re->Error(), KErrNone);
	TBuf8<32> all(_L8("aa\r\naa\r\n"));
	CHECK_EQ(re->GlobalReplaceL(_L8("bb"), all), 9);
	CHECK_EQ(all, _L8("bbabbabb\rbb\nbbabbabb\rbb\nbb"));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	
	}

	// TODO: test what happens when no PCRE_NEWLINE_* flag is set.
	//       Alas, the answer depends on how pcre was compiled.
}

static void TestExtractL()
	{
	console->Write(_L("Testing ExtractL\n"));

	TBuf8<32> s;	
	CRegEx* re = NULL;
	
	re = CRegEx::NewLC(_L8("(.*)@([^.]*)"));
	CHECK(re->ExtractL(_L8("\\2!\\1"), _L8("boris@kremvax.ru"), s));
	CHECK_EQ(s, _L8("kremvax!boris"));
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	// check the RE interface as well
	re = CRegEx::NewLC(_L8(".*"));
	CHECK(re->ExtractL(_L8("'\\0'"), _L8("foo"), s));
	CHECK_EQ(s, _L8("'foo'"));
	CleanupStack::PopAndDestroy(re);
	re = NULL;
	
	re = CRegEx::NewLC(_L8("bar"));
	CHECK(!re->ExtractL(_L8("'\\0'"), _L8("baz"), s));
	CHECK_EQ(s, _L8("'foo'"));
	CleanupStack::PopAndDestroy(re);
	}

static void TestConsumeL()
	{
	console->Write(_L("Testing ConsumeL\n"));

	TBuf8<3> word;
	TBuf8<24> input(_L8("   aaa b!@#$@#$cccc"));

	// matches a word, possibly proceeded by whitespace
	CRegEx* re = CRegEx::NewLC(_L8("\\s*(\\w+)"));
	CHECK(re->ConsumeL(input, &word));
	CHECK_EQ(word, _L8("aaa"));
	CHECK(re->ConsumeL(input, &word));
	CHECK_EQ(word, _L8("b"));
	CHECK(!re->ConsumeL(input, &word));
	CleanupStack::PopAndDestroy(re);
	}

static void TestFindAndConsumeL()
	{
	console->Write(_L("Testing FindAndConsumeL\n"));

	TBuf8<4> word;
	TBuf8<24> input(_L8("   aaa b!@#$@#$cccc"));

	CRegEx* re = CRegEx::NewLC(_L8("(\\w+)")); // matches a word
	CHECK(re->FindAndConsumeL(input, &word));
	CHECK_EQ(word, _L8("aaa"));
	CHECK(re->FindAndConsumeL(input, &word));
	CHECK_EQ(word, _L8("b"));
	CHECK(re->FindAndConsumeL(input, &word));
	CHECK_EQ(word, _L8("cccc"));
	CHECK(!re->FindAndConsumeL(input, &word));
	CleanupStack::PopAndDestroy(re);
	}

static void TestMatchNumberPeculiarityL()
	{
	console->Write(_L("Testing match-number peculiarity\n"));

	TBuf8<3> word1;
	TBuf8<3> word2;
	TBuf8<3> word3;

	CRegEx* re = CRegEx::NewLC(_L8("(foo)|(bar)|(baz)"));
	CHECK(re->PartialMatchL(_L8("foo"), &word1, &word2, &word3));
	CHECK_EQ(word1, _L8("foo"));
	CHECK_EQ(word2, KNullDesC8());
	CHECK_EQ(word3, KNullDesC8());
	CHECK(re->PartialMatchL(_L8("bar"), &word1, &word2, &word3));
	CHECK_EQ(word1, KNullDesC8());
	CHECK_EQ(word2, _L8("bar"));
	CHECK_EQ(word3, KNullDesC8());
	CHECK(re->PartialMatchL(_L8("baz"), &word1, &word2, &word3));
	CHECK_EQ(word1, KNullDesC8());
	CHECK_EQ(word2, KNullDesC8());
	CHECK_EQ(word3, _L8("baz"));
	CHECK(!re->PartialMatchL(_L8("f"), &word1, &word2, &word3));

	CleanupStack::PopAndDestroy(re);
	re = NULL;

	TBuf8<12> a;
	re = CRegEx::NewLC(_L8("(foo)|hello"));
	CHECK(re->FullMatchL(_L8("hello"), &a));
	CHECK_EQ(a, KNullDesC8());
	CleanupStack::PopAndDestroy(re);
	}

static void TestRecursionL()
	{
	console->Write(_L("Testing recursion\n"));
	
	// Get one string that passes (sometimes), one that never does.
	TBuf8<12> good(_L8("abcdefghijk"));
	TBuf8<12> bad(_L8("acdefghijkl"));

	// According to pcretest, matching text_good against (\w+)*b
	// requires match_limit of at least 8192, and match_recursion_limit
	// of at least 37.

	TRegExOptions optionsMatchLimit;
	optionsMatchLimit.SetMatchLimit(8192);
	
	CRegEx* re = CRegEx::NewLC(_L8("(\\w+)*b"), optionsMatchLimit);
	CHECK(re->PartialMatchL(good));
	CHECK(re->PartialMatchL(bad) == EFalse);
	CHECK(re->FullMatchL(good) == EFalse);
	CHECK(re->FullMatchL(bad) == EFalse);	
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	optionsMatchLimit.SetMatchLimit(1024);
	CRegEx* re2 = CRegEx::NewLC(_L8("(\\w+)*b"), optionsMatchLimit);
	CHECK(re2->PartialMatchL(good) == EFalse);   // because of match_limit
	CHECK(re2->PartialMatchL(bad) == EFalse);
	CHECK(re2->FullMatchL(good) == EFalse);
	CHECK(re2->FullMatchL(bad) == EFalse);
	CleanupStack::PopAndDestroy(re2);
	re2 = NULL;
	
	TRegExOptions optionsMathLimitRecursion;
	optionsMathLimitRecursion.SetMatchLimitRecursion(50);
	CRegEx* re3 = CRegEx::NewLC(_L8("(\\w+)*b"), optionsMathLimitRecursion);
	CHECK(re3->PartialMatchL(good));
	CHECK(re3->PartialMatchL(bad) == EFalse);
	CHECK(re3->FullMatchL(good) == EFalse);
	CHECK(re3->FullMatchL(bad) == EFalse);
	CleanupStack::PopAndDestroy(re3);
	re3 = NULL;
	
	optionsMathLimitRecursion.SetMatchLimitRecursion(10);
	CRegEx* re4 = CRegEx::NewLC(_L8("(\\w+)*b"), optionsMathLimitRecursion);
	CHECK(re4->PartialMatchL(good) == EFalse);
	CHECK(re4->PartialMatchL(bad) == EFalse);
	CHECK(re4->FullMatchL(good) == EFalse);
	CHECK(re4->FullMatchL(bad) == EFalse);
	CleanupStack::PopAndDestroy(re4);
	re4 = NULL;	
	}

// A meta-quoted string, interpreted as a pattern, should always match
// the original unquoted string.
static void TestQuoteMetaL(const TDesC8& aUnquoted, TRegExOptions aOptions =
		TRegExOptions())
	{
	HBufC8* quoted = CRegEx::QuoteMetaL(aUnquoted);
	CleanupStack::PushL(quoted);

	CRegEx* re = CRegEx::NewLC(*quoted, aOptions);
	CHECK(re->FullMatchL(aUnquoted));
	CleanupStack::PopAndDestroy(2, quoted); // re
	}

// A string containing meaningful regexp characters, which is then meta-
// quoted, should not generally match a string the unquoted string does.
static void NegativeTestQuoteMetaL(const TDesC8& aUnquoted,
		const TDesC8& aNotMatch, TRegExOptions aOptions = TRegExOptions())
	{
	HBufC8* quoted = CRegEx::QuoteMetaL(aUnquoted);
	CleanupStack::PushL(quoted);

	CRegEx* re = CRegEx::NewLC(*quoted, aOptions);
	CHECK(!re->FullMatchL(aNotMatch));
	CleanupStack::PopAndDestroy(2, quoted); // re
	}

// Tests that quoted meta characters match their original strings,
// and that a few things that shouldn't match indeed do not.
static void TestQuotaMetaSimpleL()
	{
	TestQuoteMetaL(_L8("foo"));
	TestQuoteMetaL(_L8("foo.bar"));
	TestQuoteMetaL(_L8("foo\\.bar"));
	TestQuoteMetaL(_L8("[1-9]"));
	TestQuoteMetaL(_L8("1.5-2.0?"));
	TestQuoteMetaL(_L8("\\d"));
	TestQuoteMetaL(_L8("Who doesn't like ice cream?"));
	TestQuoteMetaL(_L8("((a|b)c?d*e+[f-h]i)"));
	TestQuoteMetaL(_L8("((?!)xxx).*yyy"));
	TestQuoteMetaL(_L8("(["));
	TestQuoteMetaL(_L8("foo\0bar"));
	}

static void TestQuoteMetaSimpleNegativeL()
	{
	NegativeTestQuoteMetaL(_L8("foo"), _L8("bar"));
	NegativeTestQuoteMetaL(_L8("..."), _L8("bar"));
	NegativeTestQuoteMetaL(_L8("\\."), _L8("."));
	NegativeTestQuoteMetaL(_L8("\\."), _L8(".."));
	NegativeTestQuoteMetaL(_L8("(a)"), _L8("a"));
	NegativeTestQuoteMetaL(_L8("(a|b)"), _L8("a"));
	NegativeTestQuoteMetaL(_L8("(a|b)"), _L8("(a)"));
	NegativeTestQuoteMetaL(_L8("(a|b)"), _L8("a|b"));
	NegativeTestQuoteMetaL(_L8("[0-9]"), _L8("0"));
	NegativeTestQuoteMetaL(_L8("[0-9]"), _L8("0-9"));
	NegativeTestQuoteMetaL(_L8("[0-9]"), _L8("[9]"));
	NegativeTestQuoteMetaL(_L8("((?!)xxx)"), _L8("xxx"));
	}

static void TestQuoteMetaLatin1L()
	{
	TestQuoteMetaL(_L8("3\xb2 = 9"));
	}


static void TestQuoteMetaUtf8L()
	{
#ifdef SUPPORT_UTF8
	TRegExOptions options;
	options.SetUtf8(ETrue);
	TestQuoteMetaL(_L8("Pl\xc3\xa1\x63ido Domingo"), options);
	TestQuoteMetaL(_L8("xyz"), options);            // No fancy utf8
	TestQuoteMetaL(_L8("\xc2\xb0"), options);       // 2-byte utf8 (degree symbol)
	TestQuoteMetaL(_L8("27\xc2\xb0 degrees"), options);  // As a middle character
	TestQuoteMetaL(_L8("\xe2\x80\xb3"), options);   // 3-byte utf8 (double prime)
	TestQuoteMetaL(_L8("\xf0\x9d\x85\x9f"), options); // 4-byte utf8 (music note)
	TestQuoteMetaL(_L8("27\xc2\xb0")); // Interpreted as Latin-1, but should still work
	NegativeTestQuoteMetaL(_L8("27\xc2\xb0"),               // 2-byte utf (degree symbol)
			_L8("27\\\xc2\\\xb0"),
			options);
#endif
	}


static void TestQuoteMetaAllL()
	{
	console->Write(_L("Testing QuoteMeta\n"));

	TestQuotaMetaSimpleL();
	TestQuoteMetaSimpleNegativeL();
	TestQuoteMetaLatin1L();
	TestQuoteMetaUtf8L();
	}

//
// Options tests contributed by
// Giuseppe Maxia, CTO, Stardata s.r.l.
// July 2005
//
static void GetOneOptionResultL(
                const TDesC& aOptionName,
                const TDesC8& aRegEx,
                const TDesC8& aString,
                TRegExOptions& aOptions,
                TBool aFull,
                const TDesC8& aExpected)
	{
	console->Printf(_L("Testing Option <%S>\n"), &aOptionName);
	
	if(verboseMode)
		{
		TBuf<32> str;
		str.Copy(aString);
		TBuf<32> rx;
		rx.Copy(aRegEx);
		TBuf<32> expected;
		expected.Copy(aExpected);

		console->Printf(_L("/%S/ finds \"%S\" within \"%S\" \n"), &rx, &expected, &str);
		}
	
  TBuf8<32> captured;
  CRegEx* re = CRegEx::NewLC(aRegEx, aOptions);
  
  if (aFull)
	  {
	  CHECK(re->FullMatchL(aString, &captured));
	  }    
  else
	  {
	  CHECK(re->PartialMatchL(aString, &captured));
	  }   
  CHECK_EQ(captured, aExpected);
  
  CleanupStack::PopAndDestroy(re);
}

static void TestOneOptionL(
                const TDesC& aOptionName,
                const TDesC8& aRegEx,
                const TDesC8& aString,
                const TRegExOptions& aOptions,
                TBool aFull,
                TBool aAssertive = ETrue)
	{
	
	console->Printf(_L("Testing Option <%S>\n"), &aOptionName);
	if(verboseMode)
		{
		_LIT(KMatches, "matches");
		_LIT(KNoMatch, "doesn't match");
		TBuf<32> str;
		str.Copy(aString);
		TBuf<64> rx;
		rx.Copy(aRegEx);
		
		console->Printf(_L("'%S' %S /%S/ \n"),
	                  &str,
	                  (aAssertive? &KMatches() : &KNoMatch()),
	                  &rx);	
		}

	CRegEx* re = NULL;
	TRAPD(err, re = CRegEx::NewLC(aRegEx, aOptions); CleanupStack::Pop());
	
	if(err)
		{
		CHECK_EQ(aAssertive, EFalse);
		return;
		}
	
	CleanupStack::PushL(re);
	
	if (aAssertive)
		{
		if (aFull)
			{
			CHECK(re->FullMatchL(aString));
			}     
		else
			{
			CHECK(re->PartialMatchL(aString));
			}
		}
	else
		{
		if (aFull)
			{
			CHECK(!re->FullMatchL(aString));
			}		  
		else
			{
			CHECK(!re->PartialMatchL(aString));
			}
		}
	CleanupStack::PopAndDestroy(re);	
	}

static void TestCaseLessL()
	{
	TRegExOptions options;
	TRegExOptions options2;
	TRegExOptions caseless;
	caseless.SetCaseless(ETrue);

	options.SetCaseless(ETrue);
	TestOneOptionL(_L("CASELESS (class)"), _L8("HELLO"), _L8("hello"), options, EFalse);
	TestOneOptionL(_L("CASELESS (class2)"), _L8("HELLO"), _L8("hello"), options2.SetCaseless(ETrue), EFalse);
	TestOneOptionL(_L("CASELESS (class)"), _L8("^[A-Z]+$"), _L8("Hello"), options, EFalse);

	TestOneOptionL(_L("CASELESS (function)"), _L8("HELLO"), _L8("hello"), caseless, EFalse);
	TestOneOptionL(_L("CASELESS (function)"), _L8("^[A-Z]+$"), _L8("Hello"), caseless, EFalse);
	options.SetCaseless(EFalse);
	TestOneOptionL(_L("no CASELESS"), _L8("HELLO"), _L8("hello"), options, EFalse, EFalse);
	}

static void TestMultilineL()
	{
	TRegExOptions options;
	TRegExOptions options2;
	TRegExOptions multiline;
	multiline.SetMultiline(ETrue);

	TBuf8<32> str(_L8("HELLO\n" "cruel\n" "world\n"));

	options.SetMultiline(ETrue);
	TestOneOptionL(_L("MULTILINE (class)"), _L8("^cruel$"), str, options, EFalse);
	TestOneOptionL(_L("MULTILINE (class2)"), _L8("^cruel$"), str, options2.SetMultiline(ETrue), EFalse);
	TestOneOptionL(_L("MULTILINE (function)"), _L8("^cruel$"), str, multiline, EFalse);
	options.SetMultiline(EFalse);
	TestOneOptionL(_L("no MULTILINE"), _L8("^cruel$"), str, options, EFalse, EFalse);
	}

static void TestDotAllL()
	{
	TRegExOptions options;
	TRegExOptions options2;
	TRegExOptions dotAll;
	dotAll.SetDotAll(ETrue);

	TBuf8<32> str(_L8("HELLO\n" "cruel\n" "world"));

	options.SetDotAll(ETrue);
	TestOneOptionL(_L("DOTALL (class)"), _L8("HELLO.*world"),str, options, ETrue);
	TestOneOptionL(_L("DOTALL (class2)"), _L8("HELLO.*world"),str, options2.SetDotAll(ETrue), ETrue);
	TestOneOptionL(_L("DOTALL (function)"), _L8("HELLO.*world"),str, dotAll, ETrue);
	options.SetDotAll(EFalse);
	TestOneOptionL(_L("no DOTALL"), _L8("HELLO.*world"),str, options, ETrue, EFalse);
	}

static void TestDollarEndOnlyL()
	{
	TRegExOptions options;
	TRegExOptions options2;
	TRegExOptions dollarEndOnly;
	dollarEndOnly.SetDollarEndOnly(ETrue);

	TBuf8<32> str(_L8("HELLO world\n"));

	TestOneOptionL(_L("no DOLLAR_ENDONLY"), _L8("world$"), str, options, EFalse);
	options.SetDollarEndOnly(ETrue);
	TestOneOptionL(_L("DOLLAR_ENDONLY 1"), _L8("world$"), str, options, EFalse, EFalse);
	TestOneOptionL(_L("DOLLAR_ENDONLY 2"), _L8("world$"), str, options2.SetDollarEndOnly(ETrue), EFalse, EFalse);
	}


static void TestExtraL()
	{	
	TRegExOptions options;

	TBuf8<32> str(_L8("HELLO"));
	options.SetExtra(ETrue);
	TestOneOptionL(_L("EXTRA 1"), _L8("\\HELL\\O"), str, options, ETrue, EFalse );
	TestOneOptionL(_L("EXTRA 2"), _L8("\\HELL\\O"), str, TRegExOptions().SetExtra(ETrue), ETrue, EFalse );
	options.SetExtra(EFalse);
	TestOneOptionL(_L("no EXTRA"), _L8("\\HELL\\O"), str, options, ETrue );
	}

static void TestExtendedL()
	{
	TRegExOptions options;
	TRegExOptions options2;
	TRegExOptions extended;
	extended.SetExtended(ETrue);

	TBuf8<32> str(_L8("HELLO world"));

	options.SetExtended(ETrue);
	TestOneOptionL(_L("EXTENDED (class)"), _L8("HELLO world"), str, options, EFalse, EFalse);
	TestOneOptionL(_L("EXTENDED (class2)"), _L8("HELLO world"), str, options2.SetExtended(ETrue), EFalse, EFalse);
	TestOneOptionL(_L("EXTENDED (class)"), _L8("^ HE L{2} O \\s+        \\w+ $      "), str, options, EFalse);

	TestOneOptionL(_L("EXTENDED (function)"), _L8("HELLO world"), str, extended, EFalse, EFalse);
	TestOneOptionL(_L("EXTENDED (function)"), _L8("^ HE L{2} O \\s+        \\w+ $      "), str, extended, EFalse);

	options.SetExtended(EFalse);
	TestOneOptionL(_L("no EXTENDED"), _L8("HELLO world"), str, options, EFalse);
	}

static void TestNoAutoCaptureL()
	{
	TRegExOptions options;
	TBuf8<32> str(_L8("HELLO world"));
	TBuf8<5> captured;
	console->Write(_L("Testing Option <no NO_AUTO_CAPTURE>\n"));
	if (verboseMode)
		{
		console->Write(_L("parentheses capture text\n"));
		}

	CRegEx* re = CRegEx::NewLC(_L8("(world|universe)$"), options);
	CHECK(re->ExtractL(_L8("\\1"), str , captured));
	CHECK_EQ(captured, _L8("world"));
	options.SetNoAutoCapture(ETrue);
	console->Write(_L("testing Option <NO_AUTO_CAPTURE>\n"));

	if (verboseMode)
		{
		console->Write(_L("parentheses do not capture text\n"));
		}

	re->ExtractL(_L8("\\1"),str, captured);
	CHECK_EQ(captured, _L8("world"));
	
	CleanupStack::PopAndDestroy(re);
	}

static void TestUngreedyL()
	{
	TRegExOptions options;
	TRegExOptions ungreedy;
	ungreedy.SetUngreedy(ETrue);

	TBuf8<32> str(_L8("HELLO, 'this' is the 'world'"));

	options.SetUngreedy(ETrue);
	GetOneOptionResultL(_L("UNGREEDY 1"), _L8("('.*')"), str, options, EFalse, _L8("'this'") );
	GetOneOptionResultL(_L("UNGREEDY 2"), _L8("('.*')"), str, ungreedy, EFalse, _L8("'this'") );
	GetOneOptionResultL(_L("UNGREEDY"), _L8("('.*?')"), str, options, EFalse, _L8("'this' is the 'world'") );

	options.SetUngreedy(EFalse);
	GetOneOptionResultL(_L("no UNGREEDY"), _L8("('.*')"), str, options, EFalse, _L8("'this' is the 'world'") );
	GetOneOptionResultL(_L("no UNGREEDY"), _L8("('.*?')"), str, options, EFalse, _L8("'this'") );
	}

static void TestAllOptionsL()
	{
	TBuf8<32> str(_L8("HELLO\n" "cruel\n" "world"));
	TRegExOptions options;
	options.SetAllOptions(EPcreCaseless | EPcreDotAll);

	TestOneOptionL(_L("all_options (CASELESS|DOTALL)"), _L8("^hello.*WORLD"), str , options, EFalse);
	options.SetAllOptions(0);
	TestOneOptionL(_L("all_options (0)"), _L8("^hello.*WORLD"), str , options, EFalse, EFalse);
	options.SetAllOptions(EPcreMultiline | EPcreExtended);

	TestOneOptionL(_L("all_options (MULTILINE|EXTENDED)"), _L8(" ^ c r u e l $ "), str, options, EFalse);
	TestOneOptionL(_L("all_options (MULTILINE|EXTENDED) with constructor"),
	_L8(" ^ c r u e l $ "),
	str,
	TRegExOptions(EPcreMultiline | EPcreExtended),
	EFalse);

	TRegExOptions multilineExtended = TRegExOptions().SetMultiline(ETrue);
	multilineExtended.SetExtended(ETrue);

	TestOneOptionL(_L("all_options (MULTILINE|EXTENDED) with concatenation"),
	_L8(" ^ c r u e l $ "),
	str,
	multilineExtended, EFalse);

	options.SetAllOptions(0);
	TestOneOptionL(_L("all_options (0)"), _L8("^ c r u e l $"), str, options, EFalse, EFalse);
	}

static void TestOptionsL()
	{
	console->Write(_L("Testing Options\n"));
	TestCaseLessL();
	TestMultilineL();
	TestDotAllL();
	TestDollarEndOnlyL();
	TestExtendedL();
	TestNoAutoCaptureL();
	TestUngreedyL();
	TestExtraL();
	TestAllOptionsL();
	}


LOCAL_C void MainL()
	{
	
	_LIT(KTimingArg1, "timing1");
	_LIT(KTimingArg2, "timing2");
	_LIT(KTimingArg3, "timing3");
	
	CCommandLineArguments* args = CCommandLineArguments::NewLC();

	// Treat any flag as --help
	if(args->Count() > 1 && args->Arg(1)[0] == '-')
		{
		console->Printf(_L("Usage: %S [%S|%S|%S num-iters]\n"), &(args->Arg(0)), &KTimingArg1(), &KTimingArg2(), &KTimingArg3());
		console->Write(_L("If 'timingX ###' is specified, run the given timing test\n"));
		console->Write(_L("with the given number of iterations, rather than running\n"));
		console->Write(_L("the default corectness test.\n"));
		CleanupStack::PopAndDestroy(args);
		return;		
		}
	if(args->Count() > 1)
		{
		if(args->Count() == 2)
			{
			console->Write(_L("timing mode needs a num-iters argument\n"));
			CleanupStack::PopAndDestroy(args);
			return;			
			}
		
		TLex lex(args->Arg(2));
		TInt iters;
		TInt err = lex.Val(iters);
		
		if(err || iters == 0)
			{
			CleanupStack::PopAndDestroy(args);
			return;	
			}
		
		if(args->Arg(1) == KTimingArg1)
			{
			Timing1L(iters);
			}
		else if(args->Arg(1) == KTimingArg2)
			{
			Timing2L(iters);
			}
		else if(args->Arg(1) == KTimingArg3)
			{
			Timing3L(iters);
			}		
		else
			{
			console->Printf(_L("Unknown argument '%S'\n"), &(args->Arg(1)));
			}
		CleanupStack::PopAndDestroy(args);
		return;			
		}
	CleanupStack::PopAndDestroy(args);
	
	console->Write(_L("Testing FullMatch\n"));

	CRegEx* re = NULL;
	TInt i;
	TBuf8<10> s;
	
	/***** FullMatch with no args *****/
	re = CRegEx::NewLC(_L8("h.*o"));
	CHECK(re->FullMatchL(_L8("hello")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	re = CRegEx::NewLC(_L8("h.*o")); // Must be anchored at front
	CHECK(!re->FullMatchL(_L8("othello")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	re = CRegEx::NewLC(_L8("h.*o")); // Must be anchored at end
	CHECK(!re->FullMatchL(_L8("hello!")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	re = CRegEx::NewLC(_L8("a*")); // Fullmatch with normal op
	CHECK(re->FullMatchL(_L8("aaaa")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	re = CRegEx::NewLC(_L8("a*?")); // Fullmatch with nongreedy op
	CHECK(re->FullMatchL(_L8("aaaa")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	re = CRegEx::NewLC(_L8("a*?\\z")); // Two unusual ops
	CHECK(re->FullMatchL(_L8("aaaa")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	/***** FullMatch with args *****/

	// Zero-arg
	re = CRegEx::NewLC(_L8("\\d+"));
	CHECK(re->FullMatchL(_L8("1001")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	

	// Single-arg
	re = CRegEx::NewLC(_L8("(\\d+)"));
	CHECK(re->FullMatchL(_L8("1001"), &i));
	CHECK_EQ(i, 1001);
	CleanupStack::PopAndDestroy(re);
	re = NULL;	
	
	re = CRegEx::NewLC(_L8("(-?\\d+)"));
	CHECK(re->FullMatchL(_L8("-123"), &i));
	CHECK_EQ(i, -123);
	CleanupStack::PopAndDestroy(re);
	re = NULL;
	
	re = CRegEx::NewLC(_L8("()\\d+"));
	CHECK(!re->FullMatchL(_L8("10"), &i));
	CleanupStack::PopAndDestroy(re);
	re = NULL;
	
	re = CRegEx::NewLC(_L8("(\\d+)"));
	CHECK(!re->FullMatchL(_L8("1234567890123456789012345678901234567890"), &i));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	

	// Digits surrounding integer-arg
	re = CRegEx::NewLC(_L8("1(\\d*)4"));
	CHECK(re->FullMatchL(_L8("1234"), &i));
	CHECK_EQ(i, 23);
	CleanupStack::PopAndDestroy(re);
	re = NULL;
	
	re = CRegEx::NewLC(_L8("(\\d)\\d+"));
	CHECK(re->FullMatchL(_L8("1234"), &i));
	CHECK_EQ(i, 1);
	CleanupStack::PopAndDestroy(re);
	re = NULL;
	
	re = CRegEx::NewLC(_L8("(-\\d)\\d+"));
	CHECK(re->FullMatchL(_L8("-1234"), &i));
	CHECK_EQ(i, -1);
	CleanupStack::PopAndDestroy(re);
	re = NULL;
	
	re = CRegEx::NewLC(_L8("(\\d)"));
	CHECK(re->PartialMatchL(_L8("1234"), &i));
	CHECK_EQ(i, 1);
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	re = CRegEx::NewLC(_L8("(-\\d)"));
	CHECK(re->PartialMatchL(_L8("-1234"), &i));
	CHECK_EQ(i, -1);
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	// String-arg
	re = CRegEx::NewLC(_L8("h(.*)o"));
	CHECK(re->PartialMatchL(_L8("hello"), &s));
	CHECK_EQ(s, _L8("ell"));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	

	// Multi-arg
	re = CRegEx::NewLC(_L8("(\\w+):(\\d+)"));
	CHECK(re->PartialMatchL(_L8("ruby:1234"), &s, &i));
	CHECK_EQ(s.Length(), 4);
	CHECK_EQ(s, _L8("ruby"));
	CHECK_EQ(i, 1234);
	CleanupStack::PopAndDestroy(re);
	re = NULL;		

	// Ignore non-void* NULL arg
	re = CRegEx::NewLC(_L8("he(.*)lo"));
	CHECK(re->FullMatchL(_L8("hello"), (TDes8*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;		
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(re->FullMatchL(_L8("123"), (TInt8*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;			

	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(re->FullMatchL(_L8("12345"), (TInt16*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;			

	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(re->FullMatchL(_L8("1234567890"), (TInt32*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;			

	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(re->FullMatchL(_L8("1234567890123456"), (TInt64*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;		
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(re->FullMatchL(_L8("1234567890"), (TInt*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;		
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(re->FullMatchL(_L8("123"), (TUint8*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;		
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(re->FullMatchL(_L8("12345"), (TUint16*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;		

	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(re->FullMatchL(_L8("1234567890"), (TUint32*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;		
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(re->FullMatchL(_L8("1234567890"), (TUint*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(re->FullMatchL(_L8("123.4567890123456"), (TReal32*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(re->FullMatchL(_L8("123.4567890123456"), (TReal64*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(re->FullMatchL(_L8("123.4567890123456"), (TReal*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	
	
	// Fail on non-void* NULL arg if the match doesn't parse for the given type.
	re = CRegEx::NewLC(_L8("h(.*)lo"));
	CHECK(!re->FullMatchL(_L8("hello"), &s, (TDes8*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(!re->FullMatchL(_L8("hello"), (TInt8*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(!re->FullMatchL(_L8("hello"), (TInt16*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(!re->FullMatchL(_L8("hello"), (TInt32*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	

	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(!re->FullMatchL(_L8("hello"), (TInt64*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(!re->FullMatchL(_L8("hello"), (TInt*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(!re->FullMatchL(_L8("hello"), (TUint8*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(!re->FullMatchL(_L8("hello"), (TUint16*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(!re->FullMatchL(_L8("hello"), (TUint32*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(!re->FullMatchL(_L8("hello"), (TUint*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(!re->FullMatchL(_L8("hello"), (TReal32*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(!re->FullMatchL(_L8("hello"), (TReal64*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(!re->FullMatchL(_L8("hello"), (TReal*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;		
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(!re->FullMatchL(_L8("1234"), (TInt8*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(!re->FullMatchL(_L8("123456"), (TInt16*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(!re->FullMatchL(_L8("12345678901"), (TInt32*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(!re->FullMatchL(_L8("12345678901"), (TInt*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(!re->FullMatchL(_L8("1234"), (TUint8*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	

	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(!re->FullMatchL(_L8("123456"), (TUint16*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(!re->FullMatchL(_L8("12345678901"), (TUint32*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	
	
	re = CRegEx::NewLC(_L8("(.*)"));
	CHECK(!re->FullMatchL(_L8("12345678901"), (TUint*)NULL));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	

	// Ignored arg
	re = CRegEx::NewLC(_L8("(\\w+)(:)(\\d+)"));
	CHECK(re->FullMatchL(_L8("ruby:1234"), &s, (TAny*)NULL, &i));
	CleanupStack::PopAndDestroy(re);
	CHECK_EQ(s, _L8("ruby"));
	CHECK_EQ(i, 1234);
	re = NULL;		

// Type tests
		{
		TChar c;
		re = CRegEx::NewLC(_L8("(H)ello"));
		CHECK(re->FullMatchL(_L8("Hello"), &c));
		CHECK_EQ(c, 'H');
		CleanupStack::PopAndDestroy(re);
		re = NULL;
		}
		{
		TInt8 v;
		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(re->FullMatchL(_L8("100"), &v));
		CHECK_EQ(v, 100);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(re->FullMatchL(_L8("-100"), &v));
		CHECK_EQ(v, -100);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(re->FullMatchL(_L8("127"), &v));
		CHECK_EQ(v, KMaxTInt8);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(re->FullMatchL(_L8("-128"), &v));
		CHECK_EQ(v, KMinTInt8);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(!re->FullMatchL(_L8("128"), &v));
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(!re->FullMatchL(_L8("-129"), &v));
		CleanupStack::PopAndDestroy(re);
		re = NULL;
		}
		{
		TUint8 v;
		re = CRegEx::NewLC(_L8("(\\d+)"));
		CHECK(re->FullMatchL(_L8("100"), &v));
		CHECK_EQ(v, 100);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(\\d+)"));
		CHECK(re->FullMatchL(_L8("128"), &v));
		CHECK_EQ(v, KMaxTInt8 + 1);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(\\d+)"));
		CHECK(re->FullMatchL(_L8("255"), &v));
		CHECK_EQ(v, KMaxTUint8);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(\\d+)"));
		CHECK(!re->FullMatchL(_L8("256"), &v));
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(\\d+)"));
		CHECK(!re->FullMatchL(_L8("-100"), &v));
		CleanupStack::PopAndDestroy(re);
		re = NULL;
		}
		{
		TInt16 v;
		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(re->FullMatchL(_L8("100"), &v));
		CHECK_EQ(v, 100);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(re->FullMatchL(_L8("-100"), &v));
		CHECK_EQ(v, -100);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(re->FullMatchL(_L8("32767"), &v));
		CHECK_EQ(v, KMaxTInt16);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(re->FullMatchL(_L8("-32768"), &v));
		CHECK_EQ(v, KMinTInt16);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(!re->FullMatchL(_L8("32768"), &v));
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(!re->FullMatchL(_L8("-32769"), &v));
		CleanupStack::PopAndDestroy(re);
		re = NULL;
		}
		{
		TUint16 v;
		re = CRegEx::NewLC(_L8("(\\d+)"));
		CHECK(re->FullMatchL(_L8("100"), &v));
		CHECK_EQ(v, 100);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(\\d+)"));
		CHECK(re->FullMatchL(_L8("32767"), &v));
		CHECK_EQ(v, KMaxTInt16);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(\\d+)"));
		CHECK(re->FullMatchL(_L8("65535"), &v));
		CHECK_EQ(v, KMaxTUint16);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(\\d+)"));
		CHECK(!re->FullMatchL(_L8("655356"), &v));
		CleanupStack::PopAndDestroy(re);
		re = NULL;
		
		re = CRegEx::NewLC(_L8("(\\d+)"));
		CHECK(!re->FullMatchL(_L8("-100"), &v));
		CleanupStack::PopAndDestroy(re);
		re = NULL;		
		}		
		{
		TInt32 v;
		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(re->FullMatchL(_L8("100"), &v));
		CHECK_EQ(v, 100);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(re->FullMatchL(_L8("-100"), &v));
		CHECK_EQ(v, -100);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(re->FullMatchL(_L8("2147483647"), &v));
		CHECK_EQ(v, KMaxTInt32);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(re->FullMatchL(_L8("-2147483648"), &v));
		CHECK_EQ(v, KMinTInt32);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(!re->FullMatchL(_L8("2147483648"), &v));
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(!re->FullMatchL(_L8("-2147483649"), &v));
		CleanupStack::PopAndDestroy(re);
		re = NULL;
		}	
		{
		TUint32 v;
		re = CRegEx::NewLC(_L8("(\\d+)"));
		CHECK(re->FullMatchL(_L8("100"), &v));
		CHECK_EQ(v, 100);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(\\d+)"));
		CHECK(re->FullMatchL(_L8("2147483647"), &v));
		CHECK_EQ(v, KMaxTInt32);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(\\d+)"));
		CHECK(re->FullMatchL(_L8("4294967295"), &v));
		CHECK_EQ(v, KMaxTUint32);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(\\d+)"));
		CHECK(!re->FullMatchL(_L8("4294967296"), &v));
		CleanupStack::PopAndDestroy(re);
		re = NULL;
		
		re = CRegEx::NewLC(_L8("(\\d+)"));
		CHECK(!re->FullMatchL(_L8("-100"), &v));
		CleanupStack::PopAndDestroy(re);
		re = NULL;		
		}
		{
		TInt64 v;
		// Note  TLex treats TInt64 as unsigned
		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(re->FullMatchL(_L8("100"), &v));
		CHECK_EQ(v, 100);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(re->FullMatchL(_L8("9223372036854775807"), &v));
		CHECK_EQ(v, KMaxTInt64);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(!re->FullMatchL(_L8("-100"), &v));
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(!re->FullMatchL(_L8("-9223372036854775808"), &v));
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(!re->FullMatchL(_L8("9223372036854775808"), &v));
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(!re->FullMatchL(_L8("-9223372036854775809"), &v));
		CleanupStack::PopAndDestroy(re);
		re = NULL;
		}
		{
		TInt v;
		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(re->FullMatchL(_L8("100"), &v));
		CHECK_EQ(v, 100);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(re->FullMatchL(_L8("-100"), &v));
		CHECK_EQ(v, -100);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(re->FullMatchL(_L8("2147483647"), &v));
		CHECK_EQ(v, KMaxTInt);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(re->FullMatchL(_L8("-2147483648"), &v));
		CHECK_EQ(v, KMinTInt);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(!re->FullMatchL(_L8("2147483648"), &v));
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(-?\\d+)"));
		CHECK(!re->FullMatchL(_L8("-2147483649"), &v));
		CleanupStack::PopAndDestroy(re);
		re = NULL;
		}	
		{
		TUint v;
		re = CRegEx::NewLC(_L8("(\\d+)"));
		CHECK(re->FullMatchL(_L8("100"), &v));
		CHECK_EQ(v, 100);		
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(\\d+)"));
		CHECK(re->FullMatchL(_L8("2147483647"), &v));
		CHECK_EQ(v, KMaxTInt);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(\\d+)"));
		CHECK(re->FullMatchL(_L8("4294967295"), &v));
		CHECK_EQ(v, KMaxTUint);		
		CleanupStack::PopAndDestroy(re);

		re = NULL;

		re = CRegEx::NewLC(_L8("(\\d+)"));
		CHECK(!re->FullMatchL(_L8("4294967296"), &v));
		CleanupStack::PopAndDestroy(re);
		re = NULL;
		
		re = CRegEx::NewLC(_L8("(\\d+)"));
		CHECK(!re->FullMatchL(_L8("-100"), &v));
		CleanupStack::PopAndDestroy(re);
		re = NULL;		
		}		
		{
		TReal32 v;
		re = CRegEx::NewLC(_L8("(.*)"));
		CHECK(re->FullMatchL(_L8("100"), &v));
		CHECK_EQ(v, 100);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(.*)"));
		CHECK(re->FullMatchL(_L8("-100"), &v));
		CHECK_EQ(v, -100);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(.*)"));
		CHECK(re->FullMatchL(_L8("3.4028234663852885981170418348452e+38"), &v));
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(.*)"));
		CHECK(re->FullMatchL(_L8("-1.17549435E-38f"), &v));
		CleanupStack::PopAndDestroy(re);
		re = NULL;
		}
		{
		TReal64 v;
		re = CRegEx::NewLC(_L8("(.*)"));
		CHECK(re->FullMatchL(_L8("100"), &v));
		CHECK_EQ(v, 100);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(.*)"));
		CHECK(re->FullMatchL(_L8("-100"), &v));
		CHECK_EQ(v, -100);
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(.*)"));
		CHECK(re->FullMatchL(_L8("1.7976931348623157E+308"), &v));
		CleanupStack::PopAndDestroy(re);
		re = NULL;

		re = CRegEx::NewLC(_L8("(.*)"));
		CHECK(re->FullMatchL(_L8("-2.2250738585072015E-308"), &v));
		CleanupStack::PopAndDestroy(re);
		re = NULL;
		}
		
	// Check that matching is fully anchored
	re = CRegEx::NewLC(_L8("(\\d+)"));
	CHECK(!re->FullMatchL(_L8("x1001"), &i));
	CleanupStack::PopAndDestroy(re);
	re = NULL;	
	
	re = CRegEx::NewLC(_L8("(\\d+)"));
	CHECK(!re->FullMatchL(_L8("1001x"), &i));
	CleanupStack::PopAndDestroy(re);
	re = NULL;		
	
	re = CRegEx::NewLC(_L8("x(\\d+)"));
	CHECK(re->FullMatchL(_L8("x1001"), &i));
	CleanupStack::PopAndDestroy(re);
	CHECK_EQ(i, 1001);
	re = NULL;		
	
	re = CRegEx::NewLC(_L8("(\\d+)x"));
	CHECK(re->FullMatchL(_L8("1001x"), &i));
	CleanupStack::PopAndDestroy(re);
	CHECK_EQ(i, 1001);
	re = NULL;
	
	// Braces
	re = CRegEx::NewLC(_L8("[0-9a-f+.-]{5,}"));
	CHECK(re->FullMatchL(_L8("0abcd")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;
	
	re = CRegEx::NewLC(_L8("[0-9a-f+.-]{5,}"));
	CHECK(re->FullMatchL(_L8("0abcde")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;
	
	re = CRegEx::NewLC(_L8("[0-9a-f+.-]{5,}"));
	CHECK(!re->FullMatchL(_L8("0abc")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;
	
	// Complicated RE
	re = CRegEx::NewLC(_L8("foo|bar|[A-Z]"));
	CHECK(re->FullMatchL(_L8("foo")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;
	
	re = CRegEx::NewLC(_L8("foo|bar|[A-Z]"));
	CHECK(re->FullMatchL(_L8("bar")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;
	
	re = CRegEx::NewLC(_L8("foo|bar|[A-Z]"));
	CHECK(re->FullMatchL(_L8("X")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;
	
	re = CRegEx::NewLC(_L8("foo|bar|[A-Z]"));
	CHECK(!re->FullMatchL(_L8("XY")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;
	
	// Check full-match handling (needs '$' tacked on internally)
	re = CRegEx::NewLC(_L8("fo|foo"));
	CHECK(re->FullMatchL(_L8("fo")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	re = CRegEx::NewLC(_L8("fo|foo"));
	CHECK(re->FullMatchL(_L8("foo")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	re = CRegEx::NewLC(_L8("fo|foo$"));
	CHECK(re->FullMatchL(_L8("fo")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	re = CRegEx::NewLC(_L8("fo|foo$"));
	CHECK(re->FullMatchL(_L8("foo")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	re = CRegEx::NewLC(_L8("foo$"));
	CHECK(re->FullMatchL(_L8("foo")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	re = CRegEx::NewLC(_L8("foo\\$"));
	CHECK(!re->FullMatchL(_L8("foo$bar")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	re = CRegEx::NewLC(_L8("fo|bar"));
	CHECK(!re->FullMatchL(_L8("fox")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	// Uncomment the following if we change the handling of '$' to
	// prevent it from matching a trailing newline
	if (false)
		{
		// Check that we don't get bitten by pcre's special handling of a
		// '\n' at the end of the string matching '$'
		re = CRegEx::NewLC(_L8("foo$"));
		CHECK(!re->PartialMatchL(_L8("foo\n")));
		CleanupStack::PopAndDestroy(re);
		re = NULL;
		}

	// Number of args
	TInt a[16];

	re = CRegEx::NewLC(_L8(""));
	CHECK(re->FullMatchL(_L8("")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	memset(a, 0, sizeof(0));
	re = CRegEx::NewLC(_L8("(\\d){1}"));
	CHECK(re->FullMatchL(_L8("1"), &a[0]));
	CHECK_EQ(a[0], 1);
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	memset(a, 0, sizeof(0));
	re = CRegEx::NewLC(_L8("(\\d)(\\d)"));
	CHECK(re->FullMatchL(_L8("12"), &a[0], &a[1]));
	CHECK_EQ(a[0], 1);
	CHECK_EQ(a[1], 2);
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	memset(a, 0, sizeof(0));
	re = CRegEx::NewLC(_L8("(\\d)(\\d)(\\d)"));
	CHECK(re->FullMatchL(_L8("123"), &a[0], &a[1], &a[2]));
	CHECK_EQ(a[0], 1);
	CHECK_EQ(a[1], 2);
	CHECK_EQ(a[2], 3);
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	memset(a, 0, sizeof(0));
	re = CRegEx::NewLC(_L8("(\\d)(\\d)(\\d)(\\d)"));
	CHECK(re->FullMatchL(_L8("1234"), &a[0], &a[1], &a[2], &a[3]));
	CHECK_EQ(a[0], 1);
	CHECK_EQ(a[1], 2);
	CHECK_EQ(a[2], 3);
	CHECK_EQ(a[3], 4);
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	// Currently maximum args == 4
	/*
	memset(a, 0, sizeof(0));
	re = CRegEx::NewLC(_L8("(\\d)(\\d)(\\d)(\\d)(\\d)"));
	CHECK(re->FullMatchL(_L8("12345"), &a[0], &a[1], &a[2], &a[3], &a[4]));
	CHECK_EQ(a[0], 1);
	CHECK_EQ(a[1], 2);
	CHECK_EQ(a[2], 3);
	CHECK_EQ(a[3], 4);
	CHECK_EQ(a[4], 5);
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	memset(a, 0, sizeof(0));
	re = CRegEx::NewLC(_L8("(\\d)(\\d)(\\d)(\\d)(\\d)(\\d)"));
	CHECK(re->FullMatchL(_L8("123456"), &a[0], &a[1], &a[2], &a[3], &a[4], &a[5]));
	CHECK_EQ(a[0], 1);
	CHECK_EQ(a[1], 2);
	CHECK_EQ(a[2], 3);
	CHECK_EQ(a[3], 4);
	CHECK_EQ(a[4], 5);
	CHECK_EQ(a[5], 6);
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	memset(a, 0, sizeof(0));
	re = CRegEx::NewLC(_L8("(\\d)(\\d)(\\d)(\\d)(\\d)(\\d)(\\d)"));
	CHECK(re->FullMatchL(_L8("1234567"), &a[0], &a[1], &a[2], &a[3], &a[4], &a[5], &a[6]));
	CHECK_EQ(a[0], 1);
	CHECK_EQ(a[1], 2);
	CHECK_EQ(a[2], 3);
	CHECK_EQ(a[3], 4);
	CHECK_EQ(a[4], 5);
	CHECK_EQ(a[5], 6);
	CHECK_EQ(a[6], 7);
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	memset(a, 0, sizeof(0));
	re
			= CRegEx::NewLC(
					_L8("(\\d)(\\d)(\\d)(\\d)(\\d)(\\d)(\\d)(\\d)(\\d)(\\d)(\\d)(\\d)(\\d)(\\d)(\\d)(\\d)"));
	CHECK(re->FullMatchL(_L8("1234567890123456"), &a[0], &a[1], &a[2], &a[3],
					&a[4], &a[5], &a[6], &a[7],
					&a[8], &a[9], &a[10], &a[11],
					&a[12],&a[13], &a[14], &a[15]));
	CHECK_EQ(a[0], 1);
	CHECK_EQ(a[1], 2);
	CHECK_EQ(a[2], 3);
	CHECK_EQ(a[3], 4);
	CHECK_EQ(a[4], 5);
	CHECK_EQ(a[5], 6);
	CHECK_EQ(a[6], 7);
	CHECK_EQ(a[7], 8);
	CHECK_EQ(a[8], 9);
	CHECK_EQ(a[9], 0);
	CHECK_EQ(a[10], 1);
	CHECK_EQ(a[11], 2);
	CHECK_EQ(a[12], 3);
	CHECK_EQ(a[13], 4);
	CHECK_EQ(a[14], 5);
	CHECK_EQ(a[15], 6);
	CleanupStack::PopAndDestroy(re);
	re = NULL;
	 */
	
	/***** PartialMatch *****/
	console->Write(_L("Testing PartialMatch\n"));

	re = CRegEx::NewLC(_L8("h.*o"));
	CHECK(re->PartialMatchL(_L8("hello")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	re = CRegEx::NewLC(_L8("h.*o"));
	CHECK(re->PartialMatchL(_L8("othello")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	re = CRegEx::NewLC(_L8("h.*o"));
	CHECK(re->PartialMatchL(_L8("hello!")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	re = CRegEx::NewLC(_L8("((((((((((((((((((((x))))))))))))))))))))"));
	CHECK(re->PartialMatchL(_L8("x")));
	CleanupStack::PopAndDestroy(re);
	re = NULL;

	/***** other tests *****/

	RadixTestsL();
	TestReplaceL();
	TestExtractL();
	TestConsumeL();
	TestFindAndConsumeL();
	TestQuoteMetaAllL();
	TestMatchNumberPeculiarityL();
	
	// Check the pattern() accessor
		{
		_LIT8(KPattern, "http://([^/]+)/.*");
		re = CRegEx::NewLC(KPattern());
		CHECK_EQ(KPattern(), re->Pattern());
		CleanupStack::PopAndDestroy(re);
		re = NULL;
		}	
		// Check RE error field.
		{
		re = CRegEx::NewLC(_L8("foo"));
		CHECK_EQ(re->Error(), KErrNone);  // Must have no error
		CleanupStack::PopAndDestroy(re);
		re = NULL;
		}
		
#ifdef SUPPORT_UTF8
	  // Check UTF-8 handling
		{
		console->Write(_L("Testing UTF-8 handling\n"));

		// Three Japanese characters (nihongo)
		TBuf8<9> utf8String(_L8("\xE6\x97\xA5\xE6\x9C\xAC\xE8\xAA\x9E"));
		TBuf8<5> utf8Pattern(_L8(".\xE6\x9C\xAC."));
		TRegExOptions utf8Options;
		utf8Options.SetUtf8(ETrue);	
	
		// Both should match in either mode, bytes or UTF-8
		CRegEx* re_test1 = CRegEx::NewLC(_L8("........."));
		CHECK(re_test1->FullMatchL(utf8String));
		CleanupStack::PopAndDestroy(re_test1);
		re_test1 = NULL;
		
		CRegEx* re_test2 = CRegEx::NewLC(_L8("..."), utf8Options);
		CHECK(re_test2->FullMatchL(utf8String));
		CleanupStack::PopAndDestroy(re_test2);
		re_test2 = NULL;		

		// Check that '.' matches one byte or UTF-8 character
		// according to the mode.
		TBuf8<3> ss;
		
		CRegEx* re_test3 = CRegEx::NewLC(_L8("(.)"));
		CHECK(re_test3->PartialMatchL(utf8String, &ss));
		CHECK_EQ(ss, _L8("\xE6"));
		CleanupStack::PopAndDestroy(re_test3);
		re_test3 = NULL;	
		
		CRegEx* re_test4 = CRegEx::NewLC(_L8("(.)"), utf8Options);
		CHECK(re_test4->PartialMatchL(utf8String, &ss));
		CHECK_EQ(ss, _L8("\xE6\x97\xA5"));
		CleanupStack::PopAndDestroy(re_test4);
		re_test4 = NULL;	
		
		// Check that string matches itself in either mode
		CRegEx* re_test5 = CRegEx::NewLC(utf8String);
		CHECK(re_test5->FullMatchL(utf8String));
		CleanupStack::PopAndDestroy(re_test5);
		re_test5 = NULL;	
		
		CRegEx* re_test6 = CRegEx::NewLC(utf8String, utf8Options);
		CHECK(re_test6->FullMatchL(utf8String));
		CleanupStack::PopAndDestroy(re_test6);
		re_test6 = NULL;			
		
		// Check that pattern matches string only in UTF8 mode
		CRegEx* re_test7 = CRegEx::NewLC(utf8Pattern);
		CHECK(!re_test7->FullMatchL(utf8String));
		CleanupStack::PopAndDestroy(re_test7);
		re_test7 = NULL;
		
		CRegEx* re_test8 = CRegEx::NewLC(utf8Pattern, utf8Options);
		CHECK(re_test8->FullMatchL(utf8String));
		CleanupStack::PopAndDestroy(re_test8);
		re_test8 = NULL;
		}

	// Check that ungreedy, UTF8 regular expressions don't match when they
	// oughtn't -- see bug 82246.
		{
		// This code always worked.
		_LIT8(KPattern, "\\w+X");
		_LIT8(KTarget, "a aX");

		TRegExOptions utf8Options;
		utf8Options.SetUtf8(ETrue);	
		
		CRegEx* matchSentence = CRegEx::NewLC(KPattern());
		CHECK(!matchSentence->FullMatchL(KTarget()));
		CleanupStack::PopAndDestroy(matchSentence);
		matchSentence = NULL;
		
		CRegEx* matchSentenceRe = CRegEx::NewLC(KPattern(), utf8Options);
		CHECK(!matchSentenceRe->FullMatchL(KTarget()));
		CleanupStack::PopAndDestroy(matchSentenceRe);
		matchSentenceRe = NULL;
		}
		{
		_LIT8(KPattern, "(?U)\\w+X");
		_LIT8(KTarget, "a aX");

		TRegExOptions utf8Options;
		utf8Options.SetUtf8(ETrue);	
		
		CRegEx* matchSentence = CRegEx::NewLC(KPattern());
		CHECK(!matchSentence->FullMatchL(KTarget()));
		CleanupStack::PopAndDestroy(matchSentence);
		matchSentence = NULL;
		
		CRegEx* matchSentenceRe = CRegEx::NewLC(KPattern(), utf8Options);
		CHECK(!matchSentenceRe->FullMatchL(KTarget()));
		CleanupStack::PopAndDestroy(matchSentenceRe);
		matchSentenceRe = NULL;
		}
		
#endif  /* def SUPPORT_UTF8 */	

  console->Write(_L("Testing error reporting\n"));
	  {
	  TRAPD(err, re = CRegEx::NewLC(_L8("a\\1")); CleanupStack::PopAndDestroy());
	  CHECK(err != KErrNone);
	  re = NULL;
	  }
	  {
	  TRAPD(err, re = CRegEx::NewLC(_L8("a[x")); CleanupStack::PopAndDestroy());
	  CHECK(err != KErrNone);
	  re = NULL;
	  }
	  {
	  TRAPD(err, re = CRegEx::NewLC(_L8("a[z-a]")); CleanupStack::PopAndDestroy());
	  CHECK(err != KErrNone);
	  re = NULL;
	  }
	  {
	  TRAPD(err, re = CRegEx::NewLC(_L8("a[[:foobar:]]")); CleanupStack::PopAndDestroy());
	  CHECK(err != KErrNone);
	  re = NULL;
	  }
	  {
	  TRAPD(err, re = CRegEx::NewLC(_L8("a(b")); CleanupStack::PopAndDestroy());
	  CHECK(err != KErrNone);
	  re = NULL;
	  }
	  {
	  TRAPD(err, re = CRegEx::NewLC(_L8("a\\")); CleanupStack::PopAndDestroy());
	  CHECK(err != KErrNone);
	  re = NULL;
	  }
	  
	  // Test that recursion is stopped
	  TestRecursionL();
	  
	  // Test Options
	  TestOptionsL();

	  // Done
	  console->Write(_L("OK\n"));
	}
LOCAL_C void DoStartL()
	{
	// Create active scheduler (to run active objects)
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	MainL();

	// Delete active scheduler
	CleanupStack::PopAndDestroy(scheduler);
	}

//  Global Functions

GLDEF_C TInt E32Main()
	{
	// Create cleanup stack
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	// Create output console
	TRAPD(createError, console = Console::NewL(KTextConsoleTitle, TSize(
							KConsFullScreen, KConsFullScreen)));
	if (createError)
		return createError;
	// Run application code inside TRAP harness, wait keypress when terminated
	TRAPD(mainError, DoStartL());
	if (mainError)
		console->Printf(KTextFailed, mainError);
	console->Printf(KTextPressAnyKey);
	console->Getch();

	delete console;
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}
