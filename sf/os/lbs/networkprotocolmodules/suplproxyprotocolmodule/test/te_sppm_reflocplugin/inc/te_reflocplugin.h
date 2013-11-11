// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef REFLOCPLUGIN_H
#define REFLOCPLUGIN_H


#include <e32base.h>
#include <lbs/lbsreflocdatasourcebase.h>
#include "te_delaytimer.h"
#include <e32property.h>

// Properties used to modify the behaviour of the plugin for different test cases.
// Test Suite UID used for publishing properties.
const TInt KSuplProxyPMSuiteUid     = 0x102871F5;
const TUid KSuplProxyPMSuite        = {KSuplProxyPMSuiteUid};

//Key values for test control properties
const TUint KLbsPosSessionInitStepKey   = 0x10000000;

// These values should match the test case id found in te_suplproxypmsuite.ini
// These values are used to vary the behaviour of plugins [such as the ref loc plugin] for
// different test cases.
enum TSuplProxyTestCase
    {
    ERefLocPluginPositionError = 46,
    };

/**
 * 
 */
class CTe_LbsRefLocationSource : public CLbsRefLocationSourceBase, public MTimerCallback
	{
public:
	static CTe_LbsRefLocationSource* NewL(MLbsRefLocationObserver& aObserver);
	~CTe_LbsRefLocationSource();
	
	//Sets the requestor address (SLP address)
	virtual void SetRequestorAddress(const TDesC& aAddress);
	
	//Makes an asynchronous request for the reference position
	virtual void RequestRefPositionInfo();
	
	//Cancels the outstanding asynchronous request for the reference location
	virtual void CancelRefPositionInfoRequest();
	
	//Called on timer completion to return the ref position to the SuplProxyProtocolModule
	void CompleteCallback();
	
private:
	CTe_LbsRefLocationSource(MLbsRefLocationObserver& aObserver);
	void ConstructL();
	
private:
	TBool iReturnPosition;
	CTe_DelayTimer* iDelayTimer;
    //Used for differentiating the behaviour of this plugin based on the test case
	RProperty iTestCaseIdKey;
	};

#endif //REFLOCPLUGIN_H
