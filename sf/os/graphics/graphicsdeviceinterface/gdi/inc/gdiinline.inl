// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Contains the inline functions IsAlphaChannel, QuoteOrBracketPair and IsIgnoredCharacterForLocalisedProcFunc.
// 
//
#ifndef GDIINLINE_INL
#define GDIINLINE_INL

	
/** Utility function to check if a display mode has Alpha channel information
@param aDisplayMode - the display mode being queried
@return ETrue if display mode contains Alpha information.
@internalTechnology
@released
*/
inline TBool IsAlphaChannel(TDisplayMode aDisplayMode)
	{
	if(aDisplayMode == EColor16MAP || aDisplayMode == EColor16MA) 
		return ETrue;
	else
		return EFalse;
	}

/**
@internalTechnology
@released
*/
inline TUint QuoteOrBracketPair(TUint code)
	{
	// given the opening/closing quote or bracket, return the corresponding closing/opening quote or bracket
	switch(code)
		{
		case 0x0022: return 0x0022; // "..."
		case 0x0027: return 0x0027; // '...'
		case 0x0028: return 0x0029; // (...)
		case 0x003c: return 0x003e; // <...>
		case 0x005b: return 0x005d; // [...]
		case 0x007b: return 0x007d; // {...}
		case 0x2018: return 0x2019; // Single quotation marks
		case 0x201b: return 0x2019; // Single high-reversed-9 quotation mark
		case 0x201c: return 0x201d; // Double quotation marks
		case 0x201f: return 0x201d; // Double high-reversed-9 quotation mark
		case 0x2035: return 0x2032; // Single primes
		case 0x2036: return 0x2033; // Double primes
		case 0x2037: return 0x2034; // Triple primes
		case 0x2039: return 0x203a; // Single left/right-pointing angle quotation marks
		case 0x2045: return 0x2046; // Square brackets with quill
		
		case 0x0029: return 0x0028; // (...)
		case 0x003e: return 0x003c; // <...>
		case 0x005d: return 0x005b; // [...]
		case 0x007d: return 0x007b; // {...}
		case 0x2019: return 0x2018; // Single quotation marks
		case 0x201d: return 0x201c; // Double quotation marks
		case 0x2032: return 0x2035; // Single primes
		case 0x2033: return 0x2036; // Double primes
		case 0x2034: return 0x2037; // Triple primes
		case 0x203a: return 0x2039; // Single left/right-pointing angle quotation marks
		case 0x2046: return 0x2045; // Square brackets with quill
				
		default: return 0;
		}
	}

/**
@internalTechnology
@released
*/	
inline TBool IsIgnoredCharacterForLocalisedProcFunc(TChar aCode)
	{
	TChar::TBdCategory cat = aCode.GetBdCategory();

	if ((cat == TChar::ELeftToRight ||
		cat == TChar::ERightToLeft ||
		cat == TChar::ERightToLeftArabic) && aCode != 0 && aCode != 0xFFFF)
		return EFalse;
	
	return ETrue;
	}




#endif /* GDIINLINE_INL */
