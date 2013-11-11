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

#ifndef __CCALLCONTROLMULTIPARTYDISPATCHER_H_
#define __CCALLCONTROLMULTIPARTYDISPATCHER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <e32def.h>
#include <etelmm.h>

#include <ctsy/ltsy/mltsydispatchfactory.h>


#include "requestqueueoneshot.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "mdispatchercallback.h"
#include <ctsy/pluginapi/mmmessagemanagercallback.h>

// FORWARD DECLARATIONS
class CMmDataPackage;
class MmMessageManagerCallback;
class TDispatcherHolder;

class MLtsyDispatchCallControlMultipartyConferenceHangUp;
class MLtsyDispatchCallControlMultipartyConferenceAddCall;
class MLtsyDispatchCallControlMultipartyCreateConference;
class MLtsyDispatchCallControlMultipartyConferenceSwap;
class MLtsyDispatchCallControlMultipartyConferenceGoOneToOne;


// CLASS DECLARATION

/**
 * This class is responsible for packing and unpacking data belonging
 * to CallControlMultiparty related requests to the Licensee LTSY.
 */
class CCallControlMultipartyDispatcher : public CBase, public MDispatcherCallback
	{
public:

	virtual ~CCallControlMultipartyDispatcher();

	static CCallControlMultipartyDispatcher* NewL(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot);

	static CCallControlMultipartyDispatcher* NewLC(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot);

	// Dispatcher functions for dispatching requests DOWN to the Licensee LTSY

	TInt DispatchConferenceHangUpL(const CMmDataPackage* aDataPackage);
	TInt DispatchConferenceAddCallL(const CMmDataPackage* aDataPackage);
	TInt DispatchCreateConferenceL(const CMmDataPackage* aDataPackage);
	TInt DispatchConferenceSwapL(const CMmDataPackage* aDataPackage);
	TInt DispatchConferenceGoOneToOneL(const CMmDataPackage* aDataPackage);

	// Complete functions for receiving completions UP from the Licensee LTSY
	// via the CCtsyDispatcherCallback object.

	void CallbackConferenceHangUp(TInt aError);
	void CallbackConferenceAddCall(TInt aError);
	void CallbackCreateConference(TInt aError);
	void CallbackConferenceSwap(TInt aError);
	void CallbackConferenceGoOneToOne(TInt aError, TInt aCallId);

	// From MDispatcherCallback
	void CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage);

	// Other public functions
	void SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder);

private:

	CCallControlMultipartyDispatcher(MLtsyDispatchFactoryV1& aLtsyFactory,
	   			  	 MmMessageManagerCallback& aMessageManagerCallback,
	   			  	 CRequestQueueOneShot& aRequestAsyncOneShot);

	void ConstructL();

private:	// Not owned

	MLtsyDispatchFactoryV1& iLtsyFactoryV1;
	MmMessageManagerCallback& iMessageManagerCallback;
	CRequestQueueOneShot& iRequestAsyncOneShot;
	TDispatcherHolder* iDispatcherHolder;

	// Interfaces in the Licensee LTSY, not owned by this object

    MLtsyDispatchCallControlMultipartyConferenceHangUp* iLtsyDispatchCallControlMultipartyConferenceHangUp;
    MLtsyDispatchCallControlMultipartyConferenceAddCall* iLtsyDispatchCallControlMultipartyConferenceAddCall;
    MLtsyDispatchCallControlMultipartyCreateConference* iLtsyDispatchCallControlMultipartyCreateConference;
    MLtsyDispatchCallControlMultipartyConferenceSwap* iLtsyDispatchCallControlMultipartyConferenceSwap;
    MLtsyDispatchCallControlMultipartyConferenceGoOneToOne* iLtsyDispatchCallControlMultipartyConferenceGoOneToOne;

	}; // class CCallControlMultipartyDispatcher

#endif // __CCALLCONTROLMULTIPARTYDISPATCHER_H_

