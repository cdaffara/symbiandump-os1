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
// Declares the CRecordMessage class
// 
//

/**
 @file
 @internalAll
 @released
*/
#ifndef CRECORDMESSAGE_H
#define CRECORDMESSAGE_H

#include "et_record.h"

#ifdef ETEL_RECORDER 

#include <e32base.h>

namespace RecorderByteAlignment
	{
_LIT8(KLogVersionDesStart, "Event Recorder v1.0 - Starting Recording");
_LIT8(KLogVersionDesEnd, "Event Recorder - Stop Recording");
const TUint16 KEndianChecksum = 0xAB12;
const TUint16 KReverseEndianChecksum = 0x12AB;
const TUint8 KOneByteVal = 0xA1;
const TUint16 KTwoByteVal = 0x14BC;
const TUint16 KLogHeaderFlagDelimiter = 0xFFFF;

	class TOneByteAlignmentCheck
		{
	public:
		TUint8 iOneByte;
		};
		
	class TThreeByteAlignmentCheck
		{
	public:
		TUint16 iTwoBytes;
		TUint8 iOneByte;
		};
		
	class TFiveByteAlignmentCheck
		{
	public:
		TUint16 iTwoBytes;
		TUint16 iTwoBytes2;
		TUint8 iOneByte;
		};	
	}

class CTelObject;
class CRecordMessageImpl;
NONSHARABLE_CLASS(CRecordMessage) : public CBase
	{
public:
	enum TMessageType
		{
		EConnectSession,
		ECloseSession,
		EOpenSubSession,
		ECloseSubSession,
		ESessionIpc,
		ESubSessionIpc,
		EMessageComplete
		};
		
	typedef TUint32 TSession;	
	typedef TUint32 TSubSession;
	typedef TUint32 TSubSessionHandle;
	typedef TUint32 TIpc ;
	
	enum TArgFlags
		{
		KArgValid = 0x01,
		KArgTypeIsDes8 = 0x02,
		KArgTypeIsDes16 = 0x04,
		KArgTypeIsNull = 0x08,
		KArgTypeIsInt = 0x10
		};
	
	class TSessionInfo	
		{
	public:
		inline TSessionInfo();
	public:
		TSession iSession; //this will be the mem location of the session object
		TSubSession iSubSession; //this will be the mem location of the subsession object
		TSubSessionHandle iSubSessionHandle; //this will be a handle to the subsession client handle.  i.e. whats in aMessage.Int3()
		//NOTE: It will be combination of iSubSession && iSubSessionHandle that allow us to distinguish between
		//to client side sessions that refer to the same server side instance.
		};
	
	class TArgInfo
		{
	public:
		TUint16 iArgFlags; // a bit set of TArgFlags
		TInt16 iArgLength; //the length of the data for this arg
		};
	
	class TMessageHeader
		{
	public:
		TMessageType iMessageType;
		TArgInfo iArg0Info; 
		TArgInfo iArg1Info; 
		TArgInfo iArg2Info; 
		TArgInfo iArg3Info; 
		TIpc	iIpc;
		TReal	iTimeStamp; //in 1/1000 (milli) seconds
		};
		
	//this class is used to specify the length of buffert to alloc for the argument.  It is useful for
	//out arguments where the client has passed a buffer that may be of any size which data is written to.
	class TArgAllocBufFromLen
		{
	public:
		TArgAllocBufFromLen(TInt aLen, TArgFlags aBufType);
		inline TInt LenToAlloc() const;
		inline TArgFlags BufType() const;
		
	private:
		TInt iLenToAlloc;
		TArgFlags iBufType;		
		};

public:		
	static CRecordMessage* NewLC();
	
	~CRecordMessage();
	
	static void LogRecordHeader();	
	static void LogNewSession(const CSession2* aSession);	
	static void LogCloseSession(const CSession2* aSession);

	static void LogNewSubSession(const CSession2* aSession, TInt aIpc, const CTelObject* aSubSession, TInt aSubSessionHandle, const TDesC& aArg0Data, TArgAllocBufFromLen aArg1Data, TInt aArg2Data);	
	static void LogCloseSubSession(const CSession2* aSession, TInt aIpc, const CTelObject* aSubSession, TInt aSubSessionHandle);
	static void LogMessageComplete(const CSession2* aSession, TInt aIpc, TInt aStatus);
	static void LogMessageComplete(const CSession2* aSession, const CTelObject* aSubSession, TInt aSubSessionHandle, TInt aIpc, TInt aStatus);

	//session logs
	//0 arg
	static void LogIpc(const CSession2* aSession, TUint32 aIpc);
	//1 arg
	static void LogIpc(const CSession2* aSession, TUint32 aIpc, const TDesC& aArg0Data);
	static void LogIpc(const CSession2* aSession, TUint32 aIpc, TInt aArg0Data);
	static void LogIpc(const CSession2* aSession, TUint32 aIpc, const TDesC8& aArg0Data);
	//2 arg
	static void LogIpc(const CSession2* aSession, TUint32 aIpc, const TDesC8& aArg0Data, TInt aArg1Data);
	static void LogIpc(const CSession2* aSession, TUint32 aIpc, TInt aArg0Data, TArgAllocBufFromLen aArg1Data);
	static void LogIpc(const CSession2* aSession, TUint32 aIpc, const TDes& aArg0Data, const TDes8& aArg1Data);
	//3arg
	static void LogIpc(const CSession2* aSession, TUint32 aIpc, const TDesC& aArg0Data, const TDesC8& aArg1Data, const TDesC8& aArg2Data);

	//subsession logs
	//1 arg
	static void LogIpc(const CSession2* aSession, const CTelObject* aSubSession, TInt aSubSessionHandle, TUint32 aIpc, TInt aArg1Data);
	//2 args
	static void LogIpc(const CSession2* aSession, const CTelObject* aSubSession, TInt aSubSessionHandle, TUint32 aIpc, TInt aArg1Data, const TDesC8& aArg0Data);
	static void LogIpc(const CSession2* aSession, const CTelObject* aSubSession, TInt aSubSessionHandle, TUint32 aIpc, TInt aArg1Data, const TDesC16& aArg0Data);
	static void LogIpc(const CSession2* aSession, const CTelObject* aSubSession, TInt aSubSessionHandle, TUint32 aIpc, TInt aArg1Data, TInt aArg0Data);
	//3 args
	static void LogIpc(const CSession2* aSession, const CTelObject* aSubSession, TInt aSubSessionHandle, TUint32 aIpc, TInt aArg1Data, const TDesC8& aArg0Data, const TDesC8& aArg2Data);
	static void LogIpc(const CSession2* aSession, const CTelObject* aSubSession, TInt aSubSessionHandle, TUint32 aIpc, TInt aArg1Data, const TDesC8& aArg0Data, const TDesC16& aArg2Data);
	static void LogIpc(const CSession2* aSession, const CTelObject* aSubSession, TInt aSubSessionHandle, TUint32 aIpc, TInt aArg1Data, const TDesC16& aArg0Data, const TDesC16& aArg2Data);

	TInt InternalizeL(const TDesC8& aExternedMessage,  TInt& aMessageSize);

	TMessageHeader* GetMessageHeader();
	TSessionInfo* GetSessionInfo();

	void GetArg(TInt aArgIdx, TPtrC8& aData);	
	
	inline void SetAsFirstMessage();
	inline TBool IsFirstMessage();

protected:
	CRecordMessage();
	void ConstructL();
		
private:
	CRecordMessageImpl* iMessage;
	TBool iIsFirstMessage;
	};

//inlines
inline void CRecordMessage::SetAsFirstMessage()
	{
	iIsFirstMessage = ETrue;
	}

inline TBool CRecordMessage::IsFirstMessage()
	{
	return iIsFirstMessage;
	}

inline CRecordMessage::TSessionInfo::TSessionInfo()
: iSession(0), iSubSession(0), iSubSessionHandle(0)
	{
	
	}
inline TInt CRecordMessage::TArgAllocBufFromLen::LenToAlloc() const
	{
	return iLenToAlloc;
	}

inline CRecordMessage::TArgFlags CRecordMessage::TArgAllocBufFromLen::BufType()	 const
	{
	return iBufType;
	}

#endif // ETEL_RECORDER
#endif //CRECORDMESSAGE_H
