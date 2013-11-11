// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ctsy/pluginapi/mmmessagemanagercallback.h>
#include "cmocksmsmesshandler.h" 
#include <ctsy/serviceapi/mmtsy_ipcdefs.h> 
#include "etelmm.h"
#include "cmockltsyengine.h"
#include "tmockltsydata.h"
#include <ctsy/serviceapi/cmmsmsutility.h>

/**
Factory function
*/
CMockSmsMessHandler* CMockSmsMessHandler::NewL(CMmMessageRouter* aMessageRouter) 
    { 
    CMockSmsMessHandler* self = new (ELeave)CMockSmsMessHandler(aMessageRouter); 
    CleanupStack::PushL( self ); 
    self->ConstructL(); 
    CleanupStack::Pop(); 
    return self; 
    } 

/**
Constructor
*/
CMockSmsMessHandler::CMockSmsMessHandler(CMmMessageRouter* aMessageRouter)
	:iMessageRouter(aMessageRouter)
    {
    }

/**
2nd phase contructor
*/
void CMockSmsMessHandler::ConstructL()
    {  
    } 

/**
Destructor
*/
CMockSmsMessHandler::~CMockSmsMessHandler()
    { 
	for (TInt i = 0; i<iWcdmaBroadcastMessages.Count(); i++)
		{
		if ( iWcdmaBroadcastMessages[i] )
			{
			iWcdmaBroadcastMessages[i]->ResetAndDestroy();
			}
		}
	iWcdmaBroadcastMessages.ResetAndDestroy();

	for (TInt i = 0; i<iSmsParametersList.Count(); i++)
		{
		if ( iSmsParametersList[i] )
			{
			iSmsParametersList[i]->ResetAndDestroy();
			}
		}
	iSmsParametersList.ResetAndDestroy();
	
	for (TInt i = 0; i<iSmsMsgList.Count(); i++)
		{
		if ( iSmsMsgList[i] )
			{
			iSmsMsgList[i]->ResetAndDestroy();
			}
		}
	iSmsMsgList.ResetAndDestroy();
	iBroadcastIdLists.ResetAndDestroy();
	}
    
/**
Execute a request
*/
TInt CMockSmsMessHandler::ExtFuncL(  
    TInt aIpc, 
    const CMmDataPackage* aMmDataPackage)
    { 
     switch (aIpc)
    	{
    	case EMobileSmsMessagingSetMoSmsBearer:
    	    {
			RMobileSmsMessaging::TMobileSmsBearer smsData;
    		aMmDataPackage->UnPackData(smsData);
			TMockLtsyData1<RMobileSmsMessaging::TMobileSmsBearer> data(smsData);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);    		
    	    }
    	case EMobileSmsMessagingNackSmsStored:
    	    {
    	    TDesC8* ptr;
    	    TInt cause;
			TMockLtsyData2<TDesC8*, TInt> data(ptr, cause);
    		aMmDataPackage->UnPackData(ptr, cause);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);    		
    	    }
    	case EMobileSmsMessagingSendMessageNoFdnCheck:
    	case EMobileSmsMessagingSendMessage:
    	    {
			TSendSmsDataAndAttributes smsData;
    		aMmDataPackage->UnPackData(smsData);
			TMockLtsyData1<TSendSmsDataAndAttributes> data(smsData);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);    		
    	    }
    	case EMobileUssdMessagingSendMessageNoFdnCheck:
    	case EMobileUssdMessagingSendMessage:
    		{
    		TDes8*  ptr1 = NULL;
    		TDes8** ptr2 = NULL;

    		aMmDataPackage->UnPackData(&ptr1, &ptr2);
    		
			TPckg<TName>* msgData = ( TPckg<TName>* ) ptr1;
			TPckg<RMobileUssdMessaging::TMobileUssdAttributesV1>* msgAttributes = 
				( TPckg<RMobileUssdMessaging::TMobileUssdAttributesV1>* ) *ptr2;

			TMockLtsyData2<TName, RMobileUssdMessaging::TMobileUssdAttributesV1> 
				data( ( *msgData )(), ( *msgAttributes )());
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);    		
    		}
    	case EMobileBroadcastMessagingStoreIdList:
    		{
    		CMobilePhoneBroadcastIdList* idList = NULL;
    		RMobileBroadcastMessaging::TMobileBroadcastIdType type
    		                              = RMobileBroadcastMessaging::EGsmBroadcastId;

    		// make proper change when
    		// CTSY component defect is fixed:
    		// looking at the CTSY implementation of StoreBroadcastIdList IPC
    		// it is wrong since it does nothing.

    		aMmDataPackage->UnPackData(idList, type);

    		TMockLtsyData2<CMobilePhoneBroadcastIdList*,
    		               RMobileBroadcastMessaging::TMobileBroadcastIdType>
    		                                                  data(idList, type);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);
    		}
    	case EMobileBroadcastMessagingGetIdListPhase1:
    		{
    		RMobileBroadcastMessaging::TMobileBroadcastIdType type
    		                              = RMobileBroadcastMessaging::EGsmBroadcastId;

    		// make proper change when
    		// CTSY component defect is fixed:
    		// looking at the CTSY implementation of EMobileBroadcastMessagingGetIdListPhase1 IPC
    		// it is wrong since it does nothing.

    		aMmDataPackage->UnPackData(type);

    		TMockLtsyData1<RMobileBroadcastMessaging::TMobileBroadcastIdType>
    		                                                  data(type);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);
    		}
    	case EMobileBroadcastMessagingSetFilterSetting:
    	case EMobileBroadcastMessagingReceiveMessageCancel:
    	case EMobileBroadcastMessagingReceiveMessage:
    		{
			TCbsCbmiAndLangAndFilter *packedData;
    		aMmDataPackage->UnPackData(&packedData);
			TMockLtsyData1<TCbsCbmiAndLangAndFilter> data(*packedData);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);    		
    	    }
    	case EMobileSmsMessagingAckSmsStored:
    	    {
			TDesC8* ptr;
			
    	    if(NULL == aMmDataPackage)
    	        {
    	        ptr = NULL;
    	        }
    	    else
    	        {
        		aMmDataPackage->UnPackData(ptr);
    	        }
    	        
			TMockLtsyData1<TDesC8*> data(ptr);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);    		
    	    }
    	case EMobilePhoneStoreRead:
    		{
    		TInt index;
    		aMmDataPackage->UnPackData(index);
    		TMockLtsyData1<TInt> data(index);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);    		
    		}
		case EMobilePhoneStoreWrite:
			{
			TDes8** ptr1  = NULL;
			TInt *  index = NULL;
			
			aMmDataPackage->UnPackData(&ptr1, &index);
					
			TPckg<RMobilePhone::TMultimodeType>* pckg = 
				( TPckg<RMobilePhone::TMultimodeType>* ) *ptr1;
			
			switch( ( *pckg )().ExtensionId() )
				{
				case RMobileSmsStore::KETelMobileGsmSmsEntryV1:
					{
					TPckg<RMobileSmsStore::TMobileGsmSmsEntryV1>* entryPckg = 
						( TPckg<RMobileSmsStore::TMobileGsmSmsEntryV1>* ) *ptr1;
		    		TMockLtsyData2<RMobileSmsStore::TMobileGsmSmsEntryV1, TInt> 
		    			data( ( *entryPckg )(), *index);
					
					return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);    		
					}
				case RMobileSmsStore::KETelMobileCdmaSmsEntryV1:
					{
					TPckg<RMobileSmsStore::TMobileCdmaSmsEntryV1>* entryPckg = 
						( TPckg<RMobileSmsStore::TMobileCdmaSmsEntryV1>* ) *ptr1;
		    		TMockLtsyData2<RMobileSmsStore::TMobileCdmaSmsEntryV1, TInt> 
		    			data( ( *entryPckg )(), *index);
					
					return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);    		
					}
				default:
					{
					TMockLtsyData0 data;
					return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);
					}
				}
			}
		case EMobilePhoneStoreDelete:
			{
			TInt* index = NULL;
			aMmDataPackage->UnPackData(&index);
			TMockLtsyData1<TInt> data(*index);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);    	
			}
		case EMobilePhoneStoreReadAllPhase1:
		case EMobilePhoneStoreGetInfo:
		case EMobilePhoneStoreDeleteAll:
    	case EMobileSmsMessagingGetSmspListPhase1:
    	case EMobileSmsMessagingResumeSmsReception:
    	default:
    		{
		    TMockLtsyData0 data;
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);
    		}
    	}
    } 
 
void CMockSmsMessHandler::CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult)
	{
	CMmDataPackage dataPackage;
	switch (aIpc)
		{
		case EMmTsyActivateSmsRouting:
		    {
			TMockLtsyData1Buf<TUint8> status;
			status.DeserialiseL(aData);
			dataPackage.PackData(status.Data1Ptr());
		    }
		    break;
		case EMobileSmsMessagingGetMessageStoreInfo:
		    {
			TMockLtsyData2Buf< TInt, TInt > data;
			data.DeserialiseL(aData); 
			dataPackage.PackData(data.Data1Ptr(), data.Data2Ptr());
		    }
		    break;
		case EMobileSmsMessagingReceiveMessage:
		    {
			TMockLtsyData2Buf< TBool, TSmsMsg* > data;
			CleanupClosePushL(data);
			data.DeserialiseL(aData); 
			dataPackage.PackData(data.Data1Ptr(), data.Data2Ptr());
			iMessageRouter->MessageManagerCallback()->Complete(aIpc, &dataPackage, aResult);
			CleanupStack::PopAndDestroy(1); 
			return;
		    }
		case EMobileSmsMessagingSendMessageNoFdnCheck:
		case EMobileSmsMessagingSendMessage:
		    {
		    TMockLtsyData2Buf<TInt16, TBuf8<RMobileSmsMessaging::KGsmTpduSize> > data;
			data.DeserialiseL(aData);
			dataPackage.PackData(data.Data1Ptr(), data.Data2Ptr());
		    }
		    break;
		case EMmTsyGsmBroadcastNotifyMessageReceived:
		    {
			TMockLtsyData1Buf<TGsmCbsMsg> msg;
			msg.DeserialiseL(aData);
			dataPackage.PackData(msg.Data1Ptr());
		    }
		    break;
		case EMmTsyWcdmaBroadcastNotifyMessageReceived:
		    if ( KErrNone == aResult )
			    {
				TMockLtsyData2Buf<CArrayPtrFlat< TWcdmaCbsMsg >*, TUint8> data;
				data.DeserialiseL(aData);
				if ( data.Data1() )
					{
					iWcdmaBroadcastMessages.Append(data.Data1());
					}
				dataPackage.PackData(data.Data1Ptr(), data.Data2Ptr());
			    }
		    break;
		case EMobileSmsMessagingGetSmspListPhase1:
		    if ( KErrNone == aResult )
			    {
				TMockLtsyData1Buf<CArrayPtrFlat< TSmsParameters >*> data;
				data.DeserialiseL(aData);
				if ( data.Data1() )
					{
					iSmsParametersList.Append(data.Data1());
					}
				dataPackage.PackData(data.Data1());
			    }
		    break;
		case EMobileUssdMessagingNotifyNetworkRelease:
			{
			if (aData.Length() > sizeof(RMobilePhone::TMobilePhoneSendSSRequestV3))
				{
				TMockLtsyData2Buf<RMobilePhone::TMobilePhoneSendSSRequestV3, 
								RMobileUssdMessaging::TMobileUssdAttributesV1> msg;
				msg.DeserialiseL(aData);
				dataPackage.PackData(msg.Data1Ptr(), msg.Data2Ptr() );		
				}
			else
				{
				TMockLtsyData1Buf<RMobilePhone::TMobilePhoneSendSSRequestV3> msg;
				msg.DeserialiseL(aData);
				dataPackage.PackData(msg.Data1Ptr());
				}
			}
			break;			
		case EMobileUssdMessagingSendRelease:
			{
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneSendSSRequestV3> msg;
			msg.DeserialiseL(aData);
			dataPackage.PackData(msg.Data1Ptr());
			}
			break;
		case EMobileUssdMessagingReceiveMessage:
			{
			TMockLtsyData2Buf<TBuf8<KMaxName>,
			                  RMobileUssdMessaging::TMobileUssdAttributesV1> msg;
			msg.DeserialiseL(aData);
			dataPackage.PackData(msg.Data1Ptr(), msg.Data2Ptr());
			}
			break;
    	case EMobilePhoneStoreRead:
    		{
			TMockLtsyData1Buf<TSmsMsg> msg;
			msg.DeserialiseL(aData);
			dataPackage.PackData(msg.Data1Ptr());
			}
			break;
		case EMobilePhoneStoreWrite:
			{
			TMockLtsyData2Buf<TUint8, TBool> msg;
			msg.DeserialiseL(aData);
			dataPackage.PackData(msg.Data1Ptr(), msg.Data2Ptr());
			}
			break;
		case EMobilePhoneStoreGetInfo:
			{
			TMockLtsyData2Buf<TUint8, TInt> msg;
			msg.DeserialiseL(aData);
			dataPackage.PackData(msg.Data1Ptr(), msg.Data2Ptr());
			}
			break;
		case EMobilePhoneStoreReadAllPhase1:
			{
    		TMockLtsyData2Buf<CArrayPtrFlat< TSmsMsg >*, TBool> data;
    		data.DeserialiseL(aData);
    		if ( data.Data1() )
    			{
    			iSmsMsgList.Append(data.Data1());
    			}
    		dataPackage.PackData(data.Data1Ptr(), data.Data2Ptr());
			}
	    	break;
		case EMobileBroadcastMessagingGetIdListPhase1:
		    {
    		// make proper change when
    		// CTSY component defect is fixed:
    		// looking at the CTSY implementation of EMobileBroadcastMessagingGetIdListPhase1 IPC
    		// it is wrong since it does nothing.

		    CMobilePhoneBroadcastIdList* idList = CMobilePhoneBroadcastIdList::NewL();
			CleanupStack::PushL(idList);
			iBroadcastIdLists.Append(idList);
			CleanupStack::Pop(idList);
			TSerializer<CMobilePhoneBroadcastIdList>::DeserialiseL(aData, *idList);
			dataPackage.PackData(&idList);
		    }
			break;
		
		case EMobilePhoneStoreDelete:
		case EMobilePhoneStoreDeleteAll:
		case EMobileBroadcastMessagingSetFilterSetting:
		case EMobileBroadcastMessagingReceiveMessageCancel:
		case EMobileBroadcastMessagingReceiveMessage:
		case EMobileBroadcastMessagingStoreIdList:
		default:
			// complete event without data
			break;
		}
	iMessageRouter->MessageManagerCallback()->Complete(aIpc, &dataPackage, aResult);
	}

