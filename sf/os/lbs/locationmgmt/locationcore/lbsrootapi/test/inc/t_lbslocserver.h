/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file LbsRootApiTest0Step.h
*/
#if (!defined __T_LBSGPSLOCMANAGER_STEP_H__)
#define __T_LBSGPSLOCMANAGER_STEP_H__

#include "lbsrootapi.h"
#include "lbstimer.h"
#include "te_lbsrootapitestsuitedefs.h"

class CTLbsLocServer : public CActive, 
                           public MLbsCallbackTimerObserver
	{
public:
    static CTLbsLocServer* NewL();
	~CTLbsLocServer();

public: // From CActive
    void RunL();
    void DoCancel();

protected: // from MLbsCallbackTimerObserver
	void OnTimerEventL(TInt aTimerId);
	TInt OnTimerError(TInt aTimerId, TInt aError);


protected:
	CTLbsLocServer();
    void ConstructL();    
    
private:
    CLbsCallbackTimer* iTimer;
    RLbsSystemController iLbsRootProcessApi;
 	};

#endif
