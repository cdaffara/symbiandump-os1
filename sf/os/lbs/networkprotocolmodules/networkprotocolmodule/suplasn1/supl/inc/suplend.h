// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SUPL END - Client interface for SUPL END encoding & decoding.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __SUPLEND_H__
#define __SUPLEND_H__

#include <e32base.h>
#include <lbs/test/suplmessage.h>



class CSuplEndContent : public CBase
	{
public:
	IMPORT_C static CSuplEndContent* NewL();
	IMPORT_C static CSuplEndContent* NewLC();
	IMPORT_C~CSuplEndContent();
	TBool operator==(const CSuplEndContent& other) const;
	
private:
	CSuplEndContent();	
	
public:	
	TBool iPositionPresent; //EFalse
	TBool iStatusCodePresent; //EFalse
	TBool iVerPresent; //EFalse
	};


/**
 * The class defines the SUPL INIT Message, 
 * which is the initial message from SLP to SET
 */
class CSuplEnd : public CBase
	{
public:
	static CSuplEnd* NewL(CSuplEndContent* aSuplEndContent);
	static CSuplEnd* NewLC(CSuplEndContent* aSuplEndContent);
	~CSuplEnd();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	
private:
	CSuplEnd(CSuplEndContent* aSuplEndContent);
	void ConstructL();
	CSuplEndContent* iSuplEndContent;
	};

#endif // __SUPLEND_H__
