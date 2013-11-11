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
// SUPL RESPONSE - Client interface for SUPL RESPONSE encoding & decoding.
// 
//

/**
 @file
 @internalComponent
 @test
*/
#ifndef __SUPLRESPONSE_H__
#define __SUPLRESPONSE_H__

#include <e32base.h>
#include <lbs/test/suplmessage.h>



class CSuplResponseContent : public CBase
	{
public:
	IMPORT_C static CSuplResponseContent* NewL();
	IMPORT_C static CSuplResponseContent* NewLC();
	IMPORT_C~CSuplResponseContent();
	TBool operator==(const CSuplResponseContent& other) const;
	
private:
	CSuplResponseContent();
	
public:	
	TSuplPosMethod iPosMethod; //shared with supl init
	TBool iSlpAddressPresent;
	TBool iSetAuthKeyPresent;
	TBool iKeyId4Present;
	};


/**
 * The class defines the SUPL INIT Message, 
 * which is the initial message from SLP to SET
 */
class CSuplResponse : public CBase
	{
public:
	static CSuplResponse* NewL(CSuplResponseContent* aSuplResponseContent);
	static CSuplResponse* NewLC(CSuplResponseContent* aSuplResponseContent);
	~CSuplResponse();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	
private:
	CSuplResponse(CSuplResponseContent* aSuplResponseContent);
	void ConstructL();
	CSuplResponseContent* iSuplResponseContent;
	};

#endif // __SUPLRESONSE_H__
