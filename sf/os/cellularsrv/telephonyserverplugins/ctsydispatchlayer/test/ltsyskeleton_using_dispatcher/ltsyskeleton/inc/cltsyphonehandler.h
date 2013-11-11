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
// cltsydispatchphonehandler.h
//

/**
 @file
 @internalAll 
*/

#ifndef __CLTSYDISPATCHPHONEHANDLER_H_
#define __CLTSYDISPATCHPHONEHANDLER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <ctsy/ltsy/mltsydispatchinterface.h>
#include <ctsy/ltsy/mltsydispatchphoneinterface.h>
#include "misdispatchinterfacesupported.h"
#include "ltsymacros.h"

// Can be removed in real LTSY implementation
#include MTEST_INCLUDE_MOCKLTSYENGINE_HEADER

// FORWARD DECLARATIONS
class CCtsyDispatcherCallback;

// Can be removed in real LTSY implementation
MTEST_FORWARD_DECLARE_MOCKLTSYENGINE

// CLASS DECLARATION

/**
 * Implements interfaces from the CTSY Dispatcher. The CTSY Dispatcher uses these
 * interfaces to make requests to the Licensee LTSY.
 */
class CLtsyPhoneHandler : 	public CBase,
							public MIsDispatchInterfaceSupported, 		
							public MLtsyDispatchPhoneGetFdnStatus,
							public MLtsyDispatchPhoneGetNetworkRegistrationStatus,
							public MLtsyDispatchPhoneGetHomeNetwork,
							public MLtsyDispatchPhoneBootNotifyModemStatusReady,
							public MLtsyDispatchPhoneBootNotifySimStatusReady,
							public MLtsyDispatchPhoneSimRefreshRegister,
							public MLtsyDispatchPhoneGetServiceProviderName,
							public MLtsyDispatchPhoneGetPhoneId,
							public MLtsyDispatchPhoneGetDetectedNetworks,
							public MLtsyDispatchPhoneGetDetectedNetworksCancel,
							public MLtsyDispatchPhoneSelectNetwork,
							public MLtsyDispatchPhoneSelectNetworkCancel,
							public MLtsyDispatchPhoneSetNetworkSelectionSetting,
							public MLtsyDispatchPhoneGetCurrentNetworkInfo,
							public MLtsyDispatchPhoneGetNetworkMode,
							public MLtsyDispatchPhoneGetNitzInfo,
							public MLtsyDispatchPhoneGetSignalStrength,
							public MLtsyDispatchPhoneGetBatteryInfo,
							public MLtsyDispatchPhoneNspsWakeup,
                            public MLtsyDispatchPhoneSetSystemNetworkMode,
                            public MLtsyDispatchPhoneGetCurrentSystemNetworkModes,
                            public MLtsyDispatchPhoneResetNetServer,
                            public MLtsyDispatchPhoneSetAlwaysOnMode,
                            public MLtsyDispatchPhoneSetDriveMode,
                            public MLtsyDispatchPhoneGetHspaStatus,
                            public MLtsyDispatchPhoneSetHspaStatus,
                            public MLtsyDispatchPhoneGetNetworkProviderName,
                            public MLtsyDispatchPhoneGetOperatorName,
                            public MLtsyDispatchPhoneGetCellInfo,
							public MLtsyDispatchPhoneRegisterCellInfoChangeNotification,
							public MLtsyDispatchPhoneGetPhoneCellInfo,
							public MLtsyDispatchPhoneGetUsimServiceSupport,
							public MLtsyDispatchPhoneGetCurrentActiveUsimApplication,
							public MLtsyDispatchPhoneTerminateAllCalls,
							public MLtsyDispatchPhoneGetSystemNetworkBand,
							public MLtsyDispatchPhoneSetSystemNetworkBand
							
	{
public:

	virtual ~CLtsyPhoneHandler();
	static CLtsyPhoneHandler* NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	static CLtsyPhoneHandler* NewLC(CCtsyDispatcherCallback& aCtsyDispatcherCallback);

	// From MIsDispatchInterfaceSupported
	virtual TBool IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId);
	void IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup aIdGroup, TUint32& aIndIdBitMask);
	
	// From MLtsyDispatchPhoneGetFdnStatus
	virtual TInt HandleGetFdnStatusReqL();

	// From MLtsyDispatchPhoneGetNetworkRegistrationStatus
	virtual TInt HandleGetNetworkRegistrationStatusReqL();

	// From MLtsyDispatchPhoneGetHomeNetwork
	virtual TInt HandleGetHomeNetworkReqL();

	// From MLtsyDispatchPhoneBootNotifyModemStatusReady
	virtual TInt HandleBootNotifyModemStatusReadyReqL();

	// From MLtsyDispatchPhoneBootNotifySimStatusReady
	virtual TInt HandleBootNotifySimStatusReadyReqL();

	// From MLtsyDispatchPhoneSimRefreshRegister
	virtual TInt HandleSimRefreshRegisterReqL(TUint16 aFilesToRegister);
	
    // From MLtsyDispatchPhoneGetServiceProviderName
    virtual TInt HandleGetServiceProviderNameReqL();

    // From MLtsyDispatchPhoneGetPhoneId
    virtual TInt HandleGetPhoneIdReqL();

    // From MLtsyDispatchPhoneGetDetectedNetworks
    virtual TInt HandleGetDetectedNetworksReqL();

    // From MLtsyDispatchPhoneGetDetectedNetworksCancel
    virtual TInt HandleGetDetectedNetworksCancelReqL();

    // From MLtsyDispatchPhoneSelectNetwork
    virtual TInt HandleSelectNetworkReqL(TBool aIsManual, const RMobilePhone::TMobilePhoneNetworkManualSelection& aNetworkManualSelection);

    // From MLtsyDispatchPhoneSelectNetworkCancel
    virtual TInt HandleSelectNetworkCancelReqL();

    // From MLtsyDispatchPhoneSetNetworkSelectionSetting
    virtual TInt HandleSetNetworkSelectionSettingReqL(const RMobilePhone::TMobilePhoneNetworkSelectionV1& aPhoneNetworkSelection);
    
    // From MLtsyDispatchPhoneGetCurrentNetworkInfo
    virtual TInt HandleGetCurrentNetworkInfoReqL();
    
    // From MLtsyDispatchPhoneGetNetworkMode
    virtual TInt HandleGetNetworkModeReqL();
    
    // From MLtsyDispatchPhoneGetNitzInfo
    virtual TInt HandleGetNitzInfoReqL();
    
    // From MLtsyDispatchPhoneGetSignalStrength
    virtual TInt HandleGetSignalStrengthReqL();
    
    // From MLtsyDispatchPhoneGetBatteryInfo
    virtual TInt HandleGetBatteryInfoReqL();

	// From MLtsyDispatchPhoneNspsWakeup
	virtual TInt HandleNspsWakeupReqL();

	// From MLtsyDispatchPhoneSetSystemNetworkMode
	virtual TInt HandleSetSystemNetworkModeReqL(RMmCustomAPI::TNetworkModeCaps aMode);

	// From MLtsyDispatchPhoneGetCurrentSystemNetworkModes
	virtual TInt HandleGetCurrentSystemNetworkModesReqL();

	// From MLtsyDispatchPhoneResetNetServer
	virtual TInt HandleResetNetServerReqL();

	// From MLtsyDispatchPhoneSetAlwaysOnMode
	virtual TInt HandleSetAlwaysOnModeReqL(RMmCustomAPI::TSetAlwaysOnMode aMode);

	// From MLtsyDispatchPhoneSetDriveMode
	virtual TInt HandleSetDriveModeReqL(RMmCustomAPI::TSetDriveMode aMode);
    
	// From MLtsyDispatchPhoneGetHspaStatus
	virtual TInt HandleGetHspaStatusReqL();

	// From MLtsyDispatchPhoneSetHspaStatus
	virtual TInt HandleSetHspaStatusReqL(RMmCustomAPI::THSxPAStatus aStatus);
    
	// From MLtsyDispatchPhoneGetNetworkProviderName
	virtual TInt HandleGetNetworkProviderNameReqL();
    
	// From MLtsyDispatchPhoneGetOperatorName
	virtual TInt HandleGetOperatorNameReqL();
    
	// From MLtsyDispatchPhoneGetCellInfo
	virtual TInt HandleGetCellInfoReqL();

	// From MLtsyDispatchPhoneRegisterCellInfoChangeNotification
    virtual TInt HandleRegisterCellInfoChangeNotificationReqL();
    
    // From MLtsyDispatchPhoneGetPhoneCellInfo
    virtual TInt HandleGetPhoneCellInfoReqL();
    
    // From MLtsyDispatchPhoneGetUsimServiceSupport
    virtual TInt HandleGetGetUsimServiceSupportReqL(TInt aApplicationNumber);
    
    // From MLtsyDispatchPhoneGetCurrentActiveUsimApplication
    virtual TInt HandleGetCurrentActiveUsimApplicationReqL();

	// From MLtsyDispatchPhoneTerminateAllCalls
    virtual TInt HandleTerminateAllCallsReqL(TInt aCallId);
    
	// From MLtsyDispatchPhoneGetSystemNetworkBand
    virtual TInt HandleGetSystemNetworkBandReqL();
    
    // From MLtsyDispatchPhoneSetSystemNetworkBand
    virtual TInt HandleSetSystemNetworkBandReqL(RMmCustomAPI::TBandSelection aBand, RMmCustomAPI::TNetworkModeCaps aMode);
private:
	
	CLtsyPhoneHandler(CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	void ConstructL();

private:
	
	// Not owned
	
	/**
	 * Callback object in the CTSY Dispatcher.
	 * 
	 * Used to complete a request back to the CTSY Dispatcher.
	 */
	CCtsyDispatcherCallback& iCtsyDispatcherCallback;
	
public:
	// Can be removed in real LTSY implementation.
	MTEST_DECLARE_MOCKLTSYENGINE

	}; // class CLtsyPhoneHandler

#endif // __CLTSYDISPATCHPHONEHANDLER_H_

