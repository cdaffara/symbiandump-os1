/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Declaration of CSsmPowerSup class.
*
*/

#ifndef C_SSMPOWERSUP_H
#define C_SSMPOWERSUP_H

#include <e32base.h>
#include <ssm/ssmadaptationcli.h>
#include <ssm/ssmutility.h>

/**
* SSM Utility plugin listening for DOS originated reset and shutdown events.
*/
NONSHARABLE_CLASS( CSsmPowerSup ) : public CActive, public MSsmUtility
    {

public:

    /**
    * Two-phase constructor.
    */
	IMPORT_C static MSsmUtility* NewL();

    /**
    * Destructor.
    */
    virtual ~CSsmPowerSup();

private: // From CActive

	void RunL();
	void DoCancel();

private: // From MSsmUtility

    void InitializeL();
	void StartL();
	void Release();

private:

    /**
    * First phase constructor.
    */
    CSsmPowerSup();

    /**
    * Activate the active object.
    */
    void Activate();

private: // data

    /** Session with adaptation server. */
    RSsmStateAdaptation iAdaptation;

    /** Event data will be received here. */
	TPckgBuf<TSsmCoopSysEventType> iEventBuf;

    };

#endif // C_SSMPOWERSUP_H
