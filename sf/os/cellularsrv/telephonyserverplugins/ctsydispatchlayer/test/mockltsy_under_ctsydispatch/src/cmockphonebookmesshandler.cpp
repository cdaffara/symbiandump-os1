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

#include <ctsy/ltsy/mltsydispatchphonebookinterface.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmlist.h>
#include <etelmm.h>

#include "mmockmesshandlerbase.h"
#include "cmockphonebookmesshandler.h"
#include "cmockltsyengine.h"
#include <test/tmockltsydata.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include "mockltsyindicatorids.h"
/**
 * Factory function
 */
CMockPhonebookMessHandler* CMockPhonebookMessHandler::NewL(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback )
    {
    CMockPhonebookMessHandler* self = new (ELeave)CMockPhonebookMessHandler(aEngine, aCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
 * Constructor
 */
CMockPhonebookMessHandler::CMockPhonebookMessHandler(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback)
 : iMockLtsyEngine(aEngine), iCompletionCallback(aCallback)
    {  
    }

/**
 * 2nd phase contructor
 */
void CMockPhonebookMessHandler::ConstructL()
    {
    }

/**
 * Destructor
 */
CMockPhonebookMessHandler::~CMockPhonebookMessHandler()
    {

    }

TBool CMockPhonebookMessHandler::IsHandlerForApi(TInt aApiId)
	{
	switch (aApiId)
		{
		
		case KMockLtsyDispatchPhonebookStoreResetCacheIndId:
		case KMockLtsyDispatchPhonebookStoreSetFdnPhonebookInfoIndId:
		case MLtsyDispatchPhonebookStoreReadEntry::KLtsyDispatchPhonebookStoreReadEntryApiId:
		case MLtsyDispatchPhonebookStoreDeleteEntry::KLtsyDispatchPhonebookStoreDeleteEntryApiId:
		case MLtsyDispatchPhonebookStoreCache::KLtsyDispatchPhonebookStoreCacheApiId:
		case MLtsyDispatchPhonebookStoreGetPhonebookInfo::KLtsyDispatchPhonebookStoreGetPhonebookInfoApiId:
		case MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId:
		case MLtsyDispatchPhonebookStoreDeleteAll::KLtsyDispatchPhonebookStoreDeleteAllApiId:
		case MLtsyDispatchPhonebookSmsStoreReadAll::KLtsyDispatchPhonebookSmsStoreReadAllApiId:
		case MLtsyDispatchPhonebookStoreWriteEntry::KLtsyDispatchPhonebookStoreWriteEntryApiId:
		case MLtsyDispatchPhonebookGetPhoneStoreInfo::KLtsyDispatchPhonebookGetPhoneStoreInfoApiId:
		case MLtsyDispatchPhonebookSmsStoreDeleteEntry::KLtsyDispatchPhonebookSmsStoreDeleteEntryApiId:
		case MLtsyDispatchPhonebookSmsStoreDeleteAll::KLtsyDispatchPhonebookSmsStoreDeleteAllApiId:
		case MLtsyDispatchPhonebookSmsStoreGetInfo::KLtsyDispatchPhonebookSmsStoreGetInfoApiId:
		case MLtsyDispatchPhonebookSmsStoreReadEntry::KLtsyDispatchPhonebookSmsStoreReadEntryApiId:
		case MLtsyDispatchPhonebookSmsStoreWriteEntry::KLtsyDispatchPhonebookSmsStoreWriteEntryApiId:
		case MLtsyDispatchPhonebookStoreCacheCancel::KLtsyDispatchPhonebookStoreCacheCancelApiId:
			return iConfig.IsSupported(aApiId);
		default:
			return EFalse;
		}
	} // CMockPhonebookMessHandler::IsHandlerForApi

/**
 * Execute a request.
 */
TInt CMockPhonebookMessHandler::ExtFuncL(TInt aInterfaceId, VA_LIST& aList)
    {
    switch (aInterfaceId)
    	{
    	case MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId:
    		{
    		TMockLtsyData0 data;
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}

    	case MLtsyDispatchPhonebookSmsStoreWriteEntry::KLtsyDispatchPhonebookSmsStoreWriteEntryApiId:
    		{
    		RMobileSmsStore::TMobileGsmSmsEntryV1& mobileGsmSmsEntry = *VA_ARG(aList,RMobileSmsStore::TMobileGsmSmsEntryV1*);
    		TMockLtsyData1<RMobileSmsStore::TMobileGsmSmsEntryV1> data(mobileGsmSmsEntry);
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    		
    	case MLtsyDispatchPhonebookStoreCache::KLtsyDispatchPhonebookStoreCacheApiId:
    	case MLtsyDispatchPhonebookStoreDeleteAll::KLtsyDispatchPhonebookStoreDeleteAllApiId:
    	case MLtsyDispatchPhonebookStoreGetPhonebookInfo::KLtsyDispatchPhonebookStoreGetPhonebookInfoApiId:
    	case MLtsyDispatchPhonebookStoreCacheCancel::KLtsyDispatchPhonebookStoreCacheCancelApiId:
    		{
    		DispatcherPhonebook::TPhonebook phonebook = VA_ARG_ENUM(aList,DispatcherPhonebook::TPhonebook);
		    TMockLtsyData1<DispatcherPhonebook::TPhonebook> data(phonebook);
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}

    	case MLtsyDispatchPhonebookStoreDeleteEntry::KLtsyDispatchPhonebookStoreDeleteEntryApiId:
    		{
    		DispatcherPhonebook::TPhonebook phonebook = VA_ARG_ENUM(aList,DispatcherPhonebook::TPhonebook);
    		TInt index = VA_ARG(aList,TInt);
		    TMockLtsyData2<DispatcherPhonebook::TPhonebook,TInt> data(phonebook,index);
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	
    	case MLtsyDispatchPhonebookStoreReadEntry::KLtsyDispatchPhonebookStoreReadEntryApiId:
    		{
    		DispatcherPhonebook::TPhonebook phonebook = VA_ARG_ENUM(aList,DispatcherPhonebook::TPhonebook);
    		TInt index = VA_ARG(aList,TInt);
    		TInt numberOfSlots = VA_ARG(aList,TInt);
    		TMockLtsyData3<DispatcherPhonebook::TPhonebook,TInt,TInt> data(phonebook,index,numberOfSlots);
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    		
    	case MLtsyDispatchPhonebookStoreWriteEntry::KLtsyDispatchPhonebookStoreWriteEntryApiId:
    		{
    		DispatcherPhonebook::TPhonebook phonebook = VA_ARG_ENUM(aList,DispatcherPhonebook::TPhonebook);
    		TDesC8* entry = VA_ARG(aList,TDesC8*);
    		TMockLtsyData2<DispatcherPhonebook::TPhonebook,TDesC8*> data(phonebook,entry);
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	
    	case MLtsyDispatchPhonebookGetPhoneStoreInfo::KLtsyDispatchPhonebookGetPhoneStoreInfoApiId:
    		{
    		TDesC* storeName = VA_ARG(aList,TDesC*);
    		TMockLtsyData1<TDesC*> data(storeName);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
       		}
   		
    	case KMockLtsyDispatchPhonebookStoreResetCacheIndId:
    	case KMockLtsyDispatchPhonebookStoreSetFdnPhonebookInfoIndId:
    		{
    		_LIT(KTempPanic, "Here to remind coder to add code to deserialise data otherwise test passes even though CTSY sends down the incorrect data");
    		User::Panic(KTempPanic, KErrGeneral);
    		TMockLtsyData0 data;
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchPhonebookSmsStoreReadAll::KLtsyDispatchPhonebookSmsStoreReadAllApiId:
    		{
    		TMockLtsyData0 data;
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchPhonebookSmsStoreGetInfo::KLtsyDispatchPhonebookSmsStoreGetInfoApiId:
    		{
    		TMockLtsyData0 data;
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchPhonebookSmsStoreDeleteEntry::KLtsyDispatchPhonebookSmsStoreDeleteEntryApiId:
    		{
    		TInt index = VA_ARG(aList,TInt);
    		TMockLtsyData1<TInt> data(index);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchPhonebookSmsStoreDeleteAll::KLtsyDispatchPhonebookSmsStoreDeleteAllApiId:
    		{
    		TMockLtsyData0 data;
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchPhonebookSmsStoreReadEntry::KLtsyDispatchPhonebookSmsStoreReadEntryApiId:
    		{
    		TInt index = VA_ARG(aList,TInt);
    		TMockLtsyData1<TInt> data(index);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	default:
    		{
    		_LIT(KTempPanic, "Here to remind coder to add code to deserialise data otherwise test passes even though CTSY sends down the incorrect data");
    		User::Panic(KTempPanic, KErrGeneral);
		    TMockLtsyData0 data;
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	}
    } // CMockPhonebookMessHandler::ExtFuncL

/**
 * Complete a request
 */
void CMockPhonebookMessHandler::CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult)
	{
	switch(aIpc)
		{
		
		case KMockLtsyDispatchPhonebookStoreResetCacheIndId:
		    {
		    DispatcherPhonebook::TPhonebook phonebook;
		    TMockLtsyData1<DispatcherPhonebook::TPhonebook> resetCachePack(phonebook);
		    resetCachePack.DeserialiseL(aData);
		    iCompletionCallback.CallbackPhonebookStoreResetCacheInd(aResult,phonebook);
			}
			break;
		case KMockLtsyDispatchPhonebookStoreSetFdnPhonebookInfoIndId:
		    {
		    TInt totalEntries;
		    TInt maximumTextLength;
			TInt maximumNumberLength;
		    TMockLtsyData3<TInt,TInt,TInt> phonebookDataPack(totalEntries,maximumTextLength,maximumNumberLength);
		    phonebookDataPack.DeserialiseL(aData);
			iCompletionCallback.CallbackPhonebookStoreSetFdnPhonebookInfoInd(aResult,totalEntries,maximumTextLength,maximumNumberLength);
			}
			break;
		case MLtsyDispatchPhonebookStoreReadEntry::KLtsyDispatchPhonebookStoreReadEntryApiId:
		    {
		    DispatcherPhonebook::TPhonebook phonebook;
		    TDesC8* phonebookEntries = NULL;
		    
		    TMockLtsyData2<DispatcherPhonebook::TPhonebook,TDesC8*> phoneBookEntryPack(phonebook,phonebookEntries);
		    phoneBookEntryPack.DeserialiseL(aData);
			iCompletionCallback.CallbackPhonebookStoreReadEntryComp(aResult,phonebook,*phonebookEntries);
			
			delete phonebookEntries;
			phonebookEntries = NULL;
			}
			break;
		case MLtsyDispatchPhonebookStoreDeleteEntry::KLtsyDispatchPhonebookStoreDeleteEntryApiId:
		    {
		    DispatcherPhonebook::TPhonebook phonebook;
		    TInt maxNumberLength = -1;
		    TMockLtsyData2<DispatcherPhonebook::TPhonebook,TInt> deleteEntryPack(phonebook,maxNumberLength);
		    deleteEntryPack.DeserialiseL(aData);

		    //test the default value behaviour
		    if(maxNumberLength == -1)
		    	{
			    iCompletionCallback.CallbackPhonebookStoreDeleteEntryComp(aResult,phonebook);
		    	}
		    else
		    	{
			    iCompletionCallback.CallbackPhonebookStoreDeleteEntryComp(aResult,phonebook,maxNumberLength);
		    	}
		    
			}
			break;
		case MLtsyDispatchPhonebookStoreCache::KLtsyDispatchPhonebookStoreCacheApiId:
		    {
		    DispatcherPhonebook::TPhonebook phonebook;
		    TDesC8* phonebookEntries = NULL;
		    
		    TMockLtsyData2<DispatcherPhonebook::TPhonebook,TDesC8*> phoneBookEntryPack(phonebook,phonebookEntries);
		    phoneBookEntryPack.DeserialiseL(aData);
			iCompletionCallback.CallbackPhonebookStoreCacheComp(aResult,phonebook,*phonebookEntries);
			
			delete phonebookEntries;
			phonebookEntries = NULL;
			}
			break;
		case MLtsyDispatchPhonebookStoreGetPhonebookInfo::KLtsyDispatchPhonebookStoreGetPhonebookInfoApiId:
		    {
		    DispatcherPhonebook::TPhonebook phonebook;
		    TInt numberOfUsedEntries;
		    
		    TMockLtsyData2<DispatcherPhonebook::TPhonebook,TInt> phoneBookGetInfoPack(phonebook,numberOfUsedEntries);
		    phoneBookGetInfoPack.DeserialiseL(aData);
			iCompletionCallback.CallbackPhonebookStoreGetPhonebookInfoComp(aResult,phonebook,numberOfUsedEntries);
			}
			break;
		case MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId:
		    {
		    DispatcherPhonebook::TPhonebookStoreInfoV1 phonebookData;
		    TMockLtsyData1<DispatcherPhonebook::TPhonebookStoreInfoV1> phonebookDataPack(phonebookData);
		    phonebookDataPack.DeserialiseL(aData);
			iCompletionCallback.CallbackPhonebookStoreInitialiseComp(aResult,phonebookData);
			}
			break;
		case MLtsyDispatchPhonebookStoreDeleteAll::KLtsyDispatchPhonebookStoreDeleteAllApiId:
		    {
		    DispatcherPhonebook::TPhonebook phonebook;
		    TInt maxNumberLength = -1;
		    TMockLtsyData2<DispatcherPhonebook::TPhonebook,TInt> deleteAllEntryPack(phonebook,maxNumberLength);
		    deleteAllEntryPack.DeserialiseL(aData);
	    
		    //test the default value behaviour
		    if(maxNumberLength == -1)
		    	{
			    iCompletionCallback.CallbackPhonebookStoreDeleteAllComp(aResult,phonebook);
		    	}
		    else
		    	{
			    iCompletionCallback.CallbackPhonebookStoreDeleteAllComp(aResult,phonebook,maxNumberLength);
		    	}
			}
			break;
		case MLtsyDispatchPhonebookSmsStoreReadAll::KLtsyDispatchPhonebookSmsStoreReadAllApiId:
		    {
		    DispatcherPhonebook::TSmsData smsData;
		    TBool moreToCome;
		    TBool receivedClass2ToBeResent;
		    TInt index;
		    TMockLtsyData4<DispatcherPhonebook::TSmsData, TInt, TBool, TBool>smsStoreReadEntryPack(smsData, index, moreToCome, receivedClass2ToBeResent);
		    smsStoreReadEntryPack.DeserialiseL(aData);
			iCompletionCallback.CallbackPhonebookSmsStoreReadAllComp(aResult, smsData, index, moreToCome, receivedClass2ToBeResent);
			}
			break;
		case MLtsyDispatchPhonebookStoreWriteEntry::KLtsyDispatchPhonebookStoreWriteEntryApiId:
		    {
		    DispatcherPhonebook::TPhonebook phonebook;
		    TUint16 index;
		    TInt maxNumberLength = -1;
		    TMockLtsyData3<DispatcherPhonebook::TPhonebook,TUint16,TInt> writeEntryPack(phonebook,index,maxNumberLength);
		    writeEntryPack.DeserialiseL(aData);
		    
		    //test the default value behaviour
		    if(maxNumberLength == -1)
		    	{
			    iCompletionCallback.CallbackPhonebookStoreWriteEntryComp(aResult,phonebook,index);
		    	}
		    else
		    	{
			    iCompletionCallback.CallbackPhonebookStoreWriteEntryComp(aResult,phonebook,index,maxNumberLength);
		    	}
			}
			break;
		case MLtsyDispatchPhonebookGetPhoneStoreInfo::KLtsyDispatchPhonebookGetPhoneStoreInfoApiId:
		    {
		    RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStoreInfo;
		    TMockLtsyData1<RMobilePhoneStore::TMobilePhoneStoreInfoV1> phoneStoreInfoPack(phoneStoreInfo);
		    phoneStoreInfoPack.DeserialiseL(aData);
			iCompletionCallback.CallbackPhonebookGetPhoneStoreInfoComp(aResult,phoneStoreInfo);
			}
			break;
		case MLtsyDispatchPhonebookSmsStoreDeleteEntry::KLtsyDispatchPhonebookSmsStoreDeleteEntryApiId:
		    {
		    iCompletionCallback.CallbackPhonebookSmsStoreDeleteEntryComp(aResult);
			}
			break;
		case MLtsyDispatchPhonebookSmsStoreDeleteAll::KLtsyDispatchPhonebookSmsStoreDeleteAllApiId:
		    {
			iCompletionCallback.CallbackPhonebookSmsStoreDeleteAllComp(aResult);
			}
			break;
		case MLtsyDispatchPhonebookSmsStoreGetInfo::KLtsyDispatchPhonebookSmsStoreGetInfoApiId:
		    {
			TInt totalEntries;
		    TInt usedEntries;
		    TMockLtsyData2<TInt,TInt> smsStoreGetInfoPack(totalEntries,usedEntries);
		    smsStoreGetInfoPack.DeserialiseL(aData);
			iCompletionCallback.CallbackPhonebookSmsStoreGetInfoComp(aResult, totalEntries, usedEntries);
			}
			break;
		case MLtsyDispatchPhonebookSmsStoreReadEntry::KLtsyDispatchPhonebookSmsStoreReadEntryApiId:
		    {
		    DispatcherPhonebook::TSmsData smsData;
		    TMockLtsyData1<DispatcherPhonebook::TSmsData> smsStoreReadEntryPack(smsData);
		    smsStoreReadEntryPack.DeserialiseL(aData);
			iCompletionCallback.CallbackPhonebookSmsStoreReadEntryComp(aResult, smsData);
			}
			break;
		case MLtsyDispatchPhonebookSmsStoreWriteEntry::KLtsyDispatchPhonebookSmsStoreWriteEntryApiId:
		    {
		    TInt location;
			TBool  receivedClass2ToBeResent;
			TMockLtsyData2<TInt,TBool> smsStoreWriteEntryPack(location,receivedClass2ToBeResent);
			smsStoreWriteEntryPack.DeserialiseL(aData);
		    			
			iCompletionCallback.CallbackPhonebookSmsStoreWriteEntryComp(aResult, location, receivedClass2ToBeResent);
			}
			break;
		default:
			{
			// Shouldn't get here. will panic MessageManager()->Complete if allowed to continue
			ASSERT(NULL);
			}
			break;
		}
	} // CMockPhonebookMessHandler::CompleteL
