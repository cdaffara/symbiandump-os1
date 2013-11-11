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

class CTLbsGpsLocManager : public CActive, 
                           public MLbsProcessCloseDown
	{
public:
    static CTLbsGpsLocManager* NewL();
	~CTLbsGpsLocManager();

public: // From CActive
    void RunL();
    void DoCancel();

public: // From CTLbsGpsLocManager
    void OnProcessCloseDown();

public:  // From MLbsProcessPanic    
    void PanicMe();

protected:
	CTLbsGpsLocManager();
    void ConstructL();    
    
private:
	CLbsCloseDownRequestDetector* iCloseDownRequestDetector;
    RProperty   iPanicRequest;
	};

#endif
