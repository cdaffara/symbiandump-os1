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
// SUPL INIT - Client interface for SUPL INIT encoding & decoding.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __SUPLINIT_H__
#define __SUPLINIT_H__

#include <e32base.h>
#include <lbs/test/suplmessage.h>

enum TSuplSlpMode
	{
	EProxy = 0,
	ENoProxy = 1
	};

class CSuplInitContent : public CBase
	{
public:
	IMPORT_C static CSuplInitContent* NewL();
	IMPORT_C static CSuplInitContent* NewLC();
	IMPORT_C~CSuplInitContent();
	TBool operator==(const CSuplInitContent& other) const;
	
private:
	CSuplInitContent();
	
public:	

	TSuplPosMethod iPosMethod; //shared with supl response
	TBool iNotificationPresent;
	TBool iSLPAddressPresent;
	TBool iQopPresent;
	TSuplSlpMode iSlpMode;
	TBool iMacPresent;
	TBool iKeyIdentityPresent;	
	};


/**
 * The class defines the SUPL INIT Message, 
 * which is the initial message from SLP to SET
 */
class CSuplInit : public CBase
	{
public:
	static CSuplInit* NewL(CSuplInitContent* aSuplInitContent);
	static CSuplInit* NewLC(CSuplInitContent* aSuplInitContent);
	virtual ~CSuplInit();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	
private:
	CSuplInit(CSuplInitContent* aSuplInitContent);
	void ConstructL();
	CSuplInitContent* iSuplInitContent;
	};

#endif // __SUPLINIT_H__
