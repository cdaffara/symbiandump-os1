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

#include <ctsy/ltsy/mltsydispatchphonebookoninterface.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmlist.h>
#include <etelmm.h>

#include "mmockmesshandlerbase.h"
#include "cmockphonebookonmesshandler.h"
#include "cmockltsyengine.h"
#include <test/tmockltsydata.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include "mockltsyindicatorids.h"
/**
 * Factory function
 */
CMockPhonebookOnMessHandler* CMockPhonebookOnMessHandler::NewL(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback )
    {
    CMockPhonebookOnMessHandler* self = new (ELeave)CMockPhonebookOnMessHandler(aEngine, aCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
 * Constructor
 */
CMockPhonebookOnMessHandler::CMockPhonebookOnMessHandler(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback)
 : iMockLtsyEngine(aEngine), iCompletionCallback(aCallback)
    {  
    }

/**
 * 2nd phase contructor
 */
void CMockPhonebookOnMessHandler::ConstructL()
    {
    }

/**
 * Destructor
 */
CMockPhonebookOnMessHandler::~CMockPhonebookOnMessHandler()
    {

    }

TBool CMockPhonebookOnMessHandler::IsHandlerForApi(TInt aApiId)
	{
	switch (aApiId)
		{
		case MLtsyDispatchPhonebookOnStoreRead::KLtsyDispatchPhonebookOnStoreReadApiId:
		case MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId:
		case MLtsyDispatchPhonebookOnStoreReadEntry::KLtsyDispatchPhonebookOnStoreReadEntryApiId:
		case MLtsyDispatchPhonebookOnStoreGetInfo::KLtsyDispatchPhonebookOnStoreGetInfoApiId:
		case MLtsyDispatchPhonebookOnStoreGetReadStoreSize::KLtsyDispatchPhonebookOnStoreGetReadStoreSizeApiId:
		case MLtsyDispatchPhonebookOnStoreDeleteEntry::KLtsyDispatchPhonebookOnStoreDeleteEntryApiId:
		case MLtsyDispatchPhonebookOnStoreWriteEntry::KLtsyDispatchPhonebookOnStoreWriteEntryApiId:
		case MLtsyDispatchPhonebookOnStoreWrite::KLtsyDispatchPhonebookOnStoreWriteApiId:
		case MLtsyDispatchPhonebookOnStoreGetStoreSize::KLtsyDispatchPhonebookOnStoreGetStoreSizeApiId:
			return iConfig.IsSupported(aApiId);
		default:
			return EFalse;
		}
	} // CMockPhonebookOnMessHandler::IsHandlerForApi

/**
 * Execute a request.
 */
TInt CMockPhonebookOnMessHandler::ExtFuncL(TInt aInterfaceId, VA_LIST& aList)
    {
    switch (aInterfaceId)
    	{
    	case MLtsyDispatchPhonebookOnStoreRead::KLtsyDispatchPhonebookOnStoreReadApiId:
    		{
    		TInt index = VA_ARG(aList, TInt);
    		TMockLtsyData1<TInt> data(index);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId:
    		{
    		TMockLtsyData0 data;
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchPhonebookOnStoreReadEntry::KLtsyDispatchPhonebookOnStoreReadEntryApiId:
    		{
    		TInt index = VA_ARG(aList, TInt);
    		TMockLtsyData1<TInt> data(index);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchPhonebookOnStoreGetInfo::KLtsyDispatchPhonebookOnStoreGetInfoApiId:
    		{
    		TMockLtsyData0 data;
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchPhonebookOnStoreGetReadStoreSize::KLtsyDispatchPhonebookOnStoreGetReadStoreSizeApiId:
    		{
    		TMockLtsyData0 data;
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchPhonebookOnStoreDeleteEntry::KLtsyDispatchPhonebookOnStoreDeleteEntryApiId:
    		{
    		TInt index = VA_ARG(aList, TInt);
    		TMockLtsyData1<TInt> data(index);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchPhonebookOnStoreWriteEntry::KLtsyDispatchPhonebookOnStoreWriteEntryApiId:
    		{
    		RMobileONStore::TMobileONEntryV1& mobileONEntry = *VA_ARG(aList,RMobileONStore::TMobileONEntryV1*);
    		TMockLtsyData1<RMobileONStore::TMobileONEntryV1> data(mobileONEntry);
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchPhonebookOnStoreWrite::KLtsyDispatchPhonebookOnStoreWriteApiId:
    		{
    		RMobileONStore::TMobileONEntryV1& mobileONEntry = *VA_ARG(aList,RMobileONStore::TMobileONEntryV1*);
    		TMockLtsyData1<RMobileONStore::TMobileONEntryV1> data(mobileONEntry);
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchPhonebookOnStoreGetStoreSize::KLtsyDispatchPhonebookOnStoreGetStoreSizeApiId:
    		{
    		TMockLtsyData0 data;
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
    } // CMockPhonebookOnMessHandler::ExtFuncL

/**
 * Complete a request
 */
void CMockPhonebookOnMessHandler::CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult)
	{
	switch(aIpc)
		{
		case MLtsyDispatchPhonebookOnStoreRead::KLtsyDispatchPhonebookOnStoreReadApiId:
		    {
		    TInt location;
		    TBuf<RMobileONStore::KOwnNumberTextSize> name;
		    TBuf<KPBStoreNumSize> telNumber;
		    TMockLtsyData3< TInt, TBuf<RMobileONStore::KOwnNumberTextSize>, TBuf<KPBStoreNumSize> > data(location, name, telNumber);
		    data.DeserialiseL(aData);
			iCompletionCallback.CallbackPhonebookOnStoreReadComp(aResult, location, name, telNumber);
			}
			break;
		case MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId:
		    {
			iCompletionCallback.CallbackPhonebookOnStoreDeleteAllComp(aResult);
			}
			break;
		case MLtsyDispatchPhonebookOnStoreReadEntry::KLtsyDispatchPhonebookOnStoreReadEntryApiId:
		    {
		    TInt location;
		    TBuf<RMobileONStore::KOwnNumberTextSize> name;
		    TBuf<KPBStoreNumSize> telNumber;
		    TMockLtsyData3< TInt, TBuf<RMobileONStore::KOwnNumberTextSize>, TBuf<KPBStoreNumSize> > data(location, name, telNumber);
		    data.DeserialiseL(aData);
			iCompletionCallback.CallbackPhonebookOnStoreReadEntryComp(aResult, location, name, telNumber);
			}
			break;
		case MLtsyDispatchPhonebookOnStoreGetInfo::KLtsyDispatchPhonebookOnStoreGetInfoApiId:
		    {
		    TInt numOfEntries, usedEntries, maxNameLen, maxNumLen;
		    TMockLtsyData4<TInt,TInt, TInt, TInt> data(numOfEntries, usedEntries, maxNameLen, maxNumLen);
		    data.DeserialiseL(aData);
		    iCompletionCallback.CallbackPhonebookOnStoreGetInfoComp(aResult, numOfEntries, usedEntries, maxNameLen, maxNumLen);
			}
			break;
		case MLtsyDispatchPhonebookOnStoreGetReadStoreSize::KLtsyDispatchPhonebookOnStoreGetReadStoreSizeApiId:
		    {
		    TInt size;
		    TMockLtsyData1<TInt> data(size);
		    data.DeserialiseL(aData);
			iCompletionCallback.CallbackPhonebookOnStoreGetReadStoreSizeComp(aResult, size);
			}
			break;
		case MLtsyDispatchPhonebookOnStoreDeleteEntry::KLtsyDispatchPhonebookOnStoreDeleteEntryApiId:
		    {
			iCompletionCallback.CallbackPhonebookOnStoreDeleteEntryComp(aResult);
			}
			break;
		case MLtsyDispatchPhonebookOnStoreWriteEntry::KLtsyDispatchPhonebookOnStoreWriteEntryApiId:
		    {
			iCompletionCallback.CallbackPhonebookOnStoreWriteEntryComp(aResult);
			}
			break;
		case MLtsyDispatchPhonebookOnStoreWrite::KLtsyDispatchPhonebookOnStoreWriteApiId:
		    {
		    TInt location;
		    TMockLtsyData1<TInt> data(location);
		    data.DeserialiseL(aData);
			iCompletionCallback.CallbackPhonebookOnStoreWriteComp(aResult, location);
			}
			break;
		case MLtsyDispatchPhonebookOnStoreGetStoreSize::KLtsyDispatchPhonebookOnStoreGetStoreSizeApiId:
		    {
		    TInt size;
		    TMockLtsyData1<TInt> data(size);
		    data.DeserialiseL(aData);
			iCompletionCallback.CallbackPhonebookOnStoreGetStoreSizeComp(aResult, size);
			}
			break;
		default:
			{
			// Shouldn't get here. will panic MessageManager()->Complete if allowed to continue
			ASSERT(NULL);
			}
			break;
		}
	} // CMockPhonebookOnMessHandler::CompleteL
