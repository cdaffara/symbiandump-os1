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
//

/**
 @file
 @internalAll 
*/

#ifndef __CSATDISPATCHER_H_
#define __CSATDISPATCHER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <e32def.h>
#include <etelmm.h>
#include <etelsat.h>

#include <ctsy/ltsy/mltsydispatchfactory.h>

#include "requestqueueoneshot.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "mdispatchercallback.h"
#include <ctsy/ltsy/mltsydispatchsatinterface.h>

// FORWARD DECLARATIONS
class CMmDataPackage;
class MmMessageManagerCallback;
class TDispatcherHolder;
class MLtsyDispatchSatSmsDeliverReport;
class MLtsyDispatchSatGetImageInstance;
class MLtsyDispatchSatGetIconData;
class MLtsyDispatchSatGetClut;
class MLtsyDispatchSatGetDefaultBearerCapability;
class MLtsyDispatchSatGetSmsPpDownloadSupported;
class MLtsyDispatchSatGetSmsControlActivated;
class MLtsyDispatchSatCellBroadcastEnvelope;
class MLtsyDispatchSatGetAccessTechnology;
class MLtsyDispatchSatTimingAdvance;
class MLtsyDispatchSatSmControlResponseData;
class MLtsyDispatchSatProvideLocationInfo;
class MLtsyDispatchSatSetPollingInterval;
class MLtsyDispatchSatSmsPpDownloadEnvelope;
class MLtsyDispatchSatLocalInformationNmr;
class MLtsyDispatchSatCallAndSmsControlEnvelope;
class MLtsyDispatchSatRefreshAllowed;
class MLtsyDispatchSatReady;
class MLtsyDispatchSatPCmdNotification;
class MLtsyDispatchSatUssdControlEnvelopeError;
class MLtsyDispatchSatTimerExpirationEnvelope;
class MLtsyDispatchSatTerminalRsp;
class MLtsyDispatchSatMenuSelectionEnvelope;
class MLtsyDispatchSatEventDownloadEnvelope;
class MLtsyDispatchSatGetUssdControlSupported;

class MStkTsyCallOrigin;



// CLASS DECLARATION

/**
 * This class is responsible for packing and unpacking data belonging
 * to Sat related requests to the Licensee LTSY.
 */
class CSatDispatcher : public CBase, public MDispatcherCallback
	{
public:

	virtual ~CSatDispatcher();
	
	static CSatDispatcher* NewL(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aTsyMessageManagerCallback,
			MmMessageManagerCallback& aSatMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot);
	
	static CSatDispatcher* NewLC(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aTsyMessageManagerCallback,
			MmMessageManagerCallback& aSatMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot);
	

	// Dispatcher functions for dispatching requests DOWN to the Licensee LTSY
	
	TInt DispatchSmsDeliverReportL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetImageInstanceL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetIconDataL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetClutL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetDefaultBearerCapabilityL();
	TInt DispatchSmsPpDdlStatusL();
	TInt DispatchGetSmsControlActivatedL();
	TInt DispatchCellBroadcastL(const CMmDataPackage* aDataPackage);
	TInt DispatchTimingAdvanceL();
	TInt DispatchNotifySmControlRequestL(const CMmDataPackage* aDataPackage);
	TInt DispatchProvideLocationInfoL();
	TInt DispatchSetPollingIntervalL(const CMmDataPackage* aDataPackage);
	TInt DispatchSmsPpDdlL(const CMmDataPackage* aDataPackage);
	TInt DispatchAccessTechnologyL();
	TInt DispatchLocalInformationNmrL();
	TInt DispatchSendEnvelopeL(const CMmDataPackage* aDataPackage);
	TInt DispatchServiceRequestL(const CMmDataPackage* aDataPackage);
	TInt DispatchReadyL();
	TInt DispatchPCmdNotificationL(const CMmDataPackage* aDataPackage);
	TInt DispatchUssdControlEnvelopeErrorL();
	TInt DispatchTimerExpiredL(const CMmDataPackage* aDataPackage);
	TInt DispatchTerminalRspL(const CMmDataPackage* aDataPackage);
	TInt DispatchMenuSelectionL(const CMmDataPackage* aDataPackage);
	TInt DispatchEventDownloadL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetUssdControlSupportedL();
	
	
	// Complete functions for receiving completions UP from the Licensee LTSY
	// via the CCtsyDispatcherCallback object.
	
	void CallbackUssdControlSupported(TInt aError, TBool aUssdStatus);
	void CallbackPcmd(TInt aError, const TDesC8& aData);
	void CallbackNotifyProactiveSimSessionEnd(TInt aError, TUint8 aStatusWord1, TUint8 aStatusWord2);
	void CallbackTimeZoneChange(TInt aError, TUint8 aTimeZone);
	void CallbackAccessTechnologyChange(TInt aError, TUint8 aCurrentAccessTechnology);
	void CallbackCallConnected(TInt aError, TUint8 aTransactionId, TBool aNearEnd);
	void CallbackSsChange(TInt aError, DispatcherSat::TSsStatus aStatus);
	void CallbackCallDisconnected(TInt aError, TUint8 aTransactionId, TBool aNearEnd, const TDesC8& aCause);
	void CallbackMtCall(TInt aError, TUint8 aTransactionId, const TDesC8& aAddress, const TDesC8& aSubAddress);
	void CallbackCallControlEnvelopeResponse(TInt aError, const TDesC8& aResponseTlv);
	void CallbackCreateCallControlEnvelope(TInt aError, TUint8 aTag, const TDesC8& aBcc1, const TDesC8& aBcc2, const TDesC8& aAddress, TUint8 aTonNpi, TUint8 aDcs);
	void CallbackCreateSmControlEnvelope(TInt aError, const TDesC8& aRpDestinationAddress, const TDesC8& aTpDestinationAddress);
	void CallbackImsiChanged(TInt aError);
	void CallbackLocationStatus(TInt aError, TUint8 aLcnStatus, const TDesC8& aCountryCode,
			TUint16 aLcnAreaCode, TUint16 aCellId);
	void CallbackRemoveEventList(TInt aError);
	void CallbackCreateRefreshTerminalRsp(TInt aError, RSat::TPCmdResult aGeneralResult, const TDesC& aResultAdditionalInfo);
	void CallbackSmControlEnvelopeResponse(TInt aError, const TDesC8& aResponseTlv);
	void CallbackSatCreateSmsPpDownloadEnvelope(TInt aError, const TDesC8& aAddress, const TDesC8& aSmsTpdu);
	void CallbackCreateSmsDeliverReport(TInt aError);
	void CallbackCreateCellBroadcastEnvelope(TInt aError, const TDesC8& aCellBroadcastPage);
	void CallbackGetImageInstance(TInt aError, const TDesC8& aImageInstanceData, const TDesC8& aImageInstanceBody);
	void CallbackGetIconData(TInt aError, const TDesC8& aIconEfImgRecord);
	void CallbackGetClut(TInt aError, const TDesC8& aClut);
	void CallbackGetDefaultBearerCapability(TInt aError, const TDesC8& aCapability);
	void CallbackGetSmsPpDownloadSupported(TInt aError, TBool aSupported);
	void CallbackGetSmsControlActivated(TInt aError, TBool aSmsMoControlActivated);
	void CallbackTimingAdvance(TInt aError, TUint8 aTimingAdvance, TUint8 aMeStatus);
	void CallbackProvideLocationInfo(TInt aError, TUint16 aLocationAreaCode, TUint16 aCellId, 
			const TDesC8& aOperatorCode);
	void CallbackSetPolling(TInt aError, TUint8 aPollingInterval);
	void CallbackLocalInformationNmr(TInt aError, const TDesC8& aNmr, const TDesC& aBcchList);
	void CallbackRefreshAllowed(TInt aError);
	void CallbackReady(TInt aError);
	void CallbackGetAccessTechnology(TInt aError, TUint8 aAccessTechnology);
	void CallbackUssdControlEnvelopeError(TInt aError);
	void CallbackTerminalRsp(TInt aError);
	
	// From MDispatcherCallback
	void CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage);

	// Other public functions
	void SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder);
	
	void SetSatCallOriginInterface(MStkTsyCallOrigin& aSatCallOrigin);
	
	TBool IsSimOriginatedCall(const RMobilePhone::TMobileAddress& aDialledParty);

private:

	CSatDispatcher(MLtsyDispatchFactoryV1& aLtsyFactory,
	   			  	 MmMessageManagerCallback& aMessageManagerCallback,
	   			  	 MmMessageManagerCallback& aSatMessageManagerCallback,
	   			  	 CRequestQueueOneShot& aRequestAsyncOneShot);		
	
	void ConstructL();
	
	void CompleteNotifyLocalInfo(TInt aError);
	
private:	// Not owned
	MStkTsyCallOrigin* iStkTsyCallOrigin;

private:	// Not owned
	MLtsyDispatchFactoryV1& iLtsyFactoryV1;
	MmMessageManagerCallback& iTsyMessageManagerCallback;
	MmMessageManagerCallback& iMessageManagerCallback;
	CRequestQueueOneShot& iRequestAsyncOneShot;
	TDispatcherHolder* iDispatcherHolder;
	
	// Interfaces in the Licensee LTSY, not owned by this object
	
    MLtsyDispatchSatSmsDeliverReport* iLtsyDispatchSatSmsDeliverReport;
    MLtsyDispatchSatGetImageInstance* iLtsyDispatchSatGetImageInstance;
    MLtsyDispatchSatGetIconData* iLtsyDispatchSatGetIconData;
    MLtsyDispatchSatGetClut* iLtsyDispatchSatGetClut;
    MLtsyDispatchSatGetDefaultBearerCapability* iLtsyDispatchSatGetDefaultBearerCapability;
    MLtsyDispatchSatGetSmsPpDownloadSupported* iLtsyDispatchSatGetSmsPpDownloadSupported;
    MLtsyDispatchSatGetSmsControlActivated* iLtsyDispatchSatGetSmsControlActivated;
    MLtsyDispatchSatCellBroadcastEnvelope* iLtsyDispatchSatCellBroadcast;
    MLtsyDispatchSatTimingAdvance* iLtsyDispatchSatTimingAdvance;
    MLtsyDispatchSatSmControlResponseData* iLtsyDispatchSatSmControlResponseData;
    MLtsyDispatchSatProvideLocationInfo* iLtsyDispatchSatProvideLocationInfo;
    MLtsyDispatchSatSetPollingInterval* iLtsyDispatchSatSetPollingInterval;
    MLtsyDispatchSatSmsPpDownloadEnvelope* iLtsyDispatchSatSmsPpDownloadEnvelope;
    MLtsyDispatchSatLocalInformationNmr* iLtsyDispatchSatLocalInformationNmr;
    MLtsyDispatchSatCallAndSmsControlEnvelope* iLtsyDispatchSatSendEnvelope;
    MLtsyDispatchSatRefreshAllowed* iLtsyDispatchSatRefreshAllowed;
    MLtsyDispatchSatReady* iLtsyDispatchSatReady;
    MLtsyDispatchSatPCmdNotification* iLtsyDispatchSatPCmdNotification;
    MLtsyDispatchSatUssdControlEnvelopeError* iLtsyDispatchSatUssdControlEnvelopeError;
    MLtsyDispatchSatTimerExpirationEnvelope* iLtsyDispatchSatTimerExpired;
    MLtsyDispatchSatTerminalRsp* iLtsyDispatchSatTerminalRsp;
    MLtsyDispatchSatGetAccessTechnology* iLtsyDispatchSatAccTech;
    MLtsyDispatchSatMenuSelectionEnvelope* iLtsyDispatchSatMenuSelection;
    MLtsyDispatchSatEventDownloadEnvelope* iLtsyDispatchSatEventDownload;
    MLtsyDispatchSatGetUssdControlSupported* iLtsyDispatchSatGetUssdControlSupported;
      
    //record that a provide local info proactive command is currently operating and that
    //a request has been sent to the LTSY for the particular info we need, and when it arrives
    //we can complete the terminal response and proactive command.
    TBool iLocalInfoOngoing;

	}; // class CSatDispatcher

#endif // __CSATDISPATCHER_H_
	
