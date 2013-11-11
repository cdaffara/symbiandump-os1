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

#include <ctsy/ltsy/mltsydispatchcellbroadcastinterface.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmlist.h>
#include <etelmm.h>

#include "mmockmesshandlerbase.h"
#include "cmockcellbroadcastmesshandler.h"
#include "cmockltsyengine.h"
#include <test/tmockltsydata.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include "mockltsyindicatorids.h"
/**
 * Factory function
 */
CMockCellBroadcastMessHandler* CMockCellBroadcastMessHandler::NewL(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback )
    {
    CMockCellBroadcastMessHandler* self = new (ELeave)CMockCellBroadcastMessHandler(aEngine, aCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
 * Constructor
 */
CMockCellBroadcastMessHandler::CMockCellBroadcastMessHandler(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback)
 : iMockLtsyEngine(aEngine), iCompletionCallback(aCallback)
    {  
    }

/**
 * 2nd phase contructor
 */
void CMockCellBroadcastMessHandler::ConstructL()
    {
    }

/**
 * Destructor
 */
CMockCellBroadcastMessHandler::~CMockCellBroadcastMessHandler()
    {

    }

TBool CMockCellBroadcastMessHandler::IsHandlerForApi(TInt aApiId)
	{
	switch (aApiId)
		{
		
		case KMockLtsyDispatchCellBroadcastGsmBroadcastNotifyMessageReceivedIndId:
		case KMockLtsyDispatchCellBroadcastWcdmaBroadcastMessageReceivedIndId:
		case MLtsyDispatchCellBroadcastSetBroadcastFilterSetting::KLtsyDispatchCellBroadcastSetBroadcastFilterSettingApiId:
		case MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage::KLtsyDispatchCellBroadcastActivateBroadcastReceiveMessageApiId:
		case MLtsyDispatchCellBroadcastReceiveMessageCancel::KLtsyDispatchCellBroadcastReceiveMessageCancelApiId:
		case MLtsyDispatchCellBroadcastStartSimCbTopicBrowsing::KLtsyDispatchCellBroadcastStartSimCbTopicBrowsingApiId:
		case MLtsyDispatchCellBroadcastDeleteSimCbTopic::KLtsyDispatchCellBroadcastDeleteSimCbTopicApiId:
			return iConfig.IsSupported(aApiId);
		default:
			return EFalse;
		}
	} // CMockCellBroadcastMessHandler::IsHandlerForApi

/**
 * Execute a request.
 */
TInt CMockCellBroadcastMessHandler::ExtFuncL(TInt aInterfaceId, VA_LIST& aList)
    {
    switch (aInterfaceId)
    	{
    	case MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage::KLtsyDispatchCellBroadcastActivateBroadcastReceiveMessageApiId:
    		{
    		RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter filterSetting
    			= VA_ARG_ENUM(aList, RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter);
 		
    		TMockLtsyData1<RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter> data(filterSetting);

    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCellBroadcastReceiveMessageCancel::KLtsyDispatchCellBroadcastReceiveMessageCancelApiId:
    		{
    		RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter filterSetting
    			= VA_ARG_ENUM(aList, RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter);
 		
       		TMockLtsyData1<RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter> data(filterSetting);

    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCellBroadcastSetBroadcastFilterSetting::KLtsyDispatchCellBroadcastSetBroadcastFilterSettingApiId:    		    	
    		{
    		RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter filterSetting
    			= VA_ARG_ENUM(aList, RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter);
 		
    		TMockLtsyData1<RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter> data(filterSetting);

    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCellBroadcastStartSimCbTopicBrowsing::KLtsyDispatchCellBroadcastStartSimCbTopicBrowsingApiId:    		    	
    		{
    		TMockLtsyData0 data;
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);   		    		
    		}
    	case MLtsyDispatchCellBroadcastDeleteSimCbTopic::KLtsyDispatchCellBroadcastDeleteSimCbTopicApiId:    		    	
    		{
    		TUint index = VA_ARG(aList, TUint);
    		TBool deleteFlag = VA_ARG(aList, TBool);
    		TMockLtsyData2<TUint, TBool> data(index, deleteFlag);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);   		    		
    		}    		
    	case KMockLtsyDispatchCellBroadcastGsmBroadcastNotifyMessageReceivedIndId:
    	case KMockLtsyDispatchCellBroadcastWcdmaBroadcastMessageReceivedIndId:
    	default:
    		{
    		_LIT(KTempPanic, "Here to remind coder to add code to deserialise data otherwise test passes even though CTSY sends down the incorrect data");
    		User::Panic(KTempPanic, KErrGeneral);
		    TMockLtsyData0 data;
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	}
    } // CMockCellBroadcastMessHandler::ExtFuncL

/**
 * Complete a request
 */
void CMockCellBroadcastMessHandler::CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult)
	{
	switch(aIpc)
		{
		
		case KMockLtsyDispatchCellBroadcastGsmBroadcastNotifyMessageReceivedIndId:
		    {
		    TDesC8* cbMessage = NULL;		    
			TMockLtsyData1<TDesC8*> data(cbMessage);
			data.DeserialiseL(aData);	    
			iCompletionCallback.CallbackCellBroadcastGsmBroadcastNotifyMessageReceivedInd(aResult, *cbMessage);

			delete cbMessage;
			cbMessage = NULL;
		    }
			break;
		case KMockLtsyDispatchCellBroadcastWcdmaBroadcastMessageReceivedIndId:
		    {
		    TMockLtsyData3Buf<TDesC8* , DispatcherCellBroadcast::TWcdmaCbsMsgBase , TBool> wcdmaLtsyData;
			wcdmaLtsyData.DeserialiseL(aData);
			iCompletionCallback.CallbackCellBroadcastWcdmaBroadcastMessageReceivedInd(aResult, *wcdmaLtsyData.Data1(), 
					wcdmaLtsyData.Data2(), wcdmaLtsyData.Data3() );
			wcdmaLtsyData.Close();
			}
			break;
		case MLtsyDispatchCellBroadcastSetBroadcastFilterSetting::KLtsyDispatchCellBroadcastSetBroadcastFilterSettingApiId:
		    {
			iCompletionCallback.CallbackCellBroadcastSetBroadcastFilterSettingComp(aResult);
			}
			break;
		case MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage::KLtsyDispatchCellBroadcastActivateBroadcastReceiveMessageApiId:
		    {
			iCompletionCallback.CallbackCellBroadcastActivateBroadcastReceiveMessageComp(aResult);
			}
			break;
		case MLtsyDispatchCellBroadcastReceiveMessageCancel::KLtsyDispatchCellBroadcastReceiveMessageCancelApiId:
		    {
			iCompletionCallback.CallbackCellBroadcastReceiveMessageCancelComp(aResult);
			}
			break;
		case MLtsyDispatchCellBroadcastStartSimCbTopicBrowsing::KLtsyDispatchCellBroadcastStartSimCbTopicBrowsingApiId:
		    {
			TMockLtsyData1Buf<CArrayFixFlat< RMmCustomAPI::TSimCbTopic >* > ltsyData;
			ltsyData.DeserialiseL(aData);
			iCompletionCallback.CallbackCellBroadcastStartSimCbTopicBrowsingComp(aResult, *ltsyData.Data1() );
			ltsyData.Close();
			}
			break;
		case MLtsyDispatchCellBroadcastDeleteSimCbTopic::KLtsyDispatchCellBroadcastDeleteSimCbTopicApiId:
		    {
			iCompletionCallback.CallbackCellBroadcastDeleteSimCbTopicComp(aResult);
			}
			break;						
		default:
			{
			// Shouldn't get here. will panic MessageManager()->Complete if allowed to continue
			ASSERT(NULL);
			}
			break;
		}
	} // CMockCellBroadcastMessHandler::CompleteL
