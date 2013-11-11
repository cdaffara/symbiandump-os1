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

#include <mmf/common/mmfcontrollerframeworkbase.h>
#include "MmfDrmPluginServerProxy.h"
#include "MmfDrmPluginServerStart.h"
#include "MmfDrmPluginClientServer.h"
#include "../../../inc/mmf/common/mmfbase.hrh"    // get the UID

#define KMaxServerNameLength 256

static const TUid KUidDRMPluginServer = {KUidMmfDrmPluginServerDefine};

/*
 * patchable const data values defined in MmfDrmPluginServerConst.cpp
 */
IMPORT_C extern const TInt KMmfDrmPluginServerTimeOut;

EXPORT_C TInt RMMFDRMPluginServerProxy::Open()
	{

	const TUidType serverUid(KNullUid, KNullUid, KUidDRMPluginServer);
		
	// Assume the server is already running and attempt to create a session
	// 4 message slots
	TInt err = CreateSession(KDrmPluginServerName, TVersion(KMMFDRMPluginServerVersion,
														KMMFDRMPluginServerMinorVersionNumber,
														KMMFDRMPluginServerBuildVersionNumber));
	if(err == KErrNotFound)
		{
		// Server not running
		// Construct the server binary name
		TBuf<KMaxServerNameLength> serverFile;
		RProcess server;

		err = server.Create(KDrmPluginServerFileName, KNullDesC, serverUid);
		if(err != KErrNone)
			return err;
		// Synchronise with the server
		TRequestStatus reqStatus;
		server.Rendezvous(reqStatus);
		
		if (reqStatus!=KRequestPending)
			{
			server.Kill(0);
			}
		else
			{
			// Start the test harness
			server.Resume();
			// Server will call the reciprocal static synchronise call
			}
		User::WaitForRequest(reqStatus); // wait for rendezvous or death
		server.Close();
		TInt reqStatusValue = reqStatus.Int();
		if(reqStatusValue == KErrNoMemory || reqStatusValue == KErrNotFound)
			{
			// All error codes except KErrNoMemory and KErrNotFound are assumed
			// to be a duplicate server instance dying, then keep trying connection
			// to the server. This can happen when two servers attempt to start
			// at the same time.
			return reqStatusValue;
			}						
		// Create the root server session
		err = CreateSession(KDrmPluginServerName, TVersion(KMMFDRMPluginServerVersion,
														KMMFDRMPluginServerMinorVersionNumber,
														KMMFDRMPluginServerBuildVersionNumber));
		}
	if (err == KErrNone)
		{
		SendReceive(EMMFSetDrmPluginServerTimeout, KMmfDrmPluginServerTimeOut);
		}
		
	return err;
	}

EXPORT_C TInt RMMFDRMPluginServerProxy::LaunchControllerServer(TUint aMaxHeapSize, TBool aUseSharedHeap,
															TThreadId& aControllerThreadId, TUint aStackSize)
	{
	TPckg<TUint> maxHeapSize(aMaxHeapSize);
	TPckg<TBool> userSharedHeap(aUseSharedHeap);
	TPckgBuf<TThreadId> threadId;
	TPckg<TUint> stackSize(aStackSize);
	TIpcArgs args(&maxHeapSize, &userSharedHeap, &threadId, &stackSize);
	TInt err = RSessionBase::SendReceive(EMMFControllerLaunchRequest, args);
	aControllerThreadId = threadId();
	return err;
	}

EXPORT_C TInt RMMFDRMPluginServerProxy::GetControllerSessionHandle()
	{
	return RSessionBase::SendReceive(EMMFControllerSessionHandle);
	}

EXPORT_C TInt RMMFDRMPluginServerProxy::PanicControllerThread(TThreadId aTid, const TDesC& aCategory,TInt aReason)
	{
	TPckgBuf<TThreadId> threadId(aTid);
	TPckg<TInt> reason(aReason);
	
	TIpcArgs args(&threadId, &aCategory, &reason);
	return RSessionBase::SendReceive(EMMFControllerThreadPanic, args);
	}

EXPORT_C TInt RMMFDRMPluginServerProxy::KillControllerThread(TThreadId aTid, TInt aReason)
	{
	TPckgBuf<TThreadId> threadId(aTid);
	TPckg<TInt> reason(aReason);
	
	TIpcArgs args(&threadId, &reason);
	return RSessionBase::SendReceive(EMMFControllerThreadKill, args);
	}

EXPORT_C TInt RMMFDRMPluginServerProxy::SetThreadPriority(TThreadId aTid, TThreadPriority aPriority)
	{
	TPckgBuf<TThreadId> threadId(aTid);
	TPckgBuf<TThreadPriority> priority(aPriority);
	
	TIpcArgs args(&threadId, &priority);
	return RSessionBase::SendReceive(EMMFControllerSetThreadPriority, args);
	}

EXPORT_C void RMMFDRMPluginServerProxy::OpenDataContentL(const TDesC& aFilePath, const TDesC8& aInitData)
	{
	TIpcArgs args(&aFilePath, &aInitData);
	User::LeaveIfError(RSessionBase::SendReceive(EMMFDRMContentOpenByFilePath, args));
	}

EXPORT_C void RMMFDRMPluginServerProxy::OpenDataContentL(const RFile& aFile, const TDesC8& aInitData)
	{
	TIpcArgs args(NULL, NULL, &aInitData);
	User::LeaveIfError(aFile.TransferToServer(args, 0, 1));
	User::LeaveIfError(RSessionBase::SendReceive(EMMFDRMContentOpenByFileHandle, args));
	}

EXPORT_C TInt RMMFDRMPluginServerProxy::EvaluateDataContentIntent(TIntent aIntent)
	{
	TPckgBuf<TIntent> intentPckg(aIntent);
	return RSessionBase::SendReceive(EMMFDRMContentEvaluateIntent, TIpcArgs(&intentPckg));
	}

EXPORT_C TBool RMMFDRMPluginServerProxy::GetDataContentMimeTypeL(TDes8& aMimeType)
	{
	TPckg<TBool> success(EFalse);
	TIpcArgs args(&aMimeType, &success);
	User::LeaveIfError(RSessionBase::SendReceive(EMMFDRMContentGetMimeType, args));
	return success();
	}

EXPORT_C void RMMFDRMPluginServerProxy::GetDataContentFileHeaderL(TDes8& aHeaderData, TInt aMaxLength)
	{
	TIpcArgs args(aMaxLength, &aHeaderData);
	User::LeaveIfError(RSessionBase::SendReceive(EMMFDRMContentGetFileHeader, args));
	}

