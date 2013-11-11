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

#include <ctsy/ltsy/mltsydispatchsatinterface.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmlist.h>
#include <etelmm.h>

#include "mmockmesshandlerbase.h"
#include "cmocksatmesshandler.h"
#include "cmockltsyengine.h"
#include <test/tmockltsydata.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include "mockltsyindicatorids.h"
/**
 * Factory function
 */
CMockSatMessHandler* CMockSatMessHandler::NewL(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback )
    {
    CMockSatMessHandler* self = new (ELeave)CMockSatMessHandler(aEngine, aCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
 * Constructor
 */
CMockSatMessHandler::CMockSatMessHandler(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback)
 : iMockLtsyEngine(aEngine), iCompletionCallback(aCallback)
    {  
    }

/**
 * 2nd phase contructor
 */
void CMockSatMessHandler::ConstructL()
    {
    }

/**
 * Destructor
 */
CMockSatMessHandler::~CMockSatMessHandler()
    {

    }

TBool CMockSatMessHandler::IsHandlerForApi(TInt aApiId)
	{
	switch (aApiId)
		{
		
		case KMockLtsyDispatchSatUssdControlSupportedChangeIndId:
//		case KMockLtsyDispatchSatNotifyCallControlRequestIndId:
		case KMockLtsyDispatchSatPcmdIndId:
		case KMockLtsyDispatchSatNotifyProactiveSimSessionEndIndId:
		case KMockLtsyDispatchSatNotifyLocalInfoIndId:
		case KMockLtsyDispatchSatTimeZoneChangeIndId:
		case KMockLtsyDispatchSatAccessTechnologyChangeIndId:
		case KMockLtsyDispatchSatCallConnectedIndId:
		case KMockLtsyDispatchSatSsChangeIndId:
		case KMockLtsyDispatchSatCallDisconnectedIndId:
		case KMockLtsyDispatchSatMtCallIndId:
		case KMockLtsyDispatchSatCallControlEnvelopeResponseIndId:
		case KMockLtsyDispatchSatCreateCallControlEnvelopeIndId:
		case KMockLtsyDispatchSatCreateMoSmControlEnvelopeIndId:
		case KMockLtsyDispatchSatImsiChangedIndId:
		case KMockLtsyDispatchSatLocationStatusIndId:
		case KMockLtsyDispatchSatRemoveEventListIndId:
		case KMockLtsyDispatchSatCreateRefreshTerminalRspIndId:
		case KMockLtsyDispatchSatMoSmControlEnvelopeResponseIndId:
		case KMockLtsyDispatchSatCreateSmsPpDownloadEnvelopeIndId:
		case KMockLtsyDispatchSatCreateSmsDeliveryReportIndId:
		case KMockLtsyDispatchSatCreateCellBroadcastEnvelopeIndId:
		case MLtsyDispatchSatSmsDeliverReport::KLtsyDispatchSatSmsDeliverReportApiId:
		case MLtsyDispatchSatGetImageInstance::KLtsyDispatchSatGetImageInstanceApiId:
		case MLtsyDispatchSatGetIconData::KLtsyDispatchSatGetIconDataApiId:
		case MLtsyDispatchSatGetClut::KLtsyDispatchSatGetClutApiId:
		case MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId:
		case MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId:
		case MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId:
		case MLtsyDispatchSatCellBroadcastEnvelope::KLtsyDispatchSatCellBroadcastEnvelopeApiId:
		case MLtsyDispatchSatTimingAdvance::KLtsyDispatchSatTimingAdvanceApiId:
		case MLtsyDispatchSatSmControlResponseData::KLtsyDispatchSatSmControlResponseDataApiId:
		case MLtsyDispatchSatGetAccessTechnology::KLtsyDispatchSatGetAccessTechnologyApiId:
		case MLtsyDispatchSatProvideLocationInfo::KLtsyDispatchSatProvideLocationInfoApiId:
		case MLtsyDispatchSatSetPollingInterval::KLtsyDispatchSatSetPollingIntervalApiId:
		case MLtsyDispatchSatSmsPpDownloadEnvelope::KLtsyDispatchSatSmsPpDownloadEnvelopeApiId:
		case MLtsyDispatchSatLocalInformationNmr::KLtsyDispatchSatLocalInformationNmrApiId:
		case MLtsyDispatchSatCallAndSmsControlEnvelope::KLtsyDispatchSatCallAndSmsControlEnvelopeApiId:
		case MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId:
		case MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId:
		case MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId:
		case MLtsyDispatchSatUssdControlEnvelopeError::KLtsyDispatchSatUssdControlEnvelopeErrorApiId:
		case MLtsyDispatchSatTimerExpirationEnvelope::KLtsyDispatchSatTimerExpirationEnvelopeApiId:
		case MLtsyDispatchSatTerminalRsp::KLtsyDispatchSatTerminalRspApiId:
		case MLtsyDispatchSatMenuSelectionEnvelope::KLtsyDispatchSatMenuSelectionEnvelopeApiId:
		case MLtsyDispatchSatEventDownloadEnvelope::KLtsyDispatchSatEventDownloadEnvelopeApiId:
		case MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId:
			return iConfig.IsSupported(aApiId);
		default:
			return EFalse;
		}
	} // CMockSatMessHandler::IsHandlerForApi

/**
 * Execute a request.
 */
TInt CMockSatMessHandler::ExtFuncL(TInt aInterfaceId, VA_LIST& aList)
    {
    switch (aInterfaceId)
    	{
    	case MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId:
    		{
    		TUint8 msgType = VA_ARG_ENUM(aList,TUint8);
    		TMockLtsyData1<TUint8> data(msgType);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	
    	case MLtsyDispatchSatTerminalRsp::KLtsyDispatchSatTerminalRspApiId:
    		{
    		TDesC8* terminalRsp = VA_ARG(aList,TDesC8*);
    		TMockLtsyData1<TDesC8*> data(terminalRsp);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    		
    	case MLtsyDispatchSatMenuSelectionEnvelope::KLtsyDispatchSatMenuSelectionEnvelopeApiId:
    	case MLtsyDispatchSatEventDownloadEnvelope::KLtsyDispatchSatEventDownloadEnvelopeApiId:
    	case MLtsyDispatchSatTimerExpirationEnvelope::KLtsyDispatchSatTimerExpirationEnvelopeApiId:
    	case MLtsyDispatchSatSmsPpDownloadEnvelope::KLtsyDispatchSatSmsPpDownloadEnvelopeApiId:
    	case MLtsyDispatchSatCellBroadcastEnvelope::KLtsyDispatchSatCellBroadcastEnvelopeApiId:
    		{
    		TDesC8* envelope = VA_ARG(aList, TDesC8*);
    		TMockLtsyData1<TDesC8*> data(envelope);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}

    	case MLtsyDispatchSatCallAndSmsControlEnvelope::KLtsyDispatchSatCallAndSmsControlEnvelopeApiId:
    		{
    		TDesC8* envelope = VA_ARG(aList, TDesC8*);
    		TMockLtsyData1<TDesC8*> data(envelope);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}

    	case MLtsyDispatchSatGetIconData::KLtsyDispatchSatGetIconDataApiId:
    		{
    		TUint8 recordNumber	= VA_ARG_ENUM(aList,TUint8);
    		TDesC8* simFilePath = VA_ARG(aList,TDesC8*);
    		TMockLtsyData2<TUint8,TDesC8*> data(recordNumber,simFilePath);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}

    	case MLtsyDispatchSatGetClut::KLtsyDispatchSatGetClutApiId:
    	case MLtsyDispatchSatGetImageInstance::KLtsyDispatchSatGetImageInstanceApiId:
    		{
    		TUint8 recordNumber				= VA_ARG_ENUM(aList,TUint8);
    		TUint8 instanceNumberInRecord	= VA_ARG_ENUM(aList,TUint8);
    		TMockLtsyData2<TUint8,TUint8> data(recordNumber,instanceNumberInRecord);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);    		
    		}

    	case MLtsyDispatchSatSetPollingInterval::KLtsyDispatchSatSetPollingIntervalApiId:
    		{
    		TUint8 pollingInterval = VA_ARG_ENUM(aList,TUint8);
    		TMockLtsyData1<TUint8> data(pollingInterval);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	
    	case MLtsyDispatchSatSmControlResponseData::KLtsyDispatchSatSmControlResponseDataApiId:
    		{
    		TUint8 result					= VA_ARG_ENUM(aList,TUint8);
    		TDesC8* rpDestinationAddress 	= VA_ARG(aList,TDesC8*);
    		TDesC8* tpDestinationAddress 	= VA_ARG(aList,TDesC8*);
    		TMockLtsyData3<TUint8,TDesC8*,TDesC8*> data(result,rpDestinationAddress,tpDestinationAddress);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
		
		case MLtsyDispatchSatSmsDeliverReport::KLtsyDispatchSatSmsDeliverReportApiId:
			{
			TDesC8* deliveryReportTpdu =  VA_ARG(aList,TDesC8*);
			TMockLtsyData1<TDesC8*> data(deliveryReportTpdu);
			return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);		 		
			}

		case MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId:
			{
			TDesC8* refreshFileList = VA_ARG(aList,TDesC8*);
			TDesC8* aid				= VA_ARG(aList,TDesC8*);
			TUint16 tsyCache			= VA_ARG_ENUM(aList,TUint16);
			TMockLtsyData3<TDesC8*,TDesC8*,TUint16> data(refreshFileList,aid,tsyCache);
			return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);		 		
			}
    		
    	case MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId:
    	case MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId:
    	case MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId:
    	case MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId:
    	case MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId:
    	case MLtsyDispatchSatProvideLocationInfo::KLtsyDispatchSatProvideLocationInfoApiId:
    	case MLtsyDispatchSatLocalInformationNmr::KLtsyDispatchSatLocalInformationNmrApiId:
    	case MLtsyDispatchSatTimingAdvance::KLtsyDispatchSatTimingAdvanceApiId:
    	case MLtsyDispatchSatGetAccessTechnology::KLtsyDispatchSatGetAccessTechnologyApiId:
    	case MLtsyDispatchSatUssdControlEnvelopeError::KLtsyDispatchSatUssdControlEnvelopeErrorApiId:
    		{//no arguments
    		TMockLtsyData0 data;
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
   	
    	case KMockLtsyDispatchSatUssdControlSupportedChangeIndId:
//    	case KMockLtsyDispatchSatNotifyCallControlRequestIndId:
    	case KMockLtsyDispatchSatPcmdIndId:
    	case KMockLtsyDispatchSatNotifyProactiveSimSessionEndIndId:
    	case KMockLtsyDispatchSatTimeZoneChangeIndId:
    	case KMockLtsyDispatchSatAccessTechnologyChangeIndId:
    	case KMockLtsyDispatchSatCallConnectedIndId:
    	case KMockLtsyDispatchSatSsChangeIndId:
    	case KMockLtsyDispatchSatCallDisconnectedIndId:
    	case KMockLtsyDispatchSatMtCallIndId:
    	case KMockLtsyDispatchSatCallControlEnvelopeResponseIndId:
    	case KMockLtsyDispatchSatCreateCallControlEnvelopeIndId:
    	case KMockLtsyDispatchSatCreateMoSmControlEnvelopeIndId:
    	case KMockLtsyDispatchSatImsiChangedIndId:
    	case KMockLtsyDispatchSatLocationStatusIndId:
    	case KMockLtsyDispatchSatRemoveEventListIndId:
    	case KMockLtsyDispatchSatCreateRefreshTerminalRspIndId:
    	case KMockLtsyDispatchSatMoSmControlEnvelopeResponseIndId:
    	case KMockLtsyDispatchSatCreateSmsPpDownloadEnvelopeIndId:
    	case KMockLtsyDispatchSatCreateSmsDeliveryReportIndId:
    	case KMockLtsyDispatchSatCreateCellBroadcastEnvelopeIndId:
    	default:
    		{
    		_LIT(KTempPanic, "Here to remind coder to add code to deserialise data otherwise test passes even though CTSY sends down the incorrect data");
    		User::Panic(KTempPanic, KErrGeneral);    		
		    TMockLtsyData0 data;
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	}
    } // CMockSatMessHandler::ExtFuncL

/**
 * Complete a request
 */
void CMockSatMessHandler::CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult)
	{
	switch(aIpc)
		{
		case KMockLtsyDispatchSatUssdControlSupportedChangeIndId:
		    {
		    TBool supported;
		    TMockLtsyData1<TBool> supportedPck(supported);
		    supportedPck.DeserialiseL(aData);
			iCompletionCallback.CallbackSatUssdControlSupportedChangeInd(aResult,supported);
			}
			break;
		case KMockLtsyDispatchSatPcmdIndId:
		    {
		    TDesC8* proCmdData = NULL;
		    TMockLtsyData1<TDesC8*> proCmdDataPack(proCmdData);
		    proCmdDataPack.DeserialiseL(aData);
			iCompletionCallback.CallbackSatPcmdInd(aResult,*proCmdData);
			
			delete proCmdData;
			proCmdData = NULL;
			}
			break;
		case KMockLtsyDispatchSatNotifyProactiveSimSessionEndIndId:
		    {
		    TUint8 statusWord1 = 0;
		    TUint8 statusWord2 = 0;
		    
		    TMockLtsyData2<TUint8,TUint8> notifyProactiveSimSessionEndDataPack(statusWord1,statusWord2);
		    notifyProactiveSimSessionEndDataPack.DeserialiseL(aData);
			iCompletionCallback.CallbackSatNotifyProactiveSimSessionEndInd(aResult,statusWord1,statusWord2);
			}
			break;
		case KMockLtsyDispatchSatTimeZoneChangeIndId:
		    {
		    TUint8 timeZone;
		    TMockLtsyData1<TUint8> timeZoneDataPack(timeZone);
		    timeZoneDataPack.DeserialiseL(aData);
			iCompletionCallback.CallbackSatTimeZoneChangeInd(aResult,timeZone);
			}
			break;
		case KMockLtsyDispatchSatAccessTechnologyChangeIndId:
		    {
		    TUint8 currentAccessTechnology;
		    TMockLtsyData1<TUint8> currentAccTechDataPack(currentAccessTechnology);
		    currentAccTechDataPack.DeserialiseL(aData);
			iCompletionCallback.CallbackSatAccessTechnologyChangeInd(aResult,currentAccessTechnology);
			}
			break;
		case MLtsyDispatchSatGetAccessTechnology::KLtsyDispatchSatGetAccessTechnologyApiId:
			{
		    TUint8 currentAccessTechnology;
		    TMockLtsyData1<TUint8> currentAccTechDataPack(currentAccessTechnology);
		    currentAccTechDataPack.DeserialiseL(aData);
			iCompletionCallback.CallbackSatGetAccessTechnologyComp(aResult,currentAccessTechnology);
			}
			break;
		case KMockLtsyDispatchSatCallConnectedIndId:
		    {
		    TUint8 transactionId;
		    TBool nearEnd;
		    TMockLtsyData2<TUint8,TBool> callConnectedDataPack(transactionId,nearEnd);
		    callConnectedDataPack.DeserialiseL(aData);
			iCompletionCallback.CallbackSatCallConnectedInd(aResult,transactionId,nearEnd);
			}
			break;
		case KMockLtsyDispatchSatSsChangeIndId:
		    {
		    DispatcherSat::TSsStatus ssStatus;
		    TMockLtsyData1<DispatcherSat::TSsStatus> ssStatusPack(ssStatus);
		    ssStatusPack.DeserialiseL(aData);
			iCompletionCallback.CallbackSatSsChangeInd(aResult,ssStatus);
			}
			break;
		case KMockLtsyDispatchSatCallDisconnectedIndId:
		    {
		    TUint8 transactionId;
		    TBool nearEnd;
		    TDesC8* cause = NULL;
		    TMockLtsyData3<TUint8,TBool,TDesC8*> callDisconnectedPack(transactionId,nearEnd,cause);
		    callDisconnectedPack.DeserialiseL(aData);
		    iCompletionCallback.CallbackSatCallDisconnectedInd(aResult,transactionId,nearEnd,*cause);

		    delete cause;
		    cause = NULL;
			}
			break;
		case KMockLtsyDispatchSatMtCallIndId:
		    {
		    TUint8 transactionId;
		    TDesC8* address = NULL;
		    TDesC8* subAddress = NULL;
		    TMockLtsyData3<TUint8,TDesC8*,TDesC8*> mtCallPack(transactionId,address,subAddress);
		    mtCallPack.DeserialiseL(aData);
			iCompletionCallback.CallbackSatMtCallInd(aResult,transactionId,*address,*subAddress);

			delete address;
			address = NULL;
			
			delete subAddress;
			subAddress = NULL;
			}
			break;
		case KMockLtsyDispatchSatCallControlEnvelopeResponseIndId:
		    {
		    TDesC8* responseTlv = NULL;
		    TMockLtsyData1<TDesC8*> alphaIdPack(responseTlv);
		    alphaIdPack.DeserialiseL(aData);
			iCompletionCallback.CallbackSatCallControlEnvelopeResponseInd(aResult,*responseTlv);
			
			delete responseTlv;
			responseTlv = NULL;
			}
			break;
		case KMockLtsyDispatchSatCreateCallControlEnvelopeIndId:
		    {
			TUint8 tag=0;
			TDesC8* bcc1 = NULL;
			TDesC8* bcc2 = NULL;
			TDesC8* address = NULL;
			TUint8 tonNpi=0;
			TUint8 dcs=0;
			TMockLtsyData6<TUint8, TDesC8*, TDesC8*, TDesC8*, TUint8, TUint8> ccInfoPck(tag, bcc1, bcc2, address, tonNpi, dcs);
			ccInfoPck.DeserialiseL(aData);
			iCompletionCallback.CallbackSatCreateCallControlEnvelopeInd(aResult,tag,*bcc1,*bcc2,*address,tonNpi,dcs);
			delete bcc1;
			delete bcc2;
			delete address;
			}
			break;
		case KMockLtsyDispatchSatCreateMoSmControlEnvelopeIndId:
		    {
		    TDesC8* rpDestinatiomAddress = NULL;
		    TDesC8* tpDestinatiomAddress = NULL;
		    TMockLtsyData2<TDesC8*,TDesC8*> smsControlEnvelopePack(rpDestinatiomAddress,tpDestinatiomAddress);

		    smsControlEnvelopePack.DeserialiseL(aData);

			iCompletionCallback.CallbackSatCreateSmControlEnvelopeInd(aResult,*rpDestinatiomAddress,*tpDestinatiomAddress);
			
			delete rpDestinatiomAddress;
			rpDestinatiomAddress = NULL;
			
			delete tpDestinatiomAddress;
			tpDestinatiomAddress = NULL;
			}
			break;
		case KMockLtsyDispatchSatImsiChangedIndId:
		    {
			iCompletionCallback.CallbackSatImsiChangedInd(aResult);
			}
			break;
		case KMockLtsyDispatchSatLocationStatusIndId:
		    {
		    TUint8 locationStatus;
		    TDesC8* countryCode=NULL;
		    TUint16 locationAreaCode;
		    TUint16 cellId;
		    TMockLtsyData4<TUint8, TDesC8*, TUint16, TUint16 > lcnStatPck(locationStatus,countryCode,locationAreaCode,cellId);
		    lcnStatPck.DeserialiseL(aData);
			iCompletionCallback.CallbackSatLocationStatusInd(aResult,locationStatus, *countryCode, locationAreaCode, cellId);
			delete countryCode;
			}
			break;
		case KMockLtsyDispatchSatRemoveEventListIndId:
		    {
			iCompletionCallback.CallbackSatRemoveEventListInd(aResult);
			}
			break;
		case KMockLtsyDispatchSatCreateRefreshTerminalRspIndId:
		    {
		    TDesC* additionalInfo=NULL;
		    RSat::TPCmdResult generalResult;
		    TMockLtsyData2<RSat::TPCmdResult,TDesC*> refreshDataPck(generalResult,additionalInfo);
		    refreshDataPck.DeserialiseL(aData);
			iCompletionCallback.CallbackSatCreateRefreshTerminalRspInd(aResult,generalResult,*additionalInfo);
			
			delete additionalInfo;
			additionalInfo = NULL;
			}
			break;
		case KMockLtsyDispatchSatMoSmControlEnvelopeResponseIndId:
			{
		    TMockLtsyData1Buf<TDesC8*> moSmControlResponseTlvPack;
		    moSmControlResponseTlvPack.DeserialiseL(aData);
			iCompletionCallback.CallbackSatSmControlEnvelopeResponseInd(aResult,*moSmControlResponseTlvPack.Data1());
			moSmControlResponseTlvPack.Close();
			}
			break;
		case KMockLtsyDispatchSatCreateSmsPpDownloadEnvelopeIndId:
			{
		    TDesC8* rpOriginatingAddress = NULL;
		    TDesC8* smsDeliveryTpdu = NULL;
		    TMockLtsyData2<TDesC8*,TDesC8*> smsPpDdlPck(rpOriginatingAddress,smsDeliveryTpdu);
		    smsPpDdlPck.DeserialiseL(aData);
			iCompletionCallback.CallbackSatCreateSmsPpDownloadEnvelopeInd(aResult,*rpOriginatingAddress,*smsDeliveryTpdu);
			
			delete rpOriginatingAddress;
			rpOriginatingAddress = 0;
			
			delete smsDeliveryTpdu;
			smsDeliveryTpdu = 0;
			}
			break;
		case KMockLtsyDispatchSatCreateSmsDeliveryReportIndId:
			{
			iCompletionCallback.CallbackSatCreateSmsDeliverReportInd(aResult);
			}
			break;
		case KMockLtsyDispatchSatCreateCellBroadcastEnvelopeIndId:
			{
		    TMockLtsyData1Buf<TDesC8*> cellBroadcastPagePack;
		    cellBroadcastPagePack.DeserialiseL(aData);
			iCompletionCallback.CallbackSatCreateCellBroadcastEnvelopeInd(aResult,*cellBroadcastPagePack.Data1());
			cellBroadcastPagePack.Close();
			}
			break;
		case MLtsyDispatchSatGetImageInstance::KLtsyDispatchSatGetImageInstanceApiId:
		    {
		    TMockLtsyData2Buf<TDesC8*,TDesC8*> imageInstanceDataPck;
		    imageInstanceDataPck.DeserialiseL(aData);
		    iCompletionCallback.CallbackSatGetImageInstanceComp(aResult,*imageInstanceDataPck.Data1(),*imageInstanceDataPck.Data2());
		    imageInstanceDataPck.Close();
			}
			break;
		case MLtsyDispatchSatGetIconData::KLtsyDispatchSatGetIconDataApiId:
		    {
		    TMockLtsyData1Buf<TDesC8*> iconEfImgDataPck;
		    iconEfImgDataPck.DeserialiseL(aData);
		    iCompletionCallback.CallbackSatGetIconDataComp(aResult,*iconEfImgDataPck.Data1());
		    iconEfImgDataPck.Close();
		    }
			break;
		case MLtsyDispatchSatGetClut::KLtsyDispatchSatGetClutApiId:
		    {
		    TMockLtsyData1Buf<TDesC8*> clutDataPck;
		    clutDataPck.DeserialiseL(aData);
			iCompletionCallback.CallbackSatGetClutComp(aResult,*clutDataPck.Data1());
			clutDataPck.Close();
			}
			break;
		case MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId:
		    {
			TMockLtsyData1Buf<TDesC8*> bearerData;
			bearerData.DeserialiseL(aData);
			iCompletionCallback.CallbackSatGetDefaultBearerCapabilityComp(aResult, *bearerData.Data1());
			bearerData.Close();
			}
			break;
		case MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId:
		    {
			TBool supported;
		    TMockLtsyData1<TBool> supportedPck(supported);
			supportedPck.DeserialiseL(aData);
			iCompletionCallback.CallbackSatGetSmsPpDownloadSupportedComp(aResult, supported);
			}
			break;
		case MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId:
		    {
		    TBool activated;
		    TMockLtsyData1<TBool> activatedPck(activated);
		    activatedPck.DeserialiseL(aData);
			iCompletionCallback.CallbackSatGetSmsControlActivatedComp(aResult, activated);
			}
			break;
		case MLtsyDispatchSatTimingAdvance::KLtsyDispatchSatTimingAdvanceApiId:
		    {
		    TUint8 timingAdvance;
		    TUint8 meStatus;
			TMockLtsyData2<TUint8,TUint8> advPck(timingAdvance,meStatus);
			advPck.DeserialiseL(aData);
			iCompletionCallback.CallbackSatTimingAdvanceComp(aResult, timingAdvance, meStatus);
			}
			break;
		case MLtsyDispatchSatProvideLocationInfo::KLtsyDispatchSatProvideLocationInfoApiId:
		    {
			TUint16 locationAreaCode=0;
			TUint16 cellId=0;
			TDesC8* operatorCode=NULL;
		    TMockLtsyData3<TUint16,TUint16,TDesC8*> localInfoPack(locationAreaCode,cellId,operatorCode);
		    localInfoPack.DeserialiseL(aData);
		    iCompletionCallback.CallbackSatProvideLocationInfoComp(aResult,locationAreaCode,cellId,*operatorCode);
		    delete operatorCode;
			}
			break;
		case MLtsyDispatchSatSetPollingInterval::KLtsyDispatchSatSetPollingIntervalApiId:
		    {
		    TUint8 pollingInterval;
		    TMockLtsyData1<TUint8> pollingPck(pollingInterval);
		    pollingPck.DeserialiseL(aData);
			iCompletionCallback.CallbackSatSetPollingIntervalComp(aResult,pollingInterval);
			}
			break;
		case MLtsyDispatchSatLocalInformationNmr::KLtsyDispatchSatLocalInformationNmrApiId:
		    {
		    TDesC8* nmr=NULL;
		    TDesC* bcch=NULL;
		    TMockLtsyData2<TDesC8*,TDesC*> packed(nmr,bcch);
		    packed.DeserialiseL(aData);
			iCompletionCallback.CallbackSatLocalInformationNmrComp(aResult,*nmr,*bcch);
			delete nmr;
			delete bcch;
			}
			break;
		case MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId:
			{
			TBool supported;
		    TMockLtsyData1<TBool> supportedPck(supported);
		    supportedPck.DeserialiseL(aData);
			iCompletionCallback.CallbackSatGetUssdControlSupportedComp(aResult, supported);
			}
			break;
    	case MLtsyDispatchSatTerminalRsp::KLtsyDispatchSatTerminalRspApiId:
    		{
			iCompletionCallback.CallbackSatTerminalRspComp(aResult);
    		}
    		break;
    	case MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId:
    		{
    		iCompletionCallback.CallbackSatRefreshAllowedComp(aResult);
    		}
    		break;
    	case MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId:
    		{
    		iCompletionCallback.CallbackSatReadyComp(aResult);
    		}
    		break;
    	case MLtsyDispatchSatUssdControlEnvelopeError::KLtsyDispatchSatUssdControlEnvelopeErrorApiId:
    		{
    		iCompletionCallback.CallbackSatUssdControlEnvelopeErrorComp(aResult);
    		}
    		break;

    	//requests without completes
		case MLtsyDispatchSatSmsDeliverReport::KLtsyDispatchSatSmsDeliverReportApiId:
		case MLtsyDispatchSatCellBroadcastEnvelope::KLtsyDispatchSatCellBroadcastEnvelopeApiId:
		case MLtsyDispatchSatSmControlResponseData::KLtsyDispatchSatSmControlResponseDataApiId:
		case MLtsyDispatchSatSmsPpDownloadEnvelope::KLtsyDispatchSatSmsPpDownloadEnvelopeApiId:
		case MLtsyDispatchSatCallAndSmsControlEnvelope::KLtsyDispatchSatCallAndSmsControlEnvelopeApiId:
    	case MLtsyDispatchSatTimerExpirationEnvelope::KLtsyDispatchSatTimerExpirationEnvelopeApiId:
		case MLtsyDispatchSatEventDownloadEnvelope::KLtsyDispatchSatEventDownloadEnvelopeApiId:
    	case MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId:
    	case MLtsyDispatchSatMenuSelectionEnvelope::KLtsyDispatchSatMenuSelectionEnvelopeApiId:

    	default:
			{
			// Shouldn't get here. will panic MessageManager()->Complete if allowed to continue
			ASSERT(NULL);
			}
			break;
		}
	} // CMockSatMessHandler::CompleteL
