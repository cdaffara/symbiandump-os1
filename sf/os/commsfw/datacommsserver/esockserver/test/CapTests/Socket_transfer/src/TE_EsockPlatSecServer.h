// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test server header file for the ESOCK PlatSec tests
// 
//

/**
 @file 
 @internalAll PlatSec test code for the ESOCK
*/

#ifndef __TE_ESOCK_PLATSEC_SERVER_H__
#define __TE_ESOCK_PLATSEC_SERVER_H__

#include <test/testexecuteserverbase.h>
#include <es_sock.h>
#include <in_sock.h>


class CEsockPlatSecTestServer : public CTestServer
    {
    public:
        static  CEsockPlatSecTestServer* NewL();
        virtual CTestStep* CreateTestStep(const TDesC& aStepName);
        
        
    };



#endif //__TE_ESOCK_PLATSEC_SERVER_H__

