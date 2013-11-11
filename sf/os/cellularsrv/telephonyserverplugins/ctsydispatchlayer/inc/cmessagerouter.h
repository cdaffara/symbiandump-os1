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

#ifndef __CMESSAGEROUTER_H_
#define __CMESSAGEROUTER_H_

//#include "mmockmesshandlerbase.h"
#include <ctsy/pluginapi/mmessagerouter.h>
#include <ctsy/pluginapi/mmmessagemanagercallback.h>


// Forward declarations
class MLtsyDispatchFactoryV1;

class CCtsyDispatcherCallback;

class CCallControlDispatcher;
class CPhoneDispatcher;
class CSecurityDispatcher;
class CPhonebookEnDispatcher;
class CCellBroadcastDispatcher;
class CPhonebookOnDispatcher;
class CPhonebookDispatcher;
class CSimDispatcher;
class CSmsDispatcher;
class CCallControlMultipartyDispatcher;
class CSupplementaryServicesDispatcher;
class CPacketServicesDispatcher;
class CSatDispatcher;

class CMmDataPackage;
class CRequestQueueOneShot;
class TDispatcherHolder;

class MStkTsyCallOrigin;

/**
 * Interface to handle the boot sequence.  For each callback that triggers the next stage of
 *  the boot sequence a callback is used that is handled by message router.
 */
class MBootSequenceCallbacks
	{
public:
	virtual void HandleNotifyModemReadyReceivedL() = 0;
	}; // class MBootSequenceCallbacks

/**
 * Proxy class to mimic the MmMessageManagerCallback that is used by all the
 * dispatcher objects. It is used to allow a single place to put break points
 * when debugging the complete calls
 */
class TMessageManagerCallbackProxy : public MmMessageManagerCallback
	{
public:
	TMessageManagerCallbackProxy(MmMessageManagerCallback& aCtsyMessageManagerCallback);
	TMessageManagerCallbackProxy();

	void SetMessageManagerCallbackProxy(MmMessageManagerCallback& aCtsyMessageManagerCallback);

public: //from MmMessageManagerCallback
	virtual void Complete( TInt aIpc, TInt aResult );
	virtual void Complete( TInt aIpc, CMmDataPackage* aData, TInt aResult );

private:
	MmMessageManagerCallback* iCtsyMessageManagerCallback;

	}; // class TMessageManagerCallbackProxy


/**
 * Routes messages arriving from the Common TSY to the Licensee LTSY.
 */
class CMessageRouter : public CBase, public MMessageRouter, public MBootSequenceCallbacks
    {
public:
    static CMessageRouter* NewL(MmMessageManagerCallback& aMessageManagerCallback);
    virtual ~CMessageRouter();

    void SetSatMessageManager(MmMessageManagerCallback& aMessageManagerCallback);
    
    void SetSatCallOriginInterface(MStkTsyCallOrigin& aSatCallOrigin);
    
public: // From MMessageRouter

	virtual TInt ExtFuncL(TInt aIpc, const CMmDataPackage* aDataPackage);

public: //from MBootSequenceCallbacks
	virtual void HandleNotifyModemReadyReceivedL();

private:

	CMessageRouter(MmMessageManagerCallback& aMessageManagerCallback);
    void ConstructL();
    void QuerySupporterCallbackIndicators();
    void RetrieveNetworkInfoL();

private:

    void StartBootSequenceL();

private:    // Owned

	/**
	 * An object to handle completions of requests from the Licensee LTSY.
	 *
	 * The Licensee LTSY keeps a pointer to this object.  When the Licensee LTSY wants
	 * to notify the CTSY Dispatcher of a completion of an IPC, it calls the relevant
	 * callback function in this object passing the completion data.
	 */
	CCtsyDispatcherCallback* iCtsyDispatcherCallback;

	TMessageManagerCallbackProxy iMessageManagerCallbackProxy;

	TMessageManagerCallbackProxy iSatMessageManagerCallbackProxy;

	/**
	 * An object to hold a queue of requests along with their completion data
	 * which are serviced by the Licensee LTSY synchronously.  The Common TSY
	 * expects completion of these in the same way as asynchronous requests,
	 * hence an async one shot object is required to send the completion back
	 * to the Common TSY.
	 */
	CRequestQueueOneShot* iRequestQueueOneShot;

	TDispatcherHolder* iDispatcherHolder;

	// Dispatchers
	// Dispatchers do the actual unpacking and packing of data
	// between the Common TSY and the Licensee LTSY.
	CCallControlDispatcher* iCtsyDispatcherCallControlDispatcher;
	CPhoneDispatcher* iCtsyDispatcherPhoneDispatcher;
	CSecurityDispatcher* iCtsyDispatcherSecurityDispatcher;
	CPhonebookEnDispatcher* iCtsyDispatcherPhonebookEnDispatcher;
	CCellBroadcastDispatcher* iCtsyDispatcherCellBroadcastDispatcher;
	CPhonebookOnDispatcher* iCtsyDispatcherPhonebookOnDispatcher;
	CPhonebookDispatcher* iCtsyDispatcherPhonebookDispatcher;
	CSimDispatcher* iCtsyDispatcherSimDispatcher;
	CSmsDispatcher* iCtsyDispatcherSmsDispatcher;
	CCallControlMultipartyDispatcher* iCtsyDispatcherCallControlMultipartyDispatcher;
	CSupplementaryServicesDispatcher* iCtsyDispatcherSupplementaryServicesDispatcher;
	CPacketServicesDispatcher* iCtsyDispatcherPacketServicesDispatcher;
	CSatDispatcher* iCtsyDispatcherSatDispatcher;
	RLibrary 		iLoadedLib;
//private: // Not owned
public:
	MLtsyDispatchFactoryV1* iLtsyFactoryV1;

    }; // class CMessageRouter



#endif // __CMESSAGEROUTER_H_

