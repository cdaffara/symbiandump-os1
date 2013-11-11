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

#include <ctsy/ltsy/mltsydispatchsmsinterface.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmlist.h>
#include <etelmm.h>

#include "mmockmesshandlerbase.h"
#include "cmocksmsmesshandler.h"
#include "cmockltsyengine.h"
#include <test/tmockltsydata.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include "mockltsyindicatorids.h"
/**
 * Factory function
 */
CMockSmsMessHandler* CMockSmsMessHandler::NewL(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback )
    {
    CMockSmsMessHandler* self = new (ELeave)CMockSmsMessHandler(aEngine, aCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
 * Constructor
 */
CMockSmsMessHandler::CMockSmsMessHandler(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback)
 : iMockLtsyEngine(aEngine), iCompletionCallback(aCallback)
    {  
    }

/**
 * 2nd phase contructor
 */
void CMockSmsMessHandler::ConstructL()
    {
    }

/**
 * Destructor
 */
CMockSmsMessHandler::~CMockSmsMessHandler()
    {
    }

TBool CMockSmsMessHandler::IsHandlerForApi(TInt aApiId)
	{
	switch (aApiId)
		{
		
		case KMockLtsyDispatchSmsNotifyReceiveSmsMessageIndId:
		case MLtsyDispatchSmsSendSatSms::KLtsyDispatchSmsSendSatSmsApiId:
		case MLtsyDispatchSmsGetSmsStoreInfo::KLtsyDispatchSmsGetSmsStoreInfoApiId:
		case MLtsyDispatchSmsGetSmspList::KLtsyDispatchSmsGetSmspListApiId:
		case MLtsyDispatchSmsNackSmsStored::KLtsyDispatchSmsNackSmsStoredApiId:
		case MLtsyDispatchSmsAckSmsStored::KLtsyDispatchSmsAckSmsStoredApiId:
		case MLtsyDispatchSmsResumeSmsReception::KLtsyDispatchSmsResumeSmsReceptionApiId:
		case MLtsyDispatchSmsSendSmsMessage::KLtsyDispatchSmsSendSmsMessageApiId:
		case MLtsyDispatchSmsSendSmsMessageNoFdnCheck::KLtsyDispatchSmsSendSmsMessageNoFdnCheckApiId:
		case MLtsyDispatchSmsSetMoSmsBearer::KLtsyDispatchSmsSetMoSmsBearerApiId:
		case MLtsyDispatchSmsStoreSmspListEntry::KLtsyDispatchSmsStoreSmspListEntryApiId:
			return iConfig.IsSupported(aApiId);
		default:
			return EFalse;
		}
	} // CMockSmsMessHandler::IsHandlerForApi

/**
 * Execute a request.
 */
TInt CMockSmsMessHandler::ExtFuncL(TInt aInterfaceId, VA_LIST& aList)
    {
    switch (aInterfaceId)
    	{
    	case MLtsyDispatchSmsSendSatSms::KLtsyDispatchSmsSendSatSmsApiId:
    		{
    		TDesC8* smsTpdu = VA_ARG(aList, TDesC8*);		   		    
		    TBool more = VA_ARG(aList, TBool);
		    RMobilePhone::TMobileAddress& gsmServiceCentre = *VA_ARG(aList,RMobilePhone::TMobileAddress*);
		    RMobileSmsMessaging::TMobileSmsDataFormat dataFormat = VA_ARG_ENUM(aList,RMobileSmsMessaging::TMobileSmsDataFormat);
		    TMockLtsyData4< TDesC8*,
		    					RMobileSmsMessaging::TMobileSmsDataFormat,
		    					RMobilePhone::TMobileAddress,
		    					TBool
		                    > data(smsTpdu, dataFormat, gsmServiceCentre, more);
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);  
    		}
    	case MLtsyDispatchSmsAckSmsStored::KLtsyDispatchSmsAckSmsStoredApiId:
    		{
    		TDesC8* msgPtr = VA_ARG(aList, TDesC8*);    		
    		TMockLtsyData1<TDesC8*> data(msgPtr);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchSmsGetSmsStoreInfo::KLtsyDispatchSmsGetSmsStoreInfoApiId:
    		{
    		TMockLtsyData0 data;
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchSmsGetSmspList::KLtsyDispatchSmsGetSmspListApiId:
    		{
    		TMockLtsyData0 data;
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);   		
    		}
    	case MLtsyDispatchSmsNackSmsStored::KLtsyDispatchSmsNackSmsStoredApiId:
    		{
    		TDesC8* msgPtr = VA_ARG(aList, TDesC8*);    		
    		TInt rpCause = VA_ARG(aList, TInt);    		    	
    		TMockLtsyData2<TDesC8*, TInt> data(msgPtr, rpCause);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchSmsStoreSmspListEntry::KLtsyDispatchSmsStoreSmspListEntryApiId:
    		{        		    		
    		RMobileSmsMessaging::TMobileSmspEntryV1& smspEntry = *VA_ARG(aList, RMobileSmsMessaging::TMobileSmspEntryV1*);    		
    		TMockLtsyData1<RMobileSmsMessaging::TMobileSmspEntryV1> data(smspEntry);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchSmsSetMoSmsBearer::KLtsyDispatchSmsSetMoSmsBearerApiId:
    		{
    		RMobileSmsMessaging::TMobileSmsBearer bearer = VA_ARG_ENUM(aList, RMobileSmsMessaging::TMobileSmsBearer);
    		TMockLtsyData1<RMobileSmsMessaging::TMobileSmsBearer> data(bearer);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchSmsResumeSmsReception::KLtsyDispatchSmsResumeSmsReceptionApiId:
    		{
    		//no arguments
    		TMockLtsyData0 data;
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchSmsSendSmsMessage::KLtsyDispatchSmsSendSmsMessageApiId:
    	case MLtsyDispatchSmsSendSmsMessageNoFdnCheck::KLtsyDispatchSmsSendSmsMessageNoFdnCheckApiId:
    		{    		    		
    		RMobilePhone::TMobileAddress& destination = *VA_ARG(aList,RMobilePhone::TMobileAddress*);
		    TDesC8* smsTpdu = VA_ARG(aList, TDesC8*);		   		    
		    TBool more = VA_ARG(aList, TBool);
		    RMobilePhone::TMobileAddress& gsmServiceCentre = *VA_ARG(aList,RMobilePhone::TMobileAddress*);
		    RMobileSmsMessaging::TMobileSmsDataFormat dataFormat = VA_ARG_ENUM(aList, RMobileSmsMessaging::TMobileSmsDataFormat);
		    TMockLtsyData5< RMobilePhone::TMobileAddress, 
		                        TDesC8*,
		                        RMobileSmsMessaging::TMobileSmsDataFormat,
		                        RMobilePhone::TMobileAddress,
		                        TBool
		                    > data(destination, smsTpdu, dataFormat, gsmServiceCentre, more);
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);    		    	
    		}
    	case KMockLtsyDispatchSmsNotifyReceiveSmsMessageIndId:
     	default:
    		{
    		_LIT(KTempPanic, "Here to remind coder to add code to deserialise data otherwise test passes even though CTSY sends down the incorrect data");
    		User::Panic(KTempPanic, KErrGeneral);    		
		    TMockLtsyData0 data;
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	}
    } // CMockSmsMessHandler::ExtFuncL

/**
 * Complete a request
 */
void CMockSmsMessHandler::CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult)
	{
	switch(aIpc)
		{
		
		case KMockLtsyDispatchSmsNotifyReceiveSmsMessageIndId:
		    {
		    TMockLtsyData2Buf<TBool, TSmsMsg*> compTsyData;
		    compTsyData.DeserialiseL(aData);
			iCompletionCallback.CallbackSmsNotifyReceiveSmsMessageInd(aResult, compTsyData.Data1(), *compTsyData.Data2());
			compTsyData.Close();
			}
			break;
		case MLtsyDispatchSmsSendSatSms::KLtsyDispatchSmsSendSatSmsApiId:
		    {
			iCompletionCallback.CallbackSmsSendSatSmsComp(aResult);
			}
			break;
		case MLtsyDispatchSmsGetSmsStoreInfo::KLtsyDispatchSmsGetSmsStoreInfoApiId:
		    {
		    TMockLtsyData2Buf<TInt, TInt> msgInfoData;
		    msgInfoData.DeserialiseL(aData);		    
			iCompletionCallback.CallbackSmsGetSmsStoreInfoComp(aResult, msgInfoData.Data1(), msgInfoData.Data2());
			}
			break;
		case MLtsyDispatchSmsGetSmspList::KLtsyDispatchSmsGetSmspListApiId:
		    {
		    TMockLtsyData5Buf<TDesC* , TDesC* , TDesC* , DispatcherSim::TSmsParameters , TBool> smsParams;
		    smsParams.DeserialiseL(aData);
			iCompletionCallback.CallbackSmsGetSmspListComp(aResult, 
				*smsParams.Data1(), *smsParams.Data2(), *smsParams.Data3(), smsParams.Data4(), smsParams.Data5());
			smsParams.Close();
			}
			break;
		case MLtsyDispatchSmsNackSmsStored::KLtsyDispatchSmsNackSmsStoredApiId:
		    {
			iCompletionCallback.CallbackSmsNackSmsStoredComp(aResult);
			}
			break;
		case MLtsyDispatchSmsAckSmsStored::KLtsyDispatchSmsAckSmsStoredApiId:
		    {
			iCompletionCallback.CallbackSmsAckSmsStoredComp(aResult);
			}
			break;
		case MLtsyDispatchSmsResumeSmsReception::KLtsyDispatchSmsResumeSmsReceptionApiId:
		    {
			iCompletionCallback.CallbackSmsResumeSmsReceptionComp(aResult);
			}
			break;
		case MLtsyDispatchSmsSendSmsMessage::KLtsyDispatchSmsSendSmsMessageApiId:
		    {		        		    
		    TMockLtsyData2Buf<TInt16, TBuf8<RMobileSmsMessaging::KGsmTpduSize> > smsData;
		    smsData.DeserialiseL(aData);
			iCompletionCallback.CallbackSmsSendSmsMessageComp(aResult, smsData.Data1(), smsData.Data2());
			}
			break;
		case MLtsyDispatchSmsSendSmsMessageNoFdnCheck::KLtsyDispatchSmsSendSmsMessageNoFdnCheckApiId:
		    {
		    TMockLtsyData2Buf<TInt16, TBuf8<RMobileSmsMessaging::KGsmTpduSize> > smsData;
		    smsData.DeserialiseL(aData);
			iCompletionCallback.CallbackSmsSendSmsMessageNoFdnCheckComp(aResult, smsData.Data1(), smsData.Data2());
			}
			break;
		case MLtsyDispatchSmsSetMoSmsBearer::KLtsyDispatchSmsSetMoSmsBearerApiId:
		    {
			iCompletionCallback.CallbackSmsSetMoSmsBearerComp(aResult);
			}
			break;
		case MLtsyDispatchSmsStoreSmspListEntry::KLtsyDispatchSmsStoreSmspListEntryApiId:
		    {			
			iCompletionCallback.CallbackSmsStoreSmspListEntryComp(aResult);
			}
			break;
		default:
			{
			// Shouldn't get here. will panic MessageManager()->Complete if allowed to continue
			ASSERT(NULL);
			}
			break;
		}
	} // CMockSmsMessHandler::CompleteL
