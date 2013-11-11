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

#include <mmf/common/mmfcontrollerframework.h>
#include "MmfDrmPluginServerSession.h"
#include "MmfDrmPluginClientServer.h"
#include "MmfDrmPluginServer.h"

/*
 Create a DRM Plugin Server Session
 */
CMMFDRMPluginServerSession* CMMFDRMPluginServerSession::NewL()
	{
	CMMFDRMPluginServerSession* self = new(ELeave) CMMFDRMPluginServerSession();
	return self;
	}

CMMFDRMPluginServerSession::~CMMFDRMPluginServerSession()
	{
	CMMFDRMPluginServer* server = 
		const_cast<CMMFDRMPluginServer*>(static_cast<const CMMFDRMPluginServer*>(Server()));
	if (server)
		{
		server->DecrementSessionId();		
		}
	if (iData)
		{
		delete iData;
		}
	iControllerSessionHandle.Close();
	}

void CMMFDRMPluginServerSession::CreateL(const CMmfIpcServer& aServer)
	{
	CMmfIpcSession::CreateL(aServer);
	CMMFDRMPluginServer* server = 
			static_cast<CMMFDRMPluginServer*>(CONST_CAST(CMmfIpcServer*, &aServer));
	server->IncrementSessionId();
	}

void CMMFDRMPluginServerSession::ServiceL(const RMmfIpcMessage& aMessage)
	{
	switch (aMessage.Function())
		{
	case EMMFControllerLaunchRequest:
		DoLaunchControllerL(aMessage);
		aMessage.Complete(KErrNone);
		break;
	case EMMFControllerSessionHandle:
		if (!iControllerSessionHandle.Handle())
			{
			User::Leave(KErrNotReady);
			}
		aMessage.Complete(iControllerSessionHandle);
		break;
	case EMMFControllerThreadPanic:
		DoPanicControllerThreadL(aMessage);
		aMessage.Complete(KErrNone);
		break;
	case EMMFControllerThreadKill:
		DoKillControllerThreadL(aMessage);
		aMessage.Complete(KErrNone);
		break;
	case EMMFControllerSetThreadPriority:
		DoSetThreadPriorityL(aMessage);
		aMessage.Complete(KErrNone);
		break;
	case EMMFDRMContentOpenByFilePath:
		DoOpenContentByFilePathL(aMessage);
		aMessage.Complete(KErrNone);
		break;
	case EMMFDRMContentOpenByFileHandle:
		DoOpenContentByFileHandleL(aMessage);
		aMessage.Complete(KErrNone);
		break;
	case EMMFDRMContentEvaluateIntent:
		{
		TInt err = DoEvaluateContentIntent(aMessage);
		aMessage.Complete(err);
		}
		break;
	case EMMFDRMContentGetMimeType:
		DoGetContentMimeTypeL(aMessage);
		aMessage.Complete(KErrNone);
		break;
	case EMMFDRMContentGetFileHeader:
		DoGetContentFileHeaderL(aMessage);
		aMessage.Complete(KErrNone);
		break;
	case EMMFSetDrmPluginServerTimeout:
		DoSetDrmPluginServerTimeout(aMessage);
		aMessage.Complete(KErrNone);
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	}

CMMFDRMPluginServerSession::CMMFDRMPluginServerSession()
	{
	}

void CMMFDRMPluginServerSession::DoLaunchControllerL(const RMmfIpcMessage& aMessage)
	{
	if (iControllerSessionHandle.Handle())
		{
		User::Leave(KErrAlreadyExists);
		}
	TThreadId controllerTID;
	RMMFControllerServerProxy controllerSessionHandle;
	CMMFDRMPluginServer* server = 
		const_cast<CMMFDRMPluginServer*>(static_cast<const CMMFDRMPluginServer*>(Server()));
	
	RThread clientThread;
	CleanupClosePushL(clientThread);
	User::LeaveIfError(aMessage.Client(clientThread));
	
	TPckg<TUint> maxHeapSize(0);
	TPckg<TBool> useShareHeap(ETrue);
	TPckg<TBool> stackSize(0);
	
	aMessage.ReadL(0, maxHeapSize);
	aMessage.ReadL(1, useShareHeap);
	aMessage.ReadL(3, stackSize);
	
	User::LeaveIfError(server->StartControllerServer(clientThread, maxHeapSize(), useShareHeap(), 
													controllerSessionHandle, controllerTID, stackSize()));
	SetControllerServerInfo(controllerSessionHandle, controllerTID);
	
	TPckgBuf<TThreadId> threadId(iControllerThreadID);
	aMessage.WriteL(2, threadId);
	CleanupStack::PopAndDestroy(&clientThread);
	}

void CMMFDRMPluginServerSession::DoPanicControllerThreadL(const RMmfIpcMessage& aMessage)
	{
	TPckgBuf<TThreadId> controllerTid(0);
	aMessage.ReadL(0, controllerTid);
	
	TInt desLen = aMessage.GetDesLengthL(1);
	HBufC* category = HBufC::NewLC(desLen);
	TPtr categoryPtr(category->Des());
	aMessage.ReadL(1, categoryPtr);
	
	TPckg<TInt>	panicReason(0);
	aMessage.ReadL(2, panicReason);
	
	CMMFDRMPluginServer* server = 
		const_cast<CMMFDRMPluginServer*>(static_cast<const CMMFDRMPluginServer*>(Server()));
	server->PanicControllerThread(controllerTid(), categoryPtr, panicReason());
	
	CleanupStack::PopAndDestroy(category);
	}
	
void CMMFDRMPluginServerSession::DoKillControllerThreadL(const RMmfIpcMessage& aMessage)
	{
	TPckgBuf<TThreadId> controllerTid(0);
	aMessage.ReadL(0, controllerTid);
	
	TPckg<TInt>	killReason(0);
	aMessage.ReadL(1, killReason);
	
	CMMFDRMPluginServer* server = 
		const_cast<CMMFDRMPluginServer*>(static_cast<const CMMFDRMPluginServer*>(Server()));
	server->KillControllerThread(controllerTid(), killReason());	
	}
	
void CMMFDRMPluginServerSession::SetControllerServerInfo(RHandleBase& aControllerSessionHandle,
													TThreadId& aControllerThreadId)
	{
	iControllerSessionHandle = aControllerSessionHandle;
	iControllerThreadID = aControllerThreadId;
	}

void CMMFDRMPluginServerSession::DoSetThreadPriorityL(const RMmfIpcMessage& aMessage)
	{
	TPckgBuf<TThreadId> threadId;
	TPckgBuf<TThreadPriority> priority;
	
	aMessage.ReadL(0, threadId);
	aMessage.ReadL(1, priority);
	
	CMMFDRMPluginServer* server = 
		const_cast<CMMFDRMPluginServer*>(static_cast<const CMMFDRMPluginServer*>(Server()));
	User::LeaveIfError(server->SetThreadPriority(threadId(), priority()));
	}

void CMMFDRMPluginServerSession::DoOpenContentByFilePathL(const RMmfIpcMessage& aMessage)
	{
	if (iData == NULL)
		{
		TInt length = aMessage.GetDesLengthL(0);
		HBufC* filePath = HBufC::NewLC(length);
		TPtr filePathPtr(filePath->Des());
		aMessage.ReadL(0, filePathPtr);
		
		length = aMessage.GetDesLengthL(1);
		HBufC8* initData = HBufC8::NewLC(length);
		TPtr8 initDataPtr(initData->Des());
		aMessage.ReadL(1, initDataPtr);
		
		HBufC* uniqueId = NULL;
		TBool UIEnabled;
		ContentAccess::TIntent intent;
		GetContentInitDataL(initDataPtr, uniqueId, UIEnabled, intent);
		CleanupStack::PopAndDestroy(initData);
		CleanupStack::PushL(uniqueId);
		
		iData = CData::NewL(TVirtualPathPtr(filePathPtr, *uniqueId), EContentShareReadWrite);
		TInt err = iData->SetProperty(EAgentPropertyAgentUI, UIEnabled);
		if (err != KErrNone && err != KErrCANotSupported)
			{
			// KErrCANotSupported isn't a problem for us so eat the error code.
			User::Leave(err);
			}
		User::LeaveIfError(iData->EvaluateIntent(intent));
		
		CleanupStack::PopAndDestroy(2, filePath);	// uniqueId, filePath
		}
	else
		{
		User::Leave(KErrAlreadyExists);
		}
	}
	
void CMMFDRMPluginServerSession::DoOpenContentByFileHandleL(const RMmfIpcMessage& aMessage)
	{
	if (iData == NULL)
		{
		RFile file;
		User::LeaveIfError(file.AdoptFromClient(aMessage, 0, 1));
		CleanupClosePushL(file);
		
		TInt length = aMessage.GetDesLengthL(2);
		HBufC8* initData = HBufC8::NewLC(length);
		TPtr8 initDataPtr(initData->Des());
		aMessage.ReadL(2, initDataPtr);
		
		HBufC* uniqueId = NULL;
		TBool UIEnabled;
		ContentAccess::TIntent intent;
		GetContentInitDataL(initDataPtr, uniqueId, UIEnabled, intent);
		CleanupStack::PopAndDestroy(initData);
		CleanupStack::PushL(uniqueId);
		
		iData = CData::NewL(file, *uniqueId);
		TInt err = iData->SetProperty(EAgentPropertyAgentUI, UIEnabled);
		if (err != KErrNone && err != KErrCANotSupported)
			{
			// KErrCANotSupported isn't a problem for us so eat the error code.
			User::Leave(err);
			}
		User::LeaveIfError(iData->EvaluateIntent(intent));
		
		CleanupStack::PopAndDestroy(2, &file);	// uniqueId, file
		}
	else
		{
		User::Leave(KErrAlreadyExists);
		}	
	}

void CMMFDRMPluginServerSession::GetContentInitDataL(const TDesC8& aInitData, HBufC*& aUniqueId,
							 						 TBool& aUIEnabled, ContentAccess::TIntent& aIntent)
	{
	if (aUniqueId)
		{
		delete aUniqueId;
		aUniqueId = NULL;
		}
	
	RDesReadStream stream(aInitData);
	CleanupClosePushL(stream);
	
	TInt length = stream.ReadInt32L();
	aUniqueId = HBufC::NewLC(length);
	TPtr ptr = aUniqueId->Des();
	stream.ReadL(ptr, length);
	
	aUIEnabled = stream.ReadInt32L();
	TPckgBuf<ContentAccess::TIntent> intentPckg;
	stream.ReadL(intentPckg);
	aIntent = intentPckg();
	
	CleanupStack::Pop(aUniqueId);
	CleanupStack::PopAndDestroy(&stream);
	}

TInt CMMFDRMPluginServerSession::DoEvaluateContentIntent(const RMmfIpcMessage& aMessage)
	{
	TInt err;
	if (iData)
		{
		TPckgBuf<TIntent> intentPckg;
		err = aMessage.Read(0, intentPckg);
		if (err == KErrNone)
			{
			err = iData->EvaluateIntent(intentPckg());
			}
		}
	else
		{
		err = KErrGeneral;
		}
	return err;
	}

void CMMFDRMPluginServerSession::DoGetContentMimeTypeL(const RMmfIpcMessage& aMessage)
	{
	if (iData)
		{
		TBool success = ETrue;
		TInt length = aMessage.GetDesMaxLengthL(0);
		
		HBufC* mimeType = HBufC::NewLC(length);
		TPtr mimeTypePtr = mimeType->Des();
		TInt err = iData->GetStringAttribute(EMimeType, mimeTypePtr);
		
		if (err == KErrNone)
			{
			HBufC8* mimeType8 = HBufC8::NewLC(length);
			TPtr8 mimeTypePtr8 = mimeType8->Des();
			mimeTypePtr8.Copy(mimeTypePtr);
			
			aMessage.WriteL(0, mimeTypePtr8);
			CleanupStack::PopAndDestroy(mimeType8);
			}
		else
			{
			success = EFalse;
			}
		CleanupStack::PopAndDestroy(mimeType);
		
		TPckg<TBool> successPckg(success);
		aMessage.WriteL(1, successPckg);
		}
	else
		{
		User::Leave(KErrGeneral);
		}
	}

void CMMFDRMPluginServerSession::DoGetContentFileHeaderL(const RMmfIpcMessage& aMessage)
	{
	if (iData)
		{
		TInt size = 0;
		iData->DataSizeL(size);
		
		TInt maxLength = aMessage.Int0();
		
		TInt desLength = aMessage.GetDesMaxLengthL(1);
		HBufC8* header = HBufC8::NewLC(desLength);
		TPtr8 headerPtr = header->Des();
		
		if (size > 0)
			{
			if (size > maxLength)
				{
				size = maxLength;
				}
			TInt pos = 0;
			User::LeaveIfError(iData->Seek(ESeekStart, pos));
			User::LeaveIfError(iData->Read(headerPtr, size));
			aMessage.WriteL(1, headerPtr);
			}
		CleanupStack::PopAndDestroy(header);
		}
	else
		{
		User::Leave(KErrGeneral);
		}
	}

void CMMFDRMPluginServerSession::DoSetDrmPluginServerTimeout(const RMmfIpcMessage& aMessage)
	{
	CMMFDRMPluginServer* server = 
		const_cast<CMMFDRMPluginServer*>(static_cast<const CMMFDRMPluginServer*>(Server()));
	server->SetTimeout(aMessage.Int0());	
	}

