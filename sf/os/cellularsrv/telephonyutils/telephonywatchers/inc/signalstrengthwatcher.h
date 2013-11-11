// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SIGNALSTRENGTHWATCHER_H__
#define __SIGNALSTRENGTHWATCHER_H__

// System includes
#include <e32base.h>
#include <etelext.h>
#include <etelmm.h>
#include <etel.h>

// Base include
#include "watcherbase.h"

// Classes referenced
class RFs;


//
// ------> CSignalStrengthWatcher (header)
//
class CSignalStrengthWatcher : public CPhoneWatcher
/**
@internalComponent
*/
	{
//
public: // STATIC CONSTRUCT / DESTRUCT
//
	static CSignalStrengthWatcher* NewL(TAny* aWatcherParams);
	~CSignalStrengthWatcher();

//
private: // CONSTRUCT
//
	CSignalStrengthWatcher();
	void ConstructL();

//
private: // FROM CPhoneWatcher
//
	void HandlePhoneStateEventL(TInt aCompletionCode);
	void HandleCancel();
	void ReleasePhoneResources();

//
private: // INTERNAL
//
	enum TSignalStrengthState
		{
		ESignalStrengthNotYetInitialised = 0,
		ESignalStrengthStateRequestInitialSignalStrength,
		ESignalStrengthWaitingForInitialSignalStrength,
		ESignalStrengthStateIssuingSignalChangeNotificationRequest
		};
	inline TSignalStrengthState& SignalStrengthState() { return iState; }
	void HandleSignalStrengthUpdateL(TInt aCompletionCode);
	void SignalPanic(TWatcherPanic aPanicNumber);

//
private: // MEMBER DATA
//
	TInt32 iSignalStrength;
	TInt8 iSignalBars;
	TSignalStrengthState iState;
	RProperty iNetworkStrengthProperty;	
	};


#endif
