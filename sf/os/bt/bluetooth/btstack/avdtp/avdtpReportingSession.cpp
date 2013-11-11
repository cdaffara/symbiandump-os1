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
// Implements the avdtp reporting transport session
// 
//

/**
 @file
 @internalComponent
*/

#include "avdtpReportingSession.h"
#include "avdtp.h"
#include "avdtpStream.h"
#include "avdtpsap.h"

CReportingSession* CReportingSession::NewLC(CAvdtpProtocol& aProtocol,
											CAvdtpSAP& aSAP, CAVStream& aStream)
	{
	CReportingSession* s = new (ELeave) CReportingSession(aProtocol, aSAP, aStream);
	CleanupStack::PushL(s);
	s->ConstructL();
	return s;
	}
	
CReportingSession* CReportingSession::NewL(CAvdtpProtocol& aProtocol,
										   CAvdtpSAP& aSAP, CAVStream& aStream)
	{
	CReportingSession* s = CReportingSession::NewLC(aProtocol, aSAP, aStream);
	CleanupStack::Pop();
	return s;		
	}
	
CReportingSession::CReportingSession(CAvdtpProtocol& aProtocol, CAvdtpSAP& aSAP, CAVStream& aStream)
: CUserPlaneTransportSession(aProtocol, aSAP, EReporting, aStream)
	{
	
	}
	
void CReportingSession::ConstructL()
	{
	CUserPlaneTransportSession::ConstructL();
	
	// Balking is set to true.  This allows the circular buffer to wrap
	// and overwrite the oldest data.
	User::LeaveIfError(iSendPool.Create(4, ETrue));
	User::LeaveIfError(iReceivePool.Create(4, ETrue));
	}

TInt CReportingSession::DoActiveOpen()
	{
	__ASSERT_DEBUG(iStream, Panic(EAvdtpTransportSessionBaseNotCheckStream));

	TInt ret = KErrGeneral;
		
	// add session to the stream
	ret = iStream->AddSession(EReporting,*this,iTransportChannel);
	if (ret!=KErrNone)
		{
		// not erroring the stream, as it's not it's fault
		// and it may not exist anyway!
		iStream = NULL;
		iSAP.Error(ret);
		}
	
	return ret;
	}

TInt CReportingSession::Send(RMBufChain& aData, TUint /*aOptions*/, TSockAddr* /*aAddr*/)
	{
	TInt wrote = iTransportChannel->SendPacket(EReporting, aData);
	if (!wrote)
		{
		iTransportChannelBlocked = ETrue;
		}
	return wrote;
	}
	

void CReportingSession::CanSend()
	{
	// drain some pool? or just tell sap?
	iTransportChannelBlocked = EFalse;
	
	}
