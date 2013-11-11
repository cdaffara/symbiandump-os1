/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file lbsnetgateway1Step.h
*/
#if (!defined __LBSNETGATEWAY7MULTIPLEPROTOCOLMODULE_STEP_H__)
#define __LBSNETGATEWAY7MULTIPLEPROTOCOLMODULE_STEP_H__
#include <test/TestExecuteStepBase.h>
#include "Te_lbsnetgatewaySuiteStepBase.h"

class Clbsnetgateway7MultiplePMStep : public CTe_lbsnetgatewaySuiteStepBase
	{
public:
	Clbsnetgateway7MultiplePMStep();
	~Clbsnetgateway7MultiplePMStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
	void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage);
	void ProcessNetProtocolMessage(const TNetGatewayMsg& aMessage);
	
	void SendMessagesOverMultiplePms(TLbsAdminProtocolModulesInfo::TLbsProtocolModuleIdArray aUidArray);

	void ResetLoopCounter();

	
	
// Please add/modify your class members here:
private:
	const TLbsNetInternalMsgBase*	iNetInternalMessage;
	TNetGatewayMsg 			iNetProtocolMessage;
	TPosition 				iPosition;
	TPositionExtendedSatelliteInfo 	iPositionInfo;
	TPositionGpsMeasurementInfo iGpsPositionInfo;
	TTime 					iTimeStamp;
	TNetGatewayMsg 			iMsg;
	
	TLbsNetPosRequestPrivacy iPrivacy;
	};

_LIT(Klbsnetgateway7MultiplePMStep,"lbsnetgateway7MultiplePMStep");

#endif // __LBSNETGATEWAY7MULTIPLEPROTOCOLMODULE_STEP_H__
