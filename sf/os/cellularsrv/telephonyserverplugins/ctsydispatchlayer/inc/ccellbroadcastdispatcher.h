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

#ifndef __CCELLBROADCASTDISPATCHER_H_
#define __CCELLBROADCASTDISPATCHER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <e32def.h>
#include <etelmm.h>

#include <ctsy/ltsy/mltsydispatchfactory.h>
#include <ctsy/rmmcustomapi.h>
#include <ctsy/serviceapi/cmmsmsutility.h>

#include <ctsy/ltsy/mltsydispatchcellbroadcastinterface.h>

#include "requestqueueoneshot.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "mdispatchercallback.h"

// FORWARD DECLARATIONS
class CMmDataPackage;
class MmMessageManagerCallback;
class TDispatcherHolder;

class MLtsyDispatchCellBroadcastSetBroadcastFilterSetting;
class MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage;
class MLtsyDispatchCellBroadcastReceiveMessageCancel;
class MLtsyDispatchCellBroadcastStartSimCbTopicBrowsing;
class MLtsyDispatchCellBroadcastDeleteSimCbTopic;


// CLASS DECLARATION

/**
 * This class is responsible for packing and unpacking data belonging
 * to CellBroadcast related requests to the Licensee LTSY.
 */
class CCellBroadcastDispatcher : public CBase, public MDispatcherCallback
	{
public:

	virtual ~CCellBroadcastDispatcher();
	
	static CCellBroadcastDispatcher* NewL(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot);
	
	static CCellBroadcastDispatcher* NewLC(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot);
	
	// Dispatcher functions for dispatching requests DOWN to the Licensee LTSY
	
	TInt DispatchSetBroadcastFilterSettingL(const CMmDataPackage* aDataPackage);
	TInt DispatchActivateBroadcastReceiveMessageL(const CMmDataPackage* aDataPackage);
	TInt DispatchReceiveMessageCancelL(const CMmDataPackage* aDataPackage);
	TInt DispatchStartSimCbTopicBrowsingL();
	TInt DispatchDeleteSimCbTopicL(const CMmDataPackage* aDataPackage);		
	
	// Complete functions for receiving completions UP from the Licensee LTSY
	// via the CCtsyDispatcherCallback object.
	
	void CallbackGsmBroadcastNotifyMessageReceived(TInt aError, const TDesC8& aCbsMsg);
	void CallbackWcdmaBroadcastMessageReceived(TInt aError, const TDesC8& aWcdmaCbsData, const DispatcherCellBroadcast::TWcdmaCbsMsgBase& aWcdmaCbsMsgBase, TBool aMoreToCome);
	void CallbackSetBroadcastFilterSetting(TInt aError);
	void CallbackActivateBroadcastReceiveMessage(TInt aError);
	void CallbackReceiveMessageCancel(TInt aError);
	void CallbackStartSimCbTopicBrowsing(TInt aError, const CArrayFixFlat<RMmCustomAPI::TSimCbTopic>& aSimTopicArray);
	void CallbackDeleteSimCbTopic(TInt aError);
			
	// From MDispatcherCallback
	void CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage);

	// Other public functions
	void SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder);
	
private:

	CCellBroadcastDispatcher(MLtsyDispatchFactoryV1& aLtsyFactory,
	   			  	 MmMessageManagerCallback& aMessageManagerCallback,
	   			  	 CRequestQueueOneShot& aRequestAsyncOneShot);		
	
	void ConstructL();
	
private:	// Not owned

	MLtsyDispatchFactoryV1& iLtsyFactoryV1;
	MmMessageManagerCallback& iMessageManagerCallback;
	CRequestQueueOneShot& iRequestAsyncOneShot;
	TDispatcherHolder* iDispatcherHolder;
	
	// Interfaces in the Licensee LTSY, not owned by this object
	
    MLtsyDispatchCellBroadcastSetBroadcastFilterSetting* iLtsyDispatchCellBroadcastSetBroadcastFilterSetting;
    MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage* iLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage;
    MLtsyDispatchCellBroadcastReceiveMessageCancel* iLtsyDispatchCellBroadcastReceiveMessageCancel;
    MLtsyDispatchCellBroadcastStartSimCbTopicBrowsing* iLtsyDispatchCellBroadcastStartSimCbTopicBrowsing;
    MLtsyDispatchCellBroadcastDeleteSimCbTopic* iLtsyDispatchCellBroadcastDeleteSimCbTopic;
    
    // Received WCDMA CBS message. Not own.
	CArrayPtrFlat< TWcdmaCbsMsg >* iCbsMsg;
       
	}; // class CCellBroadcastDispatcher

#endif // __CCELLBROADCASTDISPATCHER_H_
	
