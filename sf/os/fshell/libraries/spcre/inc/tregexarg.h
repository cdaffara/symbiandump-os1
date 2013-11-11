// tregexarg.h
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

#ifndef TREGEXARG_H_
#define TREGEXARG_H_

#include <e32base.h>

class TRegExArg
	{
 public:
	 // Empty constructor so we can declare arrays of TRegExArg
	  TRegExArg();

	 // Constructor specially designed for NULL arguments
	 inline TRegExArg(TAny*);

	 typedef TBool (*Parser)(const TDesC8& aString, TAny* aDestination);
	 
// Type-specific parsers
#define PCRE_MAKE_PARSER(type,name)															\
	inline TRegExArg(type* aType) : iArg(aType), iParser(name) { }						\
	inline TRegExArg(type* aType, Parser aParser) : iArg(aType), iParser(aParser) { }	
	
	PCRE_MAKE_PARSER(TChar,				ParseTChar)
	PCRE_MAKE_PARSER(TInt8,				ParseTInt8)
	PCRE_MAKE_PARSER(TInt16,			ParseTInt16)
	PCRE_MAKE_PARSER(TInt32,			ParseTInt32)
	PCRE_MAKE_PARSER(TInt64,			ParseTInt64)
	PCRE_MAKE_PARSER(TInt,				ParseTInt)
	PCRE_MAKE_PARSER(TUint8,			ParseTUint8)
	PCRE_MAKE_PARSER(TUint16,			ParseTUint16)
	PCRE_MAKE_PARSER(TUint32,			ParseTUint32)
	PCRE_MAKE_PARSER(TUint,				ParseTUint)
	PCRE_MAKE_PARSER(TReal32,			ParseTReal32)
	PCRE_MAKE_PARSER(TReal64,			ParseTReal64)
	PCRE_MAKE_PARSER(TDes8,				ParseTDes8)
	PCRE_MAKE_PARSER(TPtrC8,			ParseTPtrC8)
	
	#undef PCRE_MAKE_PARSER

	// Generic constructors
	template <class T> inline TRegExArg(T*, Parser aParser);
	template <class T> inline TRegExArg(T* aArg);	
	
	// TBuf constructors - cannot seem to nest the template parameters
	template <TInt S>
	inline TRegExArg(TBuf8<S>* aArg, Parser aParser);
	template <TInt S>
	inline TRegExArg(TBuf8<S>* aArg);
	
	// Parse the data
	inline TBool Parse(const TDesC8& aString) const;

private:
	TAny*         iArg;
	Parser        iParser; // Pointer to parsing function
	
	IMPORT_C static TBool ParseNull			(const TDesC8& aString, TAny* aDestination);
	IMPORT_C static TBool ParseTChar		(const TDesC8& aString, TAny* aDestination);
	IMPORT_C static TBool ParseTInt8		(const TDesC8& aString, TAny* aDestination);
	IMPORT_C static TBool ParseTInt16		(const TDesC8& aString, TAny* aDestination);
	IMPORT_C static TBool ParseTInt32		(const TDesC8& aString, TAny* aDestination);
	IMPORT_C static TBool ParseTInt			(const TDesC8& aString, TAny* aDestination);
	IMPORT_C static TBool ParseTReal32		(const TDesC8& aString, TAny* aDestination);
	IMPORT_C static TBool ParseTReal64		(const TDesC8& aString, TAny* aDestination); 
	IMPORT_C static TBool ParseTDes8		(const TDesC8& aString, TAny* aDestination);
	IMPORT_C static TBool ParseTDes16		(const TDesC8& aString, TAny* aDestination);
	IMPORT_C static TBool ParseTPtrC8		(const TDesC8& aString, TAny* aDestination);

#define PCRE_DECLARE_INTEGER_PARSER(name)															\
private:																							\
	IMPORT_C static TBool Parse ## name(const TDesC8& aString, TAny* aDestination);					\
	IMPORT_C static TBool Parse ## name ## Radix(													\
				const TDesC8& aString, TAny* aDestination, TRadix aRadix);							\
public:																								\
	IMPORT_C static TBool Parse ## name ## Hex(const TDesC8& aString, TAny* aDestination);			\
	IMPORT_C static TBool Parse ## name ## Octal(const TDesC8& aString, TAny* aDestination);		\

	PCRE_DECLARE_INTEGER_PARSER(TInt64)
	PCRE_DECLARE_INTEGER_PARSER(TUint8)
	PCRE_DECLARE_INTEGER_PARSER(TUint16)
	PCRE_DECLARE_INTEGER_PARSER(TUint32)
	PCRE_DECLARE_INTEGER_PARSER(TUint)	

#undef PCRE_DECLARE_INTEGER_PARSER
	};

#include "tregexarg.inl"

#endif /* TREGEXARG_H_ */
