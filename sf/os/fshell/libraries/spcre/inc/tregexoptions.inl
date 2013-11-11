// tregexoptions.inl
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

#ifndef TREGEXOPTIONS_INL_
#define TREGEXOPTIONS_INL_

#define PCRE_SET_OR_CLEAR(b, o) \
    if (b) iAllOptions |= (o); else iAllOptions &= ~(o);

#define PCRE_IS_SET(o)  \
        (iAllOptions & o) == o

inline TInt TRegExOptions::MatchLimit() const
	{
	return iMatchLimit;
	}

inline TRegExOptions TRegExOptions::SetMatchLimit(TInt aLimit)
	{
	iMatchLimit = aLimit;
	return *this;
	}

inline TInt TRegExOptions::MatchLimitRecursion() const
	{
	return iMatchLimitRecursion;
	}

inline TRegExOptions TRegExOptions::SetMatchLimitRecursion(TInt aLimit)
	{
	iMatchLimitRecursion = aLimit;
	return *this;
	}

inline TInt TRegExOptions::AllOptions() const
	{
	return iAllOptions;
	}

inline TRegExOptions TRegExOptions::SetAllOptions(TInt aOptions)
	{
	iAllOptions = aOptions;
	return *this;
	}
	
inline TBool TRegExOptions::Caseless() const
	{
	return PCRE_IS_SET(EPcreCaseless);
	}

inline TRegExOptions TRegExOptions::SetCaseless(TBool aCaseLess)
	{
	PCRE_SET_OR_CLEAR(aCaseLess, EPcreCaseless);
	return *this;
	}

inline TBool TRegExOptions::Multiline() const
	{
	return PCRE_IS_SET(EPcreMultiline);
	}

inline TRegExOptions TRegExOptions::SetMultiline(TBool aMultiline)
	{
	PCRE_SET_OR_CLEAR(aMultiline, EPcreMultiline);
	return *this;
	}

inline TBool TRegExOptions::DotAll() const
	{
	return PCRE_IS_SET(EPcreDotAll);
	}

inline TRegExOptions TRegExOptions::SetDotAll(TBool aDotAll)
	{
	PCRE_SET_OR_CLEAR(aDotAll, EPcreDotAll);
	return *this;
	}

inline TBool TRegExOptions::Extended() const
	{
	return PCRE_IS_SET(EPcreExtended);
	}

inline TRegExOptions TRegExOptions::SetExtended(TBool aExtended)
	{
	PCRE_SET_OR_CLEAR(aExtended, EPcreExtended);
	return *this;
	}

inline TBool TRegExOptions::DollarEndOnly() const
	{
	return PCRE_IS_SET(EPcreDollarEndOnly);
	}

inline TRegExOptions TRegExOptions::SetDollarEndOnly(TBool aDollarEndOnly)
	{
	PCRE_SET_OR_CLEAR(aDollarEndOnly, EPcreDollarEndOnly);
	return *this;
	}

inline TBool TRegExOptions::Extra() const
	{
	return PCRE_IS_SET(EPcreExtra);
	}

inline TRegExOptions TRegExOptions::SetExtra(TBool aSetExtra)
	{
	PCRE_SET_OR_CLEAR(aSetExtra, EPcreExtra);
	return *this;
	}

inline TBool TRegExOptions::Ungreedy() const
	{
	return PCRE_IS_SET(EPcreUngreedy);
	}

inline TRegExOptions TRegExOptions::SetUngreedy(TBool aUngreedy)
	{
	PCRE_SET_OR_CLEAR(aUngreedy, EPcreUngreedy);
	return *this;
	}

inline TBool TRegExOptions::Utf8() const
	{
	return PCRE_IS_SET(EPcreUtf8);
	}

inline TRegExOptions TRegExOptions::SetUtf8(TBool aUtf8)
	{
	PCRE_SET_OR_CLEAR(aUtf8, EPcreUtf8);
	return *this;
	}

inline TBool TRegExOptions::NoAutoCapture() const
	{
	return PCRE_IS_SET(EPcreNoAutoCapture);
	}

inline TRegExOptions TRegExOptions::SetNoAutoCapture(TBool aAutoCapture)
	{
	PCRE_SET_OR_CLEAR(aAutoCapture, EPcreNoAutoCapture);
	return *this;
	}

#undef PCRE_IS_SET
#undef PCRE_SET_OR_CLEAR
#endif /* TREGEXOPTIONS_INL_ */
