// SensibleClient_internals.h
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef SENSIBLECLIENT_INTERNALS_H
#define SENSIBLECLIENT_INTERNALS_H

#include <e32base.h>
#include "cliserv.h"
#include "sensibleclient.h"
#include <gdi.h>

NONSHARABLE_CLASS(CServerCallbackDispatcher) : public CActive
	{
public:
	CServerCallbackDispatcher(RSensibleSessionBody& aSession);
	~CServerCallbackDispatcher();
	void Register();

private:
	void RunL();
	void DoCancel();
	
private:
	RSensibleSessionBody& iSession;
	TPckgBuf<TServerCallback> iNextCallback;
	HBufC8* iCachedCallbackResult8;
	HBufC16* iCachedCallbackResult16;
	HBufC8* iContext;
	TPtr8 iContextPtr; // Stupid client-server...

	enum TState { EWaitingForCallback, EWaitingForContext };
	TState iState;
	};

/*
// An active object that holds on to a bunch of pointers, and frees them when its
// iStatus is completed. Useful for passing temporary descriptors via asyncronous client-
// -server (Otherwise you'd have to ensure yourself that they didn't go out of scope before
// the server had read from them).
// It can optionally pass on the completion message to another TRequestStatus
class CCleanupAndComplete : public CActive
	{
public:
	static CCleanupAndComplete* NewLC(TRequestStatus* aClientStatus);
	static TInt Create(TRequestStatus*& aStatus, const TDesC16& aDes, TInt& aIpcArg); // Non-leaving convenience functions
	static TInt Create(TRequestStatus*& aStatus, const TDesC8& aDes, TInt& aIpcArg); // Non-leaving convenience functions
	void SetActive(); // Declare this so RDasherSession etc can activate us
	void RunL();
	void DoCancel();
	CCleanupAndComplete(TRequestStatus* aClientStatus);
	void AddL(CBase* aWithDestructor);
	void AddL(TAny* aWithoutDestructor);
	~CCleanupAndComplete();
	
public:
	RPointerArray<CBase> iWithDestructor;
	RArray<TAny*> iWithoutDestructor;

private:
	TRequestStatus* iClientStatus;
	};
*/

// A helper class to parse arguments out of a callback from the server
class TCallbackParser
	{
public:
	TCallbackParser(const TServerCallback& aCallback);
	void SetContext(TDesC8& aContext);

	TInt NextInt();
	TUint NextUint();
	TPoint NextPoint();
	TSize NextSize();
	TRgb NextRgb();
	TRect NextRect();
	TPtrC NextDesC();
	TPtrC8 NextDesC8();

private:
	TAny* Next(TInt aSize, char* aType);

private:
	const TServerCallback& iCallback;
	TDesC8* iContext;
	const TUint8* iNextPtr;
	TBool iInContext;
	};

#endif
