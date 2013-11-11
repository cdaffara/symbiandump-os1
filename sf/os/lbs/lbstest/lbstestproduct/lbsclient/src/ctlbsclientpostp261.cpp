// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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



//  INCLUDES
#include "ctlbsclientpostp261.h"
#include <s32strm.h>
#include <s32mem.h>

// CONSTANTS

// LOCAL FUNCTIONS
void CleanOp(TAny* aAnArray)
    {
    // Cleanup function the requestor stack
    RRequestorStack* theStack = reinterpret_cast <RRequestorStack*> (aAnArray);
    theStack->ResetAndDestroy();
    }

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp261::CT_LbsClientPosTp261(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{  
	_LIT(KTestName,"Tp261 - RequestorStack Externalize/Internalize");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp261::~CT_LbsClientPosTp261()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp261::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp261::CloseTest()
    {
    ClosePositioner();
    Disconnect();
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp261::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp261::StartL()
    {
    _LIT(KService1, "Service1");
    _LIT(KService2, "Service2");
    _LIT(KService3, "Service3");

    _LIT(KContact1, "Contact1");
    _LIT(KContact2, "Contact2");
    _LIT(KContact3, "Contact3");

    CRequestor* serviceId1 = 
		CRequestor::NewL(CRequestor::ERequestorService, CRequestor::EFormatApplication, 
                    KService1);

    CRequestor* serviceId2 = 
        CRequestor::NewL(CRequestor::ERequestorService, CRequestor::EFormatApplication, 
                    KService2);

    CRequestor* serviceId3 = 
        CRequestor::NewL(CRequestor::ERequestorService, CRequestor::EFormatApplication, 
                    KService3);

    CRequestor* contactId1 = 
        CRequestor::NewL(CRequestor::ERequestorContact, CRequestor::EFormatTelephone, 
                    KContact1);

    CRequestor* contactId2 = 
        CRequestor::NewL(CRequestor::ERequestorContact, CRequestor::EFormatUrl, 
                    KContact2);

    CRequestor* contactId3 = 
        CRequestor::NewL(CRequestor::ERequestorContact, CRequestor::EFormatMail, 
                    KContact3);

    RRequestorStack stack = RRequestorStack();
    CleanupStack::PushL(TCleanupItem(CleanOp ,&stack));

    CleanupStack::PushL(serviceId1);
    CleanupStack::PushL(serviceId2);
    CleanupStack::PushL(serviceId3);
    CleanupStack::PushL(contactId1);
    CleanupStack::PushL(contactId2);
    CleanupStack::PushL(contactId3);
    
    ConnectL();
    OpenPositioner();
    
    stack.Append(serviceId1);
    stack.Append(contactId1);
    stack.Append(contactId2);
    stack.Append(serviceId2);
    stack.Append(contactId3);
    stack.Append(serviceId3);

    CleanupStack::Pop(contactId3);
    CleanupStack::Pop(contactId2);
    CleanupStack::Pop(contactId1);
    CleanupStack::Pop(serviceId3);
    CleanupStack::Pop(serviceId2);
    CleanupStack::Pop(serviceId1);

    TInt err = iPositioner.SetRequestor(stack);
    if (err != KErrNone)
    	{
    	_LIT(KErrorAndLeave, "Problems setting requestorstack");
    	LogErrorAndLeaveL(KErrorAndLeave);
    	}
    	
    const TInt KPosBufFlatExpandSize = 100;
    
    //ExternalizeL
    CBufFlat* bufFlat = CBufFlat::NewL(KPosBufFlatExpandSize);
    CleanupStack::PushL(bufFlat);
    
    RBufWriteStream stream(*bufFlat);
    CleanupClosePushL(stream);        
    stack.ExternalizeL(stream);
    
    TPtr8 dataPtr((TUint8*)NULL, 0);
    dataPtr.Set(bufFlat->Ptr(0));    

    CleanupStack::PopAndDestroy(&stream);                

    HBufC8* tempBuf = HBufC8::NewLC(bufFlat->Size());
    TPtr8 ptr = tempBuf->Des();
    ptr = dataPtr;
    
    //Internalize
    bufFlat->InsertL(0, ptr);
    
    RBufReadStream rstream(*bufFlat);
    CleanupClosePushL(rstream);
    
    RRequestorStack stack2 = RRequestorStack();
    CleanupStack::PushL(TCleanupItem(CleanOp ,&stack2));

    stack2.InternalizeL(rstream);
    
    // Verify the requestor stack
    VerifyRequestorStackL(stack2);
    _LIT(KResult, "The RequestorStack is correct after Externalize/Internalize");
    INFO_PRINTF1(KResult);

    CleanupStack::PopAndDestroy(1); // CleanupOp stack2

    CleanupStack::PopAndDestroy(&rstream);
    CleanupStack::PopAndDestroy(tempBuf);
    CleanupStack::PopAndDestroy(bufFlat);
    CleanupStack::PopAndDestroy(1); // CleanupOp stack1

    ClosePositioner();
    Disconnect();
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp261::VerifyRequestorStackL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp261::VerifyRequestorStackL(RRequestorStack& aStack)
    {    
    TInt count = aStack.Count();
    
    _LIT(KService1, "Service1");
    _LIT(KService2, "Service2");
    _LIT(KService3, "Service3");

    _LIT(KContact1, "Contact1");
    _LIT(KContact2, "Contact2");
    _LIT(KContact3, "Contact3");

    // Hardcoded number of requestors
    if (count != 6)
        {
        _LIT(KErrorAndLeave, "Wrong number of requestors in the stack");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    if (aStack[0]->RequestorType() != CRequestor::ERequestorService
           || aStack[0]->RequestorFormat() != CRequestor::EFormatApplication
           || aStack[0]->RequestorData() != KService1)
            {
            _LIT(KErrorAndLeave, "Wrong requestor(1) in stack");
            LogErrorAndLeaveL(KErrorAndLeave);
            }

    if (aStack[1]->RequestorType() != CRequestor::ERequestorContact
           || aStack[1]->RequestorFormat() != CRequestor::EFormatTelephone
           || aStack[1]->RequestorData() != KContact1)
            {
            _LIT(KErrorAndLeave, "Wrong requestor(2) in stack");
            LogErrorAndLeaveL(KErrorAndLeave);
            }

    if (aStack[2]->RequestorType() != CRequestor::ERequestorContact
           || aStack[2]->RequestorFormat() != CRequestor::EFormatUrl
           || aStack[2]->RequestorData() != KContact2)
            {
            _LIT(KErrorAndLeave, "Wrong requestor(3) in stack");
            LogErrorAndLeaveL(KErrorAndLeave);
            }

    if (aStack[3]->RequestorType() != CRequestor::ERequestorService
           || aStack[3]->RequestorFormat() != CRequestor::EFormatApplication
           || aStack[3]->RequestorData() != KService2)
            {
            _LIT(KErrorAndLeave, "Wrong requestor(4) in stack");
            LogErrorAndLeaveL(KErrorAndLeave);
            }

    if (aStack[4]->RequestorType() != CRequestor::ERequestorContact
           || aStack[4]->RequestorFormat() != CRequestor::EFormatMail
           || aStack[4]->RequestorData() != KContact3)
            {
            _LIT(KErrorAndLeave, "Wrong requestor(5) in stack");
            LogErrorAndLeaveL(KErrorAndLeave);
            }

    if (aStack[5]->RequestorType() != CRequestor::ERequestorService
           || aStack[5]->RequestorFormat() != CRequestor::EFormatApplication
           || aStack[5]->RequestorData() != KService3)
            {
            _LIT(KErrorAndLeave, "Wrong requestor(6) in stack");
            LogErrorAndLeaveL(KErrorAndLeave);
            }    
    }
