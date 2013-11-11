// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// cltsydispatchphonehandler.cpp
//

#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>



#include "cltsyphonehandler.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "ltsymacros.h"

// Can be removed in real LTSY implementation
#include MTEST_INCLUDE_MOCKLTSYENGINE_HEADER

CLtsyPhoneHandler::CLtsyPhoneHandler(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	: iCtsyDispatcherCallback(aCtsyDispatcherCallback)
	{
	}

	
CLtsyPhoneHandler::~CLtsyPhoneHandler()
	{
	}


CLtsyPhoneHandler* CLtsyPhoneHandler::NewLC(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsyPhoneHandler* self = new (ELeave) CLtsyPhoneHandler(aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


CLtsyPhoneHandler* CLtsyPhoneHandler::NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsyPhoneHandler* self=
			CLtsyPhoneHandler::NewLC(aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}


void CLtsyPhoneHandler::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;
	
	
	} // CLtsyPhoneHandler::ConstructL

TBool CLtsyPhoneHandler::IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId)
/**
 * Returns whether a Dispatch Interface APi is supported or not.
 * 
 * @param aDispatchApiId Id of Dispatch interface being queried
 * @return indication whether interface is supported or not
 */
	{
	// REMOVE this line when implementing for real LTSY
	MTEST_QUERY_SUPPORT_MOCKLTSYENGINE(KDispatchPhoneFuncUnitId, aDispatchApiId);
	
	/* uncomment when implementing for real LTSY
	switch(aDispatchApiId)
		{
		// Insert ApiIds when support is provided in LTSY e.g.
		// case MLtsyDispatchPhoneBootNotifyModemStatusReady::HandleBootNotifyModemStatusReadyReqL: return ETrue;
		default:
			return EFalse;
		}
	*/
	}

void CLtsyPhoneHandler::IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup aIdGroup, TUint32& aIndIdBitMask)
/**
 * Returns the set of callback indicators that are supported.
 * 
* @param aIdGroup ID of group that is being queried e.g. aIdGroup=EIndIdGroup1.
* @param aIndIdBitMask [out] argument that should return a bitmask indicating which indicator callbacks are supported.
 */
	{
	// REMOVE this line when implementing for real LTSY
	MTEST_QUERY_IND_SUPPORT_MOCKLTSYENGINE(KDispatchPhoneFuncUnitId, aIdGroup, aIndIdBitMask);
	
	//Create bitmask with bitset for each indicator ID that is supported. E.g.
	/*
	if(aIdGroup == EIndIdGroup1)
		aIndIdBitMask = KLtsyDispatchPhoneNotifyNetworkModeChangeIndId | 
						KLtsyDispatchPhoneNotifyEgprsInfoChangeIndId | 
						KLtsyDispatchPhoneNotifySignalStrengthChangeIndId;
	else
		aIndIdBitMask = 0; //no indicators from other groups supported
	*/
	}

TInt CLtsyPhoneHandler::HandleGetFdnStatusReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetFdnStatusComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......
	
	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetFdnStatus::KLtsyDispatchPhoneGetFdnStatusApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetFdnStatusReqL


TInt CLtsyPhoneHandler::HandleGetNetworkRegistrationStatusReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetNetworkRegistrationStatusComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;
	
	TInt ret = KErrNotSupported;

	//Add implementation here.......
	 
	
	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetNetworkRegistrationStatus::KLtsyDispatchPhoneGetNetworkRegistrationStatusApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetNetworkRegistrationStatusReqL


TInt CLtsyPhoneHandler::HandleGetHomeNetworkReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetHomeNetworkComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetHomeNetwork::KLtsyDispatchPhoneGetHomeNetworkApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetHomeNetworkReqL


TInt CLtsyPhoneHandler::HandleBootNotifyModemStatusReadyReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneBootNotifyModemStatusReadyComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneBootNotifyModemStatusReady::KLtsyDispatchPhoneBootNotifyModemStatusReadyApiId);
	
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleBootNotifyModemStatusReadyReqL


TInt CLtsyPhoneHandler::HandleBootNotifySimStatusReadyReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneBootNotifySimStatusReadyComp()
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......
	
	 // REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneBootNotifySimStatusReady::KLtsyDispatchPhoneBootNotifySimStatusReadyApiId);
	 
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleBootNotifySimStatusReadyReqL

TInt CLtsyPhoneHandler::HandleSimRefreshRegisterReqL(TUint16 aFilesToRegister)
/**
 * @param aFilesToRegister  Bitmask containing list of files to register for refresh indications.
 * The list of possible files is specified in TCacheFileTypes.
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;
	
	TInt ret = KErrNotSupported;

	//Add implementation here.......
	
	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(	ret, 
									KDispatchPhoneFuncUnitId, 
									MLtsyDispatchPhoneSimRefreshRegister::KLtsyDispatchPhoneSimRefreshRegisterApiId,
									aFilesToRegister);
				
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleSimRefreshRegisterReqL

TInt CLtsyPhoneHandler::HandleGetServiceProviderNameReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetServiceProviderNameComp()
 *
 = * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetServiceProviderName::KLtsyDispatchPhoneGetServiceProviderNameApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetServiceProviderNameReqL


TInt CLtsyPhoneHandler::HandleGetPhoneIdReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetPhoneIdComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetPhoneIdReqL


TInt CLtsyPhoneHandler::HandleGetDetectedNetworksReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetDetectedNetworksComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetDetectedNetworks::KLtsyDispatchPhoneGetDetectedNetworksApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetDetectedNetworksReqL


TInt CLtsyPhoneHandler::HandleGetDetectedNetworksCancelReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetDetectedNetworksCancelComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetDetectedNetworksCancel::KLtsyDispatchPhoneGetDetectedNetworksCancelApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetDetectedNetworksCancelReqL


TInt CLtsyPhoneHandler::HandleSelectNetworkReqL(TBool aIsManual, const RMobilePhone::TMobilePhoneNetworkManualSelection& aNetworkManualSelection)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneSelectNetworkComp()
 *
 * @param aIsManual Specifies whether phone should use a manual or automatic network selection method.
 * @param aNetworkManualSelection If aIsManual==ETrue, then this parameter contain the user's manually selected network.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchPhoneFuncUnitId, 
										MLtsyDispatchPhoneSelectNetwork::KLtsyDispatchPhoneSelectNetworkApiId,
										aIsManual,
										&aNetworkManualSelection);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleSelectNetworkReqL


TInt CLtsyPhoneHandler::HandleSelectNetworkCancelReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneSelectNetworkCancelComp()
 *
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneSelectNetworkCancel::KLtsyDispatchPhoneSelectNetworkCancelApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleSelectNetworkCancelReqL


TInt CLtsyPhoneHandler::HandleSetNetworkSelectionSettingReqL(const RMobilePhone::TMobilePhoneNetworkSelectionV1& aPhoneNetworkSelection)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneSetNetworkSelectionSettingComp()
 *
 * @param aPhoneNetworkSelection The network selection to be set.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(	ret, 
									KDispatchPhoneFuncUnitId, 
									MLtsyDispatchPhoneSetNetworkSelectionSetting::KLtsyDispatchPhoneSetNetworkSelectionSettingApiId,
									&aPhoneNetworkSelection);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleSetNetworkSelectionSettingReqL


TInt CLtsyPhoneHandler::HandleGetCurrentNetworkInfoReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetCurrentNetworkInfoComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetCurrentNetworkInfo::KLtsyDispatchPhoneGetCurrentNetworkInfoApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetCurrentNetworkInfoReqL


TInt CLtsyPhoneHandler::HandleGetNetworkModeReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetNetworkModeComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetNetworkMode::KLtsyDispatchPhoneGetNetworkModeApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetNetworkModeReqL


TInt CLtsyPhoneHandler::HandleGetNitzInfoReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetNitzInfoComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetNitzInfo::KLtsyDispatchPhoneGetNitzInfoApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetNITZInfoReqL()


TInt CLtsyPhoneHandler::HandleGetSignalStrengthReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetSignalStrengthComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetSignalStrength::KLtsyDispatchPhoneGetSignalStrengthApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetSignalStrengthReqL()


TInt CLtsyPhoneHandler::HandleGetBatteryInfoReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetBatteryInfoComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetBatteryInfo::KLtsyDispatchPhoneGetBatteryInfoApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetBatteryInfoReqL


TInt CLtsyPhoneHandler::HandleNspsWakeupReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneNspsWakeupComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneNspsWakeup::KLtsyDispatchPhoneNspsWakeupApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleNspsWakeupReqL


TInt CLtsyPhoneHandler::HandleSetSystemNetworkModeReqL(RMmCustomAPI::TNetworkModeCaps aMode)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneSetSystemNetworkModeComp()
 *
 * @param aMode Specifies the mode to set the system network to.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchPhoneFuncUnitId,
										MLtsyDispatchPhoneSetSystemNetworkMode::KLtsyDispatchPhoneSetSystemNetworkModeApiId,
										aMode);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleSetSystemNetworkModeReqL


TInt CLtsyPhoneHandler::HandleGetCurrentSystemNetworkModesReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetCurrentSystemNetworkModesComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetCurrentSystemNetworkModes::KLtsyDispatchPhoneGetCurrentSystemNetworkModesApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetCurrentSystemNetworkModesReqL


TInt CLtsyPhoneHandler::HandleResetNetServerReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneResetNetServerComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhoneFuncUnitId,
										MLtsyDispatchPhoneResetNetServer::KLtsyDispatchPhoneResetNetServerApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleResetNetServerReqL


TInt CLtsyPhoneHandler::HandleSetAlwaysOnModeReqL(RMmCustomAPI::TSetAlwaysOnMode aMode)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneSetAlwaysOnModeComp()
 *
 * @param aMode The new "always on" mode to set, can be
 *               RMmCustomAPI::EAlwaysModeVPLMN for VPLMN (Visited Public Land Mobile Network) always on,
 *               RMmCustomAPI::EAlwaysModeHPLMN for HPLMN (Home Public Land Mobile Network) always on,
 *               RMmCustomAPI::EAlwaysModeBoth for VPLMN and HPLMN always on,
 *               RMmCustomAPI::EAlwaysModeNeither for neither VPLMN mor HPLMN always on.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchPhoneFuncUnitId,
										MLtsyDispatchPhoneSetAlwaysOnMode::KLtsyDispatchPhoneSetAlwaysOnModeApiId,
										aMode);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleSetAlwaysOnModeReqL


TInt CLtsyPhoneHandler::HandleSetDriveModeReqL(RMmCustomAPI::TSetDriveMode aMode)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneSetDriveModeComp()
 *
 * @param aMode The new mode to set (RMmCustomAPI::EDeactivateDriveMode for deactivate drive mode
 *               or RMmCustomAPI::EActivateDriveMode to activate drive mode).
 *
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchPhoneFuncUnitId,
										MLtsyDispatchPhoneSetDriveMode::KLtsyDispatchPhoneSetDriveModeApiId,
										aMode);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleSetDriveModeReqL


TInt CLtsyPhoneHandler::HandleGetHspaStatusReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetHspaStatusComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetHspaStatus::KLtsyDispatchPhoneGetHspaStatusApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetHspaStatusReqL


TInt CLtsyPhoneHandler::HandleSetHspaStatusReqL(RMmCustomAPI::THSxPAStatus aStatus)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneSetHspaStatusComp()
 *
 * @param aStatus The new HSxPA status (enable or disable).
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchPhoneFuncUnitId,
										MLtsyDispatchPhoneSetHspaStatus::KLtsyDispatchPhoneSetHspaStatusApiId,
										aStatus);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleSetHspaStatusReqL


TInt CLtsyPhoneHandler::HandleGetNetworkProviderNameReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetNetworkProviderNameComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetNetworkProviderName::KLtsyDispatchPhoneGetNetworkProviderNameApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetNetworkProviderNameReqL


TInt CLtsyPhoneHandler::HandleGetOperatorNameReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetOperatorNameComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetOperatorName::KLtsyDispatchPhoneGetOperatorNameApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetOperatorNameReqL


TInt CLtsyPhoneHandler::HandleGetCellInfoReqL()
/**
 * This request is completed by invoking one of the 
 * CCtsyDispatcherCallback::CallbackPhoneGetCellInfoComp().
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetCellInfo::KLtsyDispatchPhoneGetCellInfoApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetCellInfoReqL

TInt CLtsyPhoneHandler::HandleRegisterCellInfoChangeNotificationReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneRegisterCellInfoChangeNotificationComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneRegisterCellInfoChangeNotification::KLtsyDispatchPhoneRegisterCellInfoChangeNotificationApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleRegisterCellInfoChangeNotificationReqL 

TInt CLtsyPhoneHandler::HandleGetPhoneCellInfoReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetPhoneCellInfoComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetPhoneCellInfo::KLtsyDispatchPhoneGetPhoneCellInfoApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetPhoneCellInfoReqL

TInt CLtsyPhoneHandler::HandleGetGetUsimServiceSupportReqL(TInt aApplicationNumber)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetUsimServiceSupportComp()
 * 
 * @param aApplicationNumber The application number to check support for in the USIM.
 * 
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(	ret, 
									KDispatchPhoneFuncUnitId, 
									MLtsyDispatchPhoneGetUsimServiceSupport::KLtsyDispatchPhoneGetUsimServiceSupportApiId,
									aApplicationNumber);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetGetUsimServiceSupportReqL

TInt CLtsyPhoneHandler::HandleGetCurrentActiveUsimApplicationReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetCurrentActiveUsimApplicationComp()
 * 
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(	ret, 
									KDispatchPhoneFuncUnitId, 
									MLtsyDispatchPhoneGetCurrentActiveUsimApplication::KLtsyDispatchPhoneGetCurrentActiveUsimApplicationApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetCurrentActiveUsimApplicationReqL
  
TInt CLtsyPhoneHandler::HandleTerminateAllCallsReqL(TInt aCallId)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneTerminateAllCallsComp()
 * 
 * @param aCallId if a call is in the connecting state then this parameter will contain the id of that call, else 0.
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(	ret, 
									KDispatchPhoneFuncUnitId, 
									MLtsyDispatchPhoneTerminateAllCalls::KLtsyDispatchPhoneTerminateAllCallsApiId,
									aCallId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleTerminateAllCallsReqL

TInt CLtsyPhoneHandler::HandleGetSystemNetworkBandReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneGetSystemNetworkBandComp()
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(	ret, 
									KDispatchPhoneFuncUnitId, 
									MLtsyDispatchPhoneGetSystemNetworkBand::KLtsyDispatchPhoneGetSystemNetworkBandApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetSystemNetworkBandReqL

TInt CLtsyPhoneHandler::HandleSetSystemNetworkBandReqL(RMmCustomAPI::TBandSelection aBand, RMmCustomAPI::TNetworkModeCaps aMode)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhoneSetSystemNetworkBandComp()
 * 
 * @param aBand the requested band
 * @param aMode the requested mode
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(	ret, 
									KDispatchPhoneFuncUnitId, 
									MLtsyDispatchPhoneSetSystemNetworkBand::KLtsyDispatchPhoneSetSystemNetworkBandApiId,
									aBand, aMode);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhoneHandler::HandleGetSystemNetworkBandReqL
