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
//

/**
 @file
 @test
*/
#ifndef EGLTEST_GETERRORANDDISPLAY_H
#define EGLTEST_GETERRORANDDISPLAY_H

#include "eglteststep.h"

_LIT(KGetError, "GetError");
NONSHARABLE_CLASS(CEglTest_GetError) : public CEglTestStep
    {
public:
    TVerdict doTestStepL();    
    };

_LIT(KGetErrorMultiThread, "GetErrorMultiThread");
NONSHARABLE_CLASS(CEglTest_GetErrorMultiThread) : public CEglTestStep
    {
public:
    TVerdict doTestStepL();
protected:
    // from CEglTestStep
    void doThreadFunctionL(TInt aIdx);
    };

_LIT(KDisplay_Positive_Basic, "Display_Positive_Basic");
NONSHARABLE_CLASS(CEglTest_Display_Positive_Basic) : public CEglTestStep
    {
public:
    TVerdict doTestStepL();    
    };

_LIT(KDisplay_Negative_InvalidDisplay, "Display_Negative_InvalidDisplay");
NONSHARABLE_CLASS(CEglTest_Display_Negative_InvalidDisplay) : public CEglTestStep
    {
public:
    TVerdict doTestStepL();    
    };

_LIT(KDisplay_Negative_NotInitializedDisplay, "Display_Negative_NotInitializedDisplay");
NONSHARABLE_CLASS(CEglTest_Display_Negative_NotInitializedDisplay) : public CEglTestStep
    {
public:
    TVerdict doTestStepL();    
    };

_LIT(KDisplay_Positive_ReinitializeDisplay, "Display_Positive_ReinitializeDisplay");
NONSHARABLE_CLASS(CEglTest_Display_Positive_ReinitializeDisplay) : public CEglTestStep
    {
public:
    TVerdict doTestStepL();    
    };

_LIT(KDisplay_Positive_MultipleInitialization, "Display_Positive_MultipleInitialization");
NONSHARABLE_CLASS(CEglTest_Display_Positive_MultipleInitialization) : public CEglTestStep
    {
public:
    TVerdict doTestStepL();    
    };

_LIT(KDisplay_Positive_MultipleTermination, "Display_Positive_MultipleTermination");
NONSHARABLE_CLASS(CEglTest_Display_Positive_MultipleTermination) : public CEglTestStep
    {
public:
    TVerdict doTestStepL();    
    };

_LIT(KDisplay_Positive_Multithread_Basic, "Display_Positive_Multithread_Basic");
NONSHARABLE_CLASS(CEglTest_Display_Positive_Multithread_Basic) : public CEglTestStep
    {
public:
    TVerdict doTestStepL();    
protected:
    // from CEglTestStep
    void doThreadFunctionL(TInt aIdx);
    };

_LIT(KDisplay_Positive_Multithread_InitTerminateInOneTread, "Display_Positive_Multithread_InitTerminateInOneTread");
NONSHARABLE_CLASS(CEglTest_Display_Positive_Multithread_InitTerminateInOneTread) : public CEglTestStep
    {
public:
    TVerdict doTestStepL();    
protected:
    // from CEglTestStep
    void doThreadFunctionL(TInt aIdx);
    };

_LIT(KDisplay_Positive_Multithread_InitTerminateFromDifferentThread, "Display_Positive_Multithread_InitTerminateFromDifferentThread");
NONSHARABLE_CLASS(CEglTest_Display_Positive_Multithread_InitTerminateFromDifferentThread) : public CEglTestStep
    {
public:
    TVerdict doTestStepL();    
protected:
    // from CEglTestStep
    void doThreadFunctionL(TInt aIdx);
    };

_LIT(KDisplay_Positive_Multithread_Stress, "Display_Positive_Multithread_Stress");
NONSHARABLE_CLASS(CEglTest_Display_Positive_Multithread_Stress) : public CEglTestStep
    {
public:
    TVerdict doTestStepL();    
protected:
    // from CEglTestStep
    void doThreadFunctionL(TInt aIdx);
    };

_LIT(KDisplay_Positive_Multiprocess_Basic, "Display_Positive_Multiprocess_Basic");
NONSHARABLE_CLASS(CEglTest_Display_Positive_Multiprocess_Basic) : public CEglTestStep
    {
public:
    TVerdict doTestStepL();    
protected:
    // from CEglTestStep
    void doProcessFunctionL(TInt aIdx);
    };

_LIT(KDisplay_OOM_ClientHeap, "Display_OOM_ClientHeap");
NONSHARABLE_CLASS(CEglTest_Display_OOM_ClientHeap) : public CEglTestStep
    {
public:
    TVerdict doTestStepL();
    };

_LIT(KDisplay_OOM_ClientHeap_Initialize, "Display_OOM_ClientHeap_Initialize");
NONSHARABLE_CLASS(CEglTest_Display_OOM_ClientHeap_Initialize) : public CEglTestStep
    {
public:
    TVerdict doTestStepL();
    };
#endif // EGLTEST_GETERRORANDDISPLAY_H
