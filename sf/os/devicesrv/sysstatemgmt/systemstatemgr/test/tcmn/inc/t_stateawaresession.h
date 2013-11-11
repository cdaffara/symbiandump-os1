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
// Description: Wrapper for Testing RStateAwareSession class.
//
//
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#ifndef __T_STATEAWARESESSION_H__
#define __T_STATEAWARESESSION_H__

#include <ssm/ssmstatemanager.h>
#include <ssm/ssmstateawaresession.h>

#include <test/testexecutestepbase.h>
#include <test/datawrapper.h>

class CTestRStateAwareSession : public CDataWrapper
    {
public:
    static CTestRStateAwareSession*    NewL();
    ~CTestRStateAwareSession();

    TBool DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

    TAny* GetObject();
    void  SetObjectL(TAny* aAny);
    void  DisownObjectL();
    
    
    void DoCmdNew();
    void DoCmdDestructor();
    void DoCmdConnect(const TDesC& aSection);
    void DoCmdClose();
    void DoCmdRequestStateNotification(const TInt aAsyncErrorIndex);
    void DoCmdAcknowledgeStateNotification(const TDesC& aSection);
    void DoCmdAcknowledgeAndRequestStateNotification(const TDesC& aSection, const TInt aAsyncErrorIndex);
    void DoCmdRequestStateNotificationCancel();
    void DoCmdState(const TDesC& aSection);
    
    //helper
    void DoCleanup();
    TBool MapToDomainId(TPtrC& aDomainIdName, TDmDomainId& aDomainId );
    TBool MapToMainState( TPtrC& aGetMainState, TUint16& aMainState );

protected:
    CTestRStateAwareSession();
    void    ConstructL();

    void    RunL(CActive* aActive, TInt aIndex);
    void    DoCancel(CActive* aActive, TInt aIndex);


private:
    RSsmStateAwareSession* iSsmStateAwareSession;
    CActiveCallback* iActiveNotifyOnChange;
    };

#endif // __T_STATEAWARESESSION_H__
