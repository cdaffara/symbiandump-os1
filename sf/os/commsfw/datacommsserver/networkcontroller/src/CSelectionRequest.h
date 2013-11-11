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
 @file CSelectionRequest.h
 @internalComponent
*/

#if !defined (__CSELECTIONREQUEST_H__)
#define __CSELECTIONREQUEST_H__

#include "CNetConRequestBase.h"
#include <commdbconnpref.h>
#include <comms-infras/nifif.h>

NONSHARABLE_CLASS(CSelectionRequest) : public CNetConRequestBase
/**
The class controls the selection of connection settings such as Modem, Location and IAP.

@internalComponent
*/
	{
public:
	static CSelectionRequest* NewL(MNetConEnv* aController, MNetworkControllerObserver* aObserver, TConnStartType aStartType, const TConnPref& aPrefs, TInt aConnectionAttempt, TInt aLastConnectionError);
	virtual ~CSelectionRequest();

	/** start processing this request */
	virtual void StartRequest();

	/** force premature completion of a request with a given error */
	virtual void RequestComplete(TInt aError);

	/** bearer availability */
	virtual void SetAvailableBearers(TUint32 aBearerSet);

	/** override part of the MDialogProcessorObserver interface */
	virtual void MDPOSelectComplete(TInt aError, const TConnectionSettings& aSettings);

	/** override part of the MDialogProcessorObserver interface */
	virtual void MDPOWarnComplete(TInt aError, TBool aResponse);

	// accessors 
	const TDesC& AgentName() const;
	const TPckgBuf<TSoIfConnectionInfo>& AgentConnectionInfo() const;
	const TConnStartType& ConnectionStartType() const;
	      TUint32 ConnectionAttempt() const;
	const TConnPref& ConnPrefs() const;

private:
	CSelectionRequest(MNetConEnv* aController, MNetworkControllerObserver* aObserver, TConnStartType aDirection, const TConnPref& aPrefs, TInt aConnectionAttempt, TInt aLastConnectionError);
	void ConstructL();

	void StartSelectL();
	CStoreableOverrideSettings* MapPrefsToOverridesL(TCommDbConnPref& aPrefs, const TConnectionSettings& aSettings);
	void SelectIAPCompleteL(const TConnectionSettings& aSettings);
	void CheckBearerAvailabilityL();
	TBool ServiceAvailableInAlternateMode(TUint32& aBearerSet, const TUint32 aAvailableBearerSet);
	void SelectIapL(const TBool aSelectedServiceAvailable, const TUint32 aAvailableBearerSet);
	void SelectAgentL();
	void SetAgentNameL(const TDesC& aName);
	TInt ExtractCommDbConnPref(const TConnPref& aBasePref, TCommDbConnPref& aCommDbPref);

private:
	TConnStartType iConnStartType;
	TConnPref iConnPrefs;
	TInt iConnectionAttempt;
	TInt iLastConnectionError;
	HBufC* iAgentName;
	TPckgBuf<TSoIfConnectionInfo> iAgentConnInfo;
	TBool iDoBearerAvailability;
	};

#endif // __CSELECTIONREQUEST_H__

