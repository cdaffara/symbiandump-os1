// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "csatdispatcherTraces.h"
#endif

#include "csatdispatcher.h"

#include <etelsat.h>
#include <ctsy/ltsy/mltsydispatchfactory.h>
#include <ctsy/ltsy/mltsydispatchsatinterface.h>
#include <ctsy/pluginapi/mmmessagemanagercallback.h>
#include <ctsy/serviceapi/mstktsycallorigin.h>
#include <satcs.h>


#include <ctsy/ltsy/ltsylogger.h>
#include "ctsydispatcherpanic.h"
#include "tdispatcherholder.h"
#include <ctsy/ltsy/sat_defs.h>
#include <ctsy/rmmcustomapi.h> //for ECustomSatRefreshCompleteNotificationIPC

__DEBUG_ONLY(const TInt KNmrLength = 16;) //this value is the length of a buffer within commonstktsy - its not defined anywhere as a constant. 
__DEBUG_ONLY(const TInt KBcchListLength = 32;)  //this value is the length of a buffer within commonstktsy - its not defined anywhere as a constant.

//const TUint8 KBasicIconDataLength		= 2;
//const TUint8 KColourIconDataLength		= KIconInfoLength;

CSatDispatcher::CSatDispatcher(
		MLtsyDispatchFactoryV1& aLtsyFactory,
		MmMessageManagerCallback& aTsyMessageManagerCallback,
		MmMessageManagerCallback& aSatMessageManagerCallback,
		CRequestQueueOneShot& aRequestAsyncOneShot)
	: 	iLtsyFactoryV1(aLtsyFactory),
		iTsyMessageManagerCallback(aTsyMessageManagerCallback),
		iMessageManagerCallback(aSatMessageManagerCallback),
		iRequestAsyncOneShot(aRequestAsyncOneShot),
		iLocalInfoOngoing(EFalse)
	{
	} // CSatDispatcher::CSatDispatcher

	  
CSatDispatcher::~CSatDispatcher()
	{
	} // CSatDispatcher::~CSatDispatcher


CSatDispatcher* CSatDispatcher::NewLC(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aTsyMessageManagerCallback,
	MmMessageManagerCallback& aSatMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot)
	{
	TSYLOGENTRYEXIT;
	CSatDispatcher* self =
		new (ELeave) CSatDispatcher(aLtsyFactory, aTsyMessageManagerCallback, aSatMessageManagerCallback, aRequestAsyncOneShot);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	} // CSatDispatcher::NewLC


CSatDispatcher* CSatDispatcher::NewL(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aTsyMessageManagerCallback,
	MmMessageManagerCallback& aSatMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot)
	{
	TSYLOGENTRYEXIT;
	CSatDispatcher* self =
		CSatDispatcher::NewLC(aLtsyFactory, aTsyMessageManagerCallback, aSatMessageManagerCallback, aRequestAsyncOneShot);
	CleanupStack::Pop (self);
	return self;
	} // CSatDispatcher::NewL


void CSatDispatcher::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;
	
	// Get the Licensee LTSY interfaces related to Sat functionality
	// from the factory
	
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSatFuncUnitId, MLtsyDispatchSatSmsDeliverReport::KLtsyDispatchSatSmsDeliverReportApiId))
		{
		TAny* smsDeliverReportInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSatSmsDeliverReport::KLtsyDispatchSatSmsDeliverReportApiId,
		       	smsDeliverReportInterface);
		iLtsyDispatchSatSmsDeliverReport =
				static_cast<MLtsyDispatchSatSmsDeliverReport*>(smsDeliverReportInterface);
        __ASSERT_DEBUG(iLtsyDispatchSatSmsDeliverReport, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSatFuncUnitId, MLtsyDispatchSatGetImageInstance::KLtsyDispatchSatGetImageInstanceApiId))
		{
		TAny* getImageInstanceInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSatGetImageInstance::KLtsyDispatchSatGetImageInstanceApiId,
		       	getImageInstanceInterface);
		iLtsyDispatchSatGetImageInstance =
				static_cast<MLtsyDispatchSatGetImageInstance*>(getImageInstanceInterface);
        __ASSERT_DEBUG(iLtsyDispatchSatGetImageInstance, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSatFuncUnitId, MLtsyDispatchSatGetIconData::KLtsyDispatchSatGetIconDataApiId))
		{
		TAny* getIconDataInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSatGetIconData::KLtsyDispatchSatGetIconDataApiId,
		       	getIconDataInterface);
		iLtsyDispatchSatGetIconData =
				static_cast<MLtsyDispatchSatGetIconData*>(getIconDataInterface);
        __ASSERT_DEBUG(iLtsyDispatchSatGetIconData, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSatFuncUnitId, MLtsyDispatchSatGetClut::KLtsyDispatchSatGetClutApiId))
		{
		TAny* getClutInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSatGetClut::KLtsyDispatchSatGetClutApiId,
		       	getClutInterface);
		iLtsyDispatchSatGetClut =
				static_cast<MLtsyDispatchSatGetClut*>(getClutInterface);
        __ASSERT_DEBUG(iLtsyDispatchSatGetClut, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(	KDispatchSatFuncUnitId, 
													MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId))
		{
		TAny* getDefaultBearerCapabilityInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId,
		       	getDefaultBearerCapabilityInterface);
		iLtsyDispatchSatGetDefaultBearerCapability =
				static_cast<MLtsyDispatchSatGetDefaultBearerCapability*>(getDefaultBearerCapabilityInterface);
        __ASSERT_DEBUG(iLtsyDispatchSatGetDefaultBearerCapability, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(	KDispatchSatFuncUnitId, 
													MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId))
		{
		TAny* smsPpDdlStatusInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId,
		       	smsPpDdlStatusInterface);
		iLtsyDispatchSatGetSmsPpDownloadSupported =
				static_cast<MLtsyDispatchSatGetSmsPpDownloadSupported*>(smsPpDdlStatusInterface);
        __ASSERT_DEBUG(iLtsyDispatchSatGetSmsPpDownloadSupported, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(	KDispatchSatFuncUnitId, 
													MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId))
		{
		TAny* getSmsControlActivatedInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId,
		       	getSmsControlActivatedInterface);
		iLtsyDispatchSatGetSmsControlActivated =
				static_cast<MLtsyDispatchSatGetSmsControlActivated*>(getSmsControlActivatedInterface);
        __ASSERT_DEBUG(iLtsyDispatchSatGetSmsControlActivated, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSatFuncUnitId, MLtsyDispatchSatGetAccessTechnology::KLtsyDispatchSatGetAccessTechnologyApiId))
		{
		TAny* accTechInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSatGetAccessTechnology::KLtsyDispatchSatGetAccessTechnologyApiId,
		       	accTechInterface);
		iLtsyDispatchSatAccTech =
				static_cast<MLtsyDispatchSatGetAccessTechnology*>(accTechInterface);
        __ASSERT_DEBUG(iLtsyDispatchSatAccTech, CtsyDispatcherPanic(EInvalidNullPtr));
   		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSatFuncUnitId, MLtsyDispatchSatCellBroadcastEnvelope::KLtsyDispatchSatCellBroadcastEnvelopeApiId))
		{
		TAny* cellBroadcastInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSatCellBroadcastEnvelope::KLtsyDispatchSatCellBroadcastEnvelopeApiId,
		       	cellBroadcastInterface);
		iLtsyDispatchSatCellBroadcast =
				static_cast<MLtsyDispatchSatCellBroadcastEnvelope*>(cellBroadcastInterface);
        __ASSERT_DEBUG(iLtsyDispatchSatCellBroadcast, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSatFuncUnitId, MLtsyDispatchSatTimingAdvance::KLtsyDispatchSatTimingAdvanceApiId))
		{
		TAny* timingAdvanceInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSatTimingAdvance::KLtsyDispatchSatTimingAdvanceApiId,
		       	timingAdvanceInterface);
		iLtsyDispatchSatTimingAdvance =
				static_cast<MLtsyDispatchSatTimingAdvance*>(timingAdvanceInterface);
        __ASSERT_DEBUG(iLtsyDispatchSatTimingAdvance, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSatFuncUnitId, MLtsyDispatchSatSmControlResponseData::KLtsyDispatchSatSmControlResponseDataApiId))
		{
		TAny* smControlResponseDataInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSatSmControlResponseData::KLtsyDispatchSatSmControlResponseDataApiId,
		       	smControlResponseDataInterface);
		iLtsyDispatchSatSmControlResponseData =
				static_cast<MLtsyDispatchSatSmControlResponseData*>(smControlResponseDataInterface);
        __ASSERT_DEBUG(iLtsyDispatchSatSmControlResponseData, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSatFuncUnitId, MLtsyDispatchSatProvideLocationInfo::KLtsyDispatchSatProvideLocationInfoApiId))
		{
		TAny* provideLocationInfoInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSatProvideLocationInfo::KLtsyDispatchSatProvideLocationInfoApiId,
		       	provideLocationInfoInterface);
		iLtsyDispatchSatProvideLocationInfo =
				static_cast<MLtsyDispatchSatProvideLocationInfo*>(provideLocationInfoInterface);
        __ASSERT_DEBUG(iLtsyDispatchSatProvideLocationInfo, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSatFuncUnitId, MLtsyDispatchSatSetPollingInterval::KLtsyDispatchSatSetPollingIntervalApiId))
		{
		TAny* setPollingIntervalInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSatSetPollingInterval::KLtsyDispatchSatSetPollingIntervalApiId,
		       	setPollingIntervalInterface);
		iLtsyDispatchSatSetPollingInterval =
				static_cast<MLtsyDispatchSatSetPollingInterval*>(setPollingIntervalInterface);
        __ASSERT_DEBUG(iLtsyDispatchSatSetPollingInterval, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSatFuncUnitId, MLtsyDispatchSatSmsPpDownloadEnvelope::KLtsyDispatchSatSmsPpDownloadEnvelopeApiId))
		{
		TAny* smsPpDownloadEnvelopeInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSatSmsPpDownloadEnvelope::KLtsyDispatchSatSmsPpDownloadEnvelopeApiId,
		       	smsPpDownloadEnvelopeInterface);
		iLtsyDispatchSatSmsPpDownloadEnvelope =
				static_cast<MLtsyDispatchSatSmsPpDownloadEnvelope*>(smsPpDownloadEnvelopeInterface);
        __ASSERT_DEBUG(iLtsyDispatchSatSmsPpDownloadEnvelope, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSatFuncUnitId, MLtsyDispatchSatLocalInformationNmr::KLtsyDispatchSatLocalInformationNmrApiId))
		{
		TAny* localInformationNmrInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSatLocalInformationNmr::KLtsyDispatchSatLocalInformationNmrApiId,
		       	localInformationNmrInterface);
		iLtsyDispatchSatLocalInformationNmr =
				static_cast<MLtsyDispatchSatLocalInformationNmr*>(localInformationNmrInterface);
        __ASSERT_DEBUG(iLtsyDispatchSatLocalInformationNmr, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(	KDispatchSatFuncUnitId, 
													MLtsyDispatchSatCallAndSmsControlEnvelope::KLtsyDispatchSatCallAndSmsControlEnvelopeApiId))
		{
		TAny* callAndSmsControlEnvelopeInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSatCallAndSmsControlEnvelope::KLtsyDispatchSatCallAndSmsControlEnvelopeApiId,
		       	callAndSmsControlEnvelopeInterface);
		iLtsyDispatchSatSendEnvelope =
				static_cast<MLtsyDispatchSatCallAndSmsControlEnvelope*>(callAndSmsControlEnvelopeInterface);
        __ASSERT_DEBUG(iLtsyDispatchSatSendEnvelope, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSatFuncUnitId, MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId))
		{
		TAny* refreshAllowedInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId,
		       	refreshAllowedInterface);
		iLtsyDispatchSatRefreshAllowed =
				static_cast<MLtsyDispatchSatRefreshAllowed*>(refreshAllowedInterface);
        __ASSERT_DEBUG(iLtsyDispatchSatRefreshAllowed, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSatFuncUnitId, MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId))
		{
		TAny* readyInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId,
		       	readyInterface);
		iLtsyDispatchSatReady =
				static_cast<MLtsyDispatchSatReady*>(readyInterface);
        __ASSERT_DEBUG(iLtsyDispatchSatReady, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSatFuncUnitId, MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId))
		{
		TAny* pCmdNotificationInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId,
		       	pCmdNotificationInterface);
		iLtsyDispatchSatPCmdNotification =
				static_cast<MLtsyDispatchSatPCmdNotification*>(pCmdNotificationInterface);
        __ASSERT_DEBUG(iLtsyDispatchSatPCmdNotification, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(	KDispatchSatFuncUnitId, 
													MLtsyDispatchSatUssdControlEnvelopeError::KLtsyDispatchSatUssdControlEnvelopeErrorApiId))
		{
		TAny* envelopeErrorInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSatUssdControlEnvelopeError::KLtsyDispatchSatUssdControlEnvelopeErrorApiId,
		       	envelopeErrorInterface);
		iLtsyDispatchSatUssdControlEnvelopeError =
				static_cast<MLtsyDispatchSatUssdControlEnvelopeError*>(envelopeErrorInterface);
        __ASSERT_DEBUG(iLtsyDispatchSatUssdControlEnvelopeError, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSatFuncUnitId, MLtsyDispatchSatTimerExpirationEnvelope::KLtsyDispatchSatTimerExpirationEnvelopeApiId))
		{
		TAny* timerExpirationEnvelopInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSatTimerExpirationEnvelope::KLtsyDispatchSatTimerExpirationEnvelopeApiId,
		       	timerExpirationEnvelopInterface);
		iLtsyDispatchSatTimerExpired =
				static_cast<MLtsyDispatchSatTimerExpirationEnvelope*>(timerExpirationEnvelopInterface);
        __ASSERT_DEBUG(iLtsyDispatchSatTimerExpired, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSatFuncUnitId, MLtsyDispatchSatTerminalRsp::KLtsyDispatchSatTerminalRspApiId))
		{
		TAny* terminalRspInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSatTerminalRsp::KLtsyDispatchSatTerminalRspApiId,
		       	terminalRspInterface);
		iLtsyDispatchSatTerminalRsp =
				static_cast<MLtsyDispatchSatTerminalRsp*>(terminalRspInterface);
        __ASSERT_DEBUG(iLtsyDispatchSatTerminalRsp, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSatFuncUnitId, MLtsyDispatchSatMenuSelectionEnvelope::KLtsyDispatchSatMenuSelectionEnvelopeApiId))
		{
		TAny* menuSelectionEnvelopeInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSatMenuSelectionEnvelope::KLtsyDispatchSatMenuSelectionEnvelopeApiId,
		       	menuSelectionEnvelopeInterface);
		iLtsyDispatchSatMenuSelection =
				static_cast<MLtsyDispatchSatMenuSelectionEnvelope*>(menuSelectionEnvelopeInterface);
        __ASSERT_DEBUG(iLtsyDispatchSatMenuSelection, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSatFuncUnitId, MLtsyDispatchSatEventDownloadEnvelope::KLtsyDispatchSatEventDownloadEnvelopeApiId))
		{
		TAny* eventDownloadEnvelopeInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSatEventDownloadEnvelope::KLtsyDispatchSatEventDownloadEnvelopeApiId,
		       	eventDownloadEnvelopeInterface);
		iLtsyDispatchSatEventDownload =
				static_cast<MLtsyDispatchSatEventDownloadEnvelope*>(eventDownloadEnvelopeInterface);
        __ASSERT_DEBUG(iLtsyDispatchSatEventDownload, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSatFuncUnitId, MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId))
		{
		TAny* GetUssdControlSupportedInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
				MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId,
		       	GetUssdControlSupportedInterface);
		iLtsyDispatchSatGetUssdControlSupported =
				static_cast<MLtsyDispatchSatGetUssdControlSupported*>(GetUssdControlSupportedInterface);
        __ASSERT_DEBUG(iLtsyDispatchSatGetUssdControlSupported, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	
	} // CSatDispatcher::ConstructL

void CSatDispatcher::SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder)
/**
 * Set the dispatcher holder.  
 * 
 * @param aDispatcherHolder Reference to dispatcher holder.
 */
	{
	TSYLOGENTRYEXIT;
	
	iDispatcherHolder = &aDispatcherHolder;
	} // CSatDispatcher::SetDispatcherHolder

/**
 * Allows the CTSY Dispatcher to ascertain
 * whether a call was requested by a Proactive Command.
 *          
 * @param aSatCallOrigin Interface to the cstktsy
 */

void CSatDispatcher::SetSatCallOriginInterface(MStkTsyCallOrigin& aSatCallOrigin)
	{
	TSYLOGENTRYEXIT;
	iStkTsyCallOrigin = &aSatCallOrigin;
	}

TBool CSatDispatcher::IsSimOriginatedCall(const RMobilePhone::TMobileAddress& aDialledParty)
/**
 * Checks if the dialled party details passed into this function orignally came 
 * from the SIM. e.g. From a SET UP CALL proactive command. 
 * 
 * @param aDialledParty The dialled party details.
 * @return ETrue if the details did originally come from the SIM, EFalse otherwise.
 */
	{
	TSYLOGENTRYEXIT;	
	TInt ret = EFalse;

	 if (iStkTsyCallOrigin)
		{
		//convert 16-bit ascii to 8-bit ascii.
		TBuf8<RMobilePhone::KMaxMobileTelNumberSize> address;
		address.Copy(aDialledParty.iTelNumber);
		ret = iStkTsyCallOrigin->IsSimOriginatedCall(address);
		}
	return ret;
	} // CSatDispatcher::IsSimOriginatedCall


void CSatDispatcher::CompleteNotifyLocalInfo(TInt aError)
	{
	//send off the in progress terminal response now that we have collected the required information for it.
	if (iLocalInfoOngoing)
		{
		TTerminalRespData resp;
		if (KErrNone == aError)
			{
			resp.iGeneralResult = RSat::KSuccess;
			resp.iAdditionalInfo.Zero();
			}
		else
			{
			resp.iGeneralResult = RSat::KMeUnableToProcessCmd;
			resp.iAdditionalInfo.Zero();
			resp.iAdditionalInfo.Append( RSat::KNoSpecificMeProblem );
			}
		CMmDataPackage dataPackage;
		dataPackage.PackData(&resp);
		iMessageManagerCallback.Complete(ESatTsyNotifyLocalInfo, &dataPackage, aError);
		
		iLocalInfoOngoing = EFalse;
		}
	} //CSatDispatcher::CompleteNotifyLocalInfo


TInt CSatDispatcher::DispatchSmsDeliverReportL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ESatTsySmsDeliverReport
 * and pass request on to Licensee LTSY.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

    TBuf8<KTpduMaxSize>* reportData = NULL;
	aDataPackage->UnPackData(&reportData);

	__ASSERT_DEBUG(reportData, CtsyDispatcherPanic(EInvalidNullPtr));
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSatSmsDeliverReport)
		{
		ret = iLtsyDispatchSatSmsDeliverReport->HandleSmsDeliverReportReqL(*reportData);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSatDispatcher::DispatchSmsDeliverReportL

TInt CSatDispatcher::DispatchGetImageInstanceL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ESatTsyGetImageInstance
 * and pass request on to Licensee LTSY.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	TBuf8<KGetIconDataPckgLength> data;
	aDataPackage->UnPackData(data);

	__ASSERT_DEBUG(data.Length() == KGetIconDataPckgLength, CtsyDispatcherPanic(EBadLength));
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSatGetImageInstance)
		{
		//data[0] Record number
		//data[1] Image instance number in the record
		ret = iLtsyDispatchSatGetImageInstance->HandleGetImageInstanceReqL(data[0],data[1]);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSatDispatcher::DispatchGetImageInstanceL


TInt CSatDispatcher::DispatchGetIconDataL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ESatTsyGetIconData
 * and pass request on to Licensee LTSY.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	
	TIconInfo* iconInfo = NULL;
	aDataPackage->UnPackData(&iconInfo);
	
	__ASSERT_DEBUG(iconInfo, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSatGetIconData)
		{
		const TInt KBitsToShiftForDivisonBy256 = 8;
		// iconInfo.iOffset >> 8 undoes the conversion from the record number
		// to an offset done in the CTSY.
		ret = iLtsyDispatchSatGetIconData->HandleGetIconDataReqL(iconInfo->iOffset >> KBitsToShiftForDivisonBy256,iconInfo->iSimFilePath);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSatDispatcher::DispatchGetIconDataL

TInt CSatDispatcher::DispatchGetClutL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ESatTsyGetClut
 * and pass request on to Licensee LTSY.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	TBuf8<KGetIconDataPckgLength> data;
	aDataPackage->UnPackData(data); 

	__ASSERT_DEBUG(data.Length() == KGetIconDataPckgLength, CtsyDispatcherPanic(EBadLength));
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSatGetClut)
		{
		//data[0] Record number
		//data[1] Instance number in the record
		ret = iLtsyDispatchSatGetClut->HandleGetClutReqL(data[0], data[1]);
		}
	
	return TSYLOGSETEXITERR(ret);
	} // CSatDispatcher::DispatchGetClutL

TInt CSatDispatcher::DispatchGetDefaultBearerCapabilityL()
/**
 * Unpack data related to ESatTsyGetBearerCapability
 * and pass request on to Licensee LTSY.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSatGetDefaultBearerCapability)
		{
		ret = iLtsyDispatchSatGetDefaultBearerCapability->HandleGetDefaultBearerCapabilityReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSatDispatcher::DispatchGetDefaultBearerCapabilityL

TInt CSatDispatcher::DispatchSmsPpDdlStatusL()
/**
 * Unpack data related to ESatTsySmsPpDdlStatus
 * and pass request on to Licensee LTSY.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSatGetSmsPpDownloadSupported)
		{
		ret = iLtsyDispatchSatGetSmsPpDownloadSupported->HandleGetSmsPpDownloadSupportedReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSatDispatcher::DispatchSmsPpDdlStatusL

TInt CSatDispatcher::DispatchGetSmsControlActivatedL()
/**
 * Unpack data related to ESatTsyMoSmsControlActivation
 * and pass request on to Licensee LTSY.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSatGetSmsControlActivated)
		{
		ret = iLtsyDispatchSatGetSmsControlActivated->HandleGetSmsControlActivatedReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSatDispatcher::DispatchGetSmsControlActivatedL

TInt CSatDispatcher::DispatchCellBroadcastL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ESatTsyCellBroadcast
 * and pass request on to Licensee LTSY.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	
	TPtrC8* cellBroadcastEnvelope = NULL;
	
	aDataPackage->UnPackData(&cellBroadcastEnvelope);
	
	__ASSERT_DEBUG(cellBroadcastEnvelope, CtsyDispatcherPanic(EInvalidNullPtr));
	
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSatCellBroadcast)
		{
		ret = iLtsyDispatchSatCellBroadcast->HandleCellBroadcastEnvelopeReqL(*cellBroadcastEnvelope);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSatDispatcher::DispatchCellBroadcastL

TInt CSatDispatcher::DispatchAccessTechnologyL()
/**
 * Unpack data related to ESatTsyAccTech
 * and pass request on to Licensee LTSY.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	//record that an Local Information proactive command request is in progress
	iLocalInfoOngoing = ETrue;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSatAccTech)
		{
		ret = iLtsyDispatchSatAccTech->HandleGetAccessTechnologyReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSatDispatcher::DispatchAccessTechnologyL


TInt CSatDispatcher::DispatchTimingAdvanceL()
/**
 * Unpack data related to ESatTsyTimingAdvance
 * and pass request on to Licensee LTSY.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	//record that a Local Information proactive command request is in progress
	iLocalInfoOngoing = ETrue;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSatTimingAdvance)
		{
		ret = iLtsyDispatchSatTimingAdvance->HandleTimingAdvanceReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSatDispatcher::DispatchTimingAdvanceL

TInt CSatDispatcher::DispatchNotifySmControlRequestL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ESatNotifyMoSmControlRequest
 * and pass request on to Licensee LTSY.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	TMoSmCtrlData* moSmsControlData = NULL;
	aDataPackage->UnPackData(&moSmsControlData);
	
	__ASSERT_DEBUG(moSmsControlData, CtsyDispatcherPanic(EInvalidNullPtr));
	
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSatSmControlResponseData)
		{
		ret = iLtsyDispatchSatSmControlResponseData->HandleSmControlResponseDataReqL(	
				moSmsControlData->iResult,
				moSmsControlData->iAddressData.iRPDestinationAddr,
				moSmsControlData->iAddressData.iTPDestinationAddr);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSatDispatcher::DispatchNotifySmControlRequestL

TInt CSatDispatcher::DispatchProvideLocationInfoL()
/**
 * Pass ESatTsyProvideLocalInfo request on to Licensee LTSY.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	
	//record that a Local Information proactive command request is in progress
	iLocalInfoOngoing = ETrue;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSatProvideLocationInfo)
		{
		ret = iLtsyDispatchSatProvideLocationInfo->HandleProvideLocationInfoReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSatDispatcher::DispatchProvideLocationInfoL

TInt CSatDispatcher::DispatchSetPollingIntervalL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ESatTsySetPolling
 * and pass request on to Licensee LTSY.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	TBuf8<1> data;
	aDataPackage->UnPackData(data);

	__ASSERT_DEBUG(data.Length() == data.MaxLength(), CtsyDispatcherPanic(EInvalidNullPtr));
	
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSatSetPollingInterval)
		{
		ret = iLtsyDispatchSatSetPollingInterval->HandleSetPollingIntervalReqL(data[0]);
		}
	
	return TSYLOGSETEXITERR(ret);
	} // CSatDispatcher::DispatchSetPollingIntervalL

TInt CSatDispatcher::DispatchSmsPpDdlL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ESatTsySmsPpDdl
 * and pass request on to Licensee LTSY.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	TPtrC8* smsPpDownloadEnvelope = NULL;
	
	aDataPackage->UnPackData(&smsPpDownloadEnvelope);
	
	__ASSERT_DEBUG(smsPpDownloadEnvelope, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSatSmsPpDownloadEnvelope)
		{
		ret = iLtsyDispatchSatSmsPpDownloadEnvelope->HandleSmsPpDownloadEnvelopeReqL(*smsPpDownloadEnvelope);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSatDispatcher::DispatchSmsPpDdlL

TInt CSatDispatcher::DispatchLocalInformationNmrL()
/**
 * Unpack data related to ESatTsyLocalInformationNmr
 * and pass request on to Licensee LTSY.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	//record that a Local Information proactive command request is in progress
	iLocalInfoOngoing = ETrue;
	
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSatLocalInformationNmr)
		{
		ret = iLtsyDispatchSatLocalInformationNmr->HandleLocalInformationNmrReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSatDispatcher::DispatchLocalInformationNmrL

TInt CSatDispatcher::DispatchSendEnvelopeL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ESatTsySendEnvelope
 * and pass request on to Licensee LTSY.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	TPtrC8* envelope = NULL;
	
	aDataPackage->UnPackData(&envelope);
	
	__ASSERT_DEBUG(envelope, CtsyDispatcherPanic(EInvalidNullPtr));
	
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSatSendEnvelope)
		{
		ret = iLtsyDispatchSatSendEnvelope->HandleCallAndSmsControlEnvelopeReqL(*envelope);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSatDispatcher::DispatchSendEnvelopeL

TInt CSatDispatcher::DispatchServiceRequestL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ESatTsyServiceRequest
 * and pass request on to Licensee LTSY.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	TTerminalRespData* terminalRespData = NULL;
	TRefreshInfo* refreshInfo = NULL;
	
	aDataPackage->UnPackData(&terminalRespData,&refreshInfo);

	__ASSERT_DEBUG(terminalRespData && refreshInfo, CtsyDispatcherPanic(EInvalidNullPtr));
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSatRefreshAllowed)
		{
		ret = iLtsyDispatchSatRefreshAllowed->HandleRefreshAllowedReqL(refreshInfo->iFileList,
																		refreshInfo->iAid,
																		refreshInfo->iInternalCache);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSatDispatcher::DispatchServiceRequestL

TInt CSatDispatcher::DispatchReadyL()
/**
 * Unpack data related to ESatTsyReady
 * and pass request on to Licensee LTSY.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSatReady)
		{
		ret = iLtsyDispatchSatReady->HandleReadyReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSatDispatcher::DispatchReadyL

TInt CSatDispatcher::DispatchPCmdNotificationL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ESatTsyGetIfPendingNotification
 * and pass request on to Licensee LTSY.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	TUint8 commandtype = 0;
	aDataPackage->UnPackData(commandtype);

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSatPCmdNotification)
		{
		ret = iLtsyDispatchSatPCmdNotification->HandlePCmdNotificationReqL(commandtype);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSatDispatcher::DispatchPCmdNotificationL

TInt CSatDispatcher::DispatchUssdControlEnvelopeErrorL()
/**
 * Unpack data related to ESatTsyEnvelopeError
 * and pass request on to Licensee LTSY.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSatUssdControlEnvelopeError)
		{
		ret = iLtsyDispatchSatUssdControlEnvelopeError->HandleUssdControlEnvelopeErrorReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSatDispatcher::DispatchUssdControlEnvelopeErrorL

TInt CSatDispatcher::DispatchTimerExpiredL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ESatTsyTimerExpiredIPC
 * and pass request on to Licensee LTSY.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	TPtrC8* timerExpirationEnvelope = NULL;
	aDataPackage->UnPackData(&timerExpirationEnvelope);
	__ASSERT_DEBUG(timerExpirationEnvelope, CtsyDispatcherPanic(EInvalidNullPtr));
	
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSatTimerExpired) 
		{
		ret = iLtsyDispatchSatTimerExpired->HandleTimerExpirationEnvelopeReqL(*timerExpirationEnvelope);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSatDispatcher::DispatchTimerExpiredL

TInt CSatDispatcher::DispatchTerminalRspL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ESatTerminalRsp
 * and pass request on to Licensee LTSY.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	TTerminalRespData* response = NULL;
	TPtrC8* resultAndAddInfoTlv = NULL;
	aDataPackage->UnPackData(&response, &resultAndAddInfoTlv);
	
	__ASSERT_DEBUG(response && resultAndAddInfoTlv, CtsyDispatcherPanic(EInvalidNullPtr));
	
	if (iLtsyDispatchSatTerminalRsp)
		{
		RBuf8 terminalRsp;
		CleanupClosePushL(terminalRsp);
		
		const TInt KDeviceIdentityLength = 4;
		terminalRsp.CreateL((response->iCommandDetails).Length() + KDeviceIdentityLength + resultAndAddInfoTlv->Length());
		
		//command details
		terminalRsp.Append(response->iCommandDetails);
		
		//device identity
		const TInt KDeviceIdentityDataLength = 2;
		terminalRsp.Append(KTlvDeviceIdentityTag);
		terminalRsp.Append(KDeviceIdentityDataLength);
		terminalRsp.Append(KMe);
		terminalRsp.Append(KSim);

		//result + additional data
		terminalRsp.Append(*resultAndAddInfoTlv);
		
		__ASSERT_DEBUG(terminalRsp.Length() <= KTlvMaxSize, CtsyDispatcherPanic(EBadLength));
		__ASSERT_DEBUG(terminalRsp.Length() == terminalRsp.MaxLength(), CtsyDispatcherPanic(EBadLength));
		
		ret = iLtsyDispatchSatTerminalRsp->HandleTerminalRspReqL(terminalRsp);
		
		CleanupStack::PopAndDestroy(&terminalRsp);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSatDispatcher::DispatchTerminalRspL

TInt CSatDispatcher::DispatchMenuSelectionL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ESatMenuSelection
 * and pass request on to Licensee LTSY.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	TPtrC8* envelope = NULL;
	aDataPackage->UnPackData(&envelope);
	__ASSERT_DEBUG(envelope, CtsyDispatcherPanic(EInvalidNullPtr));
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSatMenuSelection)
		{
		ret = iLtsyDispatchSatMenuSelection->HandleMenuSelectionEnvelopeReqL(*envelope);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSatDispatcher::DispatchMenuSelectionL

TInt CSatDispatcher::DispatchEventDownloadL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ESatEventDownload
 * and pass request on to Licensee LTSY.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	TPtrC8* envelope = NULL;
	aDataPackage->UnPackData(&envelope);
	
	__ASSERT_DEBUG(envelope, CtsyDispatcherPanic(EInvalidNullPtr));
	
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSatEventDownload) 
		{
		ret = iLtsyDispatchSatEventDownload->HandleEventDownloadEnvelopeReqL(*envelope);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSatDispatcher::DispatchEventDownloadL

TInt CSatDispatcher::DispatchGetUssdControlSupportedL()
/**
 * Unpack data related to ESatTsyUssdControlSupported
 * and pass request on to Licensee LTSY.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSatGetUssdControlSupported)
		{
		ret = iLtsyDispatchSatGetUssdControlSupported->HandleGetUssdControlSupportedReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSatDispatcher::DispatchEventDownloadL


//
// Callback handlers follow
//



void CSatDispatcher::CallbackUssdControlSupported(TInt aError, TBool aSupported)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackSatGetUssdControlSupportedComp().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aSupported whether or not USSD-TLV is supported
 *  in the (u)sim service table
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aSupported=%d"), aError, aSupported);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aSupported);
	
	iMessageManagerCallback.Complete(ESatTsyUssdStatus, &dataPackage, aError);
	
	} // CSatDispatcher::CallbackUssdControlSupported

void CSatDispatcher::CallbackPcmd(TInt aError, const TDesC8& aData)
/**
 * Callback function to be used by the request to complete.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aData TLV data received from the SIM that contains the proactive command.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	__DEBUG_ONLY(const TUint8 KCommandUiccTagIndex  = 0;)
	__ASSERT_DEBUG(aData[KCommandUiccTagIndex] == KBerTlvProactiveSimCommandTag, CtsyDispatcherPanic(ECorruptTlvData));

	TPtrC8 dataPtr(aData);
	
	// Pack the data to return to the Common TSY
    CMmDataPackage dataPackage;
	dataPackage.PackData(&dataPtr);
	
	iMessageManagerCallback.Complete(ESatTsyPCmd, &dataPackage, aError);
	
	} // CSatDispatcher::CallbackPcmd

void CSatDispatcher::CallbackNotifyProactiveSimSessionEnd(TInt aError,TUint8 aStatusWord1, TUint8 aStatusWord2)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackSatNotifyProactiveSimSessionEndInd().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aStatusWord1 The Status Word 1
 * @param aStatusWord2 The Status Word 2
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aStatusWord1=%d,aStatusWord2=%d"), aError, aStatusWord1, aStatusWord2);

	CMmDataPackage dataPackage;
	dataPackage.PackData(&aStatusWord1,&aStatusWord2);
	
	iMessageManagerCallback.Complete(ESatNotifyProactiveSimSessionEnd, &dataPackage, aError);
	
	} // CSatDispatcher::CallbackNotifyProactiveSimSessionEnd

void CSatDispatcher::CallbackTimeZoneChange(TInt aError, TUint8 aTimeZone)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackSatTimeZoneChangeInd().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aTimeZone The new time zone coded as defined in "ETSI TS 123 040" "TP-Service-Centre-Time-Stamp" 
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aTimeZone=%d"), aError, aTimeZone);

	CMmDataPackage dataPackage;
	dataPackage.PackData(&aTimeZone);

	iMessageManagerCallback.Complete(ESatTsyTimeZone, &dataPackage, aError);
	
	} // CSatDispatcher::CallbackTimeZoneChange

void CSatDispatcher::CallbackAccessTechnologyChange(TInt aError, TUint8 aCurrentAccessTechnology)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackSatAccessTechnologyChangeInd().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCurrentAccessTechnology The current access technology being used by the terminal
 * See "ETSI TS 102 223" "Access technology"
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCurrentAccessTechnology=%d"), aError, aCurrentAccessTechnology);

	CMmDataPackage dataPackage;
	dataPackage.PackData(&aCurrentAccessTechnology);
	
	iMessageManagerCallback.Complete(ESatTsyAccessTechnologyChange, &dataPackage, aError);
	} // CSatDispatcher::CallbackAccessTechnologyChange

void CSatDispatcher::CallbackCallConnected(TInt aError, TUint8 aTransactionId, TBool aNearEnd)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackSatCallConnectedInd().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aTransactionId The Transaction Identifier in the call connect message.
 * @param aIsNearEnd If True, the case of connecting is at the near end (an MT call), (i.e. source = terminal, destination = UICC)
 * 					 If False (i.e. far end), the case of connecting is at the far end (an MO call), (i.e. source = network, destination = UICC)
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aTransactionId=%d, aNearEnd=%d"),aError,aTransactionId,aNearEnd);

	CMmDataPackage dataPackage;
	dataPackage.PackData(&aTransactionId,&aNearEnd);
	
	iMessageManagerCallback.Complete(ESatTsyCallConnected, &dataPackage, aError);
	
	} // CSatDispatcher::CallbackCallConnected

void CSatDispatcher::CallbackSsChange(TInt aError, DispatcherSat::TSsStatus aStatus)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackSatSsChangeInd().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aStatus whether SS or USSD is busy
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aStatus=%d"), aError, aStatus);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	
	//switch here to convert from DispatcherSat::TSsStatus to TSsStatus
	//could static cast from DispatcherSat::TSsStatus to TSsStatus be would
	//need to ensure DispatcherSat::TSsStatus enums always matchs TSsStatus enums.
	TSsStatus status;
	
	switch(aStatus)
		{
		case DispatcherSat::ENotBusy:
			status = ENotBusy;
			break;
		case DispatcherSat::ESsBusy:
			status = ESsBusy;
			break;
		case DispatcherSat::EUssdBusy:
			status = EUssdBusy;
			break;
		}
	
	dataPackage.PackData(&status);
	
	iMessageManagerCallback.Complete(ESatTsySsStatus, &dataPackage, aError);
	
	} // CSatDispatcher::CallbackSsChange

void CSatDispatcher::CallbackCallDisconnected(TInt aError, TUint8 aTransactionId, TBool aNearEnd, const TDesC8& aCause)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackSatCallDisconnectedInd().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aTransactionId The Transaction Identifier of the call being disconnected.
 * @param aIsNearEnd If True, the case of disconnection is at the near end, (i.e. source = terminal, destination = UICC)
 * 					If False (i.e. far end), the case of disconnection is at the far end, (i.e. source = network, destination = UICC)
 * @param aCause 	The cause value is defined in the appropriate access technology specification. 
 * 					Radio Link Timeout is indicated by the cause having zero length, the maximum length is defined in KMaxCauseLength
 * 					See "ETSI TS 102 223" "COMPREHENSION-TLV data objects" "Cause"
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aTransactionId=%d,aNearEnd=%d"), aError,aTransactionId,aNearEnd);

	__ASSERT_DEBUG(aCause.Length() <= KMaxCauseLength, CtsyDispatcherPanic(EBadLength));
	
	TCallDisconnectedEnvelope callDisconnectedEnvelope;
	callDisconnectedEnvelope.iCmdNumber  = aTransactionId;
	callDisconnectedEnvelope.iNearEnd	 = aNearEnd;
	callDisconnectedEnvelope.iCauseGiven = (aCause.Length() != 0);
	callDisconnectedEnvelope.iCause		 = aCause;
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&callDisconnectedEnvelope);
	iMessageManagerCallback.Complete(ESatTsyCallDisconnected, &dataPackage, aError);
	
	} // CSatDispatcher::CallbackCallDisconnected

void CSatDispatcher::CallbackMtCall(
		TInt aError, TUint8 aTransactionId, const TDesC8& aAddress, const TDesC8& aSubAddress)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSatMtCallInd().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aTransactionId The Transaction Identifier in the call setup message from the network.
 * @param aAddress Holds the Calling Party number received by the terminal in the call setup message. If
 * the Calling Party number is included in the call setup message, the Address shall be included, otherwise
 * the address shall not be included  (zero length). See "ETSI TS 102 223" "COMPREHENSION-TLV data objects" "Address" 
 * @param aSubAddress Holds the Calling Party Subaddress as received by the terminal in the call setup
 * message. If the Calling Party Subaddress is included in the call setup message, the Subaddress shall be included, otherwise 
 * the Subaddress shall not be included (zero length). See "ETSI TS 102 223" "COMPREHENSION-TLV data objects" "Subaddress"
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aTransactionId=%d"),aError,aTransactionId);

	const TInt KTonAndNpiIndex = 0;
	const TInt KTonAndNpiLength = 1;
	
	TMTCallEnvelope mtCallEnvelope;
	
	mtCallEnvelope.iCmdNumber = aTransactionId;
	
	//according to TS 24.008. If the Dialling Number/SSC String does not contain a dialling number, e.g. a control
	//string deactivating a service, the TON/NPI byte shall be set to 'FF' by the ME, but..
	//the CTSY expects it to be zero to indicate it is missing

	// chop so that it fits within the buffer (if required)
	TPtrC8 truncAddress = aAddress.Left(KNumberMaxLength + KTonAndNpiLength);
	mtCallEnvelope.iTONandNPI = (truncAddress.Length() >= KTonAndNpiLength) ? truncAddress[KTonAndNpiIndex] : 0;

	// extract the dialling number (address minus the Ton & Npi)
	mtCallEnvelope.iDiallingNumber = truncAddress.Mid(KTonAndNpiLength);
		
	mtCallEnvelope.iCalledPartySubAddress.Set(aSubAddress);
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&mtCallEnvelope);
	
	iMessageManagerCallback.Complete(ESatTsyMTCall, &dataPackage, aError);
	
	} // CSatDispatcher::CallbackMtCall

void CSatDispatcher::CallbackCallControlEnvelopeResponse(TInt aError, const TDesC8& aResponseTlv)
/**
 * Callback function to be used by the request to allow the RSat client to display any alpha identifiers
 * resulting from call control.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aResponseTlv the response from NAA to the ENVELOPE (CALL CONTROL) command. This will be parsed by the CTSY
 * and if an alpha identifier exists in the data, it will be passed to the RSat client (if any) for display to the 
 * user. As per ETSI 102.223 section 7.3.1.6, this contains 1 byte containing the Call Control result, followed by a 
 * 1-2 byte length, then various optional fields depending on the outcome of the Call/SS/USSD Control. If the NAA
 * application does not provide any response data then this is interpreted as "allowed, no modification".
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	TUint8 callControlResult = 0; 
	if (aResponseTlv.Length() > 0) //no response data is interpreted as allowed no modification
		{
		callControlResult = aResponseTlv[0];
		}
	
	RSat::TControlResult res;
	switch (callControlResult)
		{
		case 2:
			{	
			res = RSat::EAllowedWithModifications;
			}
			break;
		case 1:
			{
			res = RSat::ENotAllowed;
			}
			break;
		case 0:
		default: //default case is invalid
			{
			res = RSat::EAllowedNoModification;
			}
			break;
		}
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&const_cast<TDesC8&>(aResponseTlv), &res);
	    
	iMessageManagerCallback.Complete(ESatTsyCheckAtkDataForAlphaId, &dataPackage, aError);
	
	} // CSatDispatcher::CallbackCallControlEnvelopeResponse

void CSatDispatcher::CallbackCreateCallControlEnvelope(TInt aError, TUint8 aTag, const TDesC8& aBcc1, const TDesC8& aBcc2, 
			const TDesC8& aAddress, TUint8 aTonNpi, TUint8 aDcs)
/**
 * Callback function to be used by the request to complete.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCallControlInfo details of the Call/SS/USSD request that is being passed to UICC for possible Control
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	__ASSERT_DEBUG(aBcc1.Length() < KMaxBccLength, CtsyDispatcherPanic(EBadLength));
	__ASSERT_DEBUG(aBcc2.Length() < KMaxBccLength, CtsyDispatcherPanic(EBadLength));
	__ASSERT_DEBUG(aAddress.Length() < KMaxAddressLength, CtsyDispatcherPanic(EBadLength));
	
	TCCInfo ccInfo;
	ccInfo.iTag = aTag;
	ccInfo.iBcc1 = aBcc1;
	ccInfo.iBcc2 = aBcc2;
	ccInfo.iAddress = aAddress;
	ccInfo.iTonNpi = aTonNpi;
	ccInfo.iDcs = aDcs;

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&ccInfo);
	
	iMessageManagerCallback.Complete(ESatTsyCreateEnvelope, &dataPackage, aError);
	
	} // CSatDispatcher::CallbackCreateCallControlEnvelope

void CSatDispatcher::CallbackCreateSmControlEnvelope(TInt aError, const TDesC8& aRpDestinationAddress, const TDesC8& aTpDestinationAddress)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSatCreateSmControlEnvelopeInd().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aRpDestinationAddress RP Destination Address of the Service Center to which the ME is proposing to send the short message.
 * @param aTpDestinationAddress TP Destination Address to which the ME is proposing to send the short message.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	TAddressData addressData;
	addressData.iRPDestinationAddr.Set(aRpDestinationAddress);
	addressData.iTPDestinationAddr.Set(aTpDestinationAddress);
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&addressData);
	
	iMessageManagerCallback.Complete(ESatTsyMoSmControlEnvelope, &dataPackage, aError);
	} // CSatDispatcher::CallbackCreateSmControlEnvelope

void CSatDispatcher::CallbackImsiChanged(TInt aError)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSatImsiChangedInd().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(ESatTsyImsiChanged, aError);
	
	} // CSatDispatcher::CallbackImsiChanged

void CSatDispatcher::CallbackLocationStatus(TInt aError, TUint8 aLcnStatus, 
		const TDesC8& aCountryCode, TUint16 aLcnAreaCode, TUint16 aCellId)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSatLocationStatusInd().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aLcnStatus the current service state of the terminal - 
 * 			one of the values defined in ETSI 102.223 section 8.27
 * @param aCountryCode 3 digit string (numerical) uniquely identifying the country. The length of 
 * this descriptor should be 3.
 * @param aLcnAreaCode The Location Area Code uniquely identifies a Location Area 
 * 			with a network. 
 * @param aCellId The current cell.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	__ASSERT_DEBUG(aCountryCode.Length() <= KCountryCodeLength, CtsyDispatcherPanic(EBadLength));
	
	// Pack the data to return to the Common TSY to construct possible EVENT DOWNLOAD envelope 
	TLocationStatusEnvelope lcnStatEnv;
	lcnStatEnv.iLocationStatus = aLcnStatus;
	lcnStatEnv.iCountryCode = aCountryCode;
	lcnStatEnv.iLocationAreaCode = aLcnAreaCode;
	lcnStatEnv.iCellId = aCellId;
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&lcnStatEnv);
	
	iMessageManagerCallback.Complete(ESatTsyLocationStatus, &dataPackage, aError);
	
	//Pack the data to return to the Common TSY to cache for use in Call/SS/USSD control envelopes
	TLocalInformation localInfo;
	localInfo.iLocationAreaCode = aLcnAreaCode;
	localInfo.iCellId = aCellId;
	localInfo.iOperatorCode = aCountryCode;
	
	dataPackage.PackData(&localInfo);
	
	iMessageManagerCallback.Complete(ESatTsyProvideLocalInfo, &dataPackage, aError);
	
	} // CSatDispatcher::CallbackLocationStatus

void CSatDispatcher::CallbackRemoveEventList(TInt aError)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSatRemoveEventListInd().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(ESatTsyRemoveEventList, aError);
	
	} // CSatDispatcher::CallbackRemoveEventList

void CSatDispatcher::CallbackCreateRefreshTerminalRsp(TInt aError, RSat::TPCmdResult aGeneralResult, const TDesC& aResultAdditionalInfo)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSatCreateRefreshTerminalRspInd().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aGeneralResult The General result that specifies the result and indicates appropriate UICC action (to be included in the 
 * refresh terminal response result section, see ETSI 102 223 Under "Structure of TERMINAL RESPONSE" and Under "Result")
 * @param aResultAdditionalInfo Additional information (to be included in the refresh terminal response result section, 
 * see ETSI 102 223 Under "Structure of TERMINAL RESPONSE" and Under "Result")
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aGeneralResult=%d"), aError, aGeneralResult);

	__ASSERT_DEBUG(aResultAdditionalInfo.Length() <= RSat::KAdditionalInfoMaxSize, CtsyDispatcherPanic(EBadLength));
	
	TTerminalRespData terminalRespData;
	terminalRespData.iGeneralResult = aGeneralResult;
	terminalRespData.iAdditionalInfo = aResultAdditionalInfo;	

	CMmDataPackage dataPackage;
	dataPackage.PackData(&terminalRespData);
	
	iMessageManagerCallback.Complete(ESatTsyRefresh, &dataPackage, aError);
	
	} // CSatDispatcher::CallbackCreateRefreshTerminalRsp

void CSatDispatcher::CallbackSmControlEnvelopeResponse(TInt aError, const TDesC8& aResponseTlv)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSatSmControlEnvelopeResponseInd().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aResponseTlv Response data from the SIM encoded as defined in "3GPP 11.14 - MO Short Message Control by SIM"
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&const_cast<TDesC8&>(aResponseTlv));
	
	iMessageManagerCallback.Complete(ESatNotifyMoSmControlRequest, &dataPackage, aError);

	} // CSatDispatcher::CallbackSmControlEnvelopeResponse

void CSatDispatcher::CallbackSatCreateSmsPpDownloadEnvelope(TInt aError, const TDesC8& aRpOriginatingAddress, const TDesC8& aSmsDeliveryTpdu)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSatCreateSmsPpDownloadEnvelopeInd().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aRpOriginatingAddress RP Originating Address of the Service Centre (TS Service-Centre-Address), as defined in 3GPP TS 24.011)
 * @param aSmsDeliveryTpdu The SMS-DELIVER TPDU (See "3GPP 23.040" under "SMS DELIVER type")
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	__ASSERT_DEBUG(aRpOriginatingAddress.Length() + 1 <= KAddrMaxLength, CtsyDispatcherPanic(EBadLength));
	__ASSERT_DEBUG(aSmsDeliveryTpdu.Length() <= RMobileSmsMessaging::KGsmTpduSize, CtsyDispatcherPanic(EBadLength));
	
	//The SIM ATK CTSY takes an Rp Address with the address length prefixed to the adress (this is 
	//eventually calculated when constructing the TLV envelope and not used when SmsPp Download
	//is supported, but used when construdting the Ef(Sms) (SmsPP Download not supported))
	//The code below takes the address and process it into the format the SIM ATK CTSY expects

	TBuf8<KAddrMaxLength> address;
	
	//chop to the the max buffer size (-1 for the length)
	TPtrC8 trucRpOriginatingAddress = aRpOriginatingAddress.Left(KAddrMaxLength - 1);
	address.Append(trucRpOriginatingAddress.Length());
	address.Append(trucRpOriginatingAddress);
		
	CMmDataPackage dataPackage;
	dataPackage.PackData(&address,&const_cast<TDesC8&>(aSmsDeliveryTpdu));
	
	//the error code is not currently used in the SIM ATK TSY
	iMessageManagerCallback.Complete(ESatTsySmsPpDdl, &dataPackage, aError);
	
	} // CSatDispatcher::CallbackSatCreateSmsPpDownloadEnvelope

void CSatDispatcher::CallbackCreateSmsDeliverReport(TInt aError)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSatCreateSmsDeliverReportInd().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(ESatTsySmsDeliverReport, aError);
	
	} // CSatDispatcher::CallbackCreateRefreshTerminalRsp

void CSatDispatcher::CallbackCreateCellBroadcastEnvelope(TInt aError, const TDesC8& aCellBroadcastPage)
/**
 * Callback function to be used by the request to complete CCCtsyDispatcherCallback::CallbackSatCreateCellBroadcastEnvelopeInd().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCellBroadcastPage The Cell Broadcast page, formatted in the same way as described in 3GPP TS 23.041
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	__ASSERT_DEBUG(aCellBroadcastPage.Length() <= KCbsMsgMaxLength, CtsyDispatcherPanic(EBadLength));

	//the SIM ATK CTSY unpacks to a TBuf8 (of the stack) of maxlength KCbsMsgMaxLength
	TPtrC8 cellBroadcastPage = aCellBroadcastPage.Left(KCbsMsgMaxLength);
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&cellBroadcastPage);
		
	iMessageManagerCallback.Complete(ESatTsyCellBroadcast, &dataPackage, aError);
	} // CSatDispatcher::CallbackCreateCellBroadcastEnvelope


void CSatDispatcher::CallbackGetImageInstance(TInt aError, 
		const TDesC8& aImageInstanceData, const TDesC8& aImageInstanceBody)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSatGetImageInstanceComp().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aImageInstanceData 	Descriptor containing the image instance coding scheme of the EF(IIDF),
 * 								this must be of length 2 (KBasicIconDataLength) for basic 
 * 								or length 6 (KColourIconDataLength) for colour image instances.
 * 								See ETSI TS 131 102 under "Annex B (normative) Image Coding Schemes (B.1 & B.2)"
 * @param aImageInstanceBody	Descriptor containing the image instance body of the EF(IIDF)
 * 								See ETSI TS 131 102 under "Annex B (normative) Image Coding Schemes"
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	switch(aImageInstanceData.Length())
		{
		case KBasicIconDataLength:
			{
			//copy aImageInstanceData to a KColourIconDataLength (6) byte descriptor, 
			//the CTSY expects a length of KColourIconDataLength even if only 2 bytes are used,
			//for a basic image.
			TBuf8<KColourIconDataLength> imageInstanceData(aImageInstanceData);
			imageInstanceData.SetMax();
			
			// Pack the data to return to the Common TSY
			CMmDataPackage dataPackage;
			dataPackage.PackData(&const_cast<TDesC8&>(aImageInstanceBody),&imageInstanceData);
			iMessageManagerCallback.Complete(ESatTsyGetImageInstance, &dataPackage, aError);			
			break;
			}
		
		case KColourIconDataLength:
			{
			// Pack the data to return to the Common TSY
			CMmDataPackage dataPackage;
			dataPackage.PackData(&const_cast<TDesC8&>(aImageInstanceBody),&const_cast<TDesC8&>(aImageInstanceData));
			iMessageManagerCallback.Complete(ESatTsyGetImageInstance, &dataPackage, aError);
			break;
			}
		
		default:
			{
			// The descriptor length should be either 2 for basic (KBasicIconDataLength)
			// or 6 for colour (KColourIconDataLength)
			__ASSERT_DEBUG(EFalse, CtsyDispatcherPanic(EBadLength));
			}
		}
	} // CSatDispatcher::CallbackGetImageInstance

void CSatDispatcher::CallbackGetIconData(TInt aError, const TDesC8& aIconEfImgRecord)
/**
 * Callback function to be used by the request to complete.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError 			The error code to be returned to the CTSY Dispatcher.
 * @param aIconEfImgRecord  The icon EF(Img) record retrieved from the UICC
 * 							See ETSI TS 131 102 under "Contents of files at the DFGRAPHICS level"
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Pack the data to return to the Common TSY
	CMmDataPackage iconEfImgRecordPkg;
	iconEfImgRecordPkg.PackData(&const_cast<TDesC8&>(aIconEfImgRecord));
	
	iMessageManagerCallback.Complete(ESatTsyGetIconData, &iconEfImgRecordPkg, aError);
	
	} // CSatDispatcher::CallbackGetIconData

void CSatDispatcher::CallbackGetClut(TInt aError, const TDesC8& aClut)
/**
 * Callback function to be used by the request to complete  CCtsyDispatcherCallback::CallbackSatGetClutComp().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError 	The error code to be returned to the CTSY Dispatcher.
 * @param aClut 	The requested CLUT (Colour look up table) for a given icon instance
 * 					See ETSI TS 131 102 under "B.2 Colour Image Coding Scheme"
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Pack the data to return to the Common TSY
	CMmDataPackage clutPkg;
	clutPkg.PackData(&const_cast<TDesC8&>(aClut));
	iMessageManagerCallback.Complete(ESatTsyGetClut, &clutPkg, aError);
	} // CSatDispatcher::CallbackGetClut

void CSatDispatcher::CallbackGetDefaultBearerCapability(TInt aError,
		const TDesC8& aCapability)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSatGetDefaultBearerCapabilityComp().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCapability The default bearer capability configuration (see 3GPP TS 04.08 for info about 
 * bearer capability), this may be used by the CTSY when creating a Call Control ENVELOPE if a zero 
 * length capability parameter is supplied with the Envelope creation request.  
 * Note the maximum length capability the CTSY can store is 248, and a maximum length of 16 will be 
 * used in the creation of the Call Control ENVELOPE.
 *
 * See "ETSI TS 102 223" "Structure of ENVELOPE (CALL CONTROL)" 
 * See "3GPP TS 04.08"
 * @see CSatDispatcher::CallbackSatCreateCallControlEnvelopeInd()
 * 
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	//debug assert if the capability is to long, otherwise in release mode
	//chop it down to the max buffer length to prevent the SIM ATK from panicing
	__ASSERT_DEBUG(aCapability.Length() <= KMaxLengthDefaultBearerData, CtsyDispatcherPanic(EBadLength));
	TPtrC8 beareraCapability(aCapability.Left(KMaxLengthDefaultBearerData));
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&beareraCapability);
	
	iMessageManagerCallback.Complete(ESatTsyGetBearerCapability, &dataPackage, aError);
	
	} // CSatDispatcher::CallbackGetDefaultBearerCapability

void CSatDispatcher::CallbackGetSmsPpDownloadSupported(TInt aError, TBool aSupported)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSatGetSmsPpDownloadSupportedComp().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aSupported If the service "data download via SMS Point-to-point" is allocated and activated in the SIM Service Table 
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aSupported=%d"), aError, aSupported);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aSupported);
	
	iMessageManagerCallback.Complete(ESatTsySmsPpDdlStatus, &dataPackage, aError);
	
	} // CSatDispatcher::CallbackGetSmsPpDownloadSupported

void CSatDispatcher::CallbackGetSmsControlActivated(TInt aError, TBool aActivated)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSatGetSmsControlActivatedComp().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aActivated True if MO-SMS Control by USIM (Service n°31) is currently activated (by the USIM),
 * False otherwise
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aActivated=%d"), aError, aActivated);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aActivated);	
	iMessageManagerCallback.Complete(ESatTsyMoSmsControlActivation, &dataPackage, aError);
	
	} // CSatDispatcher::CallbackGetSmsControlActivated

void CSatDispatcher::CallbackTimingAdvance(TInt aError,
		TUint8 aTimingAdvance, TUint8 aMeStatus)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSatTimingAdvanceComp().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aTimingAdvance a value relating to the distance between the ME and the base station. This is used
 * to compensate for propagation delay.
 * @param aMeStatus Whether or not the ME is idle...this is to allow the application to know whether the
 * timing advance is based on current or cached information.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aTimingAdvance=%d,aMeStatus=%d"), aError, aTimingAdvance, aMeStatus);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aTimingAdvance, &aMeStatus);
	
	iMessageManagerCallback.Complete(ESatTsyTimingAdvance, &dataPackage, aError);
	
	//send off the in progress terminal response now that we have collected the required information for it.
	CompleteNotifyLocalInfo(aError);

	
	} // CSatDispatcher::CallbackTimingAdvance

void CSatDispatcher::CallbackGetAccessTechnology(TInt aError, TUint8 aAccessTechnology)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSatAccessTechnologyChangeInd().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aAccessTechnology The current access technology being used by the terminal
 * See "ETSI TS 102 223" "Access technology"
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aAccessTechnology=%d"), aError, aAccessTechnology);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aAccessTechnology);
	
	iMessageManagerCallback.Complete(ESatTsyAccTech, &dataPackage, aError);
	
	//send off the in progress terminal response now that we have collected the required information for it.
	CompleteNotifyLocalInfo(aError);

	
} // CSatDispatcher::CallbackGetAccessTechnology

void CSatDispatcher::CallbackProvideLocationInfo(TInt aError, TUint16 aLocationAreaCode, 
		TUint16 aCellId, const TDesC8& aOperatorCode)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSatProvideLocationInfoComp().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aLocationAreaCode the ID of the Location Area (grouping of multiple cells)
 * that the mobile is in 
 * @param aCellId Id of the current GSM cell
 * @param aOperatorCode Id of the current operator. This has a maximum length of 3.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aLocationAreaCode=%d,aCellId=%d"), aError, aLocationAreaCode, aCellId);

	__ASSERT_DEBUG(aOperatorCode.Length() <= KOperatorCodeLength, CtsyDispatcherPanic(EBadLength));

	
	// Pack the data to return to the Common TSY
	TLocalInformation localInfo;
	localInfo.iLocationAreaCode = aLocationAreaCode;
	localInfo.iCellId = aCellId;
	localInfo.iOperatorCode = aOperatorCode;
	CMmDataPackage dataPackage;
	dataPackage.PackData(&localInfo);
	
	iMessageManagerCallback.Complete(ESatTsyProvideLocalInfo, &dataPackage, aError);
	
	//send off the in progress terminal response now that we have collected the required information for it.
	CompleteNotifyLocalInfo(aError);

	
	} // CSatDispatcher::CallbackProvideLocationInfo

void CSatDispatcher::CallbackSetPolling(TInt aError, TUint8 aPollingInterval)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSatSetPollingIntervalComp().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aPollingInterval The polling interval in use by the LTSY (zero) indicate Polling is off. This will be used
 * in the terminal response to the original Polling Interval / Polling Off proactive command.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aPollingInterval=%d"), aError, aPollingInterval);

	//this proactive command number will be used in the CTSY to fill in TTerminalRespData's iPCmdNumber field,
	//the CTSY expects the LTSY to supply this.
	//As now the terminal response structure is created from the iCommandDetails which contains the
	//proactive command number anyway the dispatcher no longer needs to hide this from teh interface
	//with a cache and can just send up any number
	TUint8 pcmdNumber = 0;

	CMmDataPackage dataPackage;
	dataPackage.PackData(&pcmdNumber,&aPollingInterval);
	
	iMessageManagerCallback.Complete(ESatTsySetPolling, &dataPackage, aError);

	} // CSatDispatcher::CallbackSetPolling


void CSatDispatcher::CallbackLocalInformationNmr(TInt aError,
			const TDesC8& aNmr, const TDesC& aBcchList)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSatLocalInformationNmrComp().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aNmr - Network Measurement Results - reception level of BCCH of adjacent cells  
 * @param aBcchList - Broadcast Control Channel List - the Broadcast Control Channel 
 * of neighbouring cells (in 16-bit format, will be converted to 10-bit in CTSY).
 * 
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	__ASSERT_DEBUG(aNmr.Length() <= KNmrLength, CtsyDispatcherPanic(EBadLength));
	__ASSERT_DEBUG(aBcchList.Length() <= KBcchListLength, CtsyDispatcherPanic(EBadLength));
	
	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(const_cast<TDesC8*>(&aNmr),const_cast<TDesC*>(&aBcchList));

	iMessageManagerCallback.Complete(ESatTsyLocalInformationNmr, &dataPackage, aError);

	//send off the in progress terminal response now that we have collected the required information for it.
	CompleteNotifyLocalInfo(aError);
	
	} // CSatDispatcher::CallbackLocalInformationNmr

void CSatDispatcher::CallbackRefreshAllowed(TInt aError)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSatRefreshAllowedComp().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher and SAT client.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	//the SAT TSY
	iMessageManagerCallback.Complete(ESatTsyCompleteRefresh, aError);

	//custom TSY (will do nothing when custom TSY is not open)
	iTsyMessageManagerCallback.Complete(ECustomSatRefreshCompleteNotificationIPC, aError);
	
	} // CSatDispatcher::CallbackRefreshAllowed

void CSatDispatcher::CallbackReady(TInt aError)
/**
 * Callback function to be used by the request to completeCCtsyDispatcherCallback::CallbackSatReadyComp().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(ESatTsyReady, aError);
	
	} // CSatDispatcher::CallbackReady

void CSatDispatcher::CallbackUssdControlEnvelopeError(TInt aError)
/**
 * Callback function to be used by the request to complete CCtsyDispatcherCallback::CallbackSatUssdControlEnvelopeErrorComp().
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(ESatTsyEnvelopeError, aError);
	
	} // CSatDispatcher::CallbackUssdControlEnvelopeError

void CSatDispatcher::CallbackTerminalRsp(TInt aError)
/**
 * Callback function to be used by the request to complete.
 *
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	TBool onGoing = (aError == KErrNone) ? FALSE : TRUE;
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&onGoing);

	iMessageManagerCallback.Complete(ESatTsySetProactiveCommandOnGoingStatus, &dataPackage, aError);

	} // CSatDispatcher::CallbackTerminalRsp

void CSatDispatcher::CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage)
/**
 * Part of the MDispatcherCallback interface. Used to complete requests handled
 * synchronously by the Licensee LTSY asynchronously back to the Common TSY.
 *  
 * This function is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.  
 *  
 * @param aIpcDataPackage Package encapsulating the request.
 * 
 * @see MDispatcherCallback::CallbackSync
 */
	{
	TSYLOGENTRYEXIT;
	
	switch (aIpcDataPackage.iIpc)
		{
	
	default:
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATDISPATCHER_CALLBACKSYNC_1, "WARNING: CSatDispatcher::CallbackSync unhandled IPC=%d", aIpcDataPackage.iIpc);
		__ASSERT_DEBUG(NULL, CtsyDispatcherPanic(EUnhandledCtsyIpc));
		break;		
		} // switch (aIpcDataPackage.iIpc)
	
	} // CSatDispatcher::CallbackSync



