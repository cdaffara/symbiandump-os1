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
// Helper process header file for the ESOCK PlatSec tests
// 
//

/**
 @file
 @internalAll PlatSec test code for the ESOCK
*/

#ifndef     __ESOCK_PLATSEC_TEST_HELPER_H__
#define     __ESOCK_PLATSEC_TEST_HELPER_H__

#include "EsockPlatSecTest.h"


/**
This class processes messages from the host process and performs some test actions as a result.
*/
class CIPMsgProcessor : public CSyncMessageHandler
{

public:

 static CIPMsgProcessor* NewL(const TDesC &aMsgQueueName);

protected:
            CIPMsgProcessor() : CSyncMessageHandler() {}



 virtual    TInt ProcessMessage(const TIPMessage& aMsg);

            void TestTransferSocketL(const TDesC& aSocketName);
			void ProcessTestMessageL(const TIPMessage& aMsg);
            void TestConnectionCloningL(const TDesC& aConnName);
            void TestSetParametersL(RSubConnection::TSubConnType aConnectionType, const TDesC& aConnName);
            void TestWaitForSubConnectionL(const TDesC& aConnName);
};


#endif //__ESOCK_PLATSEC_TEST_HELPER_H__









