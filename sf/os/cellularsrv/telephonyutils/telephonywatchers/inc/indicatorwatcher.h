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

#ifndef __INDICATORWATCHER_H__
#define __INDICATORWATCHER_H__

// System includes
#include <e32base.h>

// Base include
#include "watcherbase.h"

// Local include
#include "callstatewatcher.h"

//
// ------> CIndicatorWatcher (header)
//
class CIndicatorWatcher : public CPhoneWatcher
/**
@internalComponent
*/
	{
//
public: // STATIC CONSTRUCT / DESTRUCT
//
	static CIndicatorWatcher* NewL(TAny* aWatcherParams);
	~CIndicatorWatcher();

//
private: // CONSTRUCT
//
	CIndicatorWatcher();
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
	enum TIndicatorState
		{
		EIndicatorNotYetInitialised = 0,
		EIndicatorRequestInitialIndicator,
		EIndicatorWaitingForInitialIndicator,
		EIndicatorIssuingIndicatorChangeNotification
		};
	inline TIndicatorState& IndicatorState();
	void HandleIndicatorUpdateL(TInt aCompletionCode);

	TInt GetCallInProgressInfo();
	void IndicatorPanic(TWatcherPanic aPanicNumber);

//
private: // MEMBER DATA
//
	TIndicatorState iState;
	TUint32 iIndicatorInfo;
	CCallStateWatcher* iCallStateWatcher;
	RProperty iCurrentCallProperty;
	RProperty iChargerStatusProperty;
	RProperty iNetworkStatusProperty;
	};


inline CIndicatorWatcher::TIndicatorState& CIndicatorWatcher::IndicatorState() { return iState; }

#endif
