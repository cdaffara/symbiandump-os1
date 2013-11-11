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
 @file LbsNetGatewayNetworkPsyStep.h
*/
#if (!defined __LBSNETGATEWAY_NETWORKPSY_STEP_H__)
#define __LBSNETGATEWAY_NETWORKPSY_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_lbsnetgatewaySuiteStepBase.h"

class CLbsNetGatewayNetworkPsyStep : public CTe_lbsnetgatewaySuiteStepBase
    {
public:
    CLbsNetGatewayNetworkPsyStep();
    ~CLbsNetGatewayNetworkPsyStep();
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepL();
    virtual TVerdict doTestStepPostambleL();
    
    void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage);
    void ProcessNetProtocolMessage(const TNetGatewayMsg& aMessage);

// Please add/modify your class members here:
private:
    const TLbsNetInternalMsgBase*   iNetInternalMessage;
    TNetGatewayMsg          iNetProtocolMessage;
    TPosition               iPosition;
    TPositionExtendedSatelliteInfo  iPositionInfo;
    TPositionGpsMeasurementInfo iGpsPositionInfo;
    TTime                   iTimeStamp;
    TNetGatewayMsg          iMsg;
    
    TPosition iReferencePosition;
    TLbsNetPosRequestPrivacy iPrivacy;
    };

_LIT(KLbsNetgatewayNetworkPsyStep,"lbsnetgatewaynetworkpsystep");

#endif
