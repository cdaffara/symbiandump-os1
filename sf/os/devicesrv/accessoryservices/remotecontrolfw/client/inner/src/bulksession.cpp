// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file
 @internalComponent
*/

#include <e32base.h>
#include <bluetooth/logger.h>

#include "remconbulkclient.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_INNER);
#endif

EXPORT_C RRemConBulk::RRemConBulk() 
:	iUidPckg(0),
	iOpIdPckg(0)
	{
	LOG_FUNC
	}

EXPORT_C TVersion RRemConBulk::Version() const
	{
	LOG_FUNC
	return(TVersion(	KRemConBulkSrvMajorVersionNumber,
						KRemConBulkSrvMinorVersionNumber,
						KRemConBulkSrvBuildNumber
					)
			);
	}

EXPORT_C TInt RRemConBulk::Connect()
	{
	LOG_FUNC
	const TInt KNumberOfBulkSessionAsyncApis = 2;
	return CreateSession(KRemConBulkServerName, Version(), KNumberOfBulkSessionAsyncApis);
	}

EXPORT_C TInt RRemConBulk::SendUnreliable(TUid aInterfaceUid, 
		TUint aOperationId, 
		const TDesC8& aData)
	{
	LOG_FUNC
	TIpcArgs args;
	
	args.Set(0, aInterfaceUid.iUid);
	args.Set(1, aOperationId);
	args.Set(2, &aData);
	TInt err = SendReceive(ERemConBulkSendUnreliable, args); 
	return err;
	}


EXPORT_C void RRemConBulk::Send(TRequestStatus& aStatus, 
		TUid aInterfaceUid, 
		TUint aOperationId, 
		const TDesC8& aData)
	{
	LOG_FUNC

	TIpcArgs args;
	args.Set(0, aInterfaceUid.iUid);
	args.Set(1, aOperationId);
	args.Set(2, &aData);

	SendReceive(ERemConBulkSend,
		args,
		aStatus
		);
	}
	
EXPORT_C TInt RRemConBulk::SendCancel()
	{
	LOG_FUNC
	
	//Ignore Return code because
	// a) It'll mostly be other than KErrNone because the server has terminated, in which
	//    case the original async request will have completed with the error anyway!
	// b) It's meaningless to the client whatever the return code is.
	(void)SendReceive(ERemConBulkSendCancel);
	
	return KErrNone; 
	}

EXPORT_C void RRemConBulk::Receive(TRequestStatus& aStatus, 
							   TUid& aInterfaceUid,
							   TUint& aOperationId,
							   TDes8& aData)
	{
	LOG_FUNC

	TIpcArgs args;
	iUidPckg.Set((TUint8*)&aInterfaceUid.iUid, sizeof(TUint), sizeof(TUint));
	args.Set(0, &iUidPckg);
	iOpIdPckg.Set((TUint8*)&aOperationId, sizeof(TUint), sizeof(TUint));
	args.Set(1, &iOpIdPckg);
	args.Set(2, &aData);

	SendReceive(ERemConBulkReceive, 
		args,
		aStatus);
	}

EXPORT_C TInt RRemConBulk::ReceiveCancel()
	{
	LOG_FUNC

	// See RRemConBulk::SendCancel() for comment
	(void)SendReceive(ERemConBulkReceiveCancel);
	
	return KErrNone;
	}

EXPORT_C TInt RRemConBulk::__DbgMarkHeap()
	{
	LOG_FUNC
#ifdef _DEBUG
	return SendReceive(ERemConBulkDbgMarkHeap);
#else
	return KErrNone;
#endif
	}

EXPORT_C TInt RRemConBulk::__DbgCheckHeap(TInt aCount)
	{
	LOG_FUNC
#ifdef _DEBUG
	return SendReceive(ERemConBulkDbgCheckHeap, TIpcArgs(aCount));
#else
	(void)aCount;
	return KErrNone;
#endif
	}

EXPORT_C TInt RRemConBulk::__DbgMarkEnd(TInt aCount)
	{
	LOG_FUNC
#ifdef _DEBUG
	return SendReceive(ERemConBulkDbgMarkEnd, TIpcArgs(aCount));
#else
	(void)aCount;
	return KErrNone;
#endif
	}

EXPORT_C TInt RRemConBulk::__DbgFailNext(TInt aCount)
	{
	LOG_FUNC
#ifdef _DEBUG
	return SendReceive(ERemConBulkDbgFailNext, TIpcArgs(aCount));
#else
	(void)aCount;
	return KErrNone;
#endif
	}

