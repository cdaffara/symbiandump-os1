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
#include "te_lbsrootapiTestSuiteDefs.h"

class CTLbsNetworkRequestHandler : public CActive,
                           public MLbsProcessCloseDown,
                           public MLbsProcessReset
                           
	{
public:
    static CTLbsNetworkRequestHandler* NewL();
	~CTLbsNetworkRequestHandler();

public: // From CActive
    void RunL();
    void DoCancel();

public: // From MLbsProcessCloseDown
    void OnProcessCloseDown();

public: // From MLbsProcessReset
    void OnProcessReset();

public:  // From MLbsProcessPanic    
    void PanicMe();

protected:
	CTLbsNetworkRequestHandler();
    void ConstructL();    
    
private:
	CLbsCloseDownRequestDetector* iCloseDownRequestDetector;
    RProperty                     iPanicOnCloseDownRequest;
    RProperty                     iResetFunctionCalled;
    TBool                         iPanicOnCloseDown;
	};

#endif
