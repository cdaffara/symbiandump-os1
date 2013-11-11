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
// This is the header file for ESOCK test section 27
// Integration test for DNS Query interface.
// 
//

#if (!defined __ESOCKTEST_27_H__)
#define __ESOCKTEST_27_H__
#include "TestStepEsock.h"

//-------------------------------------------------------------------------------------------------------



/**
*   Implements common functionality for the set of RHostResolver Query interface tests:
*   opening Host Resolver etc. 
*   N.B. Host Resolver is opened in IMPLICIT connection mode (without using RConnection functionality)
*/
class CTestRHostRslvQuery_27 : public CESockTestStep_OOMCapable
    {
    
    public:
        
        /** inherited from CTestStep */
        virtual TVerdict doTestStepPreambleL (void);
        virtual TVerdict doTestStepPostambleL(void);
        
    protected:
        
        TInt    ListDnsRecordsA(const TDesC8& aHostName);
        TInt    ListDnsRecordsSRV(const TDesC8& aQuery);
        TInt    ListDnsRecordsPTR(const TDesC8& aQuery);
        TInt    ListDnsRecordsNAPTR(const TDesC8& aHostName);
        TInt    ListDnsRecordsMX(const TDesC8& aHostName);
        TInt    ListDnsRecordsAAAA(const TDesC8& aHostName);
        
        TBool   GetIniFileIpAddr(const TDesC &aSectName,const TDesC &aKeyName,TInetAddr& aInetAddr);
        TBool   GetIniFileString8(const TDesC &aSectName,const TDesC &aKeyName,TDes8& aStr);
        
        TInt    ProcessDndError(TInt aErrCode, TInt aEntryCount);
        
        /** pointer type for the one of the ListDnsRecords* methods */
        typedef TInt (CTestRHostRslvQuery_27::*TpListRRFunct) (const TDesC8&);
        
        TInt    ListDnsRecordsL(TpListRRFunct apFunct);
		
        
    protected:
        
        RHostResolver   iHostRslv;
    };


//-------------------------------------------------------------------------------------------------------

/**
*   Implements common functionality for the set of RHostResolver Query interface tests:
*   establishing connection, opening Host Resolver,  etc. 
*   N.B. Host Resolver is opened in EXPLICIT connection mode (with using RConnection functionality)
*/
class CTestRHostRslvQuery_27_ExplConn : public CTestRHostRslvQuery_27
    {
    public:
        
        /** inherited from CTestStep */
        virtual TVerdict doTestStepPreambleL (void);
        virtual TVerdict doTestStepPostambleL(void);
        
    protected:
        RConnection iConnection;
        
    };

//-------------------------------------------------------------------------------------------------------


/**
*   Testing RHostResolver::Query() of 'A' type
*/
class CEsockTest27_1 : public CTestRHostRslvQuery_27
    {
    public:
       	static const TDesC& GetTestName();
        ~CEsockTest27_1() {}
        
        virtual TVerdict easyTestStepL();
        
    };

//-------------------------------------------------------------------------------------------------------

/**
*   Testing RHostResolver::Query() of 'SVR' type
*/
class CEsockTest27_2 : public CTestRHostRslvQuery_27
    {
    public:
       	static const TDesC& GetTestName();
        ~CEsockTest27_2() {}
        
        virtual TVerdict easyTestStepL();
        
    };

//-------------------------------------------------------------------------------------------------------

/**
*   Testing RHostResolver::Query() of 'PTR' type
*/
class CEsockTest27_3 : public CTestRHostRslvQuery_27
    {
    public:
       	static const TDesC& GetTestName();
        ~CEsockTest27_3() {}
        
        virtual TVerdict easyTestStepL();
        
    };

//-------------------------------------------------------------------------------------------------------

/**
*   Testing RHostResolver::Query() of 'NAPTR' type
*/
class CEsockTest27_4 : public CTestRHostRslvQuery_27
    {
    public:
       	static const TDesC& GetTestName();
        ~CEsockTest27_4() {}
        
        virtual TVerdict easyTestStepL();
        
    };

//-------------------------------------------------------------------------------------------------------

/**
*   Testing RHostResolver::Query() of 'MX' type
*/
class CEsockTest27_5 : public CTestRHostRslvQuery_27
    {
    public:
       	static const TDesC& GetTestName();
        ~CEsockTest27_5() {}
        
        virtual TVerdict easyTestStepL();
        
    };

//-------------------------------------------------------------------------------------------------------

/**
*   Testing RHostResolver::Query() of 'AAAA' type
*/
class CEsockTest27_6 : public CTestRHostRslvQuery_27
    {
    public:
       	static const TDesC& GetTestName();
        ~CEsockTest27_6() {}
        
        virtual TVerdict easyTestStepL();
        
    };



//-------------------------------------------------------------------------------------------------------

/**
*   Testing RHostResolver::Query() of 'A' type
*   Using explicit connection scenario.
*/
class CEsockTest27_1_ExplConn : public CTestRHostRslvQuery_27_ExplConn
    {
    public:
       	static const TDesC& GetTestName();
        ~CEsockTest27_1_ExplConn() {}
        
        virtual TVerdict easyTestStepL();
        
    };

//-------------------------------------------------------------------------------------------------------

/**
*   Testing RHostResolver::Query() functionality on several threads:       
*   one thread performs Query() while another GetByName()
*/
class CEsockTest27_1_MThread: public CTestRHostRslvQuery_27
    {
    public:
       	static const TDesC& GetTestName();
        ~CEsockTest27_1_MThread() {}
        
        virtual TVerdict easyTestStepL();
        
    };



//-------------------------------------------------------------------------------------------------------


#endif //(__ESOCKTEST_27_H__)

