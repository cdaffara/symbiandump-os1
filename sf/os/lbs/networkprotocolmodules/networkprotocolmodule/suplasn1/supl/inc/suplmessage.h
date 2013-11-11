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
// SUPL message - Client interface for SUPL message encoding & decoding.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __SUPLMESSAGE_H__
#define __SUPLMESSAGE_H__

#include <e32base.h>
class CAsn1PerEncoding;
class CSuplIpAddressContent;
class CSuplIpAddress;

class CSuplStartContent;
class CSuplResponseContent;
class CSuplPosInitContent;
class CSuplPosContent;
class CSuplInitContent;
class CSuplEndContent;

class CSuplStart;
class CSuplInit;
class CSuplPosInit;
class CSuplPos;
class CSuplEnd;
class CSuplResponse;


enum TSuplEncodeError
	{
	TSuplEncodeErrNone = 0	
	};
	
enum TSuplDecodeError
	{
	TSuplDncodeErrNone = 0					
	};


enum TSuplSlpAddressType
	{
	ESuplSlpIpAddressChoice = 0,
	ESuplSlpFqdnAddressChoice,
	
	ESuplSlpAddressChoiceLimit
	};


enum TSuplIpAddressType
	{
	ESuplIpV4AddressChoice = 0,
	ESuplIpV6AddressChoice,
	
	ESuplIpAddressChoiceLimit
	};

enum TSuplPosMethod
	{
	EAGPSSETassisted = 0,	
	EAGPSSETbased, 
	EAGPSSETassistedpref,
	EAGPSSETbasedpref, 
	EAutonomousGPS, 
	EAFLT, 
	EECID, 
	EEOTD, 
	EOTDOA,
	ENoPosition, 

	EPosMethodLimit
	};

enum TSuplMessageType
	{	
  	ESuplInit = 0,
  	ESuplStart,
  	ESuplResponse,
  	ESuplPosInit,
  	ESuplPos,
  	ESuplEnd,
  	ESuplAuthReq,
  	ESuplAuthResp,
  	
  	ESuplMessageLimit	
	};

enum TSuplSetIdType
	{	
  	ESuplMsisdn,
  	ESuplMdn,
  	ESuplMin,
  	ESuplImsi,
  	ESuplNai,
  	ESuplIPAddress		
	};


/**
 * The class holds data member of the Supl Message Slp address content.
 */
class CSuplSlpAddressContent : public CBase
	{
public:
	IMPORT_C static CSuplSlpAddressContent* NewL();
	IMPORT_C static CSuplSlpAddressContent* NewLC();
	IMPORT_C ~CSuplSlpAddressContent();
	TBool operator==(const CSuplSlpAddressContent& other) const;
	
private:
	CSuplSlpAddressContent();
	void ConstructL();

public:
	TSuplSlpAddressType iSlpAddressType;
	CSuplIpAddressContent* iIpAddressContent;	
	};

/**
 * The class to encode and decode the Supl Message SlpAddress.
 */	
class CSuplSlpAddress : public CBase
	{
public:
	static CSuplSlpAddress* NewL(CSuplSlpAddressContent* aSlpAddressContent);	
	static CSuplSlpAddress* NewLC(CSuplSlpAddressContent* aSlpAddressContent);
	~CSuplSlpAddress();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	
private:
	CSuplSlpAddress(CSuplSlpAddressContent* aSlpAddressContent);
	void ConstructL();
	
	CSuplSlpAddressContent* iSlpSessionIdContent;
	CSuplIpAddress* iIpAddress;
	};

	
/**
 * The class holds data member of the Supl Message SessionId.
 */
class CSuplSlpSessionIdContent : public CBase
	{
public:
	IMPORT_C static CSuplSlpSessionIdContent* NewL();
	IMPORT_C static CSuplSlpSessionIdContent* NewLC();
	IMPORT_C ~CSuplSlpSessionIdContent();
	TBool operator==(const CSuplSlpSessionIdContent& other) const;
	
private:	
	void ConstructL();
	void ConstructL(const CSuplSlpSessionIdContent* aSlpSessionIdContent);
	
public:
	TBuf8<4> iSessionId;
	CSuplSlpAddressContent* iSlpAddressContent;
	};

/**
 * The class to encode and decode the Supl Message SlpSessionId.
 */	
class CSuplSlpSessionId : public CBase
	{
public:
	static CSuplSlpSessionId* NewL(CSuplSlpSessionIdContent* aSlpSessionIdContent);	
	static CSuplSlpSessionId* NewLC(CSuplSlpSessionIdContent* aSlpSessionIdContent);
	~CSuplSlpSessionId();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	
private:
	CSuplSlpSessionId(CSuplSlpSessionIdContent* aSlpSessionIdContent);
	void ConstructL();
	CSuplSlpSessionIdContent* iSlpSessionIdContent;
	CSuplSlpAddress* iSlpAddress;
	};

/**
 * The class holds data member of the Supl Message IPAddress Content.
 */
class CSuplIpAddressContent: public CBase
	{
public:
	IMPORT_C static CSuplIpAddressContent* NewL();
	IMPORT_C static CSuplIpAddressContent* NewLC();
	IMPORT_C ~CSuplIpAddressContent();
	TBool operator==(const CSuplIpAddressContent& other) const;
	
private:
	CSuplIpAddressContent();
	void ConstructL();
		
public:	
	TSuplIpAddressType iIpAddressType;
	TBuf8<16> iIpAddress;	
	};
	
/**
 * The class to encode and decode the Supl Message IPAddress.
 */
class CSuplIpAddress: public CBase
	{
public:
	static CSuplIpAddress* NewL(CSuplIpAddressContent* aIpAddressContent);
	static CSuplIpAddress* NewLC(CSuplIpAddressContent* aIpAddressContent);
	~CSuplIpAddress();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
private:
	CSuplIpAddress(CSuplIpAddressContent* aIpAddressContent);
	void ConstructL();
	CSuplIpAddressContent* iIpAddressContent;
		
	};
	
/**
 * The class holds data member of the Supl Message SetId Content.
 */
class CSuplSetIdContent : public CBase
	{
public:
	IMPORT_C static CSuplSetIdContent* NewL();
	IMPORT_C static CSuplSetIdContent* NewLC();
	IMPORT_C ~CSuplSetIdContent();
	TBool operator==(const CSuplSetIdContent& other) const;
	
private:	
	CSuplSetIdContent();
	void ConstructL();
	
public:	
	TSuplSetIdType iSetIdType;
	CSuplIpAddressContent* iIpAddressContent;
	};

/**
 * The class to encode and decode the Supl Message SetSId.
 */

class CSuplSetId : public CBase
	{
public:
	static CSuplSetId* NewL(CSuplSetIdContent* aSetIdContent);
	static CSuplSetId* NewLC(CSuplSetIdContent* aSetContent);
	~CSuplSetId();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);

private:
	CSuplSetId(CSuplSetIdContent* aSetIdContent);
	void ConstructL();
	CSuplSetIdContent* iSetIdContent;
	CSuplIpAddress* iIpAddress;	
	};

/**
 * The class holds data member of the Supl Message SessionId.
 */	
class CSuplSetSessionIdContent : public CBase
	{
public:
	IMPORT_C static CSuplSetSessionIdContent* NewL();
	IMPORT_C static CSuplSetSessionIdContent* NewLC();
	IMPORT_C ~CSuplSetSessionIdContent();
	TBool operator==(const CSuplSetSessionIdContent& other) const;
	
private:	
	void ConstructL();
	
public:
	TUint16 iSessionId;
	CSuplSetIdContent* iSetIdContent;
	};

/**
 * The class to encode and decode the Supl Message SetSessionId.
 */
class CSuplSetSessionId : public CBase
	{
public:	
	static CSuplSetSessionId* NewL(CSuplSetSessionIdContent* aSetSessionIdContent);
	static CSuplSetSessionId* NewLC(CSuplSetSessionIdContent* aSetSessionIdContent);
	~CSuplSetSessionId();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	
private:
	CSuplSetSessionId(CSuplSetSessionIdContent* aSetSessionIdContent);
	void ConstructL();
	CSuplSetSessionIdContent* iSetSessionIdContent;
	CSuplSetId* iSetId;
	};

/**
 * The class holds data member of the Supl Message SessionId.
 */
class CSuplSessionIdContent : public CBase
	{
public:
	IMPORT_C static CSuplSessionIdContent* NewL();	
	IMPORT_C static CSuplSessionIdContent* NewLC();
	IMPORT_C ~CSuplSessionIdContent();
	TBool operator==(const CSuplSessionIdContent& other) const;	
	
private:
	void ConstructL();

public:
	TBool iSetSessionIdPresent;
	TBool iSlpSessionIdPresent;
	CSuplSetSessionIdContent* iSetSessionIdContent;
	CSuplSlpSessionIdContent* iSlpSessionIdContent;
	};
	
/**
 * The class to encode and decode the Supl Message SessionId.
 */
class CSuplSessionId : public CBase
	{
public:
	static CSuplSessionId* NewL(CSuplSessionIdContent* aSessionIdContent);
	static CSuplSessionId* NewLC(CSuplSessionIdContent* aSessionIdContent);
	~CSuplSessionId();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
						
private:
	CSuplSessionId(CSuplSessionIdContent* aSessionIdContent);
	CSuplSessionId();
	void ConstructL();
	CSuplSetSessionId* iSetSessionId;
	CSuplSlpSessionId* iSlpSessionId;
	CSuplSessionIdContent* iSessionIdContent;
	};

/**
 * The class holds data member of the Supl Message Version.
 */
 
class CSuplVersionContent : public CBase
	{
public:
	IMPORT_C static CSuplVersionContent* NewL();
	IMPORT_C static CSuplVersionContent* NewLC();
	IMPORT_C ~CSuplVersionContent();
	static CSuplVersionContent* NewL(const CSuplVersionContent*);
	static CSuplVersionContent* NewLC(const CSuplVersionContent*);
	TBool operator==(const CSuplVersionContent& other) const;
	
private:
	CSuplVersionContent();
	CSuplVersionContent(const CSuplVersionContent*);
public:
	TInt iMaj;
	TInt iMin;
	TInt iServind;
	}; 

/**
 * The class to encode and decode the Supl Message Version.
 */
class CSuplVersion : public CBase
	{
public:
	static CSuplVersion* NewL(CSuplVersionContent* aSuplVersionContent);	
	static CSuplVersion* NewLC(CSuplVersionContent* aSuplVersionContent);
	~CSuplVersion();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	
private:
	CSuplVersion(CSuplVersionContent* aSuplVersionContent);
	CSuplVersionContent* iSuplVersionContent;			
	};

/**
 * The class holds data member of the common part of the Supl Messages,
 * The specific Supl messages data content class are derived from this class
 */
class CSuplMessageContent : public CBase
	{
public:
	IMPORT_C static CSuplMessageContent* NewL();
	IMPORT_C static CSuplMessageContent* NewLC();
	IMPORT_C ~CSuplMessageContent();
	IMPORT_C TBool operator==(const CSuplMessageContent& other) const;
/*	
protected:
	void ConstructL();
	void ConstructL(const CSuplMessageContent* aMessageContent);
*/	
public:

	//Supl Message Common Part Content
	TInt iLength;
	CSuplVersionContent* iSuplVersionContent;
	CSuplSessionIdContent* iSuplSessionIdContent;
	TInt iMessageChoice;
	
	//Supl Message Specific Part Content
	CSuplStartContent* iSuplStartContent;
	CSuplResponseContent* iSuplResponseContent;
	CSuplPosInitContent* iSuplPosInitContent;
	CSuplPosContent* iSuplPosContent;
	CSuplInitContent* iSuplInitContent;
	CSuplEndContent* iSuplEndContent;
	
	//TODO: delete
	TInt iCommonPartLength;
	};


/**
 * The class defines the common part of the Supl Messages,
 * The specific Supl messages are derived from this class
 */
class CSuplMessage : public CBase
	{
public:
	IMPORT_C static CSuplMessage* NewL(const CSuplMessageContent& aSuplMsgContent);
	IMPORT_C static CSuplMessage* NewLC(const CSuplMessageContent& aSuplMsgContent);
	IMPORT_C static CSuplMessage* NewL();
	IMPORT_C static CSuplMessage* NewLC();
	IMPORT_C virtual ~CSuplMessage();
	IMPORT_C virtual void EncodeL(RBuf8& aBuf);
	IMPORT_C virtual CSuplMessageContent* DecodeL(TDesC8& aDataTobeDecoded);
	
private:
	void EncodeCommonPartL();
	CSuplMessage(const CSuplMessageContent& aMessageContent);
	CSuplMessage();
	void ConstructL();
	
	//Buffer to hold the encoded the data, owned by this class
	CAsn1PerEncoding* iEncoding; 
	
	//Common part encoder and decoder
	CSuplVersion* iVersion;
	CSuplSessionId* iSessionId;
	
	//Specific Message encoder and decoder
	CSuplStart* iSuplStart;
	CSuplInit* iSuplInit;
	CSuplPosInit* iSuplPosInit;
	CSuplPos* iSuplPos;
	CSuplEnd* iSuplEnd;
	CSuplResponse* iSuplResponse;
	
	//Supl Message content
//	CSuplMessageContent* iSuplMessageContent;
	const CSuplMessageContent* iSuplMessageContent;
	};
	
	

#endif // __SUPLMESSAGE_H__
