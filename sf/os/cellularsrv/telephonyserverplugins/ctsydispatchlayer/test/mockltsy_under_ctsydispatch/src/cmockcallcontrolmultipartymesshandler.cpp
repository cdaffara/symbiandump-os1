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

#include <ctsy/ltsy/mltsydispatchcallcontrolmultipartyinterface.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmlist.h>
#include <etelmm.h>

#include "mmockmesshandlerbase.h"
#include "cmockcallcontrolmultipartymesshandler.h"
#include "cmockltsyengine.h"
#include <test/tmockltsydata.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include "mockltsyindicatorids.h"


/**
 * Factory function
 */
CMockCallControlMultipartyMessHandler* CMockCallControlMultipartyMessHandler::NewL(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback )
    {
    CMockCallControlMultipartyMessHandler* self = new (ELeave)CMockCallControlMultipartyMessHandler(aEngine, aCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
 * Constructor
 */
CMockCallControlMultipartyMessHandler::CMockCallControlMultipartyMessHandler(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback)
 : iMockLtsyEngine(aEngine), iCompletionCallback(aCallback)
    {
    }

/**
 * 2nd phase contructor
 */
void CMockCallControlMultipartyMessHandler::ConstructL()
    {
    }

/**
 * Destructor
 */
CMockCallControlMultipartyMessHandler::~CMockCallControlMultipartyMessHandler()
    {

    }

TBool CMockCallControlMultipartyMessHandler::IsHandlerForApi(TInt aApiId)
	{
	switch (aApiId)
		{
		case MLtsyDispatchCallControlMultipartyConferenceHangUp::KLtsyDispatchCallControlMultipartyConferenceHangUpApiId:
		case MLtsyDispatchCallControlMultipartyConferenceAddCall::KLtsyDispatchCallControlMultipartyConferenceAddCallApiId:
		case MLtsyDispatchCallControlMultipartyCreateConference::KLtsyDispatchCallControlMultipartyCreateConferenceApiId:
		case MLtsyDispatchCallControlMultipartyConferenceSwap::KLtsyDispatchCallControlMultipartyConferenceSwapApiId:
		case MLtsyDispatchCallControlMultipartyConferenceGoOneToOne::KLtsyDispatchCallControlMultipartyConferenceGoOneToOneApiId:
			return iConfig.IsSupported(aApiId);
		default:
			return EFalse;
		}
	} // CMockCallControlMultipartyMessHandler::IsHandlerForApi

/**
 * Execute a request.
 */
TInt CMockCallControlMultipartyMessHandler::ExtFuncL(TInt aInterfaceId, VA_LIST& aList)
    {
    switch (aInterfaceId)
    	{
    	case MLtsyDispatchCallControlMultipartyConferenceHangUp::KLtsyDispatchCallControlMultipartyConferenceHangUpApiId:
    		{
    		RArray<TInt>& callIds =  *VA_ARG(aList, RArray<TInt>* );
    		TMockLtsyData1<RArray<TInt> > data(callIds);    		    		
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlMultipartyConferenceAddCall::KLtsyDispatchCallControlMultipartyConferenceAddCallApiId:
    		{
    		TInt callId = VA_ARG(aList, TInt);
    		TInt existingCallId = VA_ARG(aList, TInt);
    		TMockLtsyData2<TInt, TInt> data(callId, existingCallId);
  			return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlMultipartyCreateConference::KLtsyDispatchCallControlMultipartyCreateConferenceApiId:
    		{
    		TInt heldCallId = VA_ARG(aList, TInt);
    		TInt secondCallId = VA_ARG(aList, TInt);
    		TMockLtsyData2<TInt, TInt> data(heldCallId, secondCallId);
  			return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlMultipartyConferenceSwap::KLtsyDispatchCallControlMultipartyConferenceSwapApiId:
    		{
    		TInt heldCallId = VA_ARG(aList, TInt);
    		TInt connectedCallId = VA_ARG(aList, TInt);
    		TMockLtsyData2<TInt, TInt> data(heldCallId, connectedCallId);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlMultipartyConferenceGoOneToOne::KLtsyDispatchCallControlMultipartyConferenceGoOneToOneApiId:
    		{
    		TInt callId = VA_ARG(aList, TInt);
    		TMockLtsyData1<TInt> data(callId);
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
    } // CMockCallControlMultipartyMessHandler::ExtFuncL

/**
 * Complete a request
 */
void CMockCallControlMultipartyMessHandler::CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult)
	{
	switch(aIpc)
		{
		case MLtsyDispatchCallControlMultipartyConferenceHangUp::KLtsyDispatchCallControlMultipartyConferenceHangUpApiId:
		    {
			iCompletionCallback.CallbackCallControlMultipartyConferenceHangUpComp(aResult);
			}
			break;
		case MLtsyDispatchCallControlMultipartyConferenceAddCall::KLtsyDispatchCallControlMultipartyConferenceAddCallApiId:
		    {
			iCompletionCallback.CallbackCallControlMultipartyConferenceAddCallComp(aResult);
			}
			break;
		case MLtsyDispatchCallControlMultipartyCreateConference::KLtsyDispatchCallControlMultipartyCreateConferenceApiId:
		    {
			iCompletionCallback.CallbackCallControlMultipartyCreateConferenceComp(aResult);
			}
			break;
		case MLtsyDispatchCallControlMultipartyConferenceSwap::KLtsyDispatchCallControlMultipartyConferenceSwapApiId:
		    {
			iCompletionCallback.CallbackCallControlMultipartyConferenceSwapComp(aResult);
			}
			break;
		case MLtsyDispatchCallControlMultipartyConferenceGoOneToOne::KLtsyDispatchCallControlMultipartyConferenceGoOneToOneApiId:
		    {
		    TMockLtsyData1Buf<TInt> data;
		    data.DeserialiseL(aData);
			iCompletionCallback.CallbackCallControlMultipartyConferenceGoOneToOneComp(aResult, data.Data1());
			}
			break;
		default:
			{
			// Shouldn't get here. will panic MessageManager()->Complete if allowed to continue
			ASSERT(NULL);
			}
			break;
		}
	} // CMockCallControlMultipartyMessHandler::CompleteL
