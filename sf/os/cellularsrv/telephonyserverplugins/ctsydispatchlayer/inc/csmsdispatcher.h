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

#ifndef __CSMSDISPATCHER_H_
#define __CSMSDISPATCHER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <e32def.h>
#include <etelmm.h>

#include <ctsy/ltsy/mltsydispatchfactory.h>

#include <ctsy/ltsy/mltsydispatchsmsinterface.h>

#include "requestqueueoneshot.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "mdispatchercallback.h"

// FORWARD DECLARATIONS
class CMmDataPackage;
class MmMessageManagerCallback;
class TDispatcherHolder;

class MLtsyDispatchSmsSendSatSms;
class MLtsyDispatchSmsGetSmsStoreInfo;
class MLtsyDispatchSmsGetSmspList;
class MLtsyDispatchSmsNackSmsStored;
class MLtsyDispatchSmsAckSmsStored;
class MLtsyDispatchSmsResumeSmsReception;
class MLtsyDispatchSmsSendSmsMessage;
class MLtsyDispatchSmsSendSmsMessageNoFdnCheck;
class MLtsyDispatchSmsSetMoSmsBearer;
class MLtsyDispatchSmsStoreSmspList;
class MLtsyDispatchSmsStoreSmspListEntry;


// CLASS DECLARATION

/**
 * This class is responsible for packing and unpacking data belonging
 * to Sms related requests to the Licensee LTSY.
 */
class CSmsDispatcher : public CBase, public MDispatcherCallback
	{
public:

	virtual ~CSmsDispatcher();
	
	static CSmsDispatcher* NewL(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot);
	
	static CSmsDispatcher* NewLC(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot);
	
	// Dispatcher functions for dispatching requests DOWN to the Licensee LTSY
	
	TInt DispatchActivateSmsRoutingL();
	TInt DispatchSendSatSmsL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetSmsStoreInfoL();
	TInt DispatchGetSmspListL();
	TInt DispatchNackSmsStoredL(const CMmDataPackage* aDataPackage);
	TInt DispatchAckSmsStoredL(const CMmDataPackage* aDataPackage);
	TInt DispatchResumeSmsReceptionL();
	TInt DispatchSendSmsMessageL(const CMmDataPackage* aDataPackage);
	TInt DispatchSendSmsMessageNoFdnCheckL(const CMmDataPackage* aDataPackage);
	TInt DispatchSetMoSmsBearerL(const CMmDataPackage* aDataPackage);
	TInt DispatchStoreSmspListEntryL(const CMmDataPackage* aDataPackage);
	
	
	// Complete functions for receiving completions UP from the Licensee LTSY
	// via the CCtsyDispatcherCallback object.
	
	void CallbackNotifyReceiveSmsMessage(TInt aError, TBool aInd, const TSmsMsg& aSmsMessage);
	void CallbackSendSatSms(TInt aError);
	void CallbackGetSmsStoreInfo(TInt aError, TInt aTotalEntries, TInt aUsedEntries);
	void CallbackGetSmspList(TInt aError, const TDesC& aServiceCenterAddress, const TDesC& aDestinationAddress,
	           const TDesC& aAlphaTagData, const DispatcherSim::TSmsParameters& aSmsParameters, TBool aMoreToCome);
	void CallbackNackSmsStored(TInt aError);
	void CallbackAckSmsStored(TInt aError);
	void CallbackResumeSmsReception(TInt aError);
	void CallbackSendSmsMessage(TInt aError, TInt aMsgRef, const TDesC8& aSmsSubmitReport);
	void CallbackSendSmsMessageNoFdnCheck(TInt aError, TInt aMsgRef, const TDesC8& aSmsSubmitReport);
	void CallbackSetMoSmsBearer(TInt aError);
	void CallbackStoreSmspListEntry(TInt aError);
	//internal callbacks
	void CallbackActivateSmsRouting(CRequestQueueOneShot::TIpcDataPackage* aDataPackage);
	
	// From MDispatcherCallback
	void CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage);

	// Other public functions
	void SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder);
	
private:

	CSmsDispatcher(MLtsyDispatchFactoryV1& aLtsyFactory,
	   			  	 MmMessageManagerCallback& aMessageManagerCallback,
	   			  	 CRequestQueueOneShot& aRequestAsyncOneShot);		
	
	void ConstructL();
	
private:	// Not owned

	MLtsyDispatchFactoryV1& iLtsyFactoryV1;
	MmMessageManagerCallback& iMessageManagerCallback;
	CRequestQueueOneShot& iRequestAsyncOneShot;
	TDispatcherHolder* iDispatcherHolder;
	
	// Interfaces in the Licensee LTSY, not owned by this object
	
    MLtsyDispatchSmsSendSatSms* iLtsyDispatchSmsSendSatSms;
    MLtsyDispatchSmsGetSmsStoreInfo* iLtsyDispatchSmsGetSmsStoreInfo;
    MLtsyDispatchSmsGetSmspList* iLtsyDispatchSmsGetSmspList;
    MLtsyDispatchSmsNackSmsStored* iLtsyDispatchSmsNackSmsStored;
    MLtsyDispatchSmsAckSmsStored* iLtsyDispatchSmsAckSmsStored;
    MLtsyDispatchSmsResumeSmsReception* iLtsyDispatchSmsResumeSmsReception;
    MLtsyDispatchSmsSendSmsMessage* iLtsyDispatchSmsSendSmsMessage;
    MLtsyDispatchSmsSendSmsMessageNoFdnCheck* iLtsyDispatchSmsSendSmsMessageNoFdnCheck;
    MLtsyDispatchSmsSetMoSmsBearer* iLtsyDispatchSmsSetMoSmsBearer;
    MLtsyDispatchSmsStoreSmspListEntry* iLtsyDispatchSmsStoreSmspListEntry;
    
    CArrayPtrFlat< TSmsParameters >* iLtsyDataList;   

	}; // class CSmsDispatcher

#endif // __CSMSDISPATCHER_H_
	
