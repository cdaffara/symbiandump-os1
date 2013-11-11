// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of class used to shut down LBS after a specified time
// 
//

#ifndef __PRIVACYSHUTDOWN_H__
#define __PRIVACYSHUTDOWN_H__

#include <e32base.h>


/**
CPrivacyShutdown
Used to shut down LBS after the specified time
*/
class CPrivacyShutdown : public CTimer
	{
public:
	enum TShutdownState
		{
		EShutdownStateIdle = 0,
		EShutdownStateTimerStarted,
		EShutdownStateShutdownRequested
		};

	static CPrivacyShutdown* NewL();

	~CPrivacyShutdown();
	
	void Start(const TTimeIntervalMicroSeconds32 aDelay);
	
	TShutdownState State() const;
	
protected: 

	// from CActive

	void RunL();
	void DoCancel();

private:

	CPrivacyShutdown();
	void ConstructL();
	
private:
	
	TShutdownState iState;
	TTimeIntervalMicroSeconds32 iTimerCount;
	};
	
#endif // __PRIVACYSHUTDOWN_H__
