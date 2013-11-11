// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the avdtp recovery transport session
// 
//

/**
 @file
 @internalComponent
*/

#include "avdtpRecoverySession.h"
#include "avdtp.h"
#include "avdtpStream.h"
#include "avdtpsap.h"

CRecoverySession* CRecoverySession::NewLC(CAvdtpProtocol& aProtocol,
											CAvdtpSAP& aSAP, CAVStream& aStream)
	{
	CRecoverySession* s = new (ELeave) CRecoverySession(aProtocol, aSAP, aStream);
	CleanupStack::PushL(s);
	s->ConstructL();
	return s;
	}
	
CRecoverySession* CRecoverySession::NewL(CAvdtpProtocol& aProtocol,
										   CAvdtpSAP& aSAP, CAVStream& aStream)
	{
	CRecoverySession* s = CRecoverySession::NewLC(aProtocol, aSAP, aStream);
	CleanupStack::Pop();
	return s;		
	}
	
CRecoverySession::CRecoverySession(CAvdtpProtocol& aProtocol, CAvdtpSAP& aSAP, CAVStream& aStream)
: CUserPlaneTransportSession(aProtocol, aSAP, ERecovery, aStream)
	{
	
	}
	
void CRecoverySession::ConstructL()
	{
	CUserPlaneTransportSession::ConstructL();
	
	// Balking is set to true.  This allows the circular buffer to wrap
	// and overwrite the oldest data.
	User::LeaveIfError(iSendPool.Create(4, ETrue));
	User::LeaveIfError(iReceivePool.Create(4, ETrue));
	}

TInt CRecoverySession::DoActiveOpen()
	{
	__ASSERT_DEBUG(iStream, Panic(EAvdtpTransportSessionBaseNotCheckStream));

	TInt ret = KErrGeneral;
		
	// add session to the stream
	ret = iStream->AddSession(ERecovery,*this,iTransportChannel);
	if (ret!=KErrNone)
		{
		// not erroring the stream, as it's not it's fault
		// and it may not exist anyway!
		iStream = NULL;
		iSAP.Error(ret);
		}
	
	return ret;
	}

TInt CRecoverySession::Send(RMBufChain& aData, TUint /*aOptions*/, TSockAddr* /*aAddr*/)
	{
	return iTransportChannel->SendPacket(ERecovery, aData);
	}
	

void CRecoverySession::CanSend()
	{
	// drain some pool? or just tell sap?
	
	}

