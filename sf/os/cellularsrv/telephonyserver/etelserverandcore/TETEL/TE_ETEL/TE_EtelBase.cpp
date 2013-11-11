// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <c32comm.h>
#include <f32file.h>

#include <e32base.h>
#include <e32svr.h>
#include <e32property.h> 

#include "TE_EtelBase.h"
#include "Tetelcs.h"


TVerdict CEtelTestStep::doTestStepPreambleL()
	{
	__UHEAP_MARK;

	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	return TestStepResult();
	} // CEtelTestStep::doTestStepPreambleL


TVerdict CEtelTestStep::doTestStepPostambleL()
	{
	delete iScheduler;
	iScheduler = NULL;

	__UHEAP_MARKEND;
	
	return TestStepResult();
	} // CEtelTestStep::doTestStepPostambleL


void CEtelTestStep::ServerClose(RTelServer& aServer)
	{
	aServer.Close();
	User::After(KETelThreadShutdownGuardPeriod);
	}


RTestPhone::RTestPhone()
	{
	}


void RTestPhone::TestNarrowSetAndGet(TRequestStatus& aReqStatus, const TDesC8& aSet, TDes8& aGet) const
	{
	SetAndGet(ETestEtelNarrowSetAndGet, aReqStatus, aSet, aGet);
	}


void RTestPhone::TestUnicodeSetAndNarrowGet(TRequestStatus& aReqStatus, TDes8& aGet, const TDesC16& aSet) const
	{
	SetAndGet(ETestEtelUnicodeSetAndNarrowGet, aReqStatus, aGet, aSet);
	}


void RTestPhone::TestUnicodeSetAndGet(TRequestStatus& aReqStatus, const TDesC16& aSet, TDes16& aGet) const
	{
	SetAndGet(ETestEtelUnicodeSetAndGet, aReqStatus, aSet, aGet);
	}


void RTestPhone::TestNarrowSetAndUnicodeGet(TRequestStatus& aReqStatus, const TDesC8& aSet, TDes16& aGet) const
	{
	SetAndGet(ETestEtelNarrowSetAndUnicodeGet, aReqStatus, aSet, aGet);
	}

void RTestPhone::TestDoubleUnicodeGet(TRequestStatus& aReqStatus, TDes16& aGet1, TDes16& aGet2) const
	{
	Get(ETestEtelDoubleUnicodeGet, aReqStatus, aGet1, aGet2);
	}


TInt RTestPhone::TestCustomIPC(const TInt aCustomIpc)
	{
	return Blank(aCustomIpc);
	}

