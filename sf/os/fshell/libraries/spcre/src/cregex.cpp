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

#define PCRE_EXP_DECL IMPORT_C // We need to define this because the config.h that defines it when building libpcre isn't exported.

#include <fshell/descriptorutils.h>
#include "pcre.h"
#include "cregex.h"
#include "tregexarg.h"

/**
 * Maximum number of optional arguments supported by the matching interface.
 * If more arguments are required then use the more generic DoMatchL() function.
 */
static const TInt KMaxArgs = 4;

/**
 * Multiplier used to calculate size of vector that stores results from PCRE.
 * @see KVecSize
 */
static const TInt KPcreWorkspaceMultiplier = 3;

/**
* "The first two-thirds of the vector is used to pass back captured subpatterns,
* each subpattern using a pair of integers. The remaining third of the vector
* is used as workspace by pcre_exec() while matching capturing subpatterns,
* and is not available for passing back information. The number passed in 
* vecsize should always be a multiple of three. If it is not, it is rounded
* down."
*/ 
static const TInt KVecSize = (1 + KMaxArgs) * KPcreWorkspaceMultiplier;

/**
 * Number of characters in a triplet escape sequence.
 */
static const TInt KEscapeTripletLength = 3;

/**
 * Number of characters to reserve for escape sequences
 * Reserves enough room for several.
 */
static const TInt KReserveForEscapeChars = KEscapeTripletLength * 4;


// Public Functions //

/**
 * 2-Phase constructor for CRegEx objects.
 * @param aPattern regular expression pattern
 * @return a pre-compiled regular expression object ready to perform matching.
 */
EXPORT_C CRegEx* CRegEx::NewL(const TDesC8& aPattern)
	{
	CRegEx* self = NewLC(aPattern);
	CleanupStack::Pop(self);
	return self;
	}

/**
 * 2-Phase constructor for CRegEx objects.
 * @param aPattern regular expression pattern.
 * @param aOptions options to use when compiling regular expression.
 * @return a pre-compiled regular expression object ready to perform matching.
 */
EXPORT_C CRegEx* CRegEx::NewL(const TDesC8& aPattern,
	const TRegExOptions& aOptions)
	{
	CRegEx* self = NewLC(aPattern, aOptions);
	CleanupStack::Pop(self);
	return self;	
	}

/**
 * 2-Phase constructor for CRegEx objects. Object is left on the cleanup stack.
 * @param aPattern regular expression pattern.
 * @return a pre-compiled regular expression object ready to perform matching.
 */
EXPORT_C CRegEx* CRegEx::NewLC(const TDesC8& aPattern)
	{
	CRegEx* self = new(ELeave) CRegEx();
	CleanupStack::PushL(self);
	self->ConstructL(aPattern);
	return self;
	}

/**
 * 2-Phase constructor for CRegEx objects. Object is left on the cleanup stack.
 * @param aPattern regular expression pattern.
 * @param aOptions options to use when compiling regular expression.
 * @return a pre-compiled regular expression object ready to perform matching.
 */
EXPORT_C CRegEx* CRegEx::NewLC(const TDesC8& aPattern,
	const TRegExOptions& aOptions)
	{
	CRegEx* self = new(ELeave) CRegEx(aOptions);
	CleanupStack::PushL(self);
	self->ConstructL(aPattern);
	return self;	
	}

EXPORT_C CRegEx* CRegEx::NewL(const TDesC16& aPattern, const TRegExOptions& aOptions)
	{
	CRegEx* self = new(ELeave) CRegEx(aOptions);
	CleanupStack::PushL(self);
	self->ConstructL(aPattern);
	CleanupStack::Pop(self);
	return self;
	}

/**
 * Standard destructor to free resources.
 */
EXPORT_C CRegEx::~CRegEx()
	{
	delete iNoArg;
	delete iPattern;
	Cleanup();
	}

// Public matching interface //

/**
 * Checks if the regular expression (RE) matches the supplied text entirely.
 * @param aText the text to match against the regular expression.
 * @return ETrue if the RE matches the supplied text entirely, EFalse otherwise.
 * @see PartialMatchL()
 */
EXPORT_C TBool CRegEx::FullMatchL(const TDesC8& aText) const
	{
	return FullMatchL(aText, *iNoArg, *iNoArg, *iNoArg, *iNoArg);
	}

/**
 * Checks if the regular expression (RE) matches the supplied text entirely.
 * @param aText the text to match against the regular expression.
 * @param aArg1 contains the first extracted subpattern.
 * @return ETrue if the RE matches the supplied text entirely, EFalse otherwise.
 * @see PartialMatchL()
 */
EXPORT_C TBool CRegEx::FullMatchL(const TDesC8& aText,
		  const TRegExArg& aArg1) const
	{
	return FullMatchL(aText, aArg1, *iNoArg, *iNoArg, *iNoArg);
	}

/**
 * Checks if the regular expression (RE) matches the supplied text entirely.
 * @param aText the text to match against the regular expression.
 * @param aArg1 contains the first extracted subpattern.
 * @param aArg2 contains the second extracted subpattern.
 * @return ETrue if the RE matches the supplied text entirely, EFalse otherwise.
 * @see PartialMatchL()
 */
EXPORT_C TBool CRegEx::FullMatchL(const TDesC8& aText,
		  const TRegExArg& aArg1,
		  const TRegExArg& aArg2) const
	{
	return FullMatchL(aText, aArg1, aArg2, *iNoArg, *iNoArg);
	}

/**
 * Checks if the regular expression (RE) matches the supplied text entirely.
 * @param aText the text to match against the regular expression.
 * @param aArg1 contains the first extracted subpattern.
 * @param aArg2 contains the second extracted subpattern.
 * @param aArg3 contains the third extracted subpattern.
 * @return ETrue if the RE matches the supplied text entirely, EFalse otherwise.
 * @see PartialMatchL()
 */
EXPORT_C TBool CRegEx::FullMatchL(const TDesC8& aText,
		  const TRegExArg& aArg1,
		  const TRegExArg& aArg2,
		  const TRegExArg& aArg3) const
	{
	return FullMatchL(aText, aArg1, aArg2, aArg3, *iNoArg);
	}

/**
 * Checks if the regular expression (RE) matches the supplied text entirely.
 * @param aText the text to match against the regular expression.
 * @param aArg1 contains the first extracted subpattern.
 * @param aArg2 contains the second extracted subpattern.
 * @param aArg3 contains the third extracted subpattern.
 * @param aArg4 contains the fourth extracted subpattern.
 * @return ETrue if the RE matches the supplied text entirely, EFalse otherwise.
 * @see PartialMatchL()
 */
EXPORT_C TBool CRegEx::FullMatchL(const TDesC8& aText,
		  const TRegExArg& aArg1,
		  const TRegExArg& aArg2,
		  const TRegExArg& aArg3,
		  const TRegExArg& aArg4) const
	{
	RPointerArray<const TRegExArg> args;
	CleanupClosePushL(args);
	args.ReserveL(KMaxArgs);
	
	if (&aArg1 != iNoArg)
		{
		args.AppendL(&aArg1);
		}
	if (&aArg2 != iNoArg)
		{
		args.AppendL(&aArg2);		
		}
	if (&aArg3 != iNoArg)
		{
		args.AppendL(&aArg3);		
		}
	if (&aArg4 != iNoArg)
		{
		args.AppendL(&aArg4);		
		}
	
	TInt consumed = 0;
	TInt vector[KVecSize];
	
	TBool r =  DoMatchImpl(aText, EAnchorBoth, consumed, args, vector, KVecSize);
	CleanupStack::PopAndDestroy(&args);
	return r;
	}

/**
 * Checks if the regular expression (RE) matches any substring of the text.
 * @param aText the text to match against the regular expression.
 * @return ETrue if the RE  matches any substring of the supplied text,
 * EFalse otherwise.
 * @see FullMatchL()
 */
EXPORT_C TBool CRegEx::PartialMatchL(const TDesC8& aText) const
	{
	return PartialMatchL(aText, *iNoArg, *iNoArg, *iNoArg, *iNoArg);
	}

/**
 * Checks if the regular expression (RE) matches any substring of the text.
 * @param aText the text to match against the regular expression.
 * @param aArg1 contains the first extracted subpattern.
 * @return ETrue if the RE  matches any substring of the supplied text,
 * EFalse otherwise.
 * @see FullMatchL()
 */
EXPORT_C TBool CRegEx::PartialMatchL(const TDesC8& aText,
		  const TRegExArg& aArg1) const
	{
	return PartialMatchL(aText, aArg1, *iNoArg, *iNoArg, *iNoArg);
	}

/**
 * Checks if the regular expression (RE) matches any substring of the text.
 * @param aText the text to match against the regular expression.
 * @param aArg1 contains the first extracted subpattern.
 * @param aArg2 contains the second extracted subpattern.
 * @return ETrue if the RE  matches any substring of the supplied text,
 * EFalse otherwise.
 * @see FullMatchL()
 */
EXPORT_C TBool CRegEx::PartialMatchL(const TDesC8& aText,
		  const TRegExArg& aArg1,
		  const TRegExArg& aArg2) const
	{
	return PartialMatchL(aText, aArg1, aArg2, *iNoArg, *iNoArg);
	}

/**
 * Checks if the regular expression (RE) matches any substring of the text.
 * @param aText the text to match against the regular expression.
 * @param aArg1 contains the first extracted subpattern.
 * @param aArg2 contains the second extracted subpattern.
 * @param aArg3 contains the third extracted subpattern.
 * @return ETrue if the RE  matches any substring of the supplied text,
 * EFalse otherwise.
 * @see FullMatchL()
 */
EXPORT_C TBool CRegEx::PartialMatchL(const TDesC8& aText,
		  const TRegExArg& aArg1,
		  const TRegExArg& aArg2,
		  const TRegExArg& aArg3) const
	{
	return PartialMatchL(aText, aArg1, aArg2, aArg3, *iNoArg);
	}

/**
 * Checks if the regular expression (RE) matches any substring of the text.
 * @param aText the text to match against the regular expression.
 * @param aArg1 contains the first extracted subpattern.
 * @param aArg2 contains the second extracted subpattern.
 * @param aArg3 contains the third extracted subpattern.
 * @param aArg4 contains the fourth extracted subpattern. 
 * @return ETrue if the RE  matches any substring of the supplied text,
 * EFalse otherwise.
 * @see FullMatchL()
 */
EXPORT_C TBool CRegEx::PartialMatchL(const TDesC8& aText,
		  const TRegExArg& aArg1,
		  const TRegExArg& aArg2,
		  const TRegExArg& aArg3,
		  const TRegExArg& aArg4) const
	{
	RPointerArray<const TRegExArg> args;
	CleanupClosePushL(args);
	args.ReserveL(KMaxArgs);

	if (&aArg1 != iNoArg)
		{
		args.AppendL(&aArg1);
		}
	if (&aArg2 != iNoArg)
		{
		args.AppendL(&aArg2);		
		}
	if (&aArg3 != iNoArg)
		{
		args.AppendL(&aArg3);		
		}
	if (&aArg4 != iNoArg)
		{
		args.AppendL(&aArg4);		
		}
	
	TInt consumed = 0;
	TInt vector[KVecSize];
	
	TBool r =  DoMatchImpl(aText, EUnanchored, consumed, args, vector,
					KVecSize);
	CleanupStack::PopAndDestroy(&args);
	return r;
	}


/**
 * General function to perform a regular expression (RE) match on a substring
 * of the text.
 * @param aText the text to match against the regular expression.
 * @param aAnchor the type of match to perform
 * @return ETrue if the RE  matches a substring of the supplied text,
 * EFalse otherwise.
 */
EXPORT_C TBool CRegEx::DoMatchL(const TDesC8& aText,
		  TAnchor aAnchor,
		  TInt&	aConsumed) const
	{
	return DoMatchL(aText, aAnchor, aConsumed, *iNoArg, *iNoArg, *iNoArg,
				*iNoArg);
	}

/**
 * General function to perform a regular expression (RE) match on a substring
 * of the text.
 * @param aText the text to match against the regular expression.
 * @param aAnchor the type of match to perform
 * @param aArg1 contains the first extracted subpattern.
 * @return ETrue if the RE  matches a substring of the supplied text,
 * EFalse otherwise.
 */
EXPORT_C TBool CRegEx::DoMatchL(const TDesC8& aText,
		  TAnchor aAnchor,
		  TInt&	aConsumed,
		  const TRegExArg& aArg1) const
	{
	return DoMatchL(aText, aAnchor, aConsumed, aArg1, *iNoArg, *iNoArg,
				*iNoArg);
	}

/**
 * General function to perform a regular expression (RE) match on a substring
 * of the text.
 * @param aText the text to match against the regular expression.
 * @param aAnchor the type of match to perform
 * @param aArg1 contains the first extracted subpattern.
 * @param aArg2 contains the second extracted subpattern.
 * @return ETrue if the RE  matches a substring of the supplied text,
 * EFalse otherwise.
 */
EXPORT_C TBool CRegEx::DoMatchL(const TDesC8& aText,
		  TAnchor aAnchor,
		  TInt&	aConsumed,
		  const TRegExArg& aArg1,
		  const TRegExArg& aArg2) const
	{
	return DoMatchL(aText, aAnchor, aConsumed, aArg1, aArg2, *iNoArg, *iNoArg);
	}

/**
 * General function to perform a regular expression (RE) match on a substring
 * of the text.
 * @param aText the text to match against the regular expression.
 * @param aAnchor the type of match to perform
 * @param aArg1 contains the first extracted subpattern.
 * @param aArg2 contains the second extracted subpattern.
 * @param aArg3 contains the third extracted subpattern.
 * @return ETrue if the RE  matches a substring of the supplied text,
 * EFalse otherwise.
 */
EXPORT_C TBool CRegEx::DoMatchL(const TDesC8& aText,
		  TAnchor aAnchor,
		  TInt&	aConsumed,
		  const TRegExArg& aArg1,
		  const TRegExArg& aArg2,
		  const TRegExArg& aArg3) const
	{
	return DoMatchL(aText, aAnchor, aConsumed, aArg1, aArg2, aArg3, *iNoArg);
	}

/**
 * General function to perform a regular expression (RE) match on a substring
 * of the text.
 * @param aText the text to match against the regular expression.
 * @param aAnchor the type of match to perform
 * @param aArg1 contains the first extracted subpattern.
 * @param aArg2 contains the second extracted subpattern.
 * @param aArg3 contains the third extracted subpattern.
 * @param aArg4 contains the fourth extracted subpattern. 
 * @return ETrue if the RE  matches a substring of the supplied text,
 * EFalse otherwise.
 */
EXPORT_C TBool CRegEx::DoMatchL(const TDesC8& aText,
		  TAnchor aAnchor,
		  TInt&	aConsumed,
		  const TRegExArg& aArg1,
		  const TRegExArg& aArg2,
		  const TRegExArg& aArg3,
		  const TRegExArg& aArg4) const
	{
	RPointerArray<const TRegExArg> args;
	CleanupClosePushL(args);
	args.ReserveL(KMaxArgs);
	
	if (&aArg1 != iNoArg)
		{
		args.AppendL(&aArg1);
		}
	if (&aArg2 != iNoArg)
		{
		args.AppendL(&aArg2);		
		}
	if (&aArg3 != iNoArg)
		{
		args.AppendL(&aArg3);		
		}
	if (&aArg4 != iNoArg)
		{
		args.AppendL(&aArg4);		
		}	

	TInt r = DoMatchL(aText, aAnchor, aConsumed, args);
	CleanupStack::PopAndDestroy(&args);
	return r;
	}

/**
 * General function to perform a regular expression (RE) match on a substring
 * of the text.
 * @param aText the text to match against the regular expression.
 * @param aAnchor the type of match to perform
 * @param array of arguments that will contain the extracted subpatterns.
 * @return ETrue if the RE  matches a substring of the supplied text,
 * EFalse otherwise.
 */
EXPORT_C TBool CRegEx::DoMatchL(const TDesC8& aText,
		  TAnchor aAnchor,
		  TInt&	aConsumed,			
		  const RPointerArray<const TRegExArg>& aArgs) const
	{
	const TInt vectorSize = (1 + aArgs.Count()) * KPcreWorkspaceMultiplier;
	TInt* vector = new( ELeave ) TInt[ vectorSize ];
	CleanupArrayDeletePushL( vector );	
	
	TBool r =  DoMatchImpl(aText, aAnchor, aConsumed, aArgs, vector,
					vectorSize);
	CleanupStack::PopAndDestroy(vector);
	return r;	
	}

/**
 * Allows text to be scanned incrementally. Call this function repeatidly to
 * match regular expressions at the front of a string and skip over them as
 * they match. 
 * @param aText the text to match against the regular expression.
 * @return ETrue if the RE matched and a substring was consumed,
 * EFalse otherwise.
 * @see FindAndConsumeL()
 */
EXPORT_C TBool CRegEx::ConsumeL(TDes8& aText) const
	{
	return ConsumeL(aText, *iNoArg, *iNoArg, *iNoArg, *iNoArg);
	}

/**
 * Allows text to be scanned incrementally. Repeatidly call this function to
 * match regular expressions at the front of a string and skip over them as
 * they match. 
 * @param aText the text to match against the regular expression.
 * @param aArg1 contains the first extracted subpattern.
 * @return ETrue if the RE matched and a substring was consumed,
 * EFalse otherwise.
 * @see FindAndConsumeL()
 */
EXPORT_C TBool CRegEx::ConsumeL(TDes8& aText,
		  const TRegExArg& aArg1) const
	{
	return ConsumeL(aText, aArg1, *iNoArg, *iNoArg, *iNoArg);
	}

/**
 * Allows text to be scanned incrementally. Repeatidly call this function to
 * match regular expressions at the front of a string and skip over them as
 * they match. 
 * @param aText the text to match against the regular expression.
 * @param aArg1 contains the first extracted subpattern.
 * @param aArg2 contains the second extracted subpattern.
 * @return ETrue if the RE matched and a substring was consumed,
 * EFalse otherwise.
 * @see FindAndConsumeL()
 */
EXPORT_C TBool CRegEx::ConsumeL(TDes8& aText,
		  const TRegExArg& aArg1,
		  const TRegExArg& aArg2) const
	{
	return ConsumeL(aText, aArg1, aArg2, *iNoArg, *iNoArg);
	}

/**
 * Allows text to be scanned incrementally. Repeatidly call this function to
 * match regular expressions at the front of a string and skip over them as
 * they match. 
 * @param aText the text to match against the regular expression.
 * @param aArg1 contains the first extracted subpattern.
 * @param aArg2 contains the second extracted subpattern.
 * @param aArg3 contains the third extracted subpattern.
 * @return ETrue if the RE matched and a substring was consumed,
 * EFalse otherwise.
 * @see FindAndConsumeL()
 */
EXPORT_C TBool CRegEx::ConsumeL(TDes8& aText,
		  const TRegExArg& aArg1,
		  const TRegExArg& aArg2,
		  const TRegExArg& aArg3) const
	{
	return ConsumeL(aText, aArg1, aArg2, aArg3, *iNoArg);
	}

/**
 * Allows text to be scanned incrementally. Repeatidly call this function to
 * match regular expressions at the front of a string and skip over them as
 * they match. 
 * @param aText the text to match against the regular expression.
 * @param aArg1 contains the first extracted subpattern.
 * @param aArg2 contains the second extracted subpattern.
 * @param aArg3 contains the third extracted subpattern.
 * @param aArg4 contains the fourth extracted subpattern. 
 * @return ETrue if the RE matched and a substring was consumed,
 * EFalse otherwise.
 * @see FindAndConsumeL()
 */
EXPORT_C TBool CRegEx::ConsumeL(TDes8& aText,
		  const TRegExArg& aArg1,
		  const TRegExArg& aArg2,
		  const TRegExArg& aArg3,
		  const TRegExArg& aArg4) const
	{
	RPointerArray<const TRegExArg> args;
	CleanupClosePushL(args);
	args.ReserveL(KMaxArgs);

	if (&aArg1 != iNoArg)
		{
		args.AppendL(&aArg1);
		}
	if (&aArg2 != iNoArg)
		{
		args.AppendL(&aArg2);		
		}
	if (&aArg3 != iNoArg)
		{
		args.AppendL(&aArg3);		
		}
	if (&aArg4 != iNoArg)
		{
		args.AppendL(&aArg4);		
		}
	
	TInt consumed = 0;
	TInt vector[KVecSize];
	
	TBool r =  DoMatchImpl(aText, EAnchorStart, consumed, args, vector,
					KVecSize);
	if (r)
		{
		// Remove prefix
		aText.Delete(0, consumed);		
		}
	CleanupStack::PopAndDestroy(&args);
	return r;	
	}

/**
 * Allows text to be scanned incrementally. Repeatidly call this function to
 * match  regular expression in a string and extract them as they match.
 * Unlike ConsumeL, your match is not anchored to the start of the string.
 * @param aText the text to match against the regular expression.
 * @return ETrue if the RE matched and a substring was consumed,
 * EFalse otherwise.
 * @see ConsumeL()
 */
EXPORT_C TBool CRegEx::FindAndConsumeL(TDes8& aText) const
	{
	return FindAndConsumeL(aText, *iNoArg, *iNoArg, *iNoArg, *iNoArg);
	}

/**
 * Allows text to be scanned incrementally. Repeatidly call this function to
 * match  regular expression in a string and extract them as they match.
 * Unlike ConsumeL, your match is not anchored to the start of the string.
 * @param aText the text to match against the regular expression.
 * @param aArg1 contains the first extracted subpattern.
 * @return ETrue if the RE matched and a substring was consumed,
 * EFalse otherwise.
 * @see ConsumeL()
 */
EXPORT_C TBool CRegEx::FindAndConsumeL(TDes8& aText,
		  const TRegExArg& aArg1) const
	{
	return FindAndConsumeL(aText, aArg1, *iNoArg, *iNoArg, *iNoArg);
	}

/**
 * Allows text to be scanned incrementally. Repeatidly call this function to
 * match  regular expression in a string and extract them as they match.
 * Unlike ConsumeL, your match is not anchored to the start of the string.
 * @param aText the text to match against the regular expression.
 * @param aArg1 contains the first extracted subpattern.
 * @param aArg2 contains the second extracted subpattern.
 * @return ETrue if the RE matched and a substring was consumed,
 * EFalse otherwise.
 * @see ConsumeL()
 */
EXPORT_C TBool CRegEx::FindAndConsumeL(TDes8& aText,
		  const TRegExArg& aArg1,
		  const TRegExArg& aArg2) const
	{
	return FindAndConsumeL(aText, aArg1, aArg2, *iNoArg, *iNoArg);
	}

/**
 * Allows text to be scanned incrementally. Repeatidly call this function to
 * match  regular expression in a string and extract them as they match.
 * Unlike ConsumeL, your match is not anchored to the start of the string.
 * @param aText the text to match against the regular expression.
 * @param aArg1 contains the first extracted subpattern.
 * @param aArg2 contains the second extracted subpattern.
 * @param aArg3 contains the third extracted subpattern.
 * @return ETrue if the RE matched and a substring was consumed,
 * EFalse otherwise.
 * @see ConsumeL()
 */
EXPORT_C TBool CRegEx::FindAndConsumeL(TDes8& aText,
		  const TRegExArg& aArg1,
		  const TRegExArg& aArg2,
		  const TRegExArg& aArg3) const
	{
	return FindAndConsumeL(aText, aArg1, aArg2, aArg3, *iNoArg);
	}

/**
 * Allows text to be scanned incrementally. Repeatidly call this function to
 * match  regular expression in a string and extract them as they match.
 * Unlike ConsumeL, your match is not anchored to the start of the string.
 * @param aText the text to match against the regular expression.
 * @param aArg1 contains the first extracted subpattern.
 * @param aArg2 contains the second extracted subpattern.
 * @param aArg3 contains the third extracted subpattern.
 * @param aArg4 contains the fourth extracted subpattern. 
 * @return ETrue if the RE matched and a substring was consumed,
 * EFalse otherwise.
 * @see ConsumeL()
 */
EXPORT_C TBool CRegEx::FindAndConsumeL(TDes8& aText,
		  const TRegExArg& aArg1,
		  const TRegExArg& aArg2,
		  const TRegExArg& aArg3,
		  const TRegExArg& aArg4) const
	{
	RPointerArray<const TRegExArg> args;
	CleanupClosePushL(args);
	args.ReserveL(KMaxArgs);

	if (&aArg1 != iNoArg)
		{
		args.AppendL(&aArg1);
		}
	if (&aArg2 != iNoArg)
		{
		args.AppendL(&aArg2);		
		}
	if (&aArg3 != iNoArg)
		{
		args.AppendL(&aArg3);		
		}
	if (&aArg4 != iNoArg)
		{
		args.AppendL(&aArg4);		
		}	
	
	TInt consumed = 0;
	TInt vector[KVecSize];
	
	TBool r = DoMatchImpl(aText, EUnanchored, consumed, args, vector, KVecSize);
	CleanupStack::PopAndDestroy(&args);
	
	if (r)
		{
		// Remove prefix
		aText.Delete(0, consumed);
		}
	
	return r;
	}


/**
 * The first match of the regular expression in aString is
 * replaced by aRewrite.
 * 
 * Within aRewrite, backslash-escaped digits (\1 to \9) can be
 * used to insert text matching a corresponding parenthesized group from
 * the pattern. \0 in aRewrite refers to the entire matching text.
 * @param aRewrite the text to replace the matching substring with.
 * @param aText the text to match against the regular expression.
 * @return ETrue if match occurred and replace was succsessful,
 * EFalse otherwise.
 * @see GlobalReplaceL()
 */
EXPORT_C TBool CRegEx::ReplaceL(const TDesC8& aRewrite, TDes8& aString) const
	{
	TInt r = EFalse;
	TInt vector[KVecSize];
	
	TInt matches = TryMatch(aString, 0, EUnanchored, vector, KVecSize);
	if (matches == 0)
		{
		return r;
		}		
	
	HBufC8* s = HBufC8::NewLC(aString.MaxLength());
	TPtr8 pS = s->Des();
	if (!Rewrite(pS, aRewrite, aString, vector, KVecSize, matches))
		{
		CleanupStack::PopAndDestroy(s);
		return r;
		}		
	
	__ASSERT_DEBUG(vector[0] >= 0, Panic(EInvalidMatchResults));
	__ASSERT_DEBUG(vector[1] >= 0, Panic(EInvalidMatchResults));
	
	TInt replacementLength = vector[1] - vector[0];
	
	if((aString.Length() + s->Length() - replacementLength) <= aString.MaxLength())
		{
		aString.Replace(vector[0], replacementLength, *s);
		}
	else
		{
		CleanupStack::PopAndDestroy(s);
		iErrorCode = KErrRegExOutputTooBig;
		return r;
		}
	
	CleanupStack::PopAndDestroy(s);
	r = ETrue;
	return r;
	}

/**
 * All matches of the regular expression in aString are
 * replaced by aRewrite.
 * 
 * Within the rewrite string, backslash-escaped digits (\1 to \9) can be
 * used to insert text matching a corresponding parenthesized group from
 * the pattern. \0 in "aRewrite" refers to the entire matching text.
 * @param aRewrite the text to replace the matching substring with.
 * @param aText the text to match against the regular expression.
 * @return ETrue if matches occurred and replace was succsessful,
 * EFalse otherwise.
 * @see ReplaceL()
 */
EXPORT_C TInt CRegEx::GlobalReplaceL(const TDesC8& aRewrite,
	TDes8& aString) const	
	{
	TInt count = 0;
	TInt vector[KVecSize];
	
	HBufC8* out = HBufC8::NewLC(aString.MaxLength());
	TPtr8 pOut = out->Des();
	
	TInt start = 0;
	TInt lastend = -1;
	
	while (start <= aString.Length())
		{
		TInt matches = TryMatch(aString, start, EUnanchored, vector, KVecSize);
		if (matches <= 0)
			{
			break;
			}			
		TInt matchstart = vector[0];
		TInt matchend = vector[1];
		
		__ASSERT_DEBUG(matchstart >= start, EInvalidMatchResults);
		__ASSERT_DEBUG(matchend >= matchstart, EInvalidMatchResults);
		if (matchstart == matchend && matchstart == lastend)
			{
			// advance one character if we matched an empty string at the same
			// place as the last match occurred
			matchend = start + 1;
			// If the current char is CR and we're in CRLF mode, skip LF too.
			// Note it's better to call pcre_fullinfo() than to examine
			// all_options(), since options_ could have changed bewteen
			// compile-time and now, but this is simpler and safe enough.
			// Modified by PH to add ANY and ANYCRLF.
			if ((start + 1 < aString.Length()) &&
					aString[start] == '\r' && aString[start+1] == '\n' &&
					(NewlineMode(iOptions.AllOptions()) == PCRE_NEWLINE_CRLF ||
					NewlineMode(iOptions.AllOptions()) == PCRE_NEWLINE_ANY ||
					NewlineMode(iOptions.AllOptions()) == PCRE_NEWLINE_ANYCRLF))
				{
				matchend++;
				}
			// We also need to advance more than one char if we're in utf8 mode.
			#ifdef SUPPORT_UTF8
			if (iOptions.Utf8())
				{
				while ((matchend < aString.Length())
					&& (aString[matchend] & 0xc0) == 0x80)
					{
					matchend++;
					}					
				}
			#endif
			if (matchend <= aString.Length())
				{
				if(pOut.Length() + (matchend - start) <= pOut.MaxLength())
					{
					pOut.Append(aString.Mid(start, matchend - start));
					}
				else
					{
					iErrorCode = KErrRegExOutputTooBig;
					return KErrRegExOutputTooBig;
					}
				}				
			start = matchend;
			}
		else
			{
			if(pOut.Length() + (matchstart - start) <= pOut.MaxLength())
				{
				pOut.Append(aString.Mid(start, matchstart - start));
				}
			else
				{
				iErrorCode = KErrRegExOutputTooBig;
				return KErrRegExOutputTooBig;
				}
			Rewrite(pOut, aRewrite, aString, vector, KVecSize, matches);
			start = matchend;
			lastend = matchend;
			count++;
			}
		}
	
	if (count == 0)
		{
		return count;
		}	

	if (start < aString.Length())
		{
		if((aString.Length() - start) + pOut.Length() <= pOut.MaxLength())
			{
			pOut.Append(aString.Mid(start, aString.Length() - start));
			}
		else
			{
			iErrorCode = KErrRegExOutputTooBig;
			return KErrRegExOutputTooBig;
			}
		}
	
	aString.Swap(pOut);
	
	CleanupStack::PopAndDestroy(out);
	
	return count;
	}

/**
 * The first match of the regular expression in the supplied string is
 * replaced by another supplied string and copied into aOut with substitutions.
 * The non-matching portions of aString are ignored.
 * 
 * Within the rewrite string, backslash-escaped digits (\1 to \9) can be
 * used to insert text matching a corresponding parenthesized group from
 * the pattern. \0 in "aRewrite" refers to the entire matching text.
 * @param aRewrite the text to replace the matching substring with.
 * @param aText the text to match against the regular expression.
 * @return ETrue if match occurred and extraction was succsessful,
 * EFalse otherwise.
 * @see ReplaceL()
 */
EXPORT_C TBool CRegEx::ExtractL(const TDesC8& aRewrite,
					const TDesC8& aText, TDes8& aOut) const
	{
	TInt vector[KVecSize];
	
	TInt matches = TryMatch(aText, 0, EUnanchored, vector, KVecSize);
	if (matches == 0)
		{
		iErrorCode = KErrRegExZeroMatches;
		return EFalse;
		}	
	aOut.Zero();
	
	TBool r = Rewrite(aOut, aRewrite, aText, vector, KVecSize, matches);
	return r;
	}

/**
 * Returns EPcreNewlineAnyCrLf, EPcreNewlineAny, EPcreNewlineCrLf,
 * EPcreNewlineLf or EPcreNewlineCr
 * Note that EPcreNewlineCrLf is defined to be EPcreNewlineCr | EPcreNewlineLf.
 * @param aOptions
 * @return
 */

EXPORT_C TInt CRegEx::NewlineMode(TInt aOptions)
	{
	// TODO: if we can make it threadsafe, cache this var
	TInt newlineMode = 0;
	/* if (newlineMode) return newlineMode; */  // do this once it's cached
	
	if (aOptions & (EPcreNewlineCrLf|EPcreNewlineCr|EPcreNewlineLf|
			EPcreNewlineAny|EPcreNewlineAnyCrLf))
		{
		newlineMode = (aOptions &
				(EPcreNewlineCrLf|EPcreNewlineCr|EPcreNewlineLf|
						EPcreNewlineAny|EPcreNewlineAnyCrLf));
		}
	else
		{
		TInt newline;
		pcre_config(PCRE_CONFIG_NEWLINE, &newline);

		switch(newline)
			{
			case KNewLineAnyCrLf:
				{
				newlineMode = EPcreNewlineAnyCrLf;
				break;
				}			
			case KNewLineAny:
				{
				newlineMode = EPcreNewlineAny;
				break;
				}	
			case KNewLineLf:
				{
				newlineMode = EPcreNewlineLf;
				break;
				}			
			case KNewLineCr:
				{
				newlineMode = EPcreNewlineCr;
				break;
				}
			case KNewLineCrLf:
				{
				newlineMode = EPcreNewlineCrLf;
				break;
				}
			default:
				{
				__ASSERT_DEBUG(EFalse, EUnexpectedRetValFromPcre);				
				}				
			}		
		}
	return newlineMode;
	}

/** 
 * Escapes all potentially meaningful regular expression (RE) characters in
 * aUnquoted.  The returned string, used as a regular expression,
 * will exactly match the original string.  For example,
 *           1.5-2.0?
 * may become:
 *           1\.5\-2\.0\?
 * Note QuoteMeta behaves the same as perl's QuoteMeta function,
 * *except* that it escapes the NUL character (\0) as backslash + 0,
 * rather than backslash + NUL.
 * @param aUnquoted unescaped string.
 * @return string with all meaningful RE characters escaped.
 */
EXPORT_C HBufC8* CRegEx::QuoteMetaL(const TDesC8& aUnquoted)
	{
	HBufC8* result = HBufC8::NewLC(aUnquoted.Length() + KReserveForEscapeChars);
	TPtr8 pResult = result->Des();

	_LIT8(KEscapeNull, "\\0");
	_LIT8(KEscapeOther, "\\%c");
	
	// Escape any ascii character not in [A-Za-z_0-9].
	//
	// Note that it's legal to escape a character even if it has no
	// special meaning in a regular expression -- so this function does
	// that.  (This also makes it identical to the perl function of the
	// same name; see `perldoc -f quotemeta`.)  The one exception is
	// escaping NUL: rather than doing backslash + NUL, like perl does,
	// we do '\0', because pcre itself doesn't take embedded NUL chars.
	for (TInt ii = 0; ii < aUnquoted.Length(); ++ii)
	  {
	  
	  // Always make sure we have enough room to escape at least one character
	  if(pResult.MaxLength() <= pResult.Length() + KEscapeTripletLength)
		  {
		  result = result->ReAllocL(pResult.Length() + KReserveForEscapeChars);
		  CleanupStack::Pop();
		  CleanupStack::PushL(result);
		  pResult.Set(result->Des());
		  }
	  
	  if (aUnquoted[ii] == TChar('\0'))
		  {
		  pResult.Append(KEscapeNull());
		  }
	  else if ((aUnquoted[ii] < TChar('a') || aUnquoted[ii] > TChar('z')) &&
			   (aUnquoted[ii] < TChar('A') || aUnquoted[ii] > TChar('Z')) &&
			   (aUnquoted[ii] < TChar('0') || aUnquoted[ii] > TChar('9')) &&
			   aUnquoted[ii] != TChar('_') &&
			   // If this is the part of a UTF8 or Latin1 character, we need
			   // to copy this byte without escaping.  Experimentally this is
			   // what works correctly with the regexp library.
			   !(aUnquoted[ii] & TChar(0x80)))
		  {
		  pResult.AppendFormat(KEscapeOther, aUnquoted[ii]);
		  }
	  else
		  {
		  pResult.Append(aUnquoted[ii]);
		  }
	}
	CleanupStack::Pop(result);
	return result;
}

/**
 * Returns the number of capturing subpatterns, or -1 if the
 * regular expressions wasn't valid on construction.
 * @return the number of capturing subpatterns or or -1 if the regular
 * expressions wasn't valid on construction.
 */
EXPORT_C TInt CRegEx::NumberOfCapturingGroups() const
	{
	if (iRePartial == NULL) return KErrNotFound;
	
	TInt result;
	TInt pcreRetVal = pcre_fullinfo(iRePartial, // The regular expression object
								iExtraPartial,  // Study data
                                PCRE_INFO_CAPTURECOUNT,
                                &result);
	__ASSERT_DEBUG(pcreRetVal == 0, Panic(EUnexpectedRetValFromPcre));
	return result;
	}

/**
 * Analyzes a regular expression (RE) pattern further. This is especially useful
 * if an RE is going to be used several times to reduce the time taken for
 * matching.
 *  
 * "At present, studying a pattern is useful only for non-anchored patterns that
 * do not have a single fixed starting character." 
 */
EXPORT_C void CRegEx::Study()
	{
	// "At present, studying a pattern is useful only for non-anchored
	// patterns that do not have a single fixed starting character."
	if(iRePartial)
		{
		const char* compileError; // ignored
		iExtraPartial = pcre_study(
				iRePartial,     /* result of pcre_compile() */
				0,              /* no options exist */
				&compileError);	/* set to NULL or points to a message */		
		}
	}


// Private Functions //

/**
 * Standard constructor
 */
CRegEx::CRegEx()
	{
	}


/**
 * Standard constructor
 * @param aOptions options used when compiling regular expression.
 */
CRegEx::CRegEx(const TRegExOptions& aOptions)
	: iOptions(aOptions)
	{
	}

/**
 * Second phase constructor.
 */
void CRegEx::ConstructL(const TDesC8& aPattern)
	{
	iPattern = HBufC8::NewL(aPattern.Length() + 1); // Leave room for \0
	TPtr8 pPattern = iPattern->Des();
	pPattern.Copy(aPattern);
	pPattern.ZeroTerminate();
	CommonConstructL();
	}

void CRegEx::CommonConstructL()
	{
	// The default value for an argument, to indicate no arg was passed in
	iNoArg = new(ELeave) TRegExArg((TAny*)NULL);
	
	// Compile patterns used for partial and full matches.	
	iReFull = NULL;
	iRePartial = NULL;
	
	iRePartial = CompileL(EUnanchored);
	if(iRePartial)
		{
		iReFull = CompileL(EAnchorBoth);
		}	
	User::LeaveIfError(iErrorCode);	
	}	

void CRegEx::ConstructL(const TDesC16& aPattern)
	{
	LtkUtils::RLtkBuf8 narrowBuf;
	narrowBuf.CreateLC(aPattern.Length() + 1);
	if (iOptions.Utf8())
		{
		narrowBuf.CopyAsUtf8L(aPattern);
		}
	else
		{
		narrowBuf.Copy(aPattern);
		}
	narrowBuf.AppendL(0);
	iPattern = narrowBuf.ToHBuf();
	CleanupStack::Pop(&narrowBuf);

	CommonConstructL();
	}

/**
 * Compile the regular expression (RE) pattern.
 * @param aAnchor anchoring to use for the RE pattern.
 * @return pointer to PCRE object with compiled RE data. 
 */
pcre* CRegEx::CompileL(TAnchor aAnchor)
	{
	// First, convert TRegExOptions into pcre options
	TInt pcreOptions = iOptions.AllOptions();
	
	// Special treatment for anchoring.  This is needed because at
	// runtime pcre only provides an option for anchoring at the
	// beginning of a string (unless you use offset).
	//
	// There are three types of anchoring we want:
	//    EUnanchored      Compile the original pattern, and use
	//                    a pcre unanchored match.
	//    EAnchorStart    Compile the original pattern, and use
	//                    a pcre anchored match.
	//    EAnchorBoth     Tack a "\z" to the end of the original pattern
	//                    and use a pcre anchored match.
	
	pcre* re;
	TInt errCode = 0;
	TInt errOffset = 0;
	
	const char* compileError; // ignored

	if (aAnchor != EAnchorBoth)
		{
		re = pcre_compile2((const char *)iPattern->Ptr(), pcreOptions,
				&errCode,&compileError, &errOffset, NULL);
		}
	else
		{
		// Tack a '\z' at the end of RE.  Parenthesize it first so that
		// the '\z' applies to all top-level alternatives in the regexp.
		_LIT8(KWrapped, "(?:%S)\\z\x0");
		HBufC8* wrapped = HBufC8::NewLC(KWrapped().Length()
				+ iPattern->Length());
		TPtr8 pWrapped = wrapped->Des();
		pWrapped.Format(KWrapped(), iPattern);
		re = pcre_compile2((const char *)pWrapped.Ptr(), pcreOptions,
				&errCode, &compileError, &errOffset, NULL);
		CleanupStack::PopAndDestroy(wrapped);
		}
	
	if (!re && (iErrorCode == KErrNone))
		{
		iErrorCode = KErrRegExCompileBase - errCode;
		iErrorOffset = errOffset;
		}
	return re;
	}

/**
 * Cleanup the compiled regular expression and study data.
 * Separated out from destructor in case support for recompiling
 * is introduced.
 */
void CRegEx::Cleanup()
	{
	if (iReFull)
		{
		(*pcre_free)(iReFull);
		}
	
	if (iRePartial)
		{
		(*pcre_free)(iRePartial);
		}
	if(iExtraPartial)
		{
		(*pcre_free)(iExtraPartial);
		}
	}

// Internal matching and rewrite implementations //

/**
 * Match against aText, filling in aVector (up to aVector.Count() * 2/3) with
 * pairs of integers for the beginning and end positions of matched
 * text.  The first pair corresponds to the entire matched text;
 * subsequent pairs correspond, in order, to parentheses-captured
 * matches.  Returns the number of pairs (one more than the number of
 * the last subpattern with a match) if matching was successful
 * and zero if the match failed.
 * I.e. for RE("(foo)|(bar)|(baz)") it will return 2, 3, and 4 when matching
 * against "foo", "bar", and "baz" respectively.
 * When matching RE("(foo)|hello") against "hello", it will return 1.
 * But the values for all subpattern are filled in into "aVector".
 * @param aText the text to match against the regular expression.
 * @param aStartPos position in aText to start matching from
 * @param aAnchor  the type of match to perform.
 * @param aVector vector that stores pairs of integers for the start and end
 * positions of matched substrings.
 * @param aVectorSize length of aVector
 * @return the number of matched subpatterns.
 */
TInt CRegEx::TryMatch(const TDesC8& aText,
                 TInt aStartPos,
                 TAnchor aAnchor,
   			   	TInt* aVector,
   			    TInt aVectorSize) const
    {
	pcre* re = (aAnchor == EAnchorBoth) ? iReFull : iRePartial;
	if (!re)
		{
		// Matching against invalid re
		return 0;
		}
	
	pcre_extra extra = { 0, 0, 0, 0, 0, 0 };
	
	if(iExtraPartial)
		{
		extra = *iExtraPartial;
		}

	if (iOptions.MatchLimit() > 0)
		{
		extra.flags |= PCRE_EXTRA_MATCH_LIMIT;
		extra.match_limit = iOptions.MatchLimit();
		}
	
	if (iOptions.MatchLimitRecursion() > 0)
		{
		extra.flags |= PCRE_EXTRA_MATCH_LIMIT_RECURSION;
		extra.match_limit_recursion = iOptions.MatchLimitRecursion();
		}
	
	TInt rc = pcre_exec(re,              // The regular expression object
						&extra,
						(const char *)aText.Ptr(),
						aText.Length(),
						aStartPos,
						(aAnchor == EUnanchored) ? 0 : PCRE_ANCHORED,
						aVector,
						aVectorSize);
	
	// Handle errors
	if (rc == PCRE_ERROR_NOMATCH)
		{
		return 0;
		}
	else if (rc < 0)
		{
		// Unexpected return code
		return 0;
		}
	else if (rc == 0)
		{
		// pcre_exec() returns 0 as a special case when the number of
		// capturing subpatterns exceeds the size of the vector.
		// When this happens, there is a match and the output vector
		// is filled, but we miss out on the positions of the extra subpatterns.
		rc = aVectorSize / 2;
		}
	return rc;
    }

/**
 * Internal implementation of rewrite functionality used by the replace &
 * extract functions.
 * Appends the aRewrite string, with backslash subsitutions from aText
 * and aVector, to string aOut.
 * @param aOut output descriptor
 * @param aRewrite descriptor backslash subsitutions to append to aOut.
 * @param aText descriptor containing substitutions.
 * @param aVector vector that stores pairs of integers for the start and end
 * positions of matched substrings.
 * @param aVectorSize length of aVector.
 * @param aMatches number of matches.
 * @return ETrue if the operation was successfull, EFalse otherwise.
 * @see ReplaceL()
 * @see GlobalReplaceL()
 * @see ExtractL()
 */
TBool CRegEx::Rewrite(TDes8& aOut, const TDesC8& aRewrite,
                 const TDesC8& aText, TInt* aVector,
                 TInt aVectorSize, TInt aMatches) const
    {
	for(TInt i = 0; i < aRewrite.Length(); i++)
		{
		TChar c = aRewrite[i];
		
		if (c == '\\')
			{
			c = aRewrite[++i];
			if (c.IsDigit())
				{
				TUint n = c - TChar('0');
				if (n >= aMatches)
					{
					iErrorCode = KErrRegExBadBackslashSubsitution;
					return EFalse;
					}
				__ASSERT_DEBUG(aVectorSize >= 2 * n + 1, Panic(EVectorTooSmall));
				TInt start = aVector[2 * n];				
				if (start >= 0)
					{
					TInt requiredLength = aVector[2 * n + 1] - start;
					if((aOut.Length() + requiredLength) <= aOut.MaxLength())
						{
						aOut.Append(aText.Mid(start, requiredLength));
						}
					else
						{
						iErrorCode = KErrRegExOutputTooBig;
						return EFalse;
						}						
					}				
				}
				else if (c == '\\')
					{
					if((aOut.Length() + 1) <= aOut.MaxLength())
						{
						aOut.Append(c);
						}
					else
						{
						iErrorCode = KErrRegExOutputTooBig;
						return EFalse;
						}
					}
				else
					{
					// Invalid rewrite pattern
					iErrorCode = KErrRegExInvalidRewritePattern;
					return EFalse;
					}
			} 
		else
			{
			if((aOut.Length() + 1) <= aOut.MaxLength())
				{
				aOut.Append(c);
				}
			else
				{
				iErrorCode = KErrRegExOutputTooBig;
				return EFalse;
				}
			}
		}
	return ETrue;
	}

/**
 * Internal implementation of the match functionality.
 * @param aText the text to match against the regular expression.
 * @param aAnchor the type of match to perform.
 * @param aConsumed the length of the matched substring.
 * @param aArgs array of arguments that will contain the extracted subpatterns.
 * @param aVector output vector that stores pairs of integers for the start and
 * end positions of matched substrings.
 * @param aVectorSize length of aVector
 * @return
 */
TBool CRegEx::DoMatchImpl(const TDesC8& aText,
                    TAnchor aAnchor,
                    TInt& aConsumed,
                    const RPointerArray<const TRegExArg>& aArgs,
      			   	TInt* aVector,
      			    TInt aVectorSize) const
    {
    // results + PCRE workspace
	__ASSERT_DEBUG((1 + aArgs.Count()) * KPcreWorkspaceMultiplier <= aVectorSize,Panic(EVectorTooSmall) );
	TInt matches = TryMatch(aText, 0, aAnchor, aVector, aVectorSize);
	// TryMatch never returns negatives
	__ASSERT_DEBUG(matches >= 0, Panic(EInvalidMatchResults));  
	
	if (matches == 0)
		{
		iErrorCode = KErrRegExZeroMatches;
		return EFalse;
		}	
	
	aConsumed = aVector[1];
	
	if (aArgs.Count() == 0)
		{
		// We are not interested in results
		return ETrue;
		}
	
	if (NumberOfCapturingGroups() < aArgs.Count())
		{
		// RE has fewer capturing groups than number of arg pointers passed in
		iErrorCode = KErrRegExFewerCaptureGroupsThanArgs;
		return EFalse;
		}

	// If we got here, we must have matched the whole pattern.
	// We do not need (can not do) any more checks on the value of 'matches'
	// here -- see the comment for TryMatch.
	for (TInt i = 0; i < aArgs.Count(); i++)
		{
		const TInt start = aVector[2*(i+1)];
		const TInt limit = aVector[2*(i+1)+1];
		
		TBool r;
		if(start == -1 || limit == -1)
			{
			r = aArgs[i]->Parse(KNullDesC8());
			}
		else
			{
			r = aArgs[i]->Parse(aText.Mid(start, limit - start));
			}
		if(!r)
			{
			iErrorCode = KErrRegExFailedToParseArg;
			return EFalse;			
			}
		}

	return ETrue;
	}

/**
 * Panic the current thread.
 * @param aPanic panic code.
 */
void CRegEx::Panic(TRegExPanic aPanic)
	{
	User::Panic(KRegExPanic(), aPanic);
	}
