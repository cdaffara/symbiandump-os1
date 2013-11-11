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
 @file te_suplprotocolSuiteStepBase.h
 @internalTechnology
*/

#if (!defined __TE_SUPLPROTOCOL_STEP_BASE__)
#define __TE_SUPLPROTOCOL_STEP_BASE__

#include <test/testexecutestepbase.h>
#include <ecom/ecom.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <centralrepository.h>
#include "suplgatewayobserver.h"
#include "suplnetworktestobserver.h"


const TUid  KLbsSuplTestUid = {0x10285AA0};

class CTe_suplprotocolSuiteStepBase : public CTestStep
	{
public:
	virtual ~CTe_suplprotocolSuiteStepBase();
	CTe_suplprotocolSuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();
	
	void ResetObserversL();
	void ResetGatewayObserverL();
	void ResetNetworkObserverL();
	TVerdict CheckGatewayCallbackL(TInt aExpectedResult);
	TVerdict CheckNetworkCallbackL(TInt aExpectedResult);
	
	void SetDummyAssistanceData(RLbsAssistanceDataBuilderSet& aAssistData, 
								TLbsAsistanceDataGroup aDataRequestMask);
								
	// Methods for building SUPL messages							
	CSuplMessageBase* BuildSuplResponseL(TPositionModuleInfo::TTechnologyType aTechType);
	CSuplMessageBase* BuildSuplPosAssitDataL(TLbsAsistanceDataGroup aDataRequestMask, TBool moreDataFollows = EFalse);
	CSuplMessageBase* BuildSuplPosRequestL(TPositionModuleInfo::TTechnologyType, TLbsAsistanceDataGroup aDataSupplied, const TUid& posMeans = KLbsPositioningMeansGps);
	CSuplMessageBase* BuildSuplEndL(TBool aDummyPosition, TInt errorCode = KErrNone);
	
private:
	void SetSessionSetId(CSuplSetId* aSetId);
	TBool CheckSetSessionId(const CSuplSetSessionId* aSetSessionId);
		
protected:
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	
	// Active scheduler
	CActiveScheduler* iSched;
	
	// Module Observer on LBS Gateway side
	CSuplGatewayObserver* iGatewayObserver;
	
	// Module Observer on Network side
	CSuplNetworkTestObserver* iNetworkObserver;
			
	};

#endif
