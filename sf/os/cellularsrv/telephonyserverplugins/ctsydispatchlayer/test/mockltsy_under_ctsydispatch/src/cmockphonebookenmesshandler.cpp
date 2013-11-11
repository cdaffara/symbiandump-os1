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

#include <ctsy/ltsy/mltsydispatchphonebookeninterface.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmlist.h>
#include <etelmm.h>

#include "mmockmesshandlerbase.h"
#include "cmockphonebookenmesshandler.h"
#include "cmockltsyengine.h"
#include <test/tmockltsydata.h>
#include "mockltsyindicatorids.h"

/**
 * Factory function
 */
CMockPhonebookEnMessHandler* CMockPhonebookEnMessHandler::NewL(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback )
    {
    CMockPhonebookEnMessHandler* self = new (ELeave)CMockPhonebookEnMessHandler(aEngine, aCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
 * Constructor
 */
CMockPhonebookEnMessHandler::CMockPhonebookEnMessHandler(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback)
 : iMockLtsyEngine(aEngine), iCompletionCallback(aCallback)
    {  
    }

/**
 * 2nd phase contructor
 */
void CMockPhonebookEnMessHandler::ConstructL()
    {
    }

/**
 * Destructor
 */
CMockPhonebookEnMessHandler::~CMockPhonebookEnMessHandler()
    {

    }

TBool CMockPhonebookEnMessHandler::IsHandlerForApi(TInt aApiId)
	{
	switch (aApiId)
		{
		
		case MLtsyDispatchPhonebookEnStoreGetInfo::KLtsyDispatchPhonebookEnStoreGetInfoApiId:
		case MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId:
		case MLtsyDispatchPhonebookEnStoreReadEntry::KLtsyDispatchPhonebookEnStoreReadEntryApiId:
			return iConfig.IsSupported(aApiId);
		default:
			return EFalse;
		}
	} // CMockPhonebookEnMessHandler::IsHandlerForApi

/**
 * Execute a request.
 */
TInt CMockPhonebookEnMessHandler::ExtFuncL(TInt aInterfaceId, VA_LIST& aList)
    {
    switch (aInterfaceId)
    	{
    	
    	
    	case MLtsyDispatchPhonebookEnStoreGetInfo::KLtsyDispatchPhonebookEnStoreGetInfoApiId:
    		{
    		TMockLtsyData0 data;
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId:
    		{
    		TMockLtsyData0 data;
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchPhonebookEnStoreReadEntry::KLtsyDispatchPhonebookEnStoreReadEntryApiId:
    		{
    		TInt index = VA_ARG(aList, TInt);
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
    } // CMockPhonebookEnMessHandler::ExtFuncL

/**
 * Complete a request
 */
void CMockPhonebookEnMessHandler::CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult)
	{
	switch(aIpc)
		{
		
		case MLtsyDispatchPhonebookEnStoreGetInfo::KLtsyDispatchPhonebookEnStoreGetInfoApiId:
		    {
		    TInt usedEntries;
		    TMockLtsyData1<TInt> data(usedEntries);
		    data.DeserialiseL(aData);
		    iCompletionCallback.CallbackPhonebookEnStoreGetInfoComp(aResult, usedEntries);
			}
			break;
		case MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId:
		    {
		    TInt index;
		    TBuf<RMobileENStore::KEmergencyNumberSize> number;
		    TBool moreToCome;
		    TMockLtsyData3< TInt, TBuf<RMobileENStore::KEmergencyNumberSize>, TBool > data(index, number, moreToCome);
		    data.DeserialiseL(aData);
			iCompletionCallback.CallbackPhonebookEnStoreReadAllComp(aResult, index, number, moreToCome);
			}
			break;
		case MLtsyDispatchPhonebookEnStoreReadEntry::KLtsyDispatchPhonebookEnStoreReadEntryApiId:
		    {
			TInt index;
			TBuf<RMobileENStore::KEmergencyNumberSize> number;
		    TMockLtsyData2< TInt, TBuf<RMobileENStore::KEmergencyNumberSize> > data(index, number);
		    data.DeserialiseL(aData);
			iCompletionCallback.CallbackPhonebookEnStoreReadEntryComp(aResult, index, number);
			}
			break;
		default:
			{
			// Shouldn't get here. will panic MessageManager()->Complete if allowed to continue
			ASSERT(NULL);
			}
			break;
		}
	} // CMockPhonebookEnMessHandler::CompleteL
