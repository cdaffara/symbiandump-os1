/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file te_lbsnetworkprivacysuitestepbase.h
*/

#if (!defined __TE_LBSNETWORKPRIVACY_SUITE_STEP_BASE__)
#define __TE_LBSNETWORKPRIVACY_SUITE_STEP_BASE__
#include <test/testexecutestepbase.h>
#include <lbs/epos_cposnetworkprivacy.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/epos_momasuplobserver.h>
#include <lbs/epos_comasuplposhandlerbase.h>
#include <lbs/epos_comasuplpossessionbase.h>

#include "gatewaychannel.h"
#include "lbsnetinternalapi.h"
#include "lbsprocesssupervisor.h"

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTe_LbsNetworkPrivacySuiteStepBase : public CTestStep,public MLbsNetChannelObserver, public MOMASuplObserver
	{
public:
	virtual ~CTe_LbsNetworkPrivacySuiteStepBase();
    CTe_LbsNetworkPrivacySuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();

    // from MLbsNetChannelObserver - receives messages from NG
	void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage);

	// from MOMASuplObserver
	TReal SuplVersion();
	void TerminateSession();
	   
    // start/stop the lbsnetgateway.exe
	TInt StartNetGateway();
	TInt StopNetGateway();
	//start&stop all interfaces
	void InitialiseInterfacesL();
	void ShutDownInterfacesL();

protected:
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
       
    // Active scheduler
	CActiveScheduler* iSched;
	
	CGatewayChannel* iGatewayChannel;
    
    //To load privacy client 
    CPosNetworkPrivacy* iPosNetworkPrivacy; 
    // to load positioning client
    COMASuplPosHandlerBase* iPosHandler;
    COMASuplPosSessionBase* iPosSession;
    // recevies request id
    TInt iRequestId;
    
	const TLbsNetInternalMsgBase*	iNetInternalMessage;
	
	RLbsProcessSupervisor iProcessSupervisor;
	
	};	

#endif
