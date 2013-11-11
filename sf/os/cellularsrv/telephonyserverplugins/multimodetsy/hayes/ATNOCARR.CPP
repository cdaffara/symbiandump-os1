// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ATNOCARR.H"
#include "mSLOGGER.H"
#include "CALL.H"
#include "ATIO.H"

CATWaitForNoCarrier* CATWaitForNoCarrier::NewL(CATIO* aIo,CTelObject* aTelObject,CPhoneGlobals* aPhoneGlobals)
	{
	return new(ELeave) CATWaitForNoCarrier(aIo,aTelObject,aPhoneGlobals);
	}

CATWaitForNoCarrier::CATWaitForNoCarrier(CATIO* aIo,CTelObject* aTelObject,CPhoneGlobals* aPhoneGlobals) 
								: CATBase(aIo,aTelObject,aPhoneGlobals),iNoCarrierExpectString(NULL),iCallMonitoringEventString(NULL)
	{}

CATWaitForNoCarrier::~CATWaitForNoCarrier()
	{
	iIo->RemoveExpectStrings(this);
	iIo->WriteAndTimerCancel(this);	
	}

void CATWaitForNoCarrier::StartWait()
	{
	LOGTEXT(_L8("WaitForNoCarrier:\tStarting wait for no carrier"));
	if (!iNoCarrierExpectString)
		iNoCarrierExpectString=iIo->AddExpectString(this,KNoCarrierString);

//  Commented out as the same function is executed by Fax/Data calls also (except Voice)
//	This was causing serious problems in timeout situations

//	if (!iCallMonitoringEventString)
//		iCallMonitoringEventString=iIo->AddExpectString(this,KCallMonitoringEventString);
	}

void CATWaitForNoCarrier::StopWait()
	{
	iIo->RemoveExpectStrings(this);
	iNoCarrierExpectString = NULL;
	}

void CATWaitForNoCarrier::CompleteWithIOError(TEventSource /*aSource*/,TInt /*aStatus*/)
	{
	iIo->WriteAndTimerCancel(this);
	iNoCarrierExpectString = NULL;
	}

void CATWaitForNoCarrier::EventSignal(TEventSource aSource)
	{
	__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadNotExpected));
	__ASSERT_ALWAYS (((iIo->FoundChatString()==iNoCarrierExpectString) || \
					  (iIo->FoundChatString()==iCallMonitoringEventString)), \
					 Panic (EATCommand_IllegalResponse) );

	if (iIo->FoundChatString()==iCallMonitoringEventString) 
		LOGTEXT(_L8("WaitForNoCarrier:\t*ECAV Call Monitoring Event (similar to NO CARRIER) detected"));
	else
		LOGTEXT(_L8("WaitForNoCarrier:\tNO CARRIER detected"));

	// Reconfigure the Serial port to remove the DCD Obey flag (Defect fix no: MIR-4QHMBV)
	TCommConfig aConfigPckg;
	TInt ret=iPhoneGlobals->iConfiguration->PortConfig(aConfigPckg,EConfigTypeInit);
	if (ret==KErrNone)
		__ASSERT_ALWAYS(iIo->ConfigurePort(aConfigPckg) == KErrNone,Panic(EGeneral));
	
	REINTERPRET_CAST(CCallHayes*,iTelObject)->SetToIdle();
	}
