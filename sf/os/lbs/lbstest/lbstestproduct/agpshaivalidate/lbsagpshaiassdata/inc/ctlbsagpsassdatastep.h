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
// @file ctlbsagpsassdatastep.h
// 
//

#ifndef __CTLBSAGPSASSDATASTEP_H__
#define __CTLBSAGPSASSDATASTEP_H__

#include <lbs/lbsadmin.h>
#include <lbs/lbsassistancedatasourcemodule.h>

#include "ctlbsagpshaiserverroot.h"
#include "ctlbshaivalidatesteproot.h"

class CT_LbsAGPSAssDataStep : 	public CT_LbsAGPSHAIValidateStepRoot,
								public MLbsAssistanceDataSourceObserver
	{
public:
	~CT_LbsAGPSAssDataStep();

	static CT_LbsAGPSAssDataStep* New(CT_LbsAGPSHAIServerRoot& aParent);
	
	static CT_LbsAGPSAssDataStep* NewL(CT_LbsAGPSHAIServerRoot& aParent);
	
	TVerdict doTestStepL();
	
	TInt GetAssistanceDataItem(TLbsAssistanceDataItem aItem, RDataReaderRootBase& aDataRoot, TTime& aTimeStamp);
	TInt GetAssistanceDataItemTimeStamp(TLbsAssistanceDataItem aItem, TTime& aTimeStamp);
	void RequestAssistanceData(TLbsAsistanceDataGroup aDataItemMask);


// from MLbsAssistanceDataSourceObserver
	void DataSourceNotification(const TInt& aError);

private:
	// internal methods
	// private constructors:
	CT_LbsAGPSAssDataStep(CT_LbsAGPSHAIServerRoot& aParent);
	void ConstructL();
	// for the assistance data provider
	void LoadADPModuleL();
	void UnLoadADPModule();
	// config stuff
	TBool GetParametersFromConfigL();
	
	// test cases:
	void DoImmediateRequestWithUnsolicitedDataTestCase();
	
	void VerifyStandardAssistanceDataDetails();

  	// plugin uid
  	TUid iDataProviderPluginUid;	
	// agps mode
	CLbsAdmin::TGpsMode iAGPSMode;
	
	// Passed to, and filled in by, assistance data provider with request
	RLbsAssistanceDataBuilderSet iDataBuilder;

	// Assistance data that module asked for:
	TLbsAsistanceDataGroup	iRequestedAssistanceDataGroup;
	
	// Read from config. Passed to assistance data provider in case it's required
	TPosition iReferencePosition;

	// Assistance data provider (ECOM plugin)
	CAssistanceDataSourceModuleBase* iAssistanceDataPlugin;
	
	// assistance data session
	CAssistanceDataSourceBase* iAssistanceData;
	
	// number of times the gps module is expected to ask for assistance data during this test case
	TInt iMaxExpectedDataRequests;
		
	// assistance data mask to be used to tell the module what data is available (if diff from what's actually available)
	TInt iReportedAvailableAssistanceDataMask;
	
	// Assistance data that was last requested by current client:
	TInt iLastRequestedAssistanceDataMask;
	
	// Assistance data that the module has been notified is available during this test case (ie for current client):
	TInt iAssistanceDataNotified;
	
	// Assistance data that the module has been provided with during this test case (ie for current client):
	TInt iAssistanceDataReturned;
	
	// how many times the module has requested assistance data for the current test case (ie for current client):
	TInt iAssistanceDataRequestedCount;
		
	};

#endif //__CT_LBS_DATA_SRC_STEP_AGPS_H__
