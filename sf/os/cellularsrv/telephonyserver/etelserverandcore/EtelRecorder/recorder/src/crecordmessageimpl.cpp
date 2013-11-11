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
//

#include "et_record.h"

#ifdef ETEL_RECORDER

#include "crecordmessageimpl.h"
#include "ceventrecorder.h"

_LIT8(KSubSystem, "ETel");

CRecordMessageImpl* CRecordMessageImpl::NewLC()
	{
	CRecordMessageImpl* me = new(ELeave)CRecordMessageImpl();
	CleanupStack::PushL(me);
	me->ConstructL();
	return me;
	}
	
CRecordMessageImpl* CRecordMessageImpl::NewL()
	{
	CRecordMessageImpl* me = CRecordMessageImpl::NewLC();
	CleanupStack::Pop();	
	return me;
	}
	
void CRecordMessageImpl::ConstructL()
	{
	//open log
	iEventRecorder = CEventRecorder::NewL(KSubSystem);
	//alloc the RBuf to header size
	TInt len = sizeof(CRecordMessage::TMessageHeader) + sizeof(CRecordMessage::TSessionInfo);
	iMessageExternalised.CreateL(len);
	iMessageExternalised.SetLength(len);
	iMessageExternalised.FillZ();
	}
	
CRecordMessageImpl::~CRecordMessageImpl()
	{
	iMessageExternalised.Close();
	delete iEventRecorder;
	}
	
CRecordMessageImpl* CRecordMessageImpl::CreateAndPopulateMessageLC(const CSession2* aSession, const CTelObject* aSubSession, TInt aSubSessionHandle)
	{
	//create the message
	CRecordMessageImpl* temp = CRecordMessageImpl::NewLC();
	temp->SetTimeStamp();
	//set message session in header
	CRecordMessage::TSessionInfo sessInfo;
	sessInfo.iSession = reinterpret_cast<TUint32>(aSession);
	if(aSubSession)
		{
		sessInfo.iSubSession = reinterpret_cast<TUint32>(aSubSession);
		}
	sessInfo.iSubSessionHandle = aSubSessionHandle;
	temp->SetSessionInfo(sessInfo);
	return temp;
	}
	
void CRecordMessageImpl::SetTimeStamp()
	{
	TTime currenttime;
	currenttime.UniversalTime();
	
	//convert micro secs to 1/1000 sec granularity
	TReal timestampUTC = (TReal64)currenttime.Int64()/1000.0;

	CRecordMessage::TMessageHeader* hdr = DoGetMessageHeader();
	if(hdr)
		{
		hdr->iTimeStamp = timestampUTC;
		}
	}

void CRecordMessageImpl::SetSessionInfo(CRecordMessage::TSessionInfo& aSessionInfo)
	{
	CRecordMessage::TSessionInfo* sess = DoGetSessionInfo();
	if(sess)
		{
		*sess=aSessionInfo;		
		}
	}
	
CRecordMessage::TMessageHeader* CRecordMessageImpl::DoGetMessageHeader()
	{
	CRecordMessage::TMessageHeader* hdr = NULL;
	if(iMessageExternalised.MaxLength() >= sizeof(CRecordMessage::TMessageHeader) && iEventRecorder)
		{
		hdr = reinterpret_cast<CRecordMessage::TMessageHeader*>(const_cast<TUint8*>(GetTPtr8Header().Ptr()));	
		}

	// caller needs to check that returned header is not NULL before attempting to dereference it

	return hdr;	
	}
	
CRecordMessage::TSessionInfo* CRecordMessageImpl::DoGetSessionInfo()
	{
	CRecordMessage::TSessionInfo* sess = NULL;
	if(iMessageExternalised.MaxLength() >= sizeof(CRecordMessage::TMessageHeader) + sizeof(CRecordMessage::TSessionInfo) && iEventRecorder)
		{
		sess = reinterpret_cast<CRecordMessage::TSessionInfo*>(const_cast<TUint8*>(GetTPtr8Session().Ptr()));	
		}

	// caller needs to check that returned header is not NULL before attepting to dereference it
				
	return sess;	
	}
	
TPtr8 CRecordMessageImpl::GetTPtr8ArgOffset(TInt aArgIndex)
	{
	//calculate offset from start of messages
	
	//this gets us to the start of the body
	TInt argOffset = sizeof(CRecordMessage::TMessageHeader) + sizeof(CRecordMessage::TSessionInfo);
	//work out how far along the body we want to go in order to get to the start of the selected arg
	CRecordMessage::TMessageHeader* hdr = DoGetMessageHeader();
	if(hdr)
		{
		if(aArgIndex>1)
			{
			argOffset += hdr->iArg1Info.iArgLength;
			}
		if(aArgIndex > 0)
			{
			argOffset += hdr->iArg0Info.iArgLength;
			}
		}

	TPtr8 pos = iMessageExternalised.MidTPtr(argOffset);
	return pos;
	}

void CRecordMessageImpl::LogMessageToFile()
	{
	iEventRecorder->Dump(iMessageExternalised);
	}
	
void CRecordMessageImpl::SetMessageHeader(TInt aArgIdx, TUint16 aArgFlags, TUint16 aArgLength)
	{
	CRecordMessage::TMessageHeader* hdr = DoGetMessageHeader();
	if(hdr)
		{
		switch(aArgIdx)
			{
		case 0:
			hdr->iArg0Info.iArgFlags = aArgFlags;
			hdr->iArg0Info.iArgLength = aArgLength;
			break;
		case 1:
			hdr->iArg1Info.iArgFlags = aArgFlags;
			hdr->iArg1Info.iArgLength = aArgLength;
			break;
		case 2:
			hdr->iArg2Info.iArgFlags = aArgFlags;
			hdr->iArg2Info.iArgLength = aArgLength;
			break;
		case 3:
			hdr->iArg3Info.iArgFlags = aArgFlags;
			hdr->iArg3Info.iArgLength = aArgLength;
			break;
		default:
			break;
			}				
		}
	}
	
void CRecordMessageImpl::SetMessageHeader(CRecordMessage::TMessageType aType)
	{
	CRecordMessage::TMessageHeader* hdr = DoGetMessageHeader();
	if(hdr)
		{
		hdr->iMessageType = aType;
		}
	}
	
void CRecordMessageImpl::SetMessageHeader(CRecordMessage::TMessageType aType, CRecordMessage::TIpc aIpc)
	{
	CRecordMessage::TMessageHeader* hdr = DoGetMessageHeader();
	if(hdr)
		{
		hdr->iMessageType = aType;
		hdr->iIpc = aIpc;		
		}
	}
	
void CRecordMessageImpl::GrowBufferL(TInt aSizeToGrowBy)
	{
	TInt newLength = iMessageExternalised.Length() + aSizeToGrowBy;
	
	//realloc msg buffer so the body can fit
	iMessageExternalised.ReAllocL(newLength);
	iMessageExternalised.SetLength(iMessageExternalised.MaxLength());
	}

void CRecordMessageImpl::AppendArgL(TInt aArgIdx, TInt aData)
	{
	if((aArgIdx < 0 || aArgIdx > 3) && iEventRecorder)
		{
		return;
		}
			
	GrowBufferL(sizeof(aData));
	
	TPtr8 body = GetTPtr8ArgOffset(aArgIdx);
	body.Copy(reinterpret_cast<TUint8*>(&aData), sizeof(aData));	
	//set the arg info used.
	TUint8 argflags = CRecordMessage::KArgValid | CRecordMessage::KArgTypeIsInt;
	SetMessageHeader(aArgIdx,argflags, sizeof(aData));
	}
	
void CRecordMessageImpl::AppendArgL(TInt aArgIdx, const TDesC8& aData)
	{
	if((aArgIdx < 0 || aArgIdx > 3) && iEventRecorder)
		{
		return;
		}
		
	GrowBufferL(aData.Length());
	
	TPtr8 body = GetTPtr8ArgOffset(aArgIdx);
	body.Copy(aData);	
	//set the arg info used.
	TUint8 argflags = CRecordMessage::KArgValid | CRecordMessage::KArgTypeIsDes8;
	SetMessageHeader(aArgIdx,argflags, aData.Length());
	}
	
void CRecordMessageImpl::AppendArgL(TInt aArgIdx, const TDesC16& aData)
	{
	//we must convert our 2-byte buffer into a 1-byte buffer
	TInt len = aData.Length();
	GrowBufferL(len*2);//*2 since it will take up double space in Rbuf8
	
	TPtr8 body = GetTPtr8ArgOffset(aArgIdx);
	
	for(TInt i=0;i<len;i++)
		{
		body[i*2] =  aData[i] & 0xFF; //take low 8 bit val
		body[i*2+1] =  (aData[i] & 0xFF00) >> 8; //take high 8 bit val
		}
	
	//set the arg info used.
	TUint8 argflags = CRecordMessage::KArgValid | CRecordMessage::KArgTypeIsDes16;
	SetMessageHeader(aArgIdx,argflags, aData.Length()*2); //*2 since was 2-byte buffer
	}
	
void CRecordMessageImpl::AppendArgL(TInt aArgIdx, const CRecordMessage::TArgAllocBufFromLen& aData)
	{
	//we use the length field to specify a buffer that will be written 
	//use the type field to specify whether it is a 16-bit or 8-bit buffer
	if(aData.BufType() == CRecordMessage::KArgTypeIsDes8)
		{
		//alloc a temp buffer that is used as a placeholder for the buffer to be written to the log.
		//the contents do not matter as it is an out parameter.  All that matter is the type
		RBuf8 tempBuf;
		AllocBufAndAppendArgL(aArgIdx, aData.LenToAlloc(), tempBuf);
		}
	else if(aData.BufType() == CRecordMessage::KArgTypeIsDes16)
		{
		//alloc a temp buffer that is used as a placeholder for the buffer to be written to the log.
		//the contents do not matter as it is an out parameter.  All that matter is the type
		RBuf16 tempBuf;
		AllocBufAndAppendArgL(aArgIdx, aData.LenToAlloc(), tempBuf);
		}	
	}

template<class T0>
void CRecordMessageImpl::AllocBufAndAppendArgL(TInt aArgIdx, TInt aLen, T0& aBuf)
	{
	aBuf.CreateL(aLen);
	CleanupClosePushL(aBuf);
	aBuf.SetLength(aBuf.MaxLength());
	aBuf.FillZ();
	AppendArgL(aArgIdx, aBuf);
	CleanupStack::PopAndDestroy(&aBuf);
	}
	
void CRecordMessageImpl::DoLogRecordHeaderL()
	{
	CEventRecorder* logger = CEventRecorder::NewL(KSubSystem);
	logger->DumpLogHeader();
	delete logger;
	}
	
void CRecordMessageImpl::DoLogNewSessionL(const CSession2* aSession)
	{
	//create the message
	CRecordMessageImpl* temp = CreateAndPopulateMessageLC(aSession);
	//set message type in header
	temp->SetMessageHeader(CRecordMessage::EConnectSession);
	//log out to the file
	temp->LogMessageToFile();
	CleanupStack::PopAndDestroy(temp);
	}
	
void CRecordMessageImpl::DoLogCloseSessionL(const CSession2* aSession)
	{
	//create the message
	CRecordMessageImpl* temp = CreateAndPopulateMessageLC(aSession);
	//set message type in header
	temp->SetMessageHeader(CRecordMessage::ECloseSession);
	//log out to the file
	temp->LogMessageToFile();
	CleanupStack::PopAndDestroy(temp);
	}
	
void CRecordMessageImpl::DoLogNewSubSessionL( const CSession2* aSession, TInt aIpc, const CTelObject* aSubSession, TInt aSubSessionHandle, const TDesC& aArg0Data, CRecordMessage::TArgAllocBufFromLen aArg1Data, TInt aArg2Data)
	{
	//create the message
	CRecordMessageImpl* temp = CreateAndPopulateMessageLC(aSession, aSubSession, aSubSessionHandle);
	//set message type in header
	temp->SetMessageHeader(CRecordMessage::EOpenSubSession, aIpc);
	
	//set arg data.  
	temp->AppendArgL(0, aArg0Data);
	if(aArg1Data.LenToAlloc() > 0) //if size < 0 don't attempt to allocate buffer
		temp->AppendArgL(1, aArg1Data);
	temp->AppendArgL(2, aArg2Data);
	
	//log out to the file
	temp->LogMessageToFile();
		
	CleanupStack::PopAndDestroy(temp);
	}
	
void CRecordMessageImpl::DoLogCloseSubSessionL(const CSession2* aSession, TInt aIpc, const CTelObject* aSubSession, TInt aSubSessionHandle)
	{
	//create the message
	CRecordMessageImpl* temp = CreateAndPopulateMessageLC(aSession, aSubSession, aSubSessionHandle);
	//set message type in header
	temp->SetMessageHeader(CRecordMessage::ECloseSubSession, aIpc);
	//log out to the file
	temp->LogMessageToFile();
	CleanupStack::PopAndDestroy(temp);
	}

void CRecordMessageImpl::DoLogMessageCompleteL(const CSession2* aSession, const CTelObject* aSubSession, TInt aSubSessionHandle, TInt aIpc, TInt aStatus)
	{
	//create the message
	CRecordMessageImpl* temp = CreateAndPopulateMessageLC(aSession, aSubSession, aSubSessionHandle);
	//set message type in header
	temp->SetMessageHeader(CRecordMessage::EMessageComplete, aIpc);
	
	//set arg data.  
	temp->AppendArgL(0, aStatus);		//completion code goes into arg0
	
	//log out to the file
	temp->LogMessageToFile();
	CleanupStack::PopAndDestroy(temp);
	}
		
void CRecordMessageImpl::DoLogMessageCompleteL(const CSession2* aSession, TInt aIpc, TInt aStatus)
	{
	//create the message
	CRecordMessageImpl* temp = CreateAndPopulateMessageLC(aSession);
	//set message type in header
	temp->SetMessageHeader(CRecordMessage::EMessageComplete, aIpc);
	
	//set arg data.  
	temp->AppendArgL(0, aStatus);		//completion code goes into arg0
	
	//log out to the file
	temp->LogMessageToFile();
	CleanupStack::PopAndDestroy(temp);
	}

//session logs
//0 arg	
void CRecordMessageImpl::DoLogIpcL(const CSession2* aSession, TUint32 aIpc)
	{
	CRecordMessageImpl* temp = CreateAndPopulateMessageLC(aSession);	
	temp->SetMessageHeader(CRecordMessage::ESessionIpc, aIpc);
	
	//log out to the file
	temp->LogMessageToFile();
	CleanupStack::PopAndDestroy(temp);
	}

	
//subsession logs

//1 arg only
void CRecordMessageImpl::DoLogIpcL(const CSession2* aSession, const CTelObject* aSubSession, TInt aSubSessionHandle, TUint32 aIpc, TInt aArg1Data)
	{
	CRecordMessageImpl* temp = CreateAndPopulateMessageLC(aSession, aSubSession, aSubSessionHandle);	
	temp->SetMessageHeader(CRecordMessage::ESubSessionIpc, aIpc);
	
	//set arg data.  
	temp->AppendArgL(1, aArg1Data);
	
	//log out to the file
	temp->LogMessageToFile();
	CleanupStack::PopAndDestroy(temp);
	}

void CRecordMessageImpl::DoGetArg(TInt aArgIdx, TPtrC8& aData) 
	{
	if((aArgIdx < 0 || aArgIdx > 3))
		{
		return;
		}
	
	CRecordMessage::TMessageHeader* hdr = DoGetMessageHeader();
	
	TUint16 argFlags = 0;
	TUint16 argLength = 0;
	TInt argOffset = 0;
	
	if(hdr)
		{
		for(TInt i =0; i <= aArgIdx; i++)
			{
			switch(i)
				{
			case 0:
				if(aArgIdx == i)
					{
					argFlags = hdr->iArg0Info.iArgFlags;
					argLength = hdr->iArg0Info.iArgLength;
					}
				else
					{
					argOffset += hdr->iArg0Info.iArgLength;
					}
				break;
			case 1:
				if(aArgIdx == i)
					{
					argFlags = hdr->iArg1Info.iArgFlags;
					argLength = hdr->iArg1Info.iArgLength;
					}
				else
					{
					argOffset += hdr->iArg1Info.iArgLength;
					}
				break;
			case 2:
			if(aArgIdx == i)
					{
					argFlags = hdr->iArg2Info.iArgFlags;
					argLength = hdr->iArg2Info.iArgLength;
					}
				else
					{
					argOffset += hdr->iArg2Info.iArgLength;
					}
				break;
			case 3:
			if(aArgIdx == i)
					{
					argFlags = hdr->iArg3Info.iArgFlags;
					argLength = hdr->iArg3Info.iArgLength;
					}
				else
					{
					argOffset += hdr->iArg3Info.iArgLength;
					}
				break;
			default:
				break;
				}
			}
		}
		
			
	if(argFlags & CRecordMessage::KArgValid)
		{
		aData.Set(GetTPtrC8Body());
		//move to start of arg data based on offset
		aData.Set(aData.Mid(argOffset, argLength));
		}
		
	}

TInt CRecordMessageImpl::DoInternalizeL(const TDesC8& aExternedMessage, TInt& aMessageSize)
	{
	TPtrC8 cursor(aExternedMessage);
	
	aMessageSize = 0;
	
	//peek to see if there is another message. Need at least header and session info
	if(cursor.Length() >= sizeof(CRecordMessage::TMessageHeader) + sizeof(CRecordMessage::TSessionInfo))
		{
		//there looks to be a header at least
		
		//check we have not found another new log session.  This will be the case if we come across the KLogHeaderFlagDelimiter
		if(cursor[0] == 0xff && cursor[1] == 0xff)
			{
			//have found another log session
			return KErrEof;
			}
		
		const CRecordMessage::TMessageHeader* header = reinterpret_cast<const CRecordMessage::TMessageHeader*>(cursor.Ptr());	
		CRecordMessage::TMessageHeader* mh = DoGetMessageHeader();
		if ( (mh == NULL) || (header == NULL) )
			{
			return KErrCorrupt;
			}
		*mh = *header; //copy contents of header
		
		//forward on to session info
		cursor.Set(cursor.Mid(sizeof(CRecordMessage::TMessageHeader)));
		aMessageSize += sizeof(CRecordMessage::TMessageHeader);
		
		const CRecordMessage::TSessionInfo* sessInfo = reinterpret_cast<const CRecordMessage::TSessionInfo*>(cursor.Ptr());
		CRecordMessage::TSessionInfo* si = DoGetSessionInfo();
		if ( (si == NULL) || (sessInfo == NULL) )
			{
			return KErrCorrupt;
			}		
		*si = *sessInfo;	//copy contents of session info
		
		//forward on to body section
		cursor.Set(cursor.Mid(sizeof(CRecordMessage::TSessionInfo)));
		aMessageSize += sizeof(CRecordMessage::TSessionInfo);
		
		//iterate through the arg list to see which are present
		TInt szBody = 0;
		if(header->iArg0Info.iArgFlags & CRecordMessage::KArgValid )
			{
			szBody += header->iArg0Info.iArgLength;
			}
			
		if(header->iArg1Info.iArgFlags & CRecordMessage::KArgValid )
			{
			szBody += header->iArg1Info.iArgLength;
			}
			
		if(header->iArg2Info.iArgFlags & CRecordMessage::KArgValid )
			{
			szBody += header->iArg2Info.iArgLength;
			}
		
		if(header->iArg3Info.iArgFlags & CRecordMessage::KArgValid )
			{
			szBody += header->iArg3Info.iArgLength;
			}
			
		//copy the contents of the body to our RBuf 
		if(szBody > 0)
			{
			//realloc msg buffer so the body can fit
			TInt newLength = iMessageExternalised.Length() + szBody;
			iMessageExternalised.ReAllocL(newLength);
			iMessageExternalised.SetLength(iMessageExternalised.MaxLength());
			GetTPtr8Body().Copy(cursor.Ptr(), szBody);			
			aMessageSize += szBody;
			}		
		}
	else
		{
		return KErrNotFound;
		}		
	return KErrNone;		
	}
#endif // ETEL_RECORDER
