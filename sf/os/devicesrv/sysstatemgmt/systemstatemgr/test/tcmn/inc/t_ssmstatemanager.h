// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description:Wrapper for Testing RSsmStateManager class.
//
//
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#ifndef __T_SSMSTATEMANAGER_H__
#define __T_SSMSTATEMANAGER_H__

#include <ssm/ssmstatemanager.h>
#include <ssm/ssmstateawaresession.h>

#include <test/testexecutestepbase.h>
#include <test/datawrapper.h>

class CTestRSsmStateManager : public CDataWrapper
    {
public:
    static CTestRSsmStateManager* NewL();
    ~CTestRSsmStateManager();

    TBool DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

    TAny* GetObject();
    void  SetObjectL(TAny* aAny);
    void  DisownObjectL();    
    
    void DoCmdNew();
    void DoCmdDestructor();
    void DoCmdConnect();
    void DoCmdRequestStateTransition(const TDesC& aSection, const TInt aAsyncErrorIndex);
    void DoCmdRequestStateTransitionCancel();
    void DoCmdClose();    
    
    //helper
    void DoCleanup();
    TBool MapToMainState( TPtrC& aGetMainState, TUint16& aMainState );
    TBool MapToSubState( TPtrC& aGetSubState, TUint16& aSubState );

protected:
    CTestRSsmStateManager();
    void ConstructL();

    void RunL(CActive* aActive, TInt aIndex);
    void DoCancel(CActive* aActive, TInt aIndex);

private:
    RSsmStateManager* iSsmStateManager;
    CActiveCallback* iActiveNotifyOnChange;
    };

#endif // __T_SSMSTATEMANAGER_H__
