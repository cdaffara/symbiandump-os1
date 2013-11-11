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
// This is the header file for ESOCK test section 26
// Component test for DNS Query interface.
// 
//

#if (!defined __ESOCKTEST_26_H__)
#define __ESOCKTEST_26_H__
#include "TestStepEsock.h"

//-------------------------------------------------------------------------------------------------------

/**
*   Implements common functionality for the set of RHostResolver Query interface tests:
*   opening Host Resolver etc.
*/
class CTestRHostRslvQuery_26 : public CESockTestStep_OOMCapable
    {
    
    public:
        
        /** inherited from CTestStep */
        virtual TVerdict doTestStepPreambleL (void);
        virtual TVerdict doTestStepPostambleL(void);
        
    protected:
        TBool   OpenHostResolver(void);
        void    CloseHostResolver(void);
        
    protected:
        
        RHostResolver   iHostRslv;
    };

//-------------------------------------------------------------------------------------------------------

/**
*   Testing RHostResolver::Query() of 'A' type
*/
class CEsockTest26_1 : public CTestRHostRslvQuery_26
    {
    public:
       	static const TDesC& GetTestName();
        ~CEsockTest26_1() {}
        
        virtual TVerdict easyTestStepL();
        
    };

//-------------------------------------------------------------------------------------------------------

/**
*   Testing RHostResolver::Query() of 'SVR' type
*/
class CEsockTest26_2 : public CTestRHostRslvQuery_26
    {
    public:
       	static const TDesC& GetTestName();
        ~CEsockTest26_2() {}
        
        virtual TVerdict easyTestStepL();
        
    };

//-------------------------------------------------------------------------------------------------------

/**
*   Testing RHostResolver::Query() of 'PTR' type
*/
class CEsockTest26_3 : public CTestRHostRslvQuery_26
    {
    public:
       	static const TDesC& GetTestName();
        ~CEsockTest26_3() {}
        
        virtual TVerdict easyTestStepL();
        
    };

//-------------------------------------------------------------------------------------------------------

/**
*   Testing RHostResolver::Query() of 'NAPTR' type
*/
class CEsockTest26_4 : public CTestRHostRslvQuery_26
    {
    public:
       	static const TDesC& GetTestName();
        ~CEsockTest26_4() {}
        
        virtual TVerdict easyTestStepL();
        
    };

//-------------------------------------------------------------------------------------------------------

/**
*   Testing RHostResolver::Query() of 'MX' type
*/
class CEsockTest26_5 : public CTestRHostRslvQuery_26
    {
    public:
       	static const TDesC& GetTestName();
        ~CEsockTest26_5() {}
        
        virtual TVerdict easyTestStepL();
        
    };

//-------------------------------------------------------------------------------------------------------

/**
*   Testing RHostResolver::Query() of 'AAAA' type
*/
class CEsockTest26_6 : public CTestRHostRslvQuery_26
    {
    public:
       	static const TDesC& GetTestName();
        ~CEsockTest26_6() {}
        
        virtual TVerdict easyTestStepL();
        
    };



//-------------------------------------------------------------------------------------------------------

/**
*   Esock server memory allocation testing for RHostResolver Query interface
*/
class CEsockTest26_7 : public CTestRHostRslvQuery_26
    {
    public:
       	static const TDesC& GetTestName();
        ~CEsockTest26_7() {}
        
        virtual TVerdict easyTestStepL();
        
    };


#endif //(__ESOCKTEST_26_H__)

