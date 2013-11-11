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
//

#include "RPSMaster.h"
#include "RPSMasterImpl.h"
#include "rpsloggermacros.h"

EXPORT_C CRPSMaster* CRPSMaster::NewL()
	{
	CRPSMaster* master = CRPSMaster::NewLC();
	CleanupStack::Pop();
	return master;
	}

EXPORT_C CRPSMaster* CRPSMaster::NewLC()
	{
	CRPSMaster* master = new (ELeave)CRPSMaster();
	CleanupStack::PushL(master);
	master->ConstructL();
	return master;
	}
	
EXPORT_C CRPSMaster::~CRPSMaster()
	{
	delete iImpl;
	}
	
CRPSMaster::CRPSMaster()
	{	
	}

void CRPSMaster::ConstructL()
	{
	iImpl = CRPSMasterImpl::NewL();
	}

EXPORT_C void CRPSMaster::CallNumberL(const TDesC& aNumToDial, TEtelLine aLine)
	{
	RDEBUGPRINTLOGGER2(_L("RPS: CRPSMaster::CallNumberL %S"), &aNumToDial);
	iImpl->doCallNumberL(aNumToDial, aLine);
	}

EXPORT_C void CRPSMaster::AnswerNextCallL(TEtelLine aLine)
	{
	RDEBUGPRINTLOGGER1(_L("RPS: CRPSMaster::AnswerNextCallL"));
	iImpl->doAnswerNextCallL(aLine);
	}

EXPORT_C void CRPSMaster::RejectNextCallL(TEtelLine aLine)
	{
	RDEBUGPRINTLOGGER1(_L("RPS: CRPSMaster::RejectNextCallL"));
	iImpl->doRejectNextCallL(aLine);
	}

EXPORT_C void CRPSMaster::IgnoreNextCallL(TEtelLine aLine)
	{
	RDEBUGPRINTLOGGER1(_L("RPS: CRPSMaster::IgnoreNextCallL"));
	iImpl->doIgnoreNextCallL(aLine);
	}

EXPORT_C void CRPSMaster::HangUpL(TEtelLine aLine, TInt aDelayInSec)
	{
	RDEBUGPRINTLOGGER1(_L("RPS: CRPSMaster::HangUpL"));
	iImpl->doHangUpL(aLine, aDelayInSec);
	}

EXPORT_C void CRPSMaster::HoldL(TEtelLine aLine, TInt aDelayInSec)
	{
	RDEBUGPRINTLOGGER1(_L("RPS: CRPSMaster::HoldL"));
	iImpl->doHoldL(aLine, aDelayInSec);
	}
	
EXPORT_C void CRPSMaster::ResumeL(TEtelLine aLine, TInt aDelayInSec)
	{
	RDEBUGPRINTLOGGER1(_L("RPS: CRPSMaster::ResumeL"));
	iImpl->doResumeL(aLine, aDelayInSec);
	}

EXPORT_C void CRPSMaster::SendDataBackOnDataPortL(const TDesC& aDataToSend)
	{
	RDEBUGPRINTLOGGER1(_L("RPS: CRPSMaster::SendDataBackOnDataPortL"));
	iImpl->doSendDataBackOnDataPortL(aDataToSend);	
	}	

EXPORT_C void CRPSMaster::CleanupL()
	{
	RDEBUGPRINTLOGGER1(_L("RPS: CRPSMaster::Cleanup"));
	iImpl->doCleanupL();
	}
	
EXPORT_C void CRPSMaster::CallNumberHangUpL(const TDesC& aNumToDial, TEtelLine aLine, TInt aDelayInSec)
	{
	RDEBUGPRINTLOGGER2(_L("RPS: CRPSMaster::CallNumberHangUpL dial=%S"), &aNumToDial);
	iImpl->doCallNumberHangUpL(aNumToDial, aLine, aDelayInSec);
	}
	
EXPORT_C void CRPSMaster::CallNumberHangUpBeforeConnectL(const TDesC& aNumToDial, TEtelLine aLine, TInt aDelayInSec)
	{
	RDEBUGPRINTLOGGER2(_L("RPS: CRPSMaster::CallNumberHangUpBeforeConnectL dial=%S"), &aNumToDial);
	iImpl->doCallNumberHangUpBeforeConnectL(aNumToDial, aLine, aDelayInSec);
	}
	
EXPORT_C void CRPSMaster::CallNumberHangUpAfterConnectL(const TDesC& aNumToDial, TEtelLine aLine, TInt aDelayInSec)
	{
	RDEBUGPRINTLOGGER2(_L("RPS: CRPSMaster::CallNumberHangUpAfterConnectL dial=%S"), &aNumToDial);
	iImpl->doCallNumberHangUpAfterConnectL(aNumToDial, aLine, aDelayInSec);	
	}
	
EXPORT_C void CRPSMaster::AnswerNextCallHangUpL(TEtelLine aLine, TInt aDelayInSec)
	{
	RDEBUGPRINTLOGGER1(_L("RPS: CRPSMaster::AnswerNextCallHangUpL"));
	iImpl->doAnswerNextCallHangUpL(aLine, aDelayInSec);
	}
	
EXPORT_C void CRPSMaster::AnswerNextCallHoldL(TEtelLine aLine, TInt aDelayInSec)
	{
	RDEBUGPRINTLOGGER1(_L("RPS: CRPSMaster::AnswerNextCallHoldL"));
	iImpl->doAnswerNextCallHoldL(aLine, aDelayInSec);
	}

EXPORT_C void CRPSMaster::CheckRpsPresentL(TBool& aPresent)
	{
	iImpl->doCheckRpsPresentL(aPresent);
	RDEBUGPRINTLOGGER2(_L("RPS: CRPSMaster::CheckRpsPresentL returning = %d"), aPresent);
	}
	
EXPORT_C const TDesC16& CRPSMaster::RpsSlaveTelNum()
	{
	return iImpl->doRpsSlaveTelNum();
	}
	
EXPORT_C const TDesC16& CRPSMaster::RpsSlaveTelNumData()
	{
	return iImpl->doRpsSlaveTelNumData();
	}
	
EXPORT_C const TDesC16& CRPSMaster::RpsSlaveTelNumFax()
	{
	return iImpl->doRpsSlaveTelNumFax();
	}
	
EXPORT_C void CRPSMaster::HangUpNextCallBeforeConnectL(TEtelLine aLine)
	{
	RDEBUGPRINTLOGGER1(_L("RPS: CRPSMaster::HangUpNextCallBeforeConnectL"));
	iImpl->doHangUpNextCallBeforeConnectL(aLine);
	}

	
	

