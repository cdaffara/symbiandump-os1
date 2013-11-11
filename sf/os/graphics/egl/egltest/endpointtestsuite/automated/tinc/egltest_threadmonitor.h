/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#ifndef __EGLTEST_THREADMONITOR_H__
#define __EGLTEST_THREADMONITOR_H__


#include <e32std.h>
#include <e32base.h>


class CThreadMonitor : public CBase
    {
public:
    static CThreadMonitor* NewL(const RArray<TThreadId>& aThreadsToMonitor);
    ~CThreadMonitor();
    
private:
    CThreadMonitor(const RArray<TThreadId>& aThreadsToMonitor);
    void ConstructL();
    static TInt MonitorThreadEntry(TAny* aThreadsToMonitor);
    static void MonitorThreadEntryL(const RArray<TThreadId>& aThreadsToMonitor, TRequestStatus*& aNotifyCancel);
    
private:
    const RArray<TThreadId>& iThreadsToMonitor;
    TRequestStatus* iNotifyCancel;
    RThread iMonitor;
    };


#endif
