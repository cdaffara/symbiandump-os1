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
// This contains ESock Test cases from section 26
// Component test for DNS Query interface.
// Note: These tests try to make different queries to the DNS (DND), using dummy protocol pdummy.prt, which
// forges DNS answers. Ensure that pdummy.prt is present and listed in appropriate .esk file.
// This test doesn't require a network connection.
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>
#include <dns_qry.h>

// Test system includes
#include "EsockTestSection26.h"


//-------------------------------------------------------------------------------------------------------

/**
*   find "dummy protocol 1" and open Host Resolver
*/
TBool   CTestRHostRslvQuery_26::OpenHostResolver(void)
    {
    TInt            ret;
    TProtocolDesc   protoInfo;
    
    //-- find a dummy protocol 1
    Logger().WriteFormat(_L("Finding dummy protocol 1"));
    ret = iEsockSuite->iSocketServer.FindProtocol(_L("Dummy Protocol 1"), protoInfo);
    
    if(ret != KErrNone)
        {
        Logger().WriteFormat(_L("FindProtocol error code: %d"), ret);    
        return EFalse;
        }
    
    //-- open host resolver on this one
    Logger().WriteFormat(_L("Opening host resolver on dummy protocol 1"));
    ret = iHostRslv.Open(iEsockSuite->iSocketServer, protoInfo.iAddrFamily, protoInfo.iProtocol);
    
    if(ret != KErrNone)
        {
        Logger().WriteFormat(_L("iHostRslv.Open error code: %d"), ret);    
        return EFalse;
        }
    
    return ETrue;
    }

    /**
    *   close Hostresolver
*/
void    CTestRHostRslvQuery_26::CloseHostResolver(void)
    {
    Logger().WriteFormat(_L("Closing host resolver"));
    iHostRslv.Close();
    }


    /**
    *   test preamble: find "dummy protocol 1" and open Host Resolver
*/
TVerdict CTestRHostRslvQuery_26::doTestStepPreambleL (void)
    {
    
    TESTL(OpenHostResolver());
    SetTestStepResult(EPass);
    return EPass;
    }

    /**
    *   test postamble: close Host Resolver
*/
TVerdict CTestRHostRslvQuery_26::doTestStepPostambleL(void)
    {
    CloseHostResolver();
    return EPass;
    }

//-------------------------------------------------------------------------------------------------------
//--  Test step 26.1
//--  Testing RHostResolver::Query() of 'A' type
//-------------------------------------------------------------------------------------------------------

const TDesC& CEsockTest26_1::GetTestName()
    {
    // store the name of this test case
    _LIT(ret,"Test26.1");
	
	return ret;
    }

TVerdict CEsockTest26_1::easyTestStepL()
    {
    
    TVerdict    verdict = EPass;
    TInt        ret;
    
    Logger().WriteFormat(_L("Testing RHostResolver::Query() of 'A' type"));
    
    TDnsQueryBuf      dnsQryBuf;
    TDnsRespABuf      dnsRespABuf;
    
    dnsQryBuf().SetType(KDnsRRTypeA);
    dnsQryBuf().SetData(_L8("http://www.sample.net/"));
    
    //-- make an A query. 
    //-- The query result shall be forged by pdummy.prt
    ret = iHostRslv.Query(dnsQryBuf, dnsRespABuf);
    TESTEL(KErrNone == ret, ret);
    
    //-- verify query response
    const TDnsRespA& dnsRespA = dnsRespABuf();
    
    //-- this is an expected ansver, forged by pdummy.
    TInetAddr expInetAddr;
    expInetAddr.Input(_L("192.168.40.4"));
    
    //-- check answer
    TESTL(dnsRespA.HostAddress().CmpAddr(expInetAddr));
    
    TESTL(dnsRespA.RRType()  == KDnsRRTypeA);
    TESTL(dnsRespA.RRClass() == KDnsRRClassIN);
    TESTL(dnsRespA.RRTtl()   == 0x121212);
    
    //-- Get next query result. 
    //-- error code shall be KErrNotFound, see pdummy.prt
    Logger().WriteFormat(_L("Testing RHostResolver::QueryGetNext(...)"));
    
    //--
    ret = iHostRslv.QueryGetNext(dnsRespABuf);
    TESTEL(KErrNone == ret, ret);
    expInetAddr.Input(_L("177.123.221.251"));
    //-- check answer
    TESTL(dnsRespA.HostAddress().CmpAddr(expInetAddr));
    TESTL(dnsRespA.RRType()  == KDnsRRTypeA);
    TESTL(dnsRespA.RRClass() == KDnsRRClassIN);
    TESTL(dnsRespA.RRTtl()   == 0x112233);
    
    
    //-- Get next query result. 
    //-- error code shall be KErrNotFound, see pdummy.prt
    ret = iHostRslv.QueryGetNext(dnsRespABuf);
    TESTEL(KErrNotFound == ret, ret);
    
    return verdict;
    }

//-------------------------------------------------------------------------------------------------------
//--  Test step 26.2
//--  Testing RHostResolver::Query() of 'SVR' type
//-------------------------------------------------------------------------------------------------------

const TDesC& CEsockTest26_2::GetTestName()
    {
    // store the name of this test case
    _LIT(ret,"Test26.2");
	
	return ret;
    }

TVerdict CEsockTest26_2::easyTestStepL()
    {
    TVerdict    verdict = EPass;
    TInt        ret;
    
    Logger().WriteFormat(_L("Testing RHostResolver::Query() of 'SRV' type"));
    
    TDnsQueryBuf      dnsQryBuf;
    TDnsRespSRVBuf    dnsRespSrvBuf;
    
    //-- test SRV query. see also RFC2782
    _LIT8(KSRVqrySample, "_ldap._tcp.example.com"); 
    dnsQryBuf().SetType(KDnsRRTypeSRV);
    dnsQryBuf().SetData(KSRVqrySample);
    
    //-- make a SRV query. 
    //-- The query result shall be forged by pdummy.prt
    ret = iHostRslv.Query(dnsQryBuf, dnsRespSrvBuf);
    TESTEL(KErrNone == ret, ret);
    
    //-- verify query response
    const TDnsRespSRV& dnsResp=dnsRespSrvBuf();
    
    TESTL(dnsResp.RRType() == KDnsRRTypeSRV);
    TESTL(dnsResp.RRClass() == KDnsRRClassIN);
    TESTL(dnsResp.RRTtl() == 0x123456);
    
    //-- verify RR fields obtained from pdummy.
    TESTL(dnsResp.Target().CompareF(_L8("old-slow-box")) == 0);
    
    TESTL(dnsResp.Priority()== 384);
    TESTL(dnsResp.Weight()  == 784);
    TESTL(dnsResp.Port()    == 123);
    
    //-- Get next query result. 
    //-- The result shall be forged by pdummy.prt
    Logger().WriteFormat(_L("Testing RHostResolver::QueryGetNext(...)"));
    ret = iHostRslv.QueryGetNext(dnsRespSrvBuf);
    TESTEL(KErrNone == ret, ret);
    
    //-- verify QueryGetNext data
    TESTL(dnsResp.RRType()  == KDnsRRTypeSRV);
    TESTL(dnsResp.RRClass() == KDnsRRClassIN);
    TESTL(dnsResp.RRTtl()   == 0x123765);
    TESTL(dnsResp.Priority()== 236);
    TESTL(dnsResp.Weight()  == 962);
    TESTL(dnsResp.Port()    == 125);
    
    TESTL(dnsResp.Target().CompareF(_L8("new-fast-box")) == 0);
    
    //-- try to get the next result, error code shall be KErrNotFound
    //-- see pdummy.prt
    ret = iHostRslv.QueryGetNext(dnsRespSrvBuf);
    TESTEL(KErrNotFound == ret, ret);
    
    return verdict;
    }

//-------------------------------------------------------------------------------------------------------
//--  Test step 26.3
//--  Testing RHostResolver::Query() of 'PTR' type
//-------------------------------------------------------------------------------------------------------

const TDesC& CEsockTest26_3::GetTestName()
    {
    // store the name of this test case
    _LIT(ret,"Test26.3");
	
	return ret;
    }

TVerdict CEsockTest26_3::easyTestStepL()
    {
    
    TVerdict    verdict = EPass;
    TInt        ret;
    
    Logger().WriteFormat(_L("Testing RHostResolver::Query() of 'PTR' type"));
    
    TDnsQueryBuf      dnsQryBuf;
    TDnsRespPTRBuf    dnsRespPtrBuf;
    
    dnsQryBuf().SetType(KDnsRRTypePTR);
    
    //-- pass this address to pdummy.prt for resolution
    TInetAddr inetAddr;
    inetAddr.Input(_L("192.111.22.77"));
    dnsQryBuf().SetData(inetAddr);
    
    //-- make an PTR query. 
    //-- The query result shall be forged by pdummy.prt
    ret = iHostRslv.Query(dnsQryBuf, dnsRespPtrBuf);
    TESTEL(KErrNone == ret, ret);
    
    const TDnsRespPTR& dnsRespPTR = dnsRespPtrBuf();
    TESTL(dnsRespPTR.HostName().CompareF(_L8("http://www.CDatagramHostResolver_QryProcessPTR.response/")) ==0);
    TESTL(dnsRespPTR.RRType()  == KDnsRRTypePTR);
    TESTL(dnsRespPTR.RRClass() == KDnsRRClassIN);
    TESTL(dnsRespPTR.RRTtl()   == 0x223441);
    
    //-- try to get the next result, error code shall be KErrNotFound
    //-- see pdummy.prt
    ret = iHostRslv.QueryGetNext(dnsRespPtrBuf);
    TESTEL(KErrNotFound == ret, ret);
    
    
    return verdict;
    }

//-------------------------------------------------------------------------------------------------------
//--  Test step 26.4
//--  Testing RHostResolver::Query() of 'NAPTR' type
//-------------------------------------------------------------------------------------------------------

const TDesC& CEsockTest26_4::GetTestName()
    {
    // store the name of this test case
    _LIT(ret,"Test26.4");
	
	return ret;
    }

TVerdict CEsockTest26_4::easyTestStepL()
    {
    TVerdict    verdict = EPass;
    TInt        ret;
    
    Logger().WriteFormat(_L("Testing RHostResolver::Query() of 'NAPTR' type"));
    
    TDnsQueryBuf        dnsQryBuf;
    TDnsRespNAPTRBuf    dnsRespNaptrBuf;
    
    dnsQryBuf().SetType(KDnsRRTypeNAPTR);
    dnsQryBuf().SetData(_L8("http://www.foo_bar.ru/"));
    
    //-- make  NAPTR query. 
    //-- The query result shall be forged by pdummy.prt
    ret = iHostRslv.Query(dnsQryBuf, dnsRespNaptrBuf);
    TESTEL(KErrNone == ret, ret);
    
    const TDnsRespNAPTR& dnsRespNAPTR = dnsRespNaptrBuf();
    
    TESTL(dnsRespNAPTR.RRType()  == KDnsRRTypeNAPTR);
    TESTL(dnsRespNAPTR.RRClass() == KDnsRRClassIN);
    TESTL(dnsRespNAPTR.RRTtl()   == 0x2673411);
    
    TESTL(dnsRespNAPTR.Order() == 123);
    TESTL(dnsRespNAPTR.Pref()  == 99);
    
    TESTL(dnsRespNAPTR.Flags().CompareF(_L8("SAUP")) == 0);
    TESTL(dnsRespNAPTR.Service().CompareF(_L8("http+I2R")) == 0);
    TESTL(dnsRespNAPTR.Regexp().CompareF(_L8("!£%^^&($%£$~~## !!!!!")) == 0);
    TESTL(dnsRespNAPTR.Replacement().CompareF(_L8("www.next-name.it")) == 0);
    
    //-- try to get the next result, error code shall be KErrNotFound
    //-- see pdummy.prt
    ret = iHostRslv.QueryGetNext(dnsRespNaptrBuf);
    TESTEL(KErrNotFound == ret, ret);
    
    return verdict;
    }

//-------------------------------------------------------------------------------------------------------
//--  Test step 26.5
//--  Testing RHostResolver::Query() of 'MX' type
//-------------------------------------------------------------------------------------------------------

const TDesC& CEsockTest26_5::GetTestName()
    {
    // store the name of this test case
    _LIT(ret,"Test26.5");
	
	return ret;
    }

TVerdict CEsockTest26_5::easyTestStepL()
    {
    TVerdict    verdict = EPass;
    TInt        ret;
    
    Logger().WriteFormat(_L("Testing RHostResolver::Query() of 'MX' type"));
    
    TDnsQueryBuf    dnsQryBuf;
    TDnsRespMXBuf   dnsRespMXBuf;
    
    dnsQryBuf().SetType(KDnsRRTypeMX);
    dnsQryBuf().SetData(_L8("http://www.gooooooogle.ru/"));
    
    //-- make MX query. 
    //-- The query result shall be forged by pdummy.prt
    ret = iHostRslv.Query(dnsQryBuf, dnsRespMXBuf);
    TESTEL(KErrNone == ret, ret);
    
    const TDnsRespMX& dnsRespMx = dnsRespMXBuf();
    
    TESTL(dnsRespMx.RRType()  == KDnsRRTypeMX);
    TESTL(dnsRespMx.RRClass() == KDnsRRClassIN);
    TESTL(dnsRespMx.RRTtl()   == 0xdead);
    
    
    TESTL(dnsRespMx.Pref()  == 345);
    TESTL(dnsRespMx.HostName().CompareF(_L8("c.example.org")) == 0);
    
    //-- try to get the next result, error code shall be KErrNotFound
    //-- see pdummy.prt
    ret = iHostRslv.QueryGetNext(dnsRespMXBuf);
    TESTEL(KErrNotFound == ret, ret);
    
    
    return verdict;
    }

//-------------------------------------------------------------------------------------------------------
//--  Test step 26.6
//--  Testing RHostResolver::Query() of 'AAAA' type
//-------------------------------------------------------------------------------------------------------

const TDesC& CEsockTest26_6::GetTestName()
    {
    // store the name of this test case
    _LIT(ret,"Test26.6");
	
	return ret;
    }

TVerdict CEsockTest26_6::easyTestStepL()
    {
    TVerdict    verdict = EPass;
    TInt        ret;
    
    Logger().WriteFormat(_L("Testing RHostResolver::Query() of 'AAAA' type"));
    
    TDnsQueryBuf      dnsQryBuf;
    TDnsRespAAAABuf   dnsRespAAAABuf;
    
    dnsQryBuf().SetType(KDnsRRTypeAAAA);
    dnsQryBuf().SetData(_L8("http://www.sample_AAAA.net/"));
    
    //-- make an AAAA query. 
    //-- The query result shall be forged by pdummy.prt
    ret = iHostRslv.Query(dnsQryBuf, dnsRespAAAABuf);
    TESTEL(KErrNone == ret, ret);
    
    //-- verify query response
    const TDnsRespAAAA& dnsRespAAAA = dnsRespAAAABuf();
    
    //-- this is an expected ansver, forged by pdummy.
    TInetAddr expInetAddr;
    expInetAddr.Input(_L("2001:618:400:6a:0:0:0:abc"));
    
    //-- check answer
    TESTL(dnsRespAAAA.HostAddress().CmpAddr(expInetAddr));
    
    TESTL(dnsRespAAAA.RRType()  == KDnsRRTypeAAAA);
    TESTL(dnsRespAAAA.RRClass() == KDnsRRClassIN);
    TESTL(dnsRespAAAA.RRTtl()   == 0xbeef);
    
    
    //-- Get next query result. 
    //-- error code shall be KErrNotFound, see pdummy.prt
    ret = iHostRslv.QueryGetNext(dnsRespAAAABuf);
    TESTEL(KErrNotFound == ret, ret);
    
    return verdict;
    }


//-------------------------------------------------------------------------------------------------------
//--  Test step 26.7
//--  Esock server memory allocation testing for RHostResolver Query interface
//-------------------------------------------------------------------------------------------------------

const TDesC& CEsockTest26_7::GetTestName()
    {
    // store the name of this test case
    _LIT(ret,"Test26.7");
	
	return ret;
    }

TVerdict CEsockTest26_7::easyTestStepL()
    {
    
    TVerdict    verdict = EPass;
    Logger().WriteFormat(_L("Esock server memory allocation testing for RHostResolver Query interface"));
    
    TInt        ret;
    
    TDnsQueryBuf      dnsQryBuf;
    TDnsRespABuf      dnsRespABuf;
    TDnsRespNAPTRBuf  dnsRespNaptrBuf;
    TDnsRespPTRBuf    dnsRespPtrBuf;
    
    
    dnsQryBuf().SetType(KDnsRRTypeA);
    dnsQryBuf().SetData(_L8("http://www.sample.net/"));
    
    ret = iHostRslv.Query(dnsQryBuf, dnsRespABuf);
    TESTEL(KErrNone == ret, ret);
    
    CloseHostResolver();
    
    TESTL(OpenHostResolver());
    
    //-- 1. make 'A' query, it will make esock allocate memory for query and response buffers
    dnsQryBuf().SetType(KDnsRRTypeA);
    dnsQryBuf().SetData(_L8("http://www.sample.net/"));
    
    ret = iHostRslv.Query(dnsQryBuf, dnsRespABuf);
    TESTEL(KErrNone == ret, ret);
    
    //-- 2. make 'NAPTR' query, it will make esock reallocate query data buffers, because for 
    //-- this query type they are larger.
    dnsQryBuf().SetType(KDnsRRTypeNAPTR);
    dnsQryBuf().SetData(_L8("http://www.foo_bar.ru/"));
    ret = iHostRslv.Query(dnsQryBuf, dnsRespNaptrBuf);
    TESTEL(KErrNone == ret, ret);
    
    //-- 3. this will make Esock reuse eisting buffers
    TInetAddr inetAddr;
    inetAddr.Input(_L("192.111.22.77"));
    dnsQryBuf().SetData(inetAddr);
    dnsQryBuf().SetType(KDnsRRTypePTR);
    ret = iHostRslv.Query(dnsQryBuf, dnsRespPtrBuf);
    TESTEL(KErrNone == ret, ret);
    
    CloseHostResolver();
    
    //-- check esock server's heap balance by shutting down the socket server
    return verdict;
    }

