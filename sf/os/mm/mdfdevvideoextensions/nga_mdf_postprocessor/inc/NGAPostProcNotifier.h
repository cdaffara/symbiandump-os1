/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __NGAPOSTPROCNOTIFIER_H__
#define __NGAPOSTPROCNOTIFIER_H__

#include <e32base.h>

class CNGAPostProcSessionManager;

class CNGAPostProcNotifier: public CActive
    {
public:
    static CNGAPostProcNotifier* NewL( CNGAPostProcSessionManager& aParent );
    ~CNGAPostProcNotifier();
    void SetBufferId(TInt aBufId);
    void Activate();
    
private:
    void RunL();
    void DoCancel();
    TInt RunError( TInt aError );

private:
    CNGAPostProcNotifier( CNGAPostProcSessionManager& aParent );
    void ConstructL();
    
private:
    CNGAPostProcSessionManager& iParent;
    TInt iBufferId;
};

#endif //__NGAPOSTPROCNOTIFIER_H__