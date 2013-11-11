// tregexoptions.h
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

#ifndef TREGEXOPTIONS_H_
#define TREGEXOPTIONS_H_

#include <e32base.h>

enum TRegExOptionFlags
	{
	EPcreCaseless 			= 0x00000001,
	EPcreMultiline 			= 0x00000002,
	EPcreDotAll 			= 0x00000004,
	EPcreExtended 			= 0x00000008,
	EPcreAnchored 			= 0x00000010,
	EPcreDollarEndOnly 		= 0x00000020,
	EPcreExtra 				= 0x00000040,
	EPcreNotBol 			= 0x00000080,
	EPcreNotEol 			= 0x00000100,
	EPcreUngreedy 			= 0x00000200,
	EPcreNotEmpty 			= 0x00000400,
	EPcreUtf8 				= 0x00000800,
	EPcreNoAutoCapture		= 0x00001000,
	EPcreNoUtf8Check 		= 0x00002000,
	EPcreAutoCallout 		= 0x00004000,
	EPcrePartial 			= 0x00008000,
	EPcreDfaShortest 		= 0x00010000,
	EPcreDfaRestart 		= 0x00020000,
	EPcreFirstline 			= 0x00040000,
	EPcreDupnames 			= 0x00080000,
	EPcreNewlineCr 			= 0x00100000,
	EPcreNewlineLf 			= 0x00200000,
	EPcreNewlineCrLf 		= 0x00300000,
	EPcreNewlineAny			= 0x00400000,
	EPcreNewlineAnyCrLf 	= 0x00500000,
	EPcreBsrAnyCrLf 		= 0x00800000,
	EPcreBsrUnicode 		= 0x01000000,
	EPcreJavascriptCompat 	= 0x02000000
	};

class TRegExOptions
	{
public:
	IMPORT_C TRegExOptions();
	IMPORT_C TRegExOptions(TUint aOptionFlags);
	
	// Getters / Setters
	inline TInt MatchLimit() const;
	inline TRegExOptions SetMatchLimit(TInt aLimit);
	inline TInt MatchLimitRecursion() const;
	inline TRegExOptions SetMatchLimitRecursion(TInt aLimit);
	inline TInt AllOptions() const;
	inline TRegExOptions SetAllOptions(TInt aOptions);
	
	inline TBool Caseless() const;
	inline TRegExOptions SetCaseless(TBool aCaseLess);
	inline TBool Multiline() const;
	inline TRegExOptions SetMultiline(TBool aMultiline);
	inline TBool DotAll() const;
	inline TRegExOptions SetDotAll(TBool aDotAll);
	inline TBool Extended() const;
	inline TRegExOptions SetExtended(TBool aExtended);
	inline TBool DollarEndOnly() const;
	inline TRegExOptions SetDollarEndOnly(TBool aDollarEndOnly);
	inline TBool Extra() const;
	inline TRegExOptions SetExtra(TBool aSetExtra);
	inline TBool Ungreedy() const;
	inline TRegExOptions SetUngreedy(TBool aUngreedy);
	inline TBool Utf8() const;
	inline TRegExOptions SetUtf8(TBool aUtf8);
	inline TBool NoAutoCapture() const;
	inline TRegExOptions SetNoAutoCapture(TBool aAutoCapture);
	
private:
	  TInt iMatchLimit;
	  TInt iMatchLimitRecursion;
	  TUint iAllOptions;
	};

#include "tregexoptions.inl"
#endif /* TREGEXOPTIONS_H_ */
