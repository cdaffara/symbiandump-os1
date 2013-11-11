// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
*/

#include "TE_SMSEMSPRTBASE.h"

#include <commsdattypesv1_1.h>
#include <sacls.h>
#include <metadatabase.h>

#include "smsstacktestconsts.h"

using namespace CommsDat;

TVerdict CSmsEmsPrtTestStep::doTestStepPreambleL()
	{
	//base class preamble - marks the heap
	CSmsBaseTestStep::doTestStepPreambleL();
	
	iSmsStackTestUtils->OpenSmsSocketL(iSocketServer, iSocket, ESmsAddrRecvAny);

    // Set destination and SC numbers
    iTelephoneNumber=KPekka;
    iServiceCenterNumber=KRadiolinjaSC;
	return TestStepResult();
	}

TVerdict CSmsEmsPrtTestStep::doTestStepPostambleL()
	{
	iSocket.Close();

    //base class postamble - unmarks the heap
    CSmsBaseTestStep::doTestStepPostambleL();

 	return TestStepResult();
	}

 TBool CSmsEmsPrtTestStep::SendReceiveMsgL(CSmsMessage& aMsg)
     {
     // Send SMS
     SendSmsL(&aMsg,iSocket);

     CSmsMessage* rxMsg;

     // Now receive and compare
     WaitForRecvL(iSocket);
     rxMsg = RecvSmsL(iSocket);

     CleanupStack::PushL(rxMsg);
     TBool comp = EmsTestUtils::CompareEmsMsgL(aMsg, *rxMsg);
     TEST(comp);

     // compare OK
     INFO_PRINTF1(_L("Sent and received PDUs compare OK..."));

     // Now make sure the received message is valid - this can be
     // tested by ensuring that NumMessagePDUsL can be called without
     // leaving
     rxMsg->NumMessagePDUsL();

     CleanupStack::PopAndDestroy(rxMsg);

     // return the result of the comparison
     return comp;
     }
