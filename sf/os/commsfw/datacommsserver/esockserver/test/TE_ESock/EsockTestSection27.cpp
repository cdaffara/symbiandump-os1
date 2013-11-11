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
// This contains ESock Test cases from section 27
// Integration test for DNS Query interface.
// Note: These tests try to make different queries to the external DNS, using network connection.
// This test requires a network connection and a properly configured DNS.
// See also appropriate ini file, which contains information about DNS requests for testing.
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>
#include <dns_qry.h>

// Test system includes
#include "EsockTestSection27.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockTestEsckTst, "ESockTestEsckTst");
#endif


const TInt  KMaxStrLen = 256;

// From ../../networking/inhook6/inc/dnd_err.h
const TInt KErrDndNameNotFound = -5120; 


//-------------------------------------------------------------------------------------------------------


/**
*   test preamble: open Host Resolver for using with implicit connection 
*/

TVerdict CTestRHostRslvQuery_27::doTestStepPreambleL (void)
    {
    
    TInt    ret;
    
    //-- open host resolver on UDP
    Logger().WriteFormat(_L("Opening host resolver on UDP using implicit connection scenario"));
    ret = iHostRslv.Open(iEsockSuite->iSocketServer, KAfInet, KProtocolInetUdp);
    
    if(ret != KErrNone)
        {
        Logger().WriteFormat(_L("iHostRslv.Open error code: %d"), ret);    
        return EFail;
        }
    
    return EPass;
    }

	/**
	*   test postamble: close Host Resolver
*/
TVerdict CTestRHostRslvQuery_27::doTestStepPostambleL(void)
    {
    Logger().WriteFormat(_L("Closing host resolver"));
    iHostRslv.Close();
    
    return EPass;
    }

//-------------------------------------------------------------------------------------------------------

/**
*   Read string from .ini file into 8-bit string descriptor
*
*   @param  aSectName section name descriptor.
*   @param  aKeyName  key name descriptor.  
*   @param  aStr      ref. to the 8-bit descriptor for the result string.  
*
*   @return ETrue on success.
*/
TBool   CTestRHostRslvQuery_27::GetIniFileString8(const TDesC &aSectName, const TDesC &aKeyName, TDes8& aStr)
    {
    TBuf<KMaxStrLen>    buff;
    TPtrC               buffPtr(buff);
    
    TBool bRes = GetStringFromConfig(aSectName, aKeyName, buffPtr);
    
    if(bRes)
        aStr.Copy(buffPtr);
    
    return bRes;
    }

	/**
	*   read IP address from .ini file, the address may have either IPv4 or IPv6 format.
	*
	*   @param  aSectName section name descriptor.
	*   @param  aKeyName  key name descriptor.  
	*   @param  aInetAddr will contain result.
	*
	*   @return ETrue on success
*/
TBool   CTestRHostRslvQuery_27::GetIniFileIpAddr(const TDesC &aSectName, const TDesC &aKeyName, TInetAddr& aInetAddr)
    {
    TBuf<KMaxStrLen>    buff;
    TPtrC               buffPtr(buff);
    
    TBool bRes = GetStringFromConfig(aSectName, aKeyName, buffPtr);
    if(!bRes) 
        return EFalse;
    
    return aInetAddr.Input(buffPtr) == KErrNone ? ETrue : EFalse;
    }


/**
*   Internal helper function to handle DND error codes
*   
*   @param  aErrCode    original code to process
*   @param  aEntryCount determines whether it was Query() 0 or QueryGetNext >0
*   @return possibly    modified new code.
*/
TInt CTestRHostRslvQuery_27::ProcessDndError(TInt aErrCode, TInt aEntryCount)
	{
    __ASSERT_DEBUG(aEntryCount >=0, User::Panic(KSpecAssert_ESockTestEsckTst, 1));
    
    if(aErrCode == KErrNone)
        return KErrNone;
	
    if(aErrCode == KErrNotFound || aErrCode == KErrDndNameNotFound)
		{
		
        if(aEntryCount == 0)
			{//-- no result at all for the first Query(). DNS may be down or just has no records
			//-- int his case test is inconsistent.
			return KErrNotFound;
			}
        else
			{//-- OK, no more resulsts for QueryGetNext(), normal situation
			return KErrNone;
			}
		}
	else //-- real error
		{    
		TBuf<KMaxStrLen>  tmpBuf;
		tmpBuf.Format(_L("! Query finished with error code:%d"), aErrCode);
		Logger().WriteFormat(tmpBuf);
		}
	
	return aErrCode;
	}


	/**
	*   Requests all RR of type 'A' for the given host and prints them out
	*
	*   @param      aHostName name of the nost
	*
	*   @return     standard error code
*/
TInt    CTestRHostRslvQuery_27::ListDnsRecordsA(const TDesC8& aHostName)
    {
    TInt    ret = KErrNone;
    
    TDnsQueryBuf      dnsQryBuf;
    TDnsRespABuf      dnsRespABuf;
    TBuf<KMaxStrLen>  addrBuf;
    TBuf<KMaxStrLen>  tmpBuf;
    
    dnsQryBuf().SetType(KDnsRRTypeA);
    dnsQryBuf().SetData(aHostName);
    
    tmpBuf.Copy(aHostName);
    addrBuf.Copy(_L("* Listing 'A' RRs for the host: "));
    addrBuf.Append(tmpBuf);
    Logger().WriteFormat(addrBuf);
    
    
    for(TInt recCnt=0; ;++recCnt)
        {
        //-- make an 'A' query. 
        if(recCnt == 0) 
            ret = iHostRslv.Query(dnsQryBuf, dnsRespABuf);  //-- make a query and get the result
        else
            ret = iHostRslv.QueryGetNext(dnsRespABuf);      //-- get the rest of the RRs
        
        const TDnsRespA& dnsRespA = dnsRespABuf();
        
        
        if(ret == KErrNone)
            {//-- everything is all right, print record data out
            
            tmpBuf.Format(_L("RR Type=%d, Class=%d, TTL=%d, Address="),dnsRespA.RRType(), dnsRespA.RRClass(), dnsRespA.RRTtl());
            dnsRespA.HostAddress().Output(addrBuf);
            tmpBuf.Append(addrBuf);
            Logger().WriteFormat(tmpBuf);
            }
        else //-- real error
            {
			ret = ProcessDndError(ret, recCnt);
			break;
            }
		
        }// for
    
    
    Logger().WriteFormat(_L(""));
    return ret;
    }

	/**
	*   Requests all RR of type 'SRV' for the given host and prints them out
	*
	*   @param      aQuery  a query string that contains desired service, protocol and host definition
	*
	*   @return     standard error code
*/
TInt    CTestRHostRslvQuery_27::ListDnsRecordsSRV(const TDesC8& aQuery)
    {
    TInt    ret = KErrNone;
    
    TDnsQueryBuf      dnsQryBuf;
    TDnsRespSRVBuf    dnsRespSRVBuf;
    TBuf<KMaxStrLen>  targetBuf;
    TBuf<KMaxStrLen>  tmpBuf;
    
    dnsQryBuf().SetType(KDnsRRTypeSRV);
    dnsQryBuf().SetData(aQuery);
    
    tmpBuf.Copy(aQuery);
    targetBuf.Copy(_L("* Listing 'SRV' RRs according to the query: "));
    targetBuf.Append(tmpBuf);
    Logger().WriteFormat(targetBuf);
    
    for(TInt recCnt =0; ;++recCnt)
        {
        //-- make an 'SRV' query. 
        
        if(recCnt == 0) 
            ret = iHostRslv.Query(dnsQryBuf, dnsRespSRVBuf);  //-- make a query and get the result
        else
            ret = iHostRslv.QueryGetNext(dnsRespSRVBuf);      //-- get the rest of the RRs
        
        const TDnsRespSRV& dnsRespSRV = dnsRespSRVBuf();
        
        if(ret == KErrNone)
            {//-- everything is all right, print record data out
            
            tmpBuf.Format(_L("RR Type=%d, Class=%d, TTL=%d, target="),dnsRespSRV.RRType(), dnsRespSRV.RRClass(), dnsRespSRV.RRTtl());
            targetBuf.Copy(dnsRespSRV.Target());
            tmpBuf.Append(targetBuf);
            Logger().WriteFormat(tmpBuf);
            }
        else //-- real error
            {
			ret = ProcessDndError(ret, recCnt);
			break;
            }
		
        }// for
    
    Logger().WriteFormat(_L(""));
    return ret;
    }


	/**
	*   Requests all RR of type 'PTR' for the given IP address and prints them out
	*
	*   @param      aQuery  string containing 'PTR' query, pointer address representation, 
	*                       e.g "3.253.253.192.in-addr.arpa"  to resolve 192.253.253.2 address.
	*
	*   @return     standard error code
*/
TInt    CTestRHostRslvQuery_27::ListDnsRecordsPTR(const TDesC8& aQuery)
    {
    TInt    ret = KErrNone;
    
    TDnsQueryBuf      dnsQryBuf;
    TDnsRespPTRBuf    dnsRespPTRBuf;
    TBuf<KMaxStrLen>  targetBuf;
    TBuf<KMaxStrLen>  tmpBuf;
    
    dnsQryBuf().SetType(KDnsRRTypePTR);
    dnsQryBuf().SetData(aQuery);
    
    targetBuf.Copy(_L("* Listing 'PTR' RRs according to the address query: "));
    tmpBuf.Copy(aQuery);
    targetBuf.Append(tmpBuf);
    Logger().WriteFormat(targetBuf);
    
    for(TInt recCnt =0; ;++recCnt)
        {
        //-- make an 'PTR' query. 
        if(recCnt == 0) 
            ret = iHostRslv.Query(dnsQryBuf, dnsRespPTRBuf);  //-- make a query and get the result
        else
            ret = iHostRslv.QueryGetNext(dnsRespPTRBuf);      //-- get the rest of the RRs
        
        const TDnsRespPTR& dnsRespPTR = dnsRespPTRBuf();
        
        if(ret == KErrNone)
            {//-- everything is all right, print record data out
            
            tmpBuf.Format(_L("RR Type=%d, Class=%d, TTL=%d, host name="),dnsRespPTR.RRType(), dnsRespPTR.RRClass(), dnsRespPTR.RRTtl());
            targetBuf.Copy(dnsRespPTR.HostName());
            tmpBuf.Append(targetBuf);
            Logger().WriteFormat(tmpBuf);
            }
        else //-- real error
            {
			ret = ProcessDndError(ret, recCnt);
			break;
            }
		
        }// for
    
    Logger().WriteFormat(_L(""));
    return ret;
    }



	/**
	*   Requests all RR of type 'NAPTR' for the given host and prints them out
	*
	*   @param      aHostName name of the nost
	*
	*   @return     standard error code
*/
TInt    CTestRHostRslvQuery_27::ListDnsRecordsNAPTR(const TDesC8& aHostName)
    {
    TInt    ret = KErrNone;
    
    TDnsQueryBuf      dnsQryBuf;
    TDnsRespNAPTRBuf  dnsRespNAPTRBuf;
    TBuf<KMaxStrLen>  targetBuf;
    TBuf<KMaxStrLen>  tmpBuf;
    
    dnsQryBuf().SetType(KDnsRRTypeNAPTR);
    dnsQryBuf().SetData(aHostName);
    
    
    targetBuf.Copy(_L("* Listing 'NAPTR' RRs for host: "));
    tmpBuf.Copy(aHostName);
    targetBuf.Append(tmpBuf);
    Logger().WriteFormat(targetBuf);
    
    for(TInt recCnt =0; ;++recCnt)
        {
        //-- make an 'SRV' query. 
        if(recCnt == 0) 
            ret = iHostRslv.Query(dnsQryBuf, dnsRespNAPTRBuf);  //-- make a query and get the result
        else
            ret = iHostRslv.QueryGetNext(dnsRespNAPTRBuf);      //-- get the rest of the RRs
        
        const TDnsRespNAPTR& dnsRespNAPTR = dnsRespNAPTRBuf();
        
        if(ret == KErrNone)
            {//-- everything is all right, print record data out
            
            tmpBuf.Format(_L("RR Type=%d, Class=%d, TTL=%d, Order=%d, Pref=%d, Flags="),
                dnsRespNAPTR.RRType(), dnsRespNAPTR.RRClass(), dnsRespNAPTR.RRTtl(), dnsRespNAPTR.Order(), dnsRespNAPTR.Pref());
            
            targetBuf.Copy(dnsRespNAPTR.Flags());
            tmpBuf.Append(targetBuf);
            Logger().WriteFormat(tmpBuf);
            
            tmpBuf.Copy(_L("Service: "));
            targetBuf.Copy(dnsRespNAPTR.Service());
            tmpBuf.Append(targetBuf);
            
            tmpBuf.Append(_L(", Regexp: "));
            targetBuf.Copy(dnsRespNAPTR.Regexp());
            tmpBuf.Append(targetBuf);
            
            tmpBuf.Append(_L(", Replacement: "));
            targetBuf.Copy(dnsRespNAPTR.Replacement());
            tmpBuf.Append(targetBuf);
            
            Logger().WriteFormat(tmpBuf);
            }
        else //-- real error
            {
			ret = ProcessDndError(ret, recCnt);
			break;
            }
		
		
        }// for
    
    Logger().WriteFormat(_L(""));
    return ret;
    }

	/**
	*   Requests all RR of type 'MX' for the given host and prints them out
	*
	*   @param      aHostName name of the nost
	*
	*   @return     standard error code
*/
TInt    CTestRHostRslvQuery_27::ListDnsRecordsMX(const TDesC8& aHostName)
    {
    TInt    ret = KErrNone;
    
    TDnsQueryBuf      dnsQryBuf;
    TDnsRespMXBuf     dnsRespMXBuf;
    TBuf<KMaxStrLen>  addrBuf;
    TBuf<KMaxStrLen>  tmpBuf;
    
    dnsQryBuf().SetType(KDnsRRTypeMX);
    dnsQryBuf().SetData(aHostName);
    
    tmpBuf.Copy(aHostName);
    addrBuf.Copy(_L("* Listing 'MX' RRs for the host: "));
    addrBuf.Append(tmpBuf);
    Logger().WriteFormat(addrBuf);
    
    for(TInt recCnt=0; ;++recCnt)
        {
        //-- make an 'MX' query. 
        if(recCnt == 0) 
            ret = iHostRslv.Query(dnsQryBuf, dnsRespMXBuf);  //-- make a query and get the result
        else
            ret = iHostRslv.QueryGetNext(dnsRespMXBuf);      //-- get the rest of the RRs
        
        const TDnsRespMX& dnsRespMX = dnsRespMXBuf();
        
        if(ret == KErrNone)
            {//-- everything is all right, print record data out
            
            tmpBuf.Format(_L("RR Type=%d, Class=%d, TTL=%d, Pref=%d, HostName="),dnsRespMX.RRType(), dnsRespMX.RRClass(), dnsRespMX.RRTtl(), dnsRespMX.Pref());
            addrBuf.Copy(dnsRespMX.HostName());
            tmpBuf.Append(addrBuf);
            Logger().WriteFormat(tmpBuf);
            }
        else //-- real error
            {
			ret = ProcessDndError(ret, recCnt);
			break;
            }
		
        }// for
    
    Logger().WriteFormat(_L(""));
    return ret;
    }

	/**
	*   Requests all RR of type 'AAAA' for the given host and prints them out
	*
	*   @param      aHostName name of the nost
	*
	*   @return     standard error code
*/
TInt    CTestRHostRslvQuery_27::ListDnsRecordsAAAA(const TDesC8& aHostName)
    {
    TInt    ret = KErrNone;
    
    TDnsQueryBuf      dnsQryBuf;
    TDnsRespAAAABuf   dnsRespAAAABuf;
    TBuf<KMaxStrLen>  addrBuf;
    TBuf<KMaxStrLen>  tmpBuf;
    
    dnsQryBuf().SetType(KDnsRRTypeAAAA);
    dnsQryBuf().SetData(aHostName);
    
    tmpBuf.Copy(aHostName);
    addrBuf.Copy(_L("* Listing 'AAAA' RRs for the host: "));
    addrBuf.Append(tmpBuf);
    Logger().WriteFormat(addrBuf);
    
    for(TInt recCnt =0; ;++recCnt)
        {
        //-- make an 'AAAA' query. 
        if(recCnt == 0) 
            ret = iHostRslv.Query(dnsQryBuf, dnsRespAAAABuf);  //-- make a query and get the result
        else
            ret = iHostRslv.QueryGetNext(dnsRespAAAABuf);      //-- get the rest of the RRs
        
        const TDnsRespAAAA& dnsRespAAAA = dnsRespAAAABuf();
        
        if(ret == KErrNone)
            {//-- everything is all right, print record data out
            
            tmpBuf.Format(_L("RR Type=%d, Class=%d, TTL=%d, Address="),dnsRespAAAA.RRType(), dnsRespAAAA.RRClass(), dnsRespAAAA.RRTtl());
            
            dnsRespAAAA.HostAddress().Output(addrBuf);
            
            //-- remove scope modifier from the end of address representation, it may look like "%1" and confuses 'Log'
            TInt nPos = addrBuf.Locate('%');
            if(nPos >= 0)
                addrBuf.SetLength(nPos);
            
            tmpBuf.Append(addrBuf);
            Logger().WriteFormat(tmpBuf);
            }
        else //-- real error
            {
			ret = ProcessDndError(ret, recCnt);
			break;
            }
		
        }// for
    
    Logger().WriteFormat(_L(""));
    return ret;
    }

	/**
	* Internal helper function for calling appropriate method of listing a certain type of RR
	*
	*@param     apFunct pointer to the one of the ListDnsRecords* methods to be called. 
	*@return    a number of test substeps performed and passed.
	*@leave     KErrNotFound if some error occured
*/
TInt    CTestRHostRslvQuery_27::ListDnsRecordsL(TpListRRFunct apFunct)
	{
    TBuf8<KMaxStrLen>   queryString;
	
    //-- read number of entries in .ini file section
    TInt nEntries=0;
    if(!GetIntFromConfig(TestStepName(), _L("Entries"), nEntries) || nEntries <=0)
        {
		Logger().WriteFormat(_L("* Unable to read number of entries from ini file!"));
		User::Leave(KErrNotFound);
        }
    //-- make DNS query for every key data entry in .ini file section
    TBuf<64>    keyName;
    TInt        nTstExecuted=0;
    TInt        ret;
    for(TInt cnt=0; cnt<nEntries; cnt++)
		{
        keyName.Format(_L("Param%d"), cnt+1);//-- make key name for section parameter
        
        if(GetIniFileString8(TestStepName(), keyName, queryString))
			{
            //-- list RRs for the given query string, calling given function by pointer.
            ret = (this->*apFunct)(queryString);
            
            if(ret == KErrNone)
                nTstExecuted++;
            else 
				if(ret != KErrNotFound)
					TESTEL(EFalse, ret);
				
			}
		} //for
    
    if(!nTstExecuted) //-- no tests executed
		{
        Logger().WriteFormat(_L("* No tests executed, probably external DNS is down"));
		}
	
    return nTstExecuted;
	}


//-------------------------------------------------------------------------------------------------------
//--  Test step 27.1
//--  Testing RHostResolver::Query() of 'A' type
//-------------------------------------------------------------------------------------------------------

const TDesC& CEsockTest27_1::GetTestName()
    {
    // store the name of this test case
    _LIT(ret,"Test27.1");
	
	return ret;
    }

TVerdict CEsockTest27_1::easyTestStepL()
    {
    TInt                ret = KErrNone;
    TVerdict            verdict = EPass;
    TInt                nTstExecuted=0;
    Logger().WriteFormat(_L("--- Testing RHostResolver::Query() of 'A' type"));
	
    TRAP(ret, nTstExecuted=ListDnsRecordsL(&CEsockTest27_1::ListDnsRecordsA));
    TESTEL(ret == KErrNone, ret);
	
    if(!nTstExecuted) //-- no tests executed
        verdict=EFail;
    
    return verdict;
    }

//-------------------------------------------------------------------------------------------------------
//--  Test step 27.2
//--  Testing RHostResolver::Query() of 'SVR' type
//-------------------------------------------------------------------------------------------------------

const TDesC& CEsockTest27_2::GetTestName()
    {
    // store the name of this test case
    _LIT(ret,"Test27.2");
	
	return ret;
    }

TVerdict CEsockTest27_2::easyTestStepL()
    {
    TInt                ret = KErrNone;
    TVerdict            verdict = EPass;
    
    
    Logger().WriteFormat(_L("--- Testing RHostResolver::Query() of 'SRV' type"));
    
    TInt    nTstExecuted=0;
    TRAP(ret, nTstExecuted=ListDnsRecordsL(&CEsockTest27_2::ListDnsRecordsSRV));
    TESTEL(ret == KErrNone, ret);
	
    if(!nTstExecuted) //-- no tests executed
        verdict=EFail;
	
    return verdict;
    }

//-------------------------------------------------------------------------------------------------------
//--  Test step 27.3
//--  Testing RHostResolver::Query() of 'PTR' type
//-------------------------------------------------------------------------------------------------------

const TDesC& CEsockTest27_3::GetTestName()
    {
    // store the name of this test case
    _LIT(ret,"Test27.3");
	
	return ret;
    }

TVerdict CEsockTest27_3::easyTestStepL()
    {
    TInt                ret = KErrNone;
    TVerdict            verdict = EPass;
	
    Logger().WriteFormat(_L("--- Testing RHostResolver::Query() of 'PTR' type"));
	
	
    TInt    nTstExecuted=0;
    TRAP(ret, nTstExecuted=ListDnsRecordsL(&CEsockTest27_3::ListDnsRecordsPTR));
    TESTEL(ret == KErrNone, ret);
	
    if(!nTstExecuted) //-- no tests executed
        verdict=EFail;
    
    return verdict;
    }

//-------------------------------------------------------------------------------------------------------
//--  Test step 27.4
//--  Testing RHostResolver::Query() of 'NAPTR' type
//-------------------------------------------------------------------------------------------------------

const TDesC& CEsockTest27_4::GetTestName()
    {
    // store the name of this test case
    _LIT(ret,"Test27.4");
	
	return ret;
    }

TVerdict CEsockTest27_4::easyTestStepL()
    {
    TInt                ret = KErrNone;
    TVerdict            verdict = EPass;
    
    Logger().WriteFormat(_L("--- Testing RHostResolver::Query() of 'NAPTR' type"));
    
    TInt    nTstExecuted=0;
    TRAP(ret, nTstExecuted=ListDnsRecordsL(&CEsockTest27_4::ListDnsRecordsNAPTR));
    TESTEL(ret == KErrNone, ret);
	
    if(!nTstExecuted) //-- no tests executed
        verdict=EFail;
	
    return verdict;
    }

//-------------------------------------------------------------------------------------------------------
//--  Test step 27.5
//--  Testing RHostResolver::Query() of 'MX' type
//-------------------------------------------------------------------------------------------------------

const TDesC& CEsockTest27_5::GetTestName()
    {
    // store the name of this test case
    _LIT(ret,"Test27.5");
	
	return ret;
    }

TVerdict CEsockTest27_5::easyTestStepL()
    {
    TInt                ret = KErrNone;
    TVerdict            verdict = EPass;
    
    Logger().WriteFormat(_L("Testing RHostResolver::Query() of 'MX' type"));
	
	
    TInt    nTstExecuted=0;
    TRAP(ret, nTstExecuted=ListDnsRecordsL(&CEsockTest27_5::ListDnsRecordsMX));
    TESTEL(ret == KErrNone, ret);
	
    if(!nTstExecuted) //-- no tests executed
        verdict=EFail;
    
    return verdict;
    }

//-------------------------------------------------------------------------------------------------------
//--  Test step 27.6
//--  Testing RHostResolver::Query() of 'AAAA' type
//-------------------------------------------------------------------------------------------------------

const TDesC& CEsockTest27_6::GetTestName()
    {
    // store the name of this test case
    _LIT(ret,"Test27.6");
	
	return ret;
    }

TVerdict CEsockTest27_6::easyTestStepL()
    {
    TInt                ret = KErrNone;
    TVerdict            verdict = EPass;
	
    Logger().WriteFormat(_L("Testing RHostResolver::Query() of 'AAAA' type"));
    
    TInt    nTstExecuted=0;
    TRAP(ret, nTstExecuted=ListDnsRecordsL(&CEsockTest27_6::ListDnsRecordsAAAA));
    TESTEL(ret == KErrNone, ret);
	
    if(!nTstExecuted) //-- no tests executed
        verdict=EFail;
    
    return verdict;
    }



//-------------------------------------------------------------------------------------------------------

/**
*   test preamble: open Host Resolver  using RConnection
*/
TVerdict CTestRHostRslvQuery_27_ExplConn::doTestStepPreambleL (void)
    {
    TInt    ret;
    
    //-- open and start connection
    Logger().WriteFormat(_L("Opening and starting connection"));
    
    ret = iConnection.Open(iEsockSuite->iSocketServer);
    if(ret != KErrNone)
        {
        Logger().WriteFormat(_L("iConnection.Open error code: %d"), ret);    
        return EFail;
        }
    
    ret = iConnection.Start();
    if(ret != KErrNone)
        {
        Logger().WriteFormat(_L("iConnection.Start error code: %d"), ret);    
        return EFail;
        }
    
    //-- open host resolver on UDP
    Logger().WriteFormat(_L("Opening host resolver on UDP for using explicit connection"));
    ret = iHostRslv.Open(iEsockSuite->iSocketServer, KAfInet, KProtocolInetUdp, iConnection);
    
    if(ret != KErrNone)
        {
        Logger().WriteFormat(_L("iHostRslv.Open error code: %d"), ret);    
        return EFail;
        }
    
    return EPass;
    }

	/**
	*   test postamble: close Host Resolver
*/
TVerdict CTestRHostRslvQuery_27_ExplConn::doTestStepPostambleL(void)
    {
    Logger().WriteFormat(_L("stopping and closing connection"));
    iConnection.Stop();
    iConnection.Close();
    
    Logger().WriteFormat(_L("Closing host resolver"));
    iHostRslv.Close();
    
    return EPass;
    }

//-------------------------------------------------------------------------------------------------------
//--  Testing RHostResolver::Query() of 'A' type.
//--  Using Explicit connection.
//-------------------------------------------------------------------------------------------------------
const TDesC& CEsockTest27_1_ExplConn::GetTestName()
    {
    // store the name of this test case
    _LIT(ret,"Test27.7");
	
	return ret;
    }

TVerdict CEsockTest27_1_ExplConn::easyTestStepL()
    {
    TInt                ret = KErrNone;
    TVerdict            verdict = EPass;
    
    Logger().WriteFormat(_L("--- Testing RHostResolver::Query() of 'A' type"));
	
    TInt    nTstExecuted=0;
    TRAP(ret, nTstExecuted=ListDnsRecordsL(&CEsockTest27_1_ExplConn::ListDnsRecordsA));
    TESTEL(ret == KErrNone, ret);
	
    if(!nTstExecuted) //-- no tests executed
        verdict=EFail;
	
    return verdict;
    }


//-------------------------------------------------------------------------------------------------------
//  Testing RHostResolver::Query() functionality on several threads:       
//  one thread performs Query() while another GetByName()
//-------------------------------------------------------------------------------------------------------

const TDesC& CEsockTest27_1_MThread::GetTestName()
    {
    // store the name of this test case
    _LIT(ret,"Test27.8");
	
	return ret;
    }


	/**
	*   thread parameter data ctructure for Tst27_M_Thread.
*/
class TTestMTreadParam
    {
    public:
        
        TTestMTreadParam() : m_nTrials(0), m_nPass(0) {}
        
    public:
        
        TBuf<KMaxStrLen>    m_hostName; //< host name to be resolved
        TInt                m_nTrials;  //< number of name resolution trials
        TInt                m_nPass;    //< number of trials passed;
        
    };


	/**
	*   Thread function for CEsockTest27_1_MThread test.
	*   performs several GetByName calls.
	*
	*   @param  aParam pointer to the thread parameter structure, see TTestMTreadParam.
	*   @return standard error code.
*/
TInt Tst27_M_Thread(TAny* aParam)
    {
    //-- sorry, logging isn't available here.
    //-- the output will be thread completion code and counter of successful name resolutions attempts
    //-- see TTestMTreadParam
    
    TTestMTreadParam* pTestParam = static_cast<TTestMTreadParam*> (aParam);
    
    if(! pTestParam)
        User::Invariant();
    
    TInt ret = KErrNone;
    
    RHostResolver hostRslv;
    RSocketServ   socketServer;
    
    //-- connect to the socket server and open host resolver
    if(socketServer.Connect() != KErrNone || hostRslv.Open(socketServer, KAfInet, KProtocolInetUdp) != KErrNone)
        {    
        ret = KErrGeneral;    
        }
    else
        {   //-- make several name resolution attempts.
        //-- if there was no error, increase pass counter
        TNameEntry  nameEntry;
        
        for(TInt i=0; i<pTestParam->m_nTrials; ++i)
            {
            TInt nRes = hostRslv.GetByName(pTestParam->m_hostName, nameEntry);
            //TSockAddr sockAddr = nameEntry().iAddr;
            
            if(nRes == KErrNone)
                pTestParam->m_nPass++;    
            }
        }
    
    //-- close resolver and socket server
    hostRslv.Close();
    socketServer.Close();
    
    return ret;
    }

TVerdict CEsockTest27_1_MThread::easyTestStepL()
    {
    TInt                ret = KErrNone;
    TVerdict            verdict = EPass;
    TTestMTreadParam    threadParam;
    TNameEntry          nameEntry;
    
    Logger().WriteFormat(_L("--- Testing RHostResolver::Query() functionality with several threads"));
    
    //-- try to check thread functionality firstly.
    //-- try to resolve the host name and if OK, start the thread
    TPtrC buffPtr(threadParam.m_hostName);
    TESTL(GetStringFromConfig(TestStepName(), _L("Param1"), buffPtr)); //-- get host name to resolve
    threadParam.m_hostName.Copy(buffPtr);
    
    ret = iHostRslv.GetByName(threadParam.m_hostName, nameEntry);
    TESTL(ret == KErrNone);
    
    //-- set up thread parameters
    threadParam.m_nTrials = 20; //-- how many attempts the threal will make
    threadParam.m_nPass   = 0;
    
    //-- start thread that will use RHostResolver simultaniously with this one.
    Logger().WriteFormat(_L("- Creating additional thread"));
    
    RThread         thread;
    TRequestStatus  reqStat;
    
    _LIT(KThreadName,"Test27_Thread");
    const TInt KStackSize   =0x8000;
    const TInt KHeapSize    =0x8000;
    const TInt KMaxHeapSize =0x80000;
    
    ret=thread.Create(KThreadName, Tst27_M_Thread, KStackSize,KHeapSize, KMaxHeapSize, &threadParam);
    TESTL(ret == KErrNone);
    
    thread.Resume();
    thread.Logon(reqStat);
    
    Logger().WriteFormat(_L("- additional thread has started"));
    
    //-- now make several host resolver queries at the same time with thread that 
    //-- makes name resolution attempts.
    TBuf8<KMaxStrLen>   queryString;
    TInt                nTstExecuted = 0;
    const TInt          numTrials    = 10;
    
    TESTL(GetIniFileString8(TestStepName(), _L("Param2"), queryString)); //-- get the query string
    
    Logger().WriteFormat(_L("- making %d queries in the main thread"), numTrials);
    
    //-- just query SRV records several times
    for(TInt cnt = 0; cnt < numTrials; ++cnt)
        {
        ret = ListDnsRecordsSRV(queryString);
        TESTL(ret == KErrNone);
        nTstExecuted++;
        
        }// for
    
    if(!nTstExecuted) //-- no tests executed in this thread
        verdict=EInconclusive;
    
    //-- wait for the thread to finish and check the result of its job
    Logger().WriteFormat(_L("- Waiting for additional thread to finish"));
    User::WaitForRequest(reqStat);
    thread.Close();
    
    TESTL(reqStat.Int() == KErrNone);                  //-- thread completion code
    if( threadParam.m_nPass != threadParam.m_nTrials ) //-- check the thread's job result
        verdict=EInconclusive;
    
    
    return verdict;
    }



