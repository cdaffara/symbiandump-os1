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
// SUPL Message - Implementation of SUPL message encoding & decoding.
// 
//

#include "suplmessage.h"
#include "asn1per.h"
#include "suplstart.h"
#include "suplinit.h"
#include "suplpos.h"
#include "suplposinit.h"
#include "suplend.h"
#include "suplresponse.h"

/**
Implementation of Supl Message Content Class
*/

/**
CSuplMessageContent NewL
@internalTechnology
@test
 */
EXPORT_C CSuplMessageContent* CSuplMessageContent::NewL()
	{
	CSuplMessageContent* self = CSuplMessageContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

/**
CSuplMessageContent NewLC
@internalTechnology
@test
 */
EXPORT_C CSuplMessageContent* CSuplMessageContent::NewLC()
	{
	CSuplMessageContent* self=new (ELeave) CSuplMessageContent();
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C CSuplMessageContent::~CSuplMessageContent()
	{
	delete iSuplVersionContent;
	delete iSuplSessionIdContent;
	delete iSuplStartContent;
	delete iSuplResponseContent;
	delete iSuplPosInitContent;
	delete iSuplPosContent;
	delete iSuplInitContent;
	delete iSuplEndContent;
	}
	
EXPORT_C TBool CSuplMessageContent::operator==(const CSuplMessageContent& other) const
	{
	TBool ret(EFalse);
	if (iLength == other.iLength && 
			*iSuplVersionContent==*other.iSuplVersionContent &&	
			*iSuplSessionIdContent==*other.iSuplSessionIdContent &&
			iMessageChoice==other.iMessageChoice)
		{
		switch (iMessageChoice)
			{
			case ESuplStart:				
				ret=(*iSuplStartContent==*other.iSuplStartContent);
				break;
				
			case ESuplResponse:
				ret=(*iSuplResponseContent==*other.iSuplResponseContent);
				break;
				
			case ESuplPosInit:
				ret=(*iSuplPosInitContent==*other.iSuplPosInitContent);
				break;
				
			case ESuplPos:
				ret=(*iSuplPosContent==*other.iSuplPosContent);
				break;
					
			case ESuplEnd:
				ret=(*iSuplEndContent==*other.iSuplEndContent);
				break;
				
			case ESuplInit:					
			case ESuplAuthReq:
			case ESuplAuthResp:		
			default:	
				;
			}
		}
	return ret;	
	}
		
/**
Implementation of Supl Message class
*/

/**
CSuplMessage NewL for Decoding
@internalTechnology
@test
 */
EXPORT_C CSuplMessage* CSuplMessage::NewL()
	{
	CSuplMessage* self = CSuplMessage::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

/**
CSuplMessage NewLC for Decoding
@internalTechnology
@test
 */
EXPORT_C CSuplMessage* CSuplMessage::NewLC()
	{
	CSuplMessage* self = new (ELeave) CSuplMessage();
	CleanupStack::PushL(self);
	return self;				
	}

/**
CSuplMessage NewL for Encoding
@internalTechnology
@test
 */
EXPORT_C CSuplMessage* CSuplMessage::NewL(const CSuplMessageContent& aMessageContent)
	{
	CSuplMessage* self = CSuplMessage::NewLC(aMessageContent);
	CleanupStack::Pop(self);
	return self;					
	}

/**
CSuplMessage NewLC for Encoding
@internalTechnology
@test
 */
EXPORT_C CSuplMessage* CSuplMessage::NewLC(const CSuplMessageContent& aMessageContent)
	{
	CSuplMessage* self = new (ELeave) CSuplMessage(aMessageContent);
	CleanupStack::PushL(self);
	return self;				
	}

/**
CSuplMessage Destructor
@internalTechnology
@test
 */
EXPORT_C CSuplMessage::~CSuplMessage()
	{
	//Delete the encoders and decoders
	delete iVersion;
	delete iSessionId;
	delete iSuplStart;
	delete iSuplInit;
	delete iSuplPosInit;
	delete iSuplPos;
	delete iSuplEnd;
	delete iSuplResponse;
	
	//Delete the temp encoder buffer
	delete iEncoding;	
	}

/**
CSuplMessage Encoding API
@internalTechnology
@test
 */
EXPORT_C void CSuplMessage::EncodeL(RBuf8& aBuf)
	{
	// Supl message is not extensible, no "E" bit.
	// Encode the common part by call base class function
	EncodeCommonPartL();
	
	//See if the specific message is a extended one
	TBool isSuplExtMsg(EFalse);

	if (iSuplMessageContent->iMessageChoice>7)
		{
		isSuplExtMsg=ETrue;
		}
		
	//Encode choice index, which need to be in iEncoding anyway 
	CAsn1PerEncodingFragment* uplChoiceEnc=CAsn1PerChoice::EncodeLC(0,
																	iSuplMessageContent->iMessageChoice, 
																	7,
																	KAsn1PerExtensible,
																	isSuplExtMsg);
	iEncoding->AddElementL(uplChoiceEnc);		
	CleanupStack::PopAndDestroy(uplChoiceEnc);	
	
	
	// Encode Supl Message Specific Part
	switch (iSuplMessageContent->iMessageChoice)
		{
		case ESuplStart:
			iSuplStart=CSuplStart::NewL(iSuplMessageContent->iSuplStartContent);
			iSuplStart->EncodeL(*iEncoding);
			break;
			
		case ESuplResponse:
			iSuplResponse=CSuplResponse::NewL(iSuplMessageContent->iSuplResponseContent);
			iSuplResponse->EncodeL(*iEncoding);
			break;
			
		case ESuplPosInit:
			iSuplPosInit=CSuplPosInit::NewL(iSuplMessageContent->iSuplPosInitContent);
			iSuplPosInit->EncodeL(*iEncoding);
			break;
			
		case ESuplPos:
			iSuplPos=CSuplPos::NewL(iSuplMessageContent->iSuplPosContent);
			iSuplPos->EncodeL(*iEncoding);
			break;	
				
		case ESuplEnd:
			iSuplEnd=CSuplEnd::NewL(iSuplMessageContent->iSuplEndContent);
			iSuplEnd->EncodeL(*iEncoding);
			break;
			
		case ESuplInit:					
		case ESuplAuthReq:
		case ESuplAuthResp:		
		default:	
			User::Leave(KErrNotSupported);
		}
	
	// Copy the buffer to user	
	TInt length=(iEncoding->iBitLength%8)? iEncoding->iBitLength/8+1 : iEncoding->iBitLength/8;
	iEncoding->iContent.SetLength(length);
	aBuf.ReAllocL(iEncoding->iContent.Length());
	aBuf.Copy(iEncoding->iContent);					
	}

/**
CSuplMessage Decoding API
@internalTechnology
@test
 */	
EXPORT_C CSuplMessageContent* CSuplMessage::DecodeL(TDesC8& aDataToBeDecoded)
	{
	CSuplMessageContent* suplMessageContent=CSuplMessageContent::NewLC();
	
	TInt startPos(0);
	TInt bitsDecoded(0);
	TBool isExtended(EFalse);	
	
	//No "E" bit, since it is not extensible.
	// Decode the Supl Message length.	
	CAsn1PerInt::DecodeL(aDataToBeDecoded, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						65535,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						suplMessageContent->iLength);
						
	startPos+=bitsDecoded;

	//Decode the Supl Message version.	
	suplMessageContent->iSuplVersionContent=CSuplVersionContent::NewL();	
	iVersion=CSuplVersion::NewL(suplMessageContent->iSuplVersionContent);		
	iVersion->DecodeFromL(aDataToBeDecoded, startPos, bitsDecoded);
	startPos+=bitsDecoded;

	// Decode the Supl Message SessionID
	suplMessageContent->iSuplSessionIdContent=CSuplSessionIdContent::NewL();
	iSessionId=CSuplSessionId::NewL(suplMessageContent->iSuplSessionIdContent);
	iSessionId->DecodeFromL(aDataToBeDecoded, startPos, bitsDecoded);
	startPos+=bitsDecoded;
	
	// Decode the Message Type Chioce, which is extensible
	CAsn1PerChoice::DecodeL(aDataToBeDecoded,
							startPos,
							7,
							KAsn1PerExtensible,
							bitsDecoded,
							isExtended,
							suplMessageContent->iMessageChoice
							);
	startPos+=bitsDecoded;
	
	switch (suplMessageContent->iMessageChoice)
		{
	case ESuplStart:
		suplMessageContent->iSuplStartContent=CSuplStartContent::NewL();
		iSuplStart=CSuplStart::NewL(suplMessageContent->iSuplStartContent);
		iSuplStart->DecodeFromL(aDataToBeDecoded, startPos, bitsDecoded);
		break;
		
	case ESuplResponse:
		suplMessageContent->iSuplResponseContent=CSuplResponseContent::NewL();
		iSuplResponse=CSuplResponse::NewL(suplMessageContent->iSuplResponseContent);		
		iSuplResponse->DecodeFromL(aDataToBeDecoded, startPos, bitsDecoded);
		break;
		
	case ESuplPosInit:
		suplMessageContent->iSuplPosInitContent=CSuplPosInitContent::NewL();
		iSuplPosInit=CSuplPosInit::NewL(suplMessageContent->iSuplPosInitContent);			
		iSuplPosInit->DecodeFromL(aDataToBeDecoded, startPos, bitsDecoded);
		break;
		
	case ESuplPos:
		suplMessageContent->iSuplPosContent=CSuplPosContent::NewL();
		iSuplPos=CSuplPos::NewL(suplMessageContent->iSuplPosContent);
		iSuplPos->DecodeFromL(aDataToBeDecoded, startPos, bitsDecoded);
		break;	
			
	case ESuplEnd:
		suplMessageContent->iSuplEndContent=CSuplEndContent::NewL();
		iSuplEnd=CSuplEnd::NewL(suplMessageContent->iSuplEndContent);	
		iSuplEnd->DecodeFromL(aDataToBeDecoded, startPos, bitsDecoded);
		break;
		
	case ESuplInit:					
	case ESuplAuthReq:
	case ESuplAuthResp:		
	default:	
		User::Leave(KErrNotSupported);			
		}
	startPos+=bitsDecoded;
	
	CleanupStack::Pop(suplMessageContent);
	return suplMessageContent;
	}
	

CSuplMessage::CSuplMessage(const CSuplMessageContent& aMessageContent)
: iSuplMessageContent(&aMessageContent)
	{			
	}

CSuplMessage::CSuplMessage()
	{			
	}
	
void CSuplMessage::ConstructL()
	{
	}
		
void CSuplMessage::EncodeCommonPartL()
	{
	//Create the buffer to hold the encoded data.
	iEncoding=CAsn1PerEncodingBuffer::NewL(0, ETrue);
	
	//No "E" Bit
	//Encode the SUPL message length	
	CAsn1PerEncodingFragment* lenbit=CAsn1PerInt::EncodeLC(0, 
														iSuplMessageContent->iLength, 
														KAsn1PerMinConstrained, 
														0, 
														KAsn1PerMaxConstrained, 
														65535, 
														KAsn1PerNonExtensible, 
														KAsn1PerNonExtended,
														KAsn1PerNoPadding);
	
	iEncoding->AddElementL(lenbit);
	CleanupStack::PopAndDestroy(lenbit);
	
	//Encode the version
	iVersion=CSuplVersion::NewL(iSuplMessageContent->iSuplVersionContent);
	iVersion->EncodeL(*iEncoding);
	
	//Encode the SessionId
	iSessionId=CSuplSessionId::NewL(iSuplMessageContent->iSuplSessionIdContent);
	iSessionId->EncodeL(*iEncoding);	
	}
	
	
/**
Implementation of Supl Message Version content
*/	

/**
CSuplVersionContent NewL
@internalTechnology
@test
 */
EXPORT_C CSuplVersionContent* CSuplVersionContent::NewLC()
	{
	CSuplVersionContent* self = new (ELeave) CSuplVersionContent();
	CleanupStack::PushL(self);
	return self;					
	}
	
/**
CSuplVersionContent NewLC
@internalTechnology
@test
 */	
EXPORT_C CSuplVersionContent* CSuplVersionContent::NewL()
	{
	CSuplVersionContent* self = CSuplVersionContent::NewLC();
	CleanupStack::Pop(self);
	return self;	
	}

CSuplVersionContent* CSuplVersionContent::NewLC(const CSuplVersionContent* aVersionContent)
	{
	CSuplVersionContent* self = new (ELeave) CSuplVersionContent(aVersionContent);
	CleanupStack::PushL(self);
	return self;							
	}

CSuplVersionContent* CSuplVersionContent::NewL(const CSuplVersionContent* aVersionContent)
	{
	CSuplVersionContent* self = CSuplVersionContent::NewLC(aVersionContent);
	CleanupStack::Pop(self);
	return self;			
	}

TBool CSuplVersionContent::operator==(const CSuplVersionContent& other) const
	{
	return (iMaj == other.iMaj && 
			iMin==other.iMin &&	
			iServind==other.iServind);
	}

CSuplVersionContent::CSuplVersionContent(const CSuplVersionContent* aVersionContent)
	{
	iMaj=aVersionContent->iMaj;
	iMin=aVersionContent->iMin;
	iServind=aVersionContent->iServind;
	}

CSuplVersionContent::CSuplVersionContent()
	{
	}
	
EXPORT_C CSuplVersionContent::~CSuplVersionContent()
	{
	}

	
/**
Implementation of Supl Message Version class
*/
CSuplVersion* CSuplVersion::NewL(CSuplVersionContent* aSuplVersionContent)
	{
	CSuplVersion* self = CSuplVersion::NewLC(aSuplVersionContent);
	CleanupStack::Pop(self);
	return self;						
	}

CSuplVersion* CSuplVersion::NewLC(CSuplVersionContent* aSuplVersionContent)
	{
	CSuplVersion* self = new (ELeave) CSuplVersion(aSuplVersionContent);
	CleanupStack::PushL(self);
	return self;			
	}
	
CSuplVersion::CSuplVersion(CSuplVersionContent* aSuplVersionContent)
:iSuplVersionContent(aSuplVersionContent)
	{
	}

CSuplVersion::~CSuplVersion()
	{
	}
	
void CSuplVersion::EncodeL(CAsn1PerEncoding& aPerEncoding)
	{
	// Supl message version is not extensible, no "E" bit.
	
	//Encode the maj
	CAsn1PerEncodingFragment* majEnc=CAsn1PerInt::EncodeLC(0, 
														iSuplVersionContent->iMaj, 
														KAsn1PerMinConstrained, 
														0, 
														KAsn1PerMaxConstrained, 
														255, 
														KAsn1PerNonExtensible, 
														KAsn1PerNonExtended,
														KAsn1PerNoPadding);

	aPerEncoding.AddElementL(majEnc);
	CleanupStack::PopAndDestroy(majEnc);


	//Encode the min
	CAsn1PerEncodingFragment* minEnc=CAsn1PerInt::EncodeLC(0, 
														iSuplVersionContent->iMin, 
														KAsn1PerMinConstrained, 
														0, 
														KAsn1PerMaxConstrained, 
														255, 
														KAsn1PerNonExtensible, 
														KAsn1PerNonExtended,
														KAsn1PerNoPadding);


	aPerEncoding.AddElementL(minEnc);
	CleanupStack::PopAndDestroy(minEnc);


	//Encode the servind
	CAsn1PerEncodingFragment* servindEnc=CAsn1PerInt::EncodeLC(0, 
														iSuplVersionContent->iServind, 
														KAsn1PerMinConstrained, 
														0, 
														KAsn1PerMaxConstrained, 
														255, 
														KAsn1PerNonExtensible, 
														KAsn1PerNonExtended,
														KAsn1PerNoPadding);


	aPerEncoding.AddElementL(servindEnc);
	CleanupStack::PopAndDestroy(servindEnc);
	}
	
	
void CSuplVersion::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBit, TInt& aDecodedBits)	
	{
	
	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBit;
	TInt bitsDecoded=0;
	
	//no "E" bit, since it is not extensible.

	//Decode the major version	
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						255,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iSuplVersionContent->iMaj);
	aDecodedBits+=bitsDecoded;					
	startPos+=bitsDecoded;


	//Decode the minor version
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						255,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iSuplVersionContent->iMin);		
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	
	
	//Decode the servind version
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						255,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iSuplVersionContent->iServind);		
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded; //no use for the last one	
	}

/**
Implementation of Supl Message SessionId class
*/	
CSuplSessionId* CSuplSessionId::NewL(CSuplSessionIdContent* aSessionIdContent)
	{
	CSuplSessionId* self = CSuplSessionId::NewLC(aSessionIdContent);
	CleanupStack::Pop(self);
	return self;						
	}

CSuplSessionId* CSuplSessionId::NewLC(CSuplSessionIdContent* aSessionIdContent)
	{
	CSuplSessionId* self = new (ELeave) CSuplSessionId(aSessionIdContent);
	CleanupStack::PushL(self);
	return self;			
	}
		
CSuplSessionId::~CSuplSessionId()
	{
	delete iSetSessionId;
	delete iSlpSessionId;
	}
		
CSuplSessionId::CSuplSessionId(CSuplSessionIdContent* aSessionIdContent)
:iSessionIdContent(aSessionIdContent)
	{		
	}
	
void CSuplSessionId::EncodeL(CAsn1PerEncoding& aPerEncoding)
	{
	
	// Supl message SessionId sequence is not extensible, no "E" bit.	
	
	// Option bit map
	CAsn1PerEncodingFragment* opt=CAsn1PerOption::EncodeLC(aPerEncoding.iParentBitLength, iSessionIdContent->iSetSessionIdPresent);
	aPerEncoding.AddElementL(opt);
	CleanupStack::PopAndDestroy(opt);
	
	opt=CAsn1PerOption::EncodeLC(aPerEncoding.iParentBitLength, iSessionIdContent->iSlpSessionIdPresent);
	aPerEncoding.AddElementL(opt);
	CleanupStack::PopAndDestroy(opt);

	if (iSessionIdContent->iSetSessionIdPresent)
		{
		//Encode the SUPL message session ID
		iSetSessionId=CSuplSetSessionId::NewL(iSessionIdContent->iSetSessionIdContent);
		iSetSessionId->EncodeL(aPerEncoding);		
		}
		
	if (iSessionIdContent->iSlpSessionIdPresent)
		{
		//Encode the SUPL message session ID		
		iSlpSessionId=CSuplSlpSessionId::NewL(iSessionIdContent->iSlpSessionIdContent);
		iSlpSessionId->EncodeL(aPerEncoding);
		}		
	}
	
void CSuplSessionId::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
	
	aDecodedBits=0;
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;
	
	//no "E" bit, since it is not extensible.
	
	//Decode option bit map
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iSessionIdContent->iSetSessionIdPresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iSessionIdContent->iSlpSessionIdPresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	
	if (iSessionIdContent->iSetSessionIdPresent)
		{
		//Decode the Supl Message Set Session Id.
		iSessionIdContent->iSetSessionIdContent=CSuplSetSessionIdContent::NewL();
		iSetSessionId=CSuplSetSessionId::NewL(iSessionIdContent->iSetSessionIdContent);
		iSetSessionId->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
		aDecodedBits+=bitsDecoded;	
		startPos+=bitsDecoded;			
		}
	
	if (iSessionIdContent->iSlpSessionIdPresent)
		{
		// Decode the Supl Message Session Id
		iSessionIdContent->iSlpSessionIdContent=CSuplSlpSessionIdContent::NewL();
		iSlpSessionId=CSuplSlpSessionId::NewL(iSessionIdContent->iSlpSessionIdContent);
		iSlpSessionId->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
		aDecodedBits+=bitsDecoded;	
		startPos+=bitsDecoded;			
		}
	}
	
void CSuplSessionId::ConstructL()
	{
	}
	
	
	
	
	



/**
Implementation of Supl Message Slp SessionId class
*/
CSuplSlpAddress* CSuplSlpAddress::NewL(CSuplSlpAddressContent* aSlpAddressContent)
	{
	CSuplSlpAddress* self = CSuplSlpAddress::NewLC(aSlpAddressContent);
	CleanupStack::Pop(self);
	return self;						
	}
	
CSuplSlpAddress* CSuplSlpAddress::NewLC(CSuplSlpAddressContent* aSlpAddressContent)
	{
	CSuplSlpAddress* self = new (ELeave) CSuplSlpAddress(aSlpAddressContent);
	CleanupStack::PushL(self);
	return self;					
	}
	
CSuplSlpAddress::~CSuplSlpAddress()
	{
	delete iIpAddress;
	}
	
void CSuplSlpAddress::EncodeL(CAsn1PerEncoding& aPerEncoding)
	{

	// SLPAddress ::= CHOICE {iPAddress  IPAddress,
	//                       fQDN       FQDN,
	//                       ...}
	
	if (iSlpSessionIdContent->iSlpAddressType!=ESuplSlpIpAddressChoice)
		{
		User::Leave(KErrNotSupported);	
		}
	
	//Enocde the SUPL message SLP address choice index
	CAsn1PerEncodingFragment* slpAddressChoiceEnc=CAsn1PerChoice::EncodeLC(aPerEncoding.iParentBitLength,
																		iSlpSessionIdContent->iSlpAddressType, 
																		1,
																		KAsn1PerExtensible,
																		KAsn1PerNonExtended);
	aPerEncoding.AddElementL(slpAddressChoiceEnc);	
	CleanupStack::PopAndDestroy(slpAddressChoiceEnc);
	
	//Encode the SLP address	
	switch (iSlpSessionIdContent->iSlpAddressType)
		{
	case ESuplSlpIpAddressChoice:
		iIpAddress=CSuplIpAddress::NewL(iSlpSessionIdContent->iIpAddressContent);	
		iIpAddress->EncodeL(aPerEncoding);
		break;
		
	default:
		User::Leave(KErrNotSupported);
		}
		
	}
void CSuplSlpAddress::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
	// SLPAddress ::= CHOICE {iPAddress  IPAddress,
	//                       fQDN       FQDN,
	//                       ...}	
	
	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;
	
	// Decode the SLP Address choice index, which is extensible
	TInt slpAddressType(0);
	// Decode the Message Type Chioce, which is extensible
	CAsn1PerChoice::DecodeL(aBufferToDecode,
							startPos,
							1,
							KAsn1PerExtensible,
							bitsDecoded,
							isExtended,
							slpAddressType	
							);
	iSlpSessionIdContent->iSlpAddressType=(TSuplSlpAddressType)slpAddressType;
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;			
	
	if (isExtended)
		{
		User::Leave(KErrArgument);
		//Should decode the length first, if in the futrue it is extended in the spec	
		}
	
	switch (iSlpSessionIdContent->iSlpAddressType)	
		{
	case ESuplSlpIpAddressChoice:
		{
		iSlpSessionIdContent->iIpAddressContent=CSuplIpAddressContent::NewL();
		iIpAddress=CSuplIpAddress::NewL(iSlpSessionIdContent->iIpAddressContent);
		iIpAddress->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
		aDecodedBits+=bitsDecoded;	
		startPos+=bitsDecoded;		
		}
		break;
		
	default:
		User::Leave(KErrNotSupported);
		}
	}

CSuplSlpAddress::CSuplSlpAddress(CSuplSlpAddressContent* aSlpAddressContent)
:iSlpSessionIdContent(aSlpAddressContent)
	{	
	}

void CSuplSlpAddress::ConstructL()
	{
	switch (iSlpSessionIdContent->iSlpAddressType)
		{
	case ESuplSlpIpAddressChoice:
		{
		iIpAddress=CSuplIpAddress::NewL(iSlpSessionIdContent->iIpAddressContent);			
		}
		break;
	default:
		User::Leave(KErrNotSupported);
		}
	}
	
	

/**
Implementation of Supl Message Slp SessionId class
*/
CSuplSlpSessionId* CSuplSlpSessionId::NewL(CSuplSlpSessionIdContent* aSlpSessionIdContent)
	{
	CSuplSlpSessionId* self = CSuplSlpSessionId::NewLC(aSlpSessionIdContent);
	CleanupStack::Pop(self);
	return self;						
	}
	
CSuplSlpSessionId* CSuplSlpSessionId::NewLC(CSuplSlpSessionIdContent* aSlpSessionIdContent)
	{
	CSuplSlpSessionId* self = new (ELeave) CSuplSlpSessionId(aSlpSessionIdContent);
	CleanupStack::PushL(self);
	return self;					
	}
	
CSuplSlpSessionId::~CSuplSlpSessionId()
	{
	delete iSlpAddress;	
	}
	
void CSuplSlpSessionId::EncodeL(CAsn1PerEncoding& aPerEncoding)
	{

	//SlpSessionID ::= SEQUENCE {
	//  sessionID  OCTET STRING(SIZE (4)),
	//  slpId      SLPAddress}

	// Supl message SlpSessionId sequence is not extensible, no "E" bit.

	// Encode the SlpSessionId
	CAsn1PerEncodingFragment* slpSessionIdEnc= CAsn1PerOctetString::EncodeLC(aPerEncoding.iParentBitLength,
																		iSlpSessionIdContent->iSessionId,
																		KAsn1LengthNonVariable,
																		KAsn1PerMaxConstrained,
																		4,
																		4,
																		KAsn1PerNonExtensible,
																		KAsn1PerNonExtended,
																		KAsn1PerNoPadding);
	aPerEncoding.AddElementL(slpSessionIdEnc);	
	CleanupStack::PopAndDestroy(slpSessionIdEnc);
	
	// Encode the SLP Address
	iSlpAddress=CSuplSlpAddress::NewL(iSlpSessionIdContent->iSlpAddressContent);
	iSlpAddress->EncodeL(aPerEncoding);
	}
void CSuplSlpSessionId::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
	//SlpSessionID ::= SEQUENCE {
	//  sessionID  OCTET STRING(SIZE (4)),
	//  slpId      SLPAddress}

	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	//No "E" bit, since not extensible
	
	//Decode the session ID
	RBuf8 sessionId;
	CAsn1PerOctetString::DecodeL(aBufferToDecode,
								startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						4,
						4,	
						KAsn1PerNonExtensible,
						KAsn1PerNoPadding,
						isExtended,
						bitsDecoded, 
						sessionId
						);						
	iSlpSessionIdContent->iSessionId=sessionId;
	sessionId.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
			
	// Decode the SLP Address
	iSlpSessionIdContent->iSlpAddressContent=CSuplSlpAddressContent::NewL();
	iSlpAddress=CSuplSlpAddress::NewL(iSlpSessionIdContent->iSlpAddressContent);
	iSlpAddress->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
	}

CSuplSlpSessionId::CSuplSlpSessionId(CSuplSlpSessionIdContent* aSlpSessionIdContent)
:iSlpSessionIdContent(aSlpSessionIdContent)
	{
		
	}

void CSuplSlpSessionId::ConstructL()
	{
	}


/**
Implementation of Supl Message IP Address class
*/

CSuplIpAddress* CSuplIpAddress::NewL(CSuplIpAddressContent* aIpAddressContent)
	{
	CSuplIpAddress* self = CSuplIpAddress::NewLC(aIpAddressContent);
	CleanupStack::Pop(self);
	return self;						
	}
	
CSuplIpAddress* CSuplIpAddress::NewLC(CSuplIpAddressContent* aIpAddressContent)
	{
	CSuplIpAddress* self = new (ELeave) CSuplIpAddress(aIpAddressContent);
	CleanupStack::PushL(self);
	return self;							
	}

CSuplIpAddress::~CSuplIpAddress()
	{
	}

void CSuplIpAddress::EncodeL(CAsn1PerEncoding& aPerEncoding)
	{
	
	//IPAddress ::= CHOICE {
	//  ipv4Address  OCTET STRING(SIZE (4)),
	//  ipv6Address  OCTET STRING(SIZE (16))}
	
	//Enocde the SUPL message IP address choice index
	CAsn1PerEncodingFragment* ipAddressChoiceEnc=CAsn1PerChoice::EncodeLC(aPerEncoding.iParentBitLength,
																		iIpAddressContent->iIpAddressType, 
																		1,
																		KAsn1PerNonExtensible,
																		KAsn1PerNonExtended);
	aPerEncoding.AddElementL(ipAddressChoiceEnc);	
	CleanupStack::PopAndDestroy(ipAddressChoiceEnc);
	
	TInt stringLen(0);
	if (iIpAddressContent->iIpAddressType==ESuplIpV4AddressChoice)
		{
		stringLen=4;	
		}
	else
		{
		stringLen=16;	
		}	
	
	//Encode the IPAddress	
	CAsn1PerEncodingFragment* ipAddressEnc= CAsn1PerOctetString::EncodeLC(aPerEncoding.iParentBitLength,
																		iIpAddressContent->iIpAddress,
																		KAsn1LengthNonVariable,
																		KAsn1PerMaxConstrained,
																		stringLen,
																		stringLen,
																		KAsn1PerNonExtensible,
																		KAsn1PerNonExtended,
																		KAsn1PerNoPadding);

	aPerEncoding.AddElementL(ipAddressEnc);	
	CleanupStack::PopAndDestroy(ipAddressEnc);
	}
	
void CSuplIpAddress::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
	//IPAddress ::= CHOICE {
	//  ipv4Address  OCTET STRING(SIZE (4)),
	//  ipv6Address  OCTET STRING(SIZE (16))}

	
	
	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;
	
	TInt ipAddressType(0);
	//Decode IP address choice index, which is not extensible
	CAsn1PerChoice::DecodeL(aBufferToDecode,
							startPos,
							1,
							KAsn1PerNonExtensible,
							bitsDecoded,
							isExtended,
							ipAddressType	
							);
	iIpAddressContent->iIpAddressType=(TSuplIpAddressType)ipAddressType;
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
	
	TInt stringLength(0);				
	switch (iIpAddressContent->iIpAddressType)	
		{
	case ESuplIpV4AddressChoice:
		{
		stringLength=4;
		}
		break;
		
	case ESuplIpV6AddressChoice:
		{
		stringLength=16;
		}
		break;
	default:
		User::Leave(KErrNotSupported);
		}
	
	//Decode the IP address
	RBuf8 ipAddress;
	CAsn1PerOctetString::DecodeL(aBufferToDecode,
								startPos,
								KAsn1LengthNonVariable,
								KAsn1PerMaxConstrained,
								stringLength,
								stringLength,	
								KAsn1PerNonExtensible,
								KAsn1PerNoPadding,
								isExtended,
								bitsDecoded, 
								ipAddress
								);						
	iIpAddressContent->iIpAddress=ipAddress;
	ipAddress.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;			
	}

CSuplIpAddress::CSuplIpAddress(CSuplIpAddressContent* aIpAddressContent)
:iIpAddressContent(aIpAddressContent)
	{
		
	}
void CSuplIpAddress::ConstructL()
	{
	}



/**
Implementation of Supl Message SetId class
*/

CSuplSetId* CSuplSetId::NewL(CSuplSetIdContent* aSetIdContent)
	{
	CSuplSetId* self = CSuplSetId::NewLC(aSetIdContent);
	CleanupStack::Pop(self);
	return self;						
	}
	
CSuplSetId* CSuplSetId::NewLC(CSuplSetIdContent* aSetIdContent)
	{
	CSuplSetId* self = new (ELeave) CSuplSetId(aSetIdContent);
	CleanupStack::PushL(self);
	return self;							
	}

CSuplSetId::~CSuplSetId()
	{
	delete 	iIpAddress;
	}

void CSuplSetId::EncodeL(CAsn1PerEncoding& aPerEncoding)
	{
	
	//SETId ::= CHOICE {
	//  msisdn     OCTET STRING(SIZE (8)),
	//  mdn        OCTET STRING(SIZE (8)),
	//  min        BIT STRING(SIZE (34)), -- coded according to TIA-553
	//  imsi       OCTET STRING(SIZE (8)),
	//  nai        IA5String(SIZE (1..1000)),
	//  iPAddress  IPAddress,
	//  ...}


	// Now only support IP Address	
	if (iSetIdContent->iSetIdType != ESuplIPAddress)
		{
		User::Leave(KErrNotSupported);
		}
	
	//Enocde the SUPL message SETID choice index
	CAsn1PerEncodingFragment* setIdChoiceEnc=CAsn1PerChoice::EncodeLC(aPerEncoding.iParentBitLength,
																	iSetIdContent->iSetIdType, 
																	5,
																	KAsn1PerExtensible,
																	KAsn1PerNonExtended);
	aPerEncoding.AddElementL(setIdChoiceEnc);
	
	CleanupStack::PopAndDestroy(setIdChoiceEnc);


	//Encode the IPAddress
	iIpAddress=CSuplIpAddress::NewL(iSetIdContent->iIpAddressContent);
	iIpAddress->EncodeL(aPerEncoding);
	
	}
void CSuplSetId::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
	
	//SETId ::= CHOICE {
	//  msisdn     OCTET STRING(SIZE (8)),
	//  mdn        OCTET STRING(SIZE (8)),
	//  min        BIT STRING(SIZE (34)), -- coded according to TIA-553
	//  imsi       OCTET STRING(SIZE (8)),
	//  nai        IA5String(SIZE (1..1000)),
	//  iPAddress  IPAddress,
	//  ...}

	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;
	
	//Decode IP address choice index, which is not extensible
	TInt setIdType(0);
	CAsn1PerChoice::DecodeL(aBufferToDecode,
							startPos,
							5,
							KAsn1PerExtensible,
							bitsDecoded,
							isExtended,
							setIdType	
							);
	iSetIdContent->iSetIdType=(TSuplSetIdType)setIdType;
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
	
	if (isExtended)
		{
		User::Leave(KErrArgument);
		// Need to decode length if the choice index is a extended one.
		}
	
	//Only Support IP Address for now	
	switch (iSetIdContent->iSetIdType)	
		{
	case ESuplIPAddress:
		{
		iSetIdContent->iIpAddressContent=CSuplIpAddressContent::NewL();
		iIpAddress=CSuplIpAddress::NewL(iSetIdContent->iIpAddressContent);
		iIpAddress->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
		aDecodedBits+=bitsDecoded;	
		startPos+=bitsDecoded;		
		}
		break;
		
	default:
		User::Leave(KErrNotSupported);
		}
	}

CSuplSetId::CSuplSetId(CSuplSetIdContent* aSetIdContent)
:iSetIdContent(aSetIdContent)
	{
		
	}
void CSuplSetId::ConstructL()
	{
	//iIpAddress=CSuplIpAddress::NewL(iSetIdContent->iIpAddressContent);
	}


/**
Implementation of Supl Message SetSessionId class
*/

CSuplSetSessionId* CSuplSetSessionId::NewL(CSuplSetSessionIdContent* aSetSessionIdContent)
	{
	CSuplSetSessionId* self = CSuplSetSessionId::NewLC(aSetSessionIdContent);
	CleanupStack::Pop(self);
	return self;						
	}
	
CSuplSetSessionId* CSuplSetSessionId::NewLC(CSuplSetSessionIdContent* aSetSessionIdContent)
	{
	CSuplSetSessionId* self = new (ELeave) CSuplSetSessionId(aSetSessionIdContent);
	CleanupStack::PushL(self);
	return self;							
	}

CSuplSetSessionId::~CSuplSetSessionId()
	{
	delete iSetId;	
	}

void CSuplSetSessionId::EncodeL(CAsn1PerEncoding& aPerEncoding)
	{
	
	//SetSessionID ::= SEQUENCE {sessionId  INTEGER(0..65535),
	//						setId      SETId}
	
		
	// Supl message SetSessionId squence is not extensible, no "E" bit.	
	
	//Encode the sessionId
	CAsn1PerEncodingFragment* sessionIdEnc=CAsn1PerInt::EncodeLC(0, 
														iSetSessionIdContent->iSessionId, 
														KAsn1PerMinConstrained, 
														0, 
														KAsn1PerMaxConstrained, 
														65535, 
														KAsn1PerNonExtensible, 
														KAsn1PerNonExtended,
														KAsn1PerNoPadding);
	
	aPerEncoding.AddElementL(sessionIdEnc);
	CleanupStack::PopAndDestroy(sessionIdEnc);
	

	//Encode SetId, TODO : a new CAsn1PerEncoding
	iSetId=CSuplSetId::NewL(iSetSessionIdContent->iSetIdContent);
	iSetId->EncodeL(aPerEncoding);
	
	
	
	}
void CSuplSetSessionId::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
	//SetSessionID ::= SEQUENCE {sessionId  INTEGER(0..65535),
	//						setId      SETId}

	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;
	
	//no "E" bit, since it is not extensible.

	//Decode the major version	
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						65535,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iSetSessionIdContent->iSessionId);
	aDecodedBits+=bitsDecoded;					
	startPos+=bitsDecoded;

	iSetSessionIdContent->iSetIdContent=CSuplSetIdContent::NewL();
	iSetId=CSuplSetId::NewL(iSetSessionIdContent->iSetIdContent);
	iSetId->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;	
	}

CSuplSetSessionId::CSuplSetSessionId(CSuplSetSessionIdContent* aSetSessionIdContent)
:iSetSessionIdContent(aSetSessionIdContent)
	{
		
	}
void CSuplSetSessionId::ConstructL()
	{
	iSetId=CSuplSetId::NewL(iSetSessionIdContent->iSetIdContent);
	}
	

/**
Implementation of Supl Message SlpSessionId content class
*/
/**
CSuplSlpSessionIdContent NewL
@internalTechnology
@test
 */	
EXPORT_C CSuplSlpSessionIdContent* CSuplSlpSessionIdContent::NewL()
	{
	CSuplSlpSessionIdContent* self = CSuplSlpSessionIdContent::NewLC();
	CleanupStack::Pop(self);
	return self;							
	}
/**
CSuplSlpSessionIdContent NewLC
@internalTechnology
@test
 */	
EXPORT_C CSuplSlpSessionIdContent* CSuplSlpSessionIdContent::NewLC()
	{
	CSuplSlpSessionIdContent* self = new (ELeave) CSuplSlpSessionIdContent();
	CleanupStack::PushL(self);
	return self;									
	}

EXPORT_C CSuplSlpSessionIdContent::~CSuplSlpSessionIdContent()
	{	
	delete iSlpAddressContent;
	}

TBool CSuplSlpSessionIdContent::operator==(const CSuplSlpSessionIdContent& other) const
	{
	return (iSessionId == other.iSessionId && 
			*iSlpAddressContent==*other.iSlpAddressContent);
	}	


void CSuplSlpSessionIdContent::ConstructL()
	{
	}	

/**
Implementation of Supl Message SetSessionId content class
*/
/**
CSuplSetSessionIdContent NewL
@internalTechnology
@test
 */	
EXPORT_C CSuplSetSessionIdContent* CSuplSetSessionIdContent::NewL()
	{	
	CSuplSetSessionIdContent* self = CSuplSetSessionIdContent::NewLC();
	CleanupStack::Pop(self);
	return self;							
	}
/**
CSuplSetSessionIdContent NewLC
@internalTechnology
@test
 */	
EXPORT_C CSuplSetSessionIdContent* CSuplSetSessionIdContent::NewLC()
	{
	CSuplSetSessionIdContent* self = new (ELeave) CSuplSetSessionIdContent();
	CleanupStack::PushL(self);
	return self;									
	}

EXPORT_C CSuplSetSessionIdContent::~CSuplSetSessionIdContent()
	{
	delete iSetIdContent;
	}
	
void CSuplSetSessionIdContent::ConstructL()
	{
	}
	
TBool CSuplSetSessionIdContent::operator==(const CSuplSetSessionIdContent& other) const
	{
	return (iSessionId==other.iSessionId && 
			*iSetIdContent==*other.iSetIdContent);
	}
	

/**
Implementation of Supl Message SessionId content class
*/
/**
CSuplSessionIdContent NewL
@internalTechnology
@test
 */	
EXPORT_C CSuplSessionIdContent* CSuplSessionIdContent::NewL()
	{
	CSuplSessionIdContent* self = CSuplSessionIdContent::NewLC();
	CleanupStack::Pop(self);
	return self;								
	}
	
/**
CSuplSessionIdContent NewLC
@internalTechnology
@test
 */		
EXPORT_C CSuplSessionIdContent* CSuplSessionIdContent::NewLC()
	{
	CSuplSessionIdContent* self = new (ELeave) CSuplSessionIdContent();
	CleanupStack::PushL(self);
	return self;									
	}


EXPORT_C CSuplSessionIdContent::~CSuplSessionIdContent()
	{
	delete iSetSessionIdContent;
	delete iSlpSessionIdContent;
	}

void CSuplSessionIdContent::ConstructL()
	{
	}

TBool CSuplSessionIdContent::operator==(const CSuplSessionIdContent& other) const
	{
	return (iSetSessionIdPresent==other.iSetSessionIdPresent && 
			iSlpSessionIdPresent==other.iSlpSessionIdPresent &&
			*iSetSessionIdContent==*other.iSetSessionIdContent && 
			*iSlpSessionIdContent==*other.iSlpSessionIdContent);
	}	
	
	
/**
Implementation of Supl Message SetId content class
*/
/**
CSuplSetIdContent NewL
@internalTechnology
@test
 */	
EXPORT_C CSuplSetIdContent* CSuplSetIdContent::NewL()
	{
	CSuplSetIdContent* self = CSuplSetIdContent::NewLC();
	CleanupStack::Pop(self);
	return self;								
	}
/**
CSuplSetIdContent NewLC
@internalTechnology
@test
 */		
EXPORT_C CSuplSetIdContent* CSuplSetIdContent::NewLC()
	{
	CSuplSetIdContent* self = new (ELeave) CSuplSetIdContent();
	CleanupStack::PushL(self);
	return self;									
	}

CSuplSetIdContent::CSuplSetIdContent()
	{
	}

void CSuplSetIdContent::ConstructL()
	{
	}
		
EXPORT_C CSuplSetIdContent::~CSuplSetIdContent()
	{
	delete iIpAddressContent;
	}
	
TBool CSuplSetIdContent::operator==(const CSuplSetIdContent& other) const
	{
	return (iSetIdType==other.iSetIdType && 
			*iIpAddressContent==*other.iIpAddressContent);
	}	
	
	
/**
Implementation of Supl Message IPAddress content class
*/
/**
CSuplIpAddressContent NewL
@internalTechnology
@test
 */	
EXPORT_C CSuplIpAddressContent* CSuplIpAddressContent::NewL()
	{
	CSuplIpAddressContent* self = CSuplIpAddressContent::NewLC();
	CleanupStack::Pop(self);
	return self;								
	}
/**
CSuplIpAddressContent NewLC
@internalTechnology
@test
 */		
EXPORT_C CSuplIpAddressContent* CSuplIpAddressContent::NewLC()
	{
	CSuplIpAddressContent* self = new (ELeave) CSuplIpAddressContent();
	CleanupStack::PushL(self);
	return self;									
	}

CSuplIpAddressContent::CSuplIpAddressContent()
	{
	}

void CSuplIpAddressContent::ConstructL()
	{
	}
		
EXPORT_C CSuplIpAddressContent::~CSuplIpAddressContent()
	{
	}
	
TBool CSuplIpAddressContent::operator==(const CSuplIpAddressContent& other) const
	{
	return (iIpAddressType==other.iIpAddressType && 
			iIpAddress==other.iIpAddress);
	}	
	
/**
Implementation of Supl Message IPAddress content class
*/
/**
CSuplSlpAddressContent NewL
@internalTechnology
@test
 */	
EXPORT_C CSuplSlpAddressContent* CSuplSlpAddressContent::NewL()
	{
	CSuplSlpAddressContent* self = CSuplSlpAddressContent::NewLC();
	CleanupStack::Pop(self);
	return self;								
	}
/**
CSuplSlpAddressContent NewLC
@internalTechnology
@test
 */		
EXPORT_C CSuplSlpAddressContent* CSuplSlpAddressContent::NewLC()
	{
	CSuplSlpAddressContent* self = new (ELeave) CSuplSlpAddressContent();
	CleanupStack::PushL(self);
	return self;									
	}

CSuplSlpAddressContent::CSuplSlpAddressContent()
	{
	}

void CSuplSlpAddressContent::ConstructL()
	{
	}
		
EXPORT_C CSuplSlpAddressContent::~CSuplSlpAddressContent()
	{
	delete iIpAddressContent;
	}

TBool CSuplSlpAddressContent::operator==(const CSuplSlpAddressContent& other) const
	{
	return (iSlpAddressType == other.iSlpAddressType && 
			*iIpAddressContent==*other.iIpAddressContent);
	}
	

