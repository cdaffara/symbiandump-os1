// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Defines the CRecordMessageImpl class
// 
//

/**
 @file
 @internalAll
 @released
*/
#ifndef CRECORDMESSAGEIMPL_H
#define CRECORDMESSAGEIMPL_H

#include "et_record.h"

#ifdef ETEL_RECORDER

#include <e32base.h>
#include <comms-infras/commsdebugutility.h>
#include "crecordmessage.h"

class CEventRecorder;
class CTelObject;
NONSHARABLE_CLASS(CRecordMessageImpl) : public CBase
	{
protected:
	friend class CRecordMessage;
	static CRecordMessageImpl* NewLC();	
	static CRecordMessageImpl* NewL();	
	~CRecordMessageImpl();
	
	static void DoLogRecordHeaderL();
	
	CRecordMessage::TMessageHeader* DoGetMessageHeader();
	CRecordMessage::TSessionInfo* DoGetSessionInfo();

	static void DoLogNewSessionL(const CSession2* aSession);	

	static void DoLogCloseSessionL(const CSession2* aSession);
	static void DoLogNewSubSessionL(const CSession2* aSession, TInt aIpc, const CTelObject* aSubSession, TInt aSubSessionHandle, const TDesC& aArg0Data, CRecordMessage::TArgAllocBufFromLen aArg1Data, TInt aArg2Data);
	static void DoLogCloseSubSessionL(const CSession2* aSession, TInt aIpc, const CTelObject* aSubSession, TInt aSubSessionHandle);
	static void DoLogMessageCompleteL(const CSession2* aSession, TInt aIpc, TInt aStatus);
	static void DoLogMessageCompleteL(const CSession2* aSession, const CTelObject* aSubSession, TInt aSubSessionHandle, TInt aIpc, TInt aStatus);
	
	void DoGetArg(TInt aArgIdx, TPtrC8& aData);
	TInt DoInternalizeL(const TDesC8& aExternedMessage, TInt& aMessageSize);
	
	//CW does not let us put function body of a templated function into separate translation unit
	//It causes a link error where function is invoked. Instead we need to provide function body
	//in the header for use by invoking translation unit.
	
	//session logs	
	//0 arg
	static void DoLogIpcL( const CSession2* aSession, TUint32 aIpc);

	//1 arg
	template<class T0>
	static void DoLogIpcL( const CSession2* aSession, TUint32 aIpc, const T0& aArg0Data);
	//2 arg
	template<class T0, class T1>
	static void DoLogIpcL(const CSession2* aSession, TUint32 aIpc, const T0& aArg0Data, const T1& aArg1Data);
	//3 arg
	template<class T0, class T1, class T2>
	static void DoLogIpcL(const CSession2* aSession, TUint32 aIpc, const T0& aArg0Data, const T1& aArg1Data, const T2& aArg2Data);
	
	//subsession logs
	//1 arg
	static void DoLogIpcL(const CSession2* aSession, const CTelObject* aSubSession, TInt aSubSessionHandle, TUint32 aIpc, TInt aArg1Data);
	//2 args only
	template<class T0>//only need one arg as templated because arg1 always is a TInt
	static void DoLogIpcL(const CSession2* aSession, const CTelObject* aSubSession, TInt aSubSessionHandle, TUint32 aIpc, TInt aArg1Data, const T0& aArg0Data);
	//3 args only
	template<class T0, class T2>//only need two args as templated because arg1 always is a TInt
	static void DoLogIpcL(const CSession2* aSession, const CTelObject* aSubSession, TInt aSubSessionHandle, TUint32 aIpc, TInt aArg1Data, const T0& aArg0Data, const T2& aArg2Data);
			
private:
	static CRecordMessageImpl* CreateAndPopulateMessageLC(const CSession2* aSession, const CTelObject* aSubSession = NULL, TInt aSubSessionHandle=0);	
	void ConstructL();
	void SetTimeStamp();
	void SetSessionInfo(CRecordMessage::TSessionInfo& aSessionInfo);
	TPtr8 GetTPtr8ArgOffset(TInt aArgIndex);
	
	void LogMessageToFile();
	
	inline TPtrC8	GetTPtrC8Header();
	inline TPtr8	GetTPtr8Header();
	inline TPtrC8	GetTPtrC8Session();
	inline TPtr8	GetTPtr8Session();
	inline TPtrC8	GetTPtrC8Body();
	inline TPtr8	GetTPtr8Body();
	
	void SetMessageHeader(CRecordMessage::TMessageType aType); 
	void SetMessageHeader(CRecordMessage::TMessageType aType, CRecordMessage::TIpc aIpc);
	void SetMessageHeader(TInt aArgIdx, TUint16 aArgFlags, TUint16 aArgLength);
	void GrowBufferL(TInt aSizeToGrowBy);
	
	void AppendArgL(TInt aArgIdx, const TDesC8& aData);	
	void AppendArgL(TInt aArgIdx, const TDesC16& aData);
	void AppendArgL(TInt aArgIdx, const TInt aData);
	void AppendArgL(TInt aArgIdx, const CRecordMessage::TArgAllocBufFromLen& aData);
		
	template<class T0>
	void AllocBufAndAppendArgL(TInt aArgIdx, TInt aLen, T0& aBuf);
	
	
private:
	RBuf8 iMessageExternalised;
	CEventRecorder* iEventRecorder;
	};


inline TPtrC8 CRecordMessageImpl::GetTPtrC8Header()
	{
	TPtrC8 pos(iMessageExternalised.LeftTPtr(sizeof(CRecordMessage::TMessageHeader)));
	return pos;
	}
	
inline TPtr8 CRecordMessageImpl::GetTPtr8Header()
	{
	TPtr8 pos(iMessageExternalised.LeftTPtr(sizeof(CRecordMessage::TMessageHeader)));
	return pos;
	}
	
inline TPtrC8 CRecordMessageImpl::GetTPtrC8Session()
	{
	TPtrC8 pos(iMessageExternalised.MidTPtr(sizeof(CRecordMessage::TMessageHeader)).Ptr(), (TInt)sizeof(CRecordMessage::TSessionInfo));
	return pos;
	}
	
inline TPtr8 CRecordMessageImpl::GetTPtr8Session()
	{
	TPtr8 pos(const_cast<TUint8*>(iMessageExternalised.MidTPtr(sizeof(CRecordMessage::TMessageHeader)).Ptr()), (TInt)sizeof(CRecordMessage::TSessionInfo));
	return pos;
	}
	
inline TPtrC8 CRecordMessageImpl::GetTPtrC8Body()
	{
	TPtrC8 pos(iMessageExternalised.MidTPtr(sizeof(CRecordMessage::TMessageHeader) + sizeof(CRecordMessage::TSessionInfo)));
	return pos;
	}
	
inline TPtr8 CRecordMessageImpl::GetTPtr8Body()
	{
	TPtr8 pos = iMessageExternalised.MidTPtr(sizeof(CRecordMessage::TMessageHeader) + sizeof(CRecordMessage::TSessionInfo));
	return pos;
	}

//out of line defnitions
template<class T0>
void CRecordMessageImpl::DoLogIpcL( const CSession2* aSession, TUint32 aIpc, const T0& aArg0Data)
		{
		CRecordMessageImpl* temp = CreateAndPopulateMessageLC(aSession);	
		temp->SetMessageHeader(CRecordMessage::ESessionIpc, aIpc);
		//set arg data.  
		temp->AppendArgL(0, aArg0Data);
		
		//log out to the file
		temp->LogMessageToFile();
		CleanupStack::Pop(); //temp	
		delete temp;
		}
		
template<class T0, class T1>
void CRecordMessageImpl::DoLogIpcL(const CSession2* aSession, TUint32 aIpc, const T0& aArg0Data, const T1& aArg1Data)
	{
	CRecordMessageImpl* temp = CreateAndPopulateMessageLC(aSession);	
	temp->SetMessageHeader(CRecordMessage::ESessionIpc, aIpc);
	
	//set arg data
	//NOTE: These must be called in arg index order as the AppendArgL will only append
	//and so if they are appended out of sequence it will not be read correctly.
	temp->AppendArgL(0, aArg0Data);
	temp->AppendArgL(1, aArg1Data);
	
	//log out to the file
	temp->LogMessageToFile();
	CleanupStack::Pop(); //temp	
	delete temp;
	}
//3 arg
template<class T0, class T1, class T2>
void CRecordMessageImpl::DoLogIpcL(const CSession2* aSession, TUint32 aIpc, const T0& aArg0Data, const T1& aArg1Data, const T2& aArg2Data)
	{
	CRecordMessageImpl* temp = CreateAndPopulateMessageLC(aSession);	
	temp->SetMessageHeader(CRecordMessage::ESessionIpc, aIpc);
	
	//set arg data
	//NOTE: These must be called in arg index order as the AppendArgL will only append
	//and so if they are appended out of sequence it will not be read correctly.
	temp->AppendArgL(0, aArg0Data);
	temp->AppendArgL(1, aArg1Data);
	temp->AppendArgL(2, aArg2Data);
	
	//log out to the file
	temp->LogMessageToFile();
	CleanupStack::Pop(); //temp	
	delete temp;
	}
		
template<class T0>//only need one arg as templated because arg1 always is a TInt
void CRecordMessageImpl::DoLogIpcL(const CSession2* aSession, const CTelObject* aSubSession, TInt aSubSessionHandle, TUint32 aIpc, TInt aArg1Data, const T0& aArg0Data)
	{
	CRecordMessageImpl* temp = CreateAndPopulateMessageLC(aSession, aSubSession, aSubSessionHandle);	
	temp->SetMessageHeader(CRecordMessage::ESubSessionIpc, aIpc);
	
	//set arg data.  
	temp->AppendArgL(0, aArg0Data);
	temp->AppendArgL(1, aArg1Data);
	
	//log out to the file
	temp->LogMessageToFile();
	CleanupStack::Pop(); //temp	
	delete temp;
	}

//3 args only
template<class T0, class T2>//only need two args as templated because arg1 always is a TInt
void CRecordMessageImpl::DoLogIpcL(const CSession2* aSession, const CTelObject* aSubSession, TInt aSubSessionHandle, TUint32 aIpc, TInt aArg1Data, const T0& aArg0Data, const T2& aArg2Data)
	{
	CRecordMessageImpl* temp = CreateAndPopulateMessageLC(aSession, aSubSession, aSubSessionHandle);	
	temp->SetMessageHeader(CRecordMessage::ESubSessionIpc, aIpc);
	
	//set arg data.  
	temp->AppendArgL(0, aArg0Data);
	temp->AppendArgL(1, aArg1Data);
	temp->AppendArgL(2, aArg2Data);
	
	//log out to the file
	temp->LogMessageToFile();
	CleanupStack::Pop(); //temp	
	delete temp;	
	}

#endif // ETEL_RECORDER

#endif //CRECORDMESSAGEIMPL_H
