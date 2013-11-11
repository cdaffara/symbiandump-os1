// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Declares utility classes private to the GSMU dll
// 
//

/**
 @file
 @file
 @internalComponent 
*/


#ifndef __GSMUPRIV_H__
#define __GSMUPRIV_H__

#include <charconv.h>
#include "Gsmuelem.h"



/**
 *  TSmsPacker - packs and unpacks data encoded in an SMS alphabet
 *  @internalComponent
 */
class TSmsAlphabetPacker
	{
public:
// Construction
	TSmsAlphabetPacker(TSmsDataCodingScheme::TSmsAlphabet aAlphabet,TBool aIsBinary,TInt aStartBit);

// Packing / unpacking methods
	TInt PackL(TDes8& aOut,const TDesC8& aIn);
	TInt UnpackL(const TDesC8& aIn,TDes8& aOut,TInt aNumUDUnits);
	TInt ConvertAndPackL(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs,TDes8& aOut,const TDesC& aIn,TInt& aConvertedNumUDUnits);
	TInt UnpackAndConvertL(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs,const TDesC8& aIn,TDes& aOut,TInt aNumUDUnits);

	TInt PackedOctetsRequiredL(TInt aNumUDUnits) const;
	TInt NumUDUnitsL(TInt aOctets) const;

private:
// Private helper methods
	TInt ElementSizeInBitsL() const;

private:
	TSmsDataCodingScheme::TSmsAlphabet iAlphabet;
	TBool iIsBinary;
	TInt iStartBit;
	};

#endif // __GSMUPRIV_H__
