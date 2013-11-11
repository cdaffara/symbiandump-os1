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
// This is the header file for ESOCK basic socket test section 31
// 
//


#if (!defined __SOCKETTEST_31_H__)
#define __SOCKETTEST_31_H__

#include "TestStepSocket.h"

class CSocketTest31_1 : public CTestStepSocket
    {
    public:
        static const TDesC &GetTestName();
        virtual enum TVerdict InternalDoTestStepL( void );
    };

class CSocketTest31_2 : public CTestStepSocket
    {
    public:
        static const TDesC &GetTestName();
        virtual enum TVerdict InternalDoTestStepL( void );
    };

class CSocketTest31_3 : public CTestStepSocket
    {
    public:
        static const TDesC &GetTestName();
        virtual enum TVerdict InternalDoTestStepL( void );
    };

class CSocketTest31_4 : public CTestStepSocket
    {
    public:
        static const TDesC &GetTestName();
        virtual enum TVerdict InternalDoTestStepL( void );
    };

class CSocketTest31_5 : public CTestStepSocket
    {
public:
    static const TDesC &GetTestName();
    virtual enum TVerdict InternalDoTestStepL( void );
private:
    TInt MakePendingRequestL();
    
    // Member data largely as convenience; not cleaned up by destructor
    RSocketServ iSess;
    class CSockReq : public CBase
        {
    public:
        static CSockReq* NewLC();
        ~CSockReq();
    public:
        RSocket iSock;
        TRequestStatus iReq;
        TBuf8<20> iData;
        };
    RPointerArray<CSockReq> iSocks;
    };

#endif //__SOCKETTEST_31_H__


