/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file CNetConRequestBase.h
 @internalComponent
*/

#if !defined (__CNETCONREQUESTBASE_H__)
#define __CNETCONREQUESTBASE_H__

#include <e32base.h>
#include <comms-infras/dialogprocessor.h>
#include <comms-infras/dbaccess.h>

class MNetConDialogProcAccess;
class MCommsDbAccess;
class MNetConEnv;
class MNetworkControllerObserver;
class MServiceChangeObserver;
class CStoreableOverrideSettings;
class CCommsDbAccess;

class CNetConDlgProcessor;
NONSHARABLE_CLASS(CNetConRequestBase) : public CBase, public MDialogProcessorObserver
/**
This class provides the functionality common to both selection and reconnection requests.

@internalComponent
*/
	{
	friend class CNetworkController;
public:
	virtual ~CNetConRequestBase();

	/**
	Start processing this request
	*/
	virtual void StartRequest() = 0;

	/**
	force premature completion of a request with a given error

	@param aError,error code for premature completion of request
	*/
	virtual void RequestComplete(TInt aError) = 0;
	/**
	Cancel processing this request
	*/	
	virtual void CancelRequest();

	/**
	Bearer availability

	@param aBearerSet, set of bearer available.
	*/
	virtual void SetAvailableBearers(TUint32 aBearerSet) = 0;

	/**
 	Return a copy of the pointer to the CCommsDbAccess object allowing reading of CommDb
 	@return CCommsDbAccess
 	*/
	CCommsDbAccess* DbAccess();

	CStoreableOverrideSettings* Overrides() const;
	MNetworkControllerObserver* Observer() const;

protected:
	CNetConRequestBase(MNetConEnv* aController, MNetworkControllerObserver* aObserver, CStoreableOverrideSettings* aOverrides);
	void ConstructL();

	TBool SelectedServiceAvailable(TUint32 aAvailableBearerSet) const;

	// implementation of Dialog Accessors
	void SelectConnection(const TConnectionPrefs& aPrefs);
	void SelectConnection(const TConnectionPrefs& aPrefs, TInt aLastError);
	void SelectModemAndLocation();
	void WarnNewConnection(const TConnectionPrefs& aPrefs, const TDesC* aNewIapName, const TIspConnectionNames* aNewConnectionNames, TInt aLastError);
	void Reconnect();

private:
	TInt ConstructDialogProcessor();

protected:

	CCommsDbAccess* iDatabase;							///< Network controller database access

	MNetConEnv* const iController;						///< For upcalls from the requests
	MNetworkControllerObserver* const iObserver;		///< from a Network Controller towards NIFMAN
	CStoreableOverrideSettings* iOverrides;		///< CommDB overrides
	TConnectionSettings iSettings; 						///< Connection settings of network controller towards nifman.
	TSglQueLink iLink;									///< link to next element in the queue
	MServiceChangeObserver* ipServiceChangeObserver;	///< Requests service change notification

private:
	CNetConDlgProcessor *iDialogPrc;					///< Dialog processor access
	};

#endif  // __CNETCONREQUESTBASE_H__

