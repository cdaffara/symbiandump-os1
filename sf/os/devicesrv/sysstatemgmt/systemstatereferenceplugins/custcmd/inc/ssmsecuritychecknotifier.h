/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Declaration of CStrtSecurityNote class.
*
*/




/**
 @file
 @internalComponent
 @released
*/

#ifndef __SSMSECURITYCHECKNOTIFIER_H__
#define __SSMSECURITYCHECKNOTIFIER_H__

#include <ssm/ssmcustomcommand.h>
#include <ssm/ssmcmd.hrh>
#include "ssmpanic.h"

#include "strtsecuritynotetype.h"
#include "securitynotification.h"
#include "e32property.h"

NONSHARABLE_CLASS (CSsmSecurityCheckNotifier) : public CActive
	{
public:
	static CSsmSecurityCheckNotifier* NewL();
	static CSsmSecurityCheckNotifier* NewL(TStrtSecurityNoteType aNoteType);

	~CSsmSecurityCheckNotifier();
	TBool IsCodeAccepted();
	void ShowNoteL(TStrtSecurityNoteType aNoteType, TRequestStatus& aRequest);
	void ShowNoteL(TRequestStatus& aRequest);
	TInt SecurityCheckResult() const;
protected:
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

	void ConstructL();
	void CleanupAndCancel(TInt aError);

private:
	CSsmSecurityCheckNotifier();
	CSsmSecurityCheckNotifier(TStrtSecurityNoteType aNoteType);
	void StartNotifier();
	TBool IsDlgCancellableL();

private:
	enum TPinCustomCmdState
        {
        EIdle = 1,
        ENotifierIsActive,
        EEmergencyCallIsActive
        };

    // Indicates whether its waiting for response from security notifier or
    // an emergency call to finish.
    TPinCustomCmdState iCmdState;
	TPckgBuf<TInt> iPinResult;
	RProperty iSsmEmergencyCallProperty;
	RNotifier iSecurityPinNotifier;
	TBool iIsDlgCancellable;
	TRequestStatus* iExecuteRequest;
	TStrtSecurityNoteType iNoteType;
	TBool iCodeOk;
	TUid iSecurityPinNotifierUid;

	// Indicates whether the query is made during or after startup. The
	// information affects for instance whether the query can be cancelled.
	// This value is set when a new query is made.
	TBool iAfterStartup;
	
    // Prameters for SecurityNotifier.
    TSecurityNotificationPckg iParams;
	};

#endif // __SSMSECURITYCHECKNOTIFIER_H__
