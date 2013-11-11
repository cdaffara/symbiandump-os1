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
// cltsydispatchsathandler.h
//

/**
 @file
 @internalAll 
*/

#ifndef __CLTSYDISPATCHSATHANDLER_H_
#define __CLTSYDISPATCHSATHANDLER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <ctsy/ltsy/mltsydispatchinterface.h>
#include <ctsy/ltsy/mltsydispatchsatinterface.h>
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
class CLtsySatHandler : 	public CBase,
							public MIsDispatchInterfaceSupported, 
							public MLtsyDispatchSatSmsDeliverReport,
							public MLtsyDispatchSatGetImageInstance,
							public MLtsyDispatchSatGetIconData,
							public MLtsyDispatchSatGetClut,
							public MLtsyDispatchSatGetDefaultBearerCapability,
							public MLtsyDispatchSatGetSmsPpDownloadSupported,
							public MLtsyDispatchSatGetSmsControlActivated,
							public MLtsyDispatchSatCellBroadcastEnvelope,
							public MLtsyDispatchSatTimingAdvance,
							public MLtsyDispatchSatSmControlResponseData,
							public MLtsyDispatchSatProvideLocationInfo,
							public MLtsyDispatchSatSetPollingInterval,
							public MLtsyDispatchSatSmsPpDownloadEnvelope,
							public MLtsyDispatchSatLocalInformationNmr,
							public MLtsyDispatchSatGetAccessTechnology,
							public MLtsyDispatchSatGetUssdControlSupported,
							public MLtsyDispatchSatCallAndSmsControlEnvelope,
							public MLtsyDispatchSatRefreshAllowed,
							public MLtsyDispatchSatReady,
							public MLtsyDispatchSatPCmdNotification,
							public MLtsyDispatchSatUssdControlEnvelopeError,
							public MLtsyDispatchSatTimerExpirationEnvelope,
							public MLtsyDispatchSatTerminalRsp,
							public MLtsyDispatchSatMenuSelectionEnvelope,
							public MLtsyDispatchSatEventDownloadEnvelope
							
	{
public:

	virtual ~CLtsySatHandler();
	static CLtsySatHandler* NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	static CLtsySatHandler* NewLC(CCtsyDispatcherCallback& aCtsyDispatcherCallback);

	// From MIsDispatchInterfaceSupported
	virtual TBool IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId);
	void IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup aIdGroup, TUint32& aIndIdBitMask);
	


    // From MLtsyDispatchSatSmsDeliverReport
    virtual TInt HandleSmsDeliverReportReqL(const TDesC8& aSmsDeliverReportTpdu);

    // From MLtsyDispatchSatGetImageInstance
    virtual TInt HandleGetImageInstanceReqL(TUint8 aEfImgRecordNumber, TUint8 aInstanceNumberInEfImgRecord);

    // From MLtsyDispatchSatGetIconData
    virtual TInt HandleGetIconDataReqL(TUint8 aEfImgRecordNumber, const TDesC8& aSimFilePath);

    // From MLtsyDispatchSatGetClut
    virtual TInt HandleGetClutReqL(TUint8 aEfImgRecordNumber, TUint8 aInstanceNumberInEfImgRecord);

    // From MLtsyDispatchSatGetDefaultBearerCapability
    virtual TInt HandleGetDefaultBearerCapabilityReqL();

    // From MLtsyDispatchSatGetSmsPpDownloadSupported
    virtual TInt HandleGetSmsPpDownloadSupportedReqL();

    // From MLtsyDispatchSatGetSmsControlActivated
    virtual TInt HandleGetSmsControlActivatedReqL();

    // From MLtsyDispatchSatCellBroadcastEnvelope
    virtual TInt HandleCellBroadcastEnvelopeReqL(const TDesC8& aEnvelope);

    // From MLtsyDispatchSatTimingAdvance
    virtual TInt HandleTimingAdvanceReqL();

    // From MLtsyDispatchSatSmControlResponseData
    virtual TInt HandleSmControlResponseDataReqL(
    		TUint8 aResult, const TDesC8& aRpDestinationAddress, const TDesC8& aTpDestinationAddress);

	// From MLtsyDispatchSatGetAccessTechnology
    virtual TInt HandleGetAccessTechnologyReqL();
    
    // From MLtsyDispatchSatProvideLocationInfo
    virtual TInt HandleProvideLocationInfoReqL();

    // From MLtsyDispatchSatSetPollingInterval
    virtual TInt HandleSetPollingIntervalReqL(TUint8 aPollingInterval);

    // From MLtsyDispatchSatSmsPpDownloadEnvelope
    virtual TInt HandleSmsPpDownloadEnvelopeReqL(const TDesC8& aEnvelope);

    // From MLtsyDispatchSatLocalInformationNmr
    virtual TInt HandleLocalInformationNmrReqL();
    
    // From MLtsyDispatchSatGetUssdControlSupported
    virtual TInt HandleGetUssdControlSupportedReqL();

    // From MLtsyDispatchSatCallAndSmsControlEnvelope
    virtual TInt HandleCallAndSmsControlEnvelopeReqL(const TDesC8& aEnvelope);

    // From MLtsyDispatchSatRefreshAllowed
    virtual TInt HandleRefreshAllowedReqL(const TDesC8& aRefreshFileList, const TDesC8& aAid, TUint16 aTsyCache);

    // From MLtsyDispatchSatReady
    virtual TInt HandleReadyReqL();

    // From MLtsyDispatchSatPCmdNotification
    virtual TInt HandlePCmdNotificationReqL(TUint8 aPCmdType);

    // From MLtsyDispatchSatUssdControlEnvelopeError
    virtual TInt HandleUssdControlEnvelopeErrorReqL();

    // From MLtsyDispatchSatTimerExpirationEnvelope
    virtual TInt HandleTimerExpirationEnvelopeReqL(const TDesC8& aEnvelope);

    // From MLtsyDispatchSatTerminalRsp
    virtual TInt HandleTerminalRspReqL(const TDesC8& aTerminalRsp);

    // From MLtsyDispatchSatMenuSelectionEnvelope
    virtual TInt HandleMenuSelectionEnvelopeReqL(const TDesC8& aEnvelope);

    // From MLtsyDispatchSatEventDownloadEnvelope
    virtual TInt HandleEventDownloadEnvelopeReqL(const TDesC8& aEnvelope);


private:
	
	CLtsySatHandler(CCtsyDispatcherCallback& aCtsyDispatcherCallback);
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

	}; // class CLtsySatHandler

#endif // __CLTSYDISPATCHSATHANDLER_H_

