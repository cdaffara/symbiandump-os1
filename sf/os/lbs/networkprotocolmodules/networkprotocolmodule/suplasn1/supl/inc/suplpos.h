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
// SUPL POS - Client interface for SUPL POS encoding & decoding.
// 
//

/**
 @file
 @internalComponent
 @test
*/
#ifndef __SUPLPOS_H__
#define __SUPLPOS_H__

#include <lbs/test/suplmessage.h>


enum TSuplPosPayloadType
	{
	ESuplPosPayloadTia801 = 0,
	ESuplPosPayloadRrc,
	ESuplPosPayloadRrlp,

	ESuplPosPayloadTypeLimit
	};

class CSuplPosPayloadContent : public CBase
	{
public:
	IMPORT_C static CSuplPosPayloadContent* NewL();
	IMPORT_C static CSuplPosPayloadContent* NewLC();
	IMPORT_C ~CSuplPosPayloadContent();
	TBool operator==(const CSuplPosPayloadContent& other) const;
	
private:
	CSuplPosPayloadContent();
	void ConstructL();
	
public:
	TInt iPayloadType;
	RBuf8 iPayload; // Hold the ecoded RRC payload to further encode SUPL POS
	TUint16 iPayloadOctetLength; // Length of RRC for encoding SUPL POS and decoding RRC payload
	TUint16 iPayloadStartBitPos; //Start bit pos for decoding RRC payload
	};

class CSuplPosPayload : public CBase
	{
public:
	static CSuplPosPayload* NewL(CSuplPosPayloadContent* aPosPayloadContent);
	static CSuplPosPayload* NewLC(CSuplPosPayloadContent* aPosPayloadContent);
	~CSuplPosPayload();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	
private:
	CSuplPosPayload(CSuplPosPayloadContent* aPosPayloadContent);	
	void ConstructL();
	CSuplPosPayloadContent* iPosPayloadContent;
	};


/**
 * The class holds the SUPL POS Message content
 */
class CSuplPosContent : public CBase
	{
public:
	IMPORT_C static CSuplPosContent* NewL();	
	IMPORT_C static CSuplPosContent* NewLC();
	IMPORT_C ~CSuplPosContent();
	TBool operator==(const CSuplPosContent& other) const;
	
private:
	CSuplPosContent();
	void ConstructL();
	void ConstructL(const CSuplMessageContent* aSuplMessageContent);
public:
	TBool iVelocityPresent; //EFalse
	CSuplPosPayloadContent* iPosPayloadContent;
	};


/**
 * The class encodes and decodes the SUPL POS Message, 
 * which is the initial message from SLP to SET
 */
class CSuplPos : public CBase
	{
public:
	static CSuplPos* NewL(CSuplPosContent* aSuplPosContent);
	static CSuplPos* NewLC(CSuplPosContent* aSuplPosContent);
	~CSuplPos();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);

private:
	CSuplPos(CSuplPosContent* aSuplPosContent);
	void ConstructL();
	CSuplPosContent* iSuplPosContent;
	CSuplPosPayload* iPosPayload;
	};



#endif // __SUPLPOS_H__
