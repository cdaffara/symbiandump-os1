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
// @file ctlbsstep_setupstandaloneprivacymode.cpp
// This is the class implementation for the Set Autonomous Moder Test Step
// 
//

/**
 @file
 @internalTechnology
 @test
*/
		  
#include "ctlbsstepsetupstandaloneprivacymode.h"
#include <lbs/lbsadmin.h>
#include <centralrepository.h>


CT_LbsStep_SetupStandalonePrivacyMode::CT_LbsStep_SetupStandalonePrivacyMode(CT_LbsServer& aParent) : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_SetupStandalonePrivacyMode);
	}

/**
Static Constructor
*/
CT_LbsStep_SetupStandalonePrivacyMode* CT_LbsStep_SetupStandalonePrivacyMode::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_SetupStandalonePrivacyMode(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}


/**
@pre 	
@return Test verdict
@post	LBS sub-system set to standalone privacy mode.
*/
TVerdict CT_LbsStep_SetupStandalonePrivacyMode::doTestStepL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_SetupStandalonePrivacyMode::doTestStepL()"));
	
	// Set the privacy protocol module as the protocol module to load in admin settings
	CLbsAdmin* adminApi = CLbsAdmin::NewL();
	CleanupStack::PushL(adminApi);
	
	const TLbsProtocolModuleId KPrivacyPMUid = { 0x1028373C };
	TInt err = adminApi->Set(KLbsSettingHomeProtocolModule, KPrivacyPMUid);	
	User::LeaveIfError(err);
	TUid uid;
	err = adminApi->Get(KLbsSettingHomeProtocolModule, uid);	
	User::LeaveIfError(err);
	
	if(uid.iUid != KPrivacyPMUid.iUid)
		{
		INFO_PRINTF1(_L("Admin setting didn't work"));
		SetTestStepResult(EFail);
		}					
	
	//Get privacy handler type
	CLbsAdmin::TPrivacyHandler privacyHandlerType;
	err = adminApi->Get(KLbsSettingPrivacyHandler, privacyHandlerType);	
	User::LeaveIfError(err);
	
	// Turn off the LBS logger
	err = adminApi->Set(KLbsSettingLogger, CLbsAdmin::ELoggerOff);
	User::LeaveIfError(err);

	// Set the privacy timeout action as 'network defined'
	err = adminApi->Set(KLbsSettingPrivacyTimeoutAction, CLbsAdmin::EPrivacyTimeoutNetworkDefined);
	User::LeaveIfError(err);

	// Set the maximum number of simultaneous privacy requests allowed
	const TUint KMaxExternalLocateRequests(150);
	err = adminApi->Set(KLbsSettingMaximumExternalLocateRequests, KMaxExternalLocateRequests);
	User::LeaveIfError(err);

	CleanupStack::PopAndDestroy(adminApi);

	// Alow the setting to be propagated
	User::After(2000000);
		
	// Set-up the LbsRoot cenrep file so that:
	// 1) LbsRoot is transient.
	// 2) LbsRoot starts the LBS subsystem in transient mode
	// 3) LbsRoot only starts the NRH and NG.

	CRepository* rootRep = CRepository::NewLC(KLbsCenRepUid);
	
	// Set Lbs system and LbsRoot process as transient.
	
	// Start the LBS sub-system in transient mode.
	User::LeaveIfError(rootRep->Set(KLbsSystemTransientKey, 1));
	User::LeaveIfError(rootRep->Set(KLbsRootProcessTransientKey, 1));
	
	// Set only NRH and NG to be started.
	// First, clear out any existing startup entries.
	TUint32 errorKey;
	User::LeaveIfError(rootRep->Delete(KLbsRootProcessNameFirstKey, KRootKeyMask, errorKey));
	User::LeaveIfError(rootRep->Delete(KLbsRootProcessUidFirstKey, KRootKeyMask, errorKey));
	User::LeaveIfError(rootRep->Delete(KLbsRootProcessIsServerFirstKey, KRootKeyMask, errorKey));
	
	// Set NRH and NG as startup entries.
	_LIT(KNGProcessName, "lbsnetgateway.exe");
	const TUid KLbsNetworkGatewayUid = { 0x10281D46 };
	User::LeaveIfError(rootRep->Set(KLbsRootProcessNameFirstKey, KNGProcessName));
	User::LeaveIfError(rootRep->Set(KLbsRootProcessUidFirstKey, TInt(KLbsNetworkGatewayUid.iUid)));
	User::LeaveIfError(rootRep->Set(KLbsRootProcessIsServerFirstKey, EFalse));

	_LIT(KNRHProcessName, "lbsnetworkrequesthandler.exe");
	const TUid KLbsNetRequestHandlerUid = { 0x10281D45 };
	User::LeaveIfError(rootRep->Set(KLbsRootProcessNameFirstKey+1, KNRHProcessName));
	User::LeaveIfError(rootRep->Set(KLbsRootProcessUidFirstKey+1, TInt(KLbsNetRequestHandlerUid.iUid)));
	User::LeaveIfError(rootRep->Set(KLbsRootProcessIsServerFirstKey+1, ETrue));
	
	//If we are using Privacy Controller mode, start privacy controller as well
	if(privacyHandlerType == CLbsAdmin::EPrivacyHandleByController)
		{
		_LIT(KPrivCtrlProcessName, "lbstestprivacycontroller.exe");
		const TUid KLbsPrivacyControllerUid = { 0x10283750 };
		User::LeaveIfError(rootRep->Set(KLbsRootProcessNameFirstKey+2, KPrivCtrlProcessName));
		User::LeaveIfError(rootRep->Set(KLbsRootProcessUidFirstKey+2, TInt(KLbsPrivacyControllerUid.iUid)));
		User::LeaveIfError(rootRep->Set(KLbsRootProcessIsServerFirstKey+2, EFalse));
		}
	
	CleanupStack::PopAndDestroy(rootRep);

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_SetupStandalonePrivacyMode::doTestStepL()"));

	return TestStepResult();
	}

