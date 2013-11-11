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
#if (!defined __LBSNETGATEWAY1_STEP_H__)
#define __LBSNETGATEWAY1_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_lbsnetgatewaySuiteStepBase.h"

class Clbsnetgateway1Step : public CTe_lbsnetgatewaySuiteStepBase
	{
public:
	Clbsnetgateway1Step();
	~Clbsnetgateway1Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
	void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage);
	void ProcessNetProtocolMessage(const TNetGatewayMsg& aMessage);

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

_LIT(Klbsnetgateway1Step,"lbsnetgateway1Step");

#endif
