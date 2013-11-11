// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(SAVENOTF_H)
#define SAVENOTF_H

#include <e32std.h>
#include <e32base.h>

/**
This interface has to be implemented by all clients, which want to be notified about events
like shutdown/low memory. When this happens the implementation of MSaveObserver::SaveL() will
be called.
@see CSaveNotifier
@publishedPartner
@released
*/
class MSaveObserver
	{
public:
	enum TSaveType
		{
		ESaveNone,
		ESaveData,
		ESaveAll,
		ESaveQuick,
		EReleaseRAM,
		EReleaseDisk,
		};
public:
	virtual void SaveL(TSaveType aSaveType)=0;
	};

/**
This class describes a client side session object, handling requests
to the shutdown server.
@internalComponent
*/
class RSaveSession : public RSessionBase
	{
public:
	TInt ConnectL();
	void NotifySave(TRequestStatus& aStatus);
	void NotifySaveCancel();
    TInt SwitchOff(MSaveObserver::TSaveType aAction, TBool aPowerOff);
    TInt ServerPowerState(TBool& aPowerOff);
private:
	TInt StartServerL();
	};

/**
This class describes an object, which can be used by the clients, which want to be notified
about events like powerdown/low memory. The clients have to implement MSaveObserver interface.
When a particular event happens, the client's implementation of MSaveObserver::SaveL() will
be called.
@see MSaveObserver
@publishedPartner
@released 
*/
class CSaveNotifier : public CActive
	{
    friend class CPowerdownClient;//test class
    friend class CPowerdownClient2;//test class
public:
	IMPORT_C static CSaveNotifier* NewL(MSaveObserver& aObserver);
	IMPORT_C ~CSaveNotifier();
	IMPORT_C void DelayRequeue();
	IMPORT_C void Queue();
	IMPORT_C void HandleError(TInt /*aError*/);
private:
	IMPORT_C TInt SwitchOff(MSaveObserver::TSaveType aAction, TBool aPowerOff);
	IMPORT_C TInt ServerPowerState(TBool& aPowerOff);
	inline CSaveNotifier(MSaveObserver& aObserver);
	void ConstructL();
	void Start();
private: // from CActive
	void DoCancel();
	void RunL();
private:
	RSaveSession iSaveSession;
	MSaveObserver& iSaveObserver;
	TUint iFlags;
	};

#endif
