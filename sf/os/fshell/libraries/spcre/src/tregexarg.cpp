// tregexarg.cpp
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

/***** Parsers for various types *****/
#include "tregexarg.h"

EXPORT_C TBool TRegExArg::ParseNull(const TDesC8& /*aString*/,
	TAny* aDestination)
	{
	// We fail if somebody asked us to store into a non-NULL TAny* pointer
	return (aDestination == NULL);
	}

EXPORT_C TBool TRegExArg::ParseTDes8(const TDesC8& aString, TAny* aDestination)
	{
	if (aDestination == NULL) return ETrue;
	TDes8* dest = reinterpret_cast<TDes8*>(aDestination);
	if(dest->MaxLength() < aString.Length())
		{
		return EFalse;
		}
	else
		{
		dest->Copy(aString);
		return ETrue;
		}
	}

EXPORT_C TBool TRegExArg::ParseTPtrC8(const TDesC8& aString, TAny* aDestination)
	{
	if (aDestination == NULL) return ETrue;
	TPtrC8* dest = reinterpret_cast<TPtrC8*>(aDestination);
	
	dest->Set(aString);
	return ETrue;
	}

EXPORT_C TBool TRegExArg::ParseTChar(const TDesC8& aString, TAny* aDestination)
	{
	if (aString.Length() != 1) return EFalse;
	if (aDestination == NULL) return ETrue;
	*(reinterpret_cast<TChar*>(aDestination)) = aString[0];
	return ETrue;
	}

EXPORT_C TBool TRegExArg::ParseTInt8(const TDesC8& aString, TAny* aDestination)
	{
	if (aString.Length() == 0) return EFalse;
	
	TInt8 val = 0;
	TLex8 lex(aString);
	TInt err = lex.Val(val);
	if(err) return EFalse;
	if (aDestination == NULL) return ETrue;
	
	*(reinterpret_cast<TInt8*>(aDestination)) = val;
	return ETrue;
	}

EXPORT_C TBool TRegExArg::ParseTInt16(const TDesC8& aString, TAny* aDestination)
	{
	if (aString.Length() == 0) return EFalse;
	
	TInt16 val = 0;
	TLex8 lex(aString);
	TInt err = lex.Val(val);
	if(err) return EFalse;
	if (aDestination == NULL) return ETrue;
	
	*(reinterpret_cast<TInt16*>(aDestination)) = val;
	return ETrue;
	}

EXPORT_C TBool TRegExArg::ParseTInt32(const TDesC8& aString, TAny* aDestination)
	{
	if (aString.Length() == 0) return EFalse;

	TInt32 val = 0;
	TLex8 lex(aString);
	TInt err = lex.Val(val);
	if(err) return EFalse;
	if (aDestination == NULL) return ETrue;
	
	*(reinterpret_cast<TInt32*>(aDestination)) = val;
	return ETrue;
	}

EXPORT_C TBool TRegExArg::ParseTInt(const TDesC8& aString, TAny* aDestination)
	{
	if (aString.Length() == 0) return EFalse;

	TInt val = 0;
	TLex8 lex(aString);
	TInt err = lex.Val(val);
	if(err) return EFalse;
	if (aDestination == NULL) return ETrue;
	
	*(reinterpret_cast<TInt*>(aDestination)) = val;
	return ETrue;
	}

TBool TRegExArg::ParseTInt64Radix(const TDesC8& aString, TAny* aDestination,
		TRadix aRadix)
	{
	if (aString.Length() == 0)
		return EFalse;

	TInt64 val = 0;
	TLex8 lex(aString);
	// Note TInt64 is treated as unsigned by TLex
	TInt err = lex.Val(val, aRadix);
	
	// If val is returned as negative then it means we have overflowed.
	if (err || val < 0)	
		return EFalse;
	
	if (aDestination == NULL) return ETrue;
	
	*(reinterpret_cast<TInt64*> (aDestination)) = val;
	return ETrue;
	}

TBool TRegExArg::ParseTUint8Radix(const TDesC8& aString, TAny* aDestination,
		TRadix aRadix)
	{
	if (aString.Length() == 0)
		return EFalse;

	TUint8 val = 0;
	TLex8 lex(aString);
	TInt err = lex.Val(val, aRadix);
	if (err)
		return EFalse;
	if (aDestination == NULL) return ETrue;
	
	*(reinterpret_cast<TUint8*> (aDestination)) = val;
	return ETrue;
	}

TBool TRegExArg::ParseTUint16Radix(const TDesC8& aString, TAny* aDestination,
		TRadix aRadix)
	{
	if (aString.Length() == 0)
		return EFalse;

	TUint16 val = 0;
	TLex8 lex(aString);
	TInt err = lex.Val(val, aRadix);
	if (err)
		return EFalse;
	if (aDestination == NULL) return ETrue;
	
	*(reinterpret_cast<TUint16*> (aDestination)) = val;
	return ETrue;
	}

TBool TRegExArg::ParseTUint32Radix(const TDesC8& aString, TAny* aDestination,
		TRadix aRadix)
	{
	if (aString.Length() == 0)
		return EFalse;

	TUint32 val = 0;
	TLex8 lex(aString);
	TInt err = lex.Val(val, aRadix);
	if (err)
		return EFalse;
	if (aDestination == NULL) return ETrue;
	
	*(reinterpret_cast<TUint32*> (aDestination)) = val;
	return ETrue;
	}

TBool TRegExArg::ParseTUintRadix(const TDesC8& aString, TAny* aDestination,
		TRadix aRadix)
	{
	if (aString.Length() == 0)
		return EFalse;

	TUint val = 0;
	TLex8 lex(aString);
	TInt err = lex.Val(val, aRadix);
	if (err)
		return EFalse;
	if (aDestination == NULL) return ETrue;
	
	*(reinterpret_cast<TUint*> (aDestination)) = val;
	return ETrue;
	}
EXPORT_C TBool TRegExArg::ParseTReal32(const TDesC8& aString,
	TAny* aDestination)
	{
	if (aString.Length() == 0) return EFalse;

	TReal32 val = 0;
	TLex8 lex(aString);
	TInt err = lex.Val(val); 
	if(err) return EFalse;
	if (aDestination == NULL) return ETrue;
	
	*(reinterpret_cast<TReal32*>(aDestination)) = val;
	return ETrue;
	}

EXPORT_C TBool TRegExArg::ParseTReal64(const TDesC8& aString,
	TAny* aDestination)
	{
	if (aString.Length() == 0) return EFalse;

	TReal64 val = 0;
	TLex8 lex(aString);
	TInt err = lex.Val(val); 
	if(err) return EFalse;
	if (aDestination == NULL) return ETrue;
	
	*(reinterpret_cast<TReal64*>(aDestination)) = val;
	return ETrue;
	}

#define DEFINE_INTEGER_PARSERS(name)										\
	EXPORT_C TBool TRegExArg::Parse##name(const TDesC8& aString,			\
		TAny* aDestination)													\
		{																	\
		return Parse##name##Radix(aString, aDestination, EDecimal);			\
		}																	\
	EXPORT_C TBool TRegExArg::Parse##name##Hex(const TDesC8& aString,		\
		TAny* aDestination)													\
		{																	\
		return Parse##name##Radix(aString, aDestination, EHex);				\
		}																	\
	EXPORT_C TBool TRegExArg::Parse##name##Octal(const TDesC8& aString,		\
		TAny* aDestination)													\
		{																	\
		return Parse##name##Radix(aString, aDestination, EOctal);			\
		}																				

	DEFINE_INTEGER_PARSERS(TInt64)
	DEFINE_INTEGER_PARSERS(TUint8)
	DEFINE_INTEGER_PARSERS(TUint16)
	DEFINE_INTEGER_PARSERS(TUint32)
	DEFINE_INTEGER_PARSERS(TUint)

#undef DEFINE_INTEGER_PARSERS
