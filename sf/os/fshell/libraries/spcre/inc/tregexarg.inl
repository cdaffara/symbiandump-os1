// tregexarg.inl
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

#ifndef TREGEXARG_INL_
#define TREGEXARG_INL_

template <class T>
class TRegExMatchObject {
 public:
  static inline TBool Parse(const TDesC8& aString, TAny* aDestination) {
    if (aDestination == NULL) return ETrue;
    T* object = reinterpret_cast<T*>(aDestination);
    return object->ParseFrom(aString);
  }
};

template <class T>
inline TRegExArg::TRegExArg(T* aArg)
		: iArg(aArg), iParser(TRegExMatchObject<T>::Parse)
		{}	

template <TInt S>
/*inline TRegExArg::TRegExArg(TBuf8<S>* aArg, Parser aParser)
	: iArg(aArg), iParser(aParser)
	{}
*/
//template <TInt S>
inline TRegExArg::TRegExArg(TBuf8<S>* aArg)
	: iArg(aArg), iParser(ParseNull)
	{}


inline TRegExArg::TRegExArg() : iArg(NULL), iParser(ParseNull) { }

inline TRegExArg::TRegExArg(TAny* aArg) : iArg(aArg), iParser(ParseNull) { }

inline TBool TRegExArg::Parse(const TDesC8& aString) const
	{
	return (*iParser)(aString, iArg);
	}	



// This part of the parser, appropriate only for ints, deals with bases
#define MAKE_INTEGER_PARSER(type)		 							\
inline TRegExArg Hex(type* aType) { 								\
    return TRegExArg(aType, TRegExArg::Parse ## type ## Hex); }		\
inline TRegExArg Octal(type* aType) {								\
    return TRegExArg(aType, TRegExArg::Parse ## type ## Octal); }

MAKE_INTEGER_PARSER(TInt64)
MAKE_INTEGER_PARSER(TUint8)
MAKE_INTEGER_PARSER(TUint16)
MAKE_INTEGER_PARSER(TUint32)
MAKE_INTEGER_PARSER(TUint)

#undef MAKE_INTEGER_PARSER

#endif /* TREGEXARG_INL_ */
