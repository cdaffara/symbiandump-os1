/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file lbsnetgatewaynetregstatusstep.h
*/
#ifndef LBSNETGATEWAYNETREGSTATUSSTEP_H_
#define LBSNETGATEWAYNETREGSTATUSSTEP_H_

#include <etelmm.h>
#include <test/testexecutestepbase.h>
#include "Te_lbsnetgatewaySuiteStepBase.h"

class CLbsNetGatewayNetRegStatusStep : public CTe_lbsnetgatewaySuiteStepBase
	{
private:
	enum TModuleLoadingConfig
		{
		EByStatusTheSameProt,
		EByStatusDiffProtHomeFirst,
		EByStatusDiffProtRoamingFirst,
		EAllProtTheSameProt,
		EAllProtDiffProt
		};
	
	enum TAcceptPrivacyRequest
		{
		EAcceptPrivReq,
		ERejectPrivReq
		};
	
public:
	CLbsNetGatewayNetRegStatusStep();
	~CLbsNetGatewayNetRegStatusStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

	void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId,
			const TLbsNetInternalMsgBase& aMessage);
	void ProcessNetProtocolMessage(const TNetGatewayMsg& aMessage);
	void ProcessNetworkLocationResponseMessage(const TLbsNetLocMsgBase& aMessage);
	
	void WaitForNextResponse();
	void TestNewRequestResponseL(TBool iExpectRequestToBeRejected, TAcceptPrivacyRequest iAcceptPrivReq);
	void SetNetworkRegistrationStatusL(RMobilePhone::TMobilePhoneRegistrationStatus aNetRegStatus);
	
private:
	TVerdict MakeByStatusTheSameProtTestsL();
	TVerdict MakeByStatusDiffProtTestsL();
	TVerdict MakeByStatusDiffProtHomeFirstTestsL();
	TVerdict MakeByStatusDiffProtRoamingFirstTestsL();
	TVerdict MakeAllProtTheSameProtTestsL();
	TVerdict MakeAllProtDiffProtL();

private:
	CActiveSchedulerWait iActiveWait;
	
	// Buffers used to hold messages that have arrived from 
	// the network gateway. The messages can come from any
	// of the interfaces it uses.
	
	// Messages sent to the plugin protocol module.
	RPointerArray<TNetGatewayMsg> iPmMsgArray;
	
	// Messages sent to the Agps manager or NRH. (they use the same classes).
	RPointerArray<TLbsNetInternalMsgBase> iNrhOrAgpsMsgArray;
	
	// Messages sent to the Network Location Manager.
	RPointerArray<TLbsNetLocMsgBase> iNlmMsgArray;
	
	// Session id
	TUint iSessionId;
	
	//The message to send from the PM to the NG
	TNetGatewayMsg 	iMsg;
	
	//The pointer to the channel to use
	RNetGatewayProtocolTestChannel* iCurChannel;
	
	//The UID of the current protocol module
	TUid iCurProtocolUid;
	
	};

_LIT(KLbsNetGatewayNetRegStatusStep, "lbsnetgatewaynetregstatusstep");


#endif /*LBSNETGATEWAYNETREGSTATUSSTEP_H_*/
