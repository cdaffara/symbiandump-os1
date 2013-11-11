// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ptestcases.h"	

#ifdef SYMBIAN_OLD_EXPORT_LOCATION
#include "es_dummy.h"
#else
#include <test/es_dummy.h>
#endif
#include "es_ptestinternalsocket.h"
#include <es_mbuf.h>
#include <es_sock_internal.h>
#include "agregate.h"
#include "ss_eintsock_msgs.h"
#include <elements/nm_address_internal.h>

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockTestptstcs, "ESockTestptstcs.");
#endif


//-------------------------------------------------------------------------------------------------
//	CEIntSockTestBase
//-------------------------------------------------------------------------------------------------	
	
/**	This is a static method that is called to get a given test running
	@param aTestName The name of the test to be run
	@the name of the provider starting the test
 */
 
//static const TInt KWaitInterval = 10000; //Wait 10 milliseconds before doing checks;
//static const TInt KMaxNumWait = 100; // Wait KWaitIntercal 100 times; 	

#define BEGIN_TESTLIST

#define ADD_TEST(name) \
	if (aTestName == _L(#name))\
		{\
		return CEIntSock ## name::NewL(aProvd);\
		}
	
#define END_TESTLIST \
	User::Leave(KErrNotSupported);\
	return NULL;	
	
CEIntSockTestBase* CEIntSockTestBase::CreateTestL(const TDesC& aTestName, CProxyProvd* aProvd)
	{
	BEGIN_TESTLIST
		ADD_TEST(Test1)
		ADD_TEST(Test2)
		ADD_TEST(Test3)
		ADD_TEST(Test4)
		ADD_TEST(Test5)
		ADD_TEST(Test6)
		ADD_TEST(Test7)
		ADD_TEST(Test8)
		ADD_TEST(Test9)
		ADD_TEST(Test10)
		ADD_TEST(Test11)
		ADD_TEST(Test12)
		ADD_TEST(Test13)
		ADD_TEST(Test14)
		ADD_TEST(Test15)
		ADD_TEST(Test16)
		ADD_TEST(Test17)
		ADD_TEST(Test18)
		ADD_TEST(Test19)
		ADD_TEST(Test20)
		ADD_TEST(Test21)
		ADD_TEST(Test22)
		ADD_TEST(Test23)
		ADD_TEST(Test25A)
		ADD_TEST(Test25B)
		ADD_TEST(Test25C)
		ADD_TEST(Test26)
		ADD_TEST(Test27)
		ADD_TEST(Test28A)
		ADD_TEST(Test28B)
		ADD_TEST(Test29)
		ADD_TEST(Test30)
		ADD_TEST(Test31)
		ADD_TEST(Test32)
		ADD_TEST(Test33)
		ADD_TEST(Test34)
		ADD_TEST(Test35)
	    ADD_TEST(Test36)
	    ADD_TEST(Test37)
	    ADD_TEST(Test38)
	    ADD_TEST(Test39)	    
	    ADD_TEST(Test40)	    	    
	    ADD_TEST(Test41)	    	    	    
	    ADD_TEST(Test42)	    	    	    	    
	    ADD_TEST(Test43)	    	    	   
	    ADD_TEST(Test44)	    	    	    	    	    
	    ADD_TEST(Test45)	    	    	    		
	    ADD_TEST(Test46)	    	    	    			    
	    ADD_TEST(Test47)	    	    	    		
	    ADD_TEST(Test48)	    	    	    			    
	    ADD_TEST(Test49)	    	    	    			    
	    ADD_TEST(Test50)	    	    	    		
	    ADD_TEST(Test51)	    	    	    			    
	    ADD_TEST(Test52)	    	    	    			    
	    ADD_TEST(Test53)	    	    	    		
	    ADD_TEST(Test54)	    	    	    			    
	    ADD_TEST(Test55)	    	    	    			    	    
		ADD_TEST(Test56)	    
		ADD_TEST(Test57)		
		ADD_TEST(Test58)				
		ADD_TEST(Test59)			
		ADD_TEST(Test60)					
		ADD_TEST(Test61)                    
	END_TESTLIST
	}


void CEIntSockTestBase::StartTestL(const TDesC& aTestName, CProxyProvd* aProvd)
	{
	CEIntSockTestBase* test = CreateTestL(aTestName,aProvd);
	CleanupStack::PushL(test);
	test->CEIntSockTestBase::ConstructL();
	CActiveScheduler::Add(test);
	CleanupStack::Pop(test);
	TRequestStatus* stat = &test->iStatus;
	test->SetActive();
	User::RequestComplete(stat,KErrNone);
	}

CEIntSockTestBase::CEIntSockTestBase(CProxyProvd* aProvd) :
	CActive(EPriorityNormal),
	iProvd(aProvd)
	{
	iProvd->SetTestStepNotify(this);
	}
	
void CEIntSockTestBase::ConstructL()
	{
	SetLoggerL();
	}
	
CEIntSockTestBase::~CEIntSockTestBase()
	{
	if(iProvd)
		{
		iProvd->SetTestStepNotify(NULL);
		}
	iLogger.Close();
	}

/**	Override this function if you want cleanup to happen before the completion of
	the test case. This function is called before the test is completed 
 */
void CEIntSockTestBase::Cleanup()
	{}

/** Sets the test case as active again and prevents the object from being destroyed
	when the RunL completes.
	
	Note: The active object is not complete the request again automatically, if this behaviour is
	desired the user must call User::RequestComplete
 */
void CEIntSockTestBase::Reschedule()
	{
	SetActive();
	iReschedule = TRUE;
	}

/** Attaches the iLogger variable to the TestExecute Log Server. This enable this prt to write to
	the testexecute logs as if it were running directly from TestExecute
 */
// 	SetLoggerL() is taken directly from CTestServer::StartLoggerL() in the TEF src TestServerBase.Cpp


void CEIntSockTestBase::SetLoggerL()
	{
    	// Create a cinidata object for parsing the testexecute.ini
	CTestExecuteIniData* parseTestExecuteIni = NULL;
	TBuf<KMaxTestExecuteNameLength> resultFilePath;
	TBuf<KMaxTestExecuteNameLength> xmlFilePath;
	TInt logMode;
	TInt logLevel;
	
	TRAPD(err,parseTestExecuteIni = CTestExecuteIniData::NewL());
	if (err == KErrNone)
		{
		CleanupStack::PushL(parseTestExecuteIni);
		parseTestExecuteIni->ExtractValuesFromIni();
		parseTestExecuteIni->GetKeyValueFromIni(KTEFHtmlKey, resultFilePath);
		parseTestExecuteIni->GetKeyValueFromIni(KTEFXmlKey, xmlFilePath);
		parseTestExecuteIni->GetKeyValueFromIni(KTEFLogMode, logMode);
		parseTestExecuteIni->GetKeyValueFromIni(KTEFLogSeverityKey, logLevel);
		}
	else
		{
		resultFilePath.Copy(KTestExecuteLogPath);
		xmlFilePath.Copy(KTestExecuteLogPath);
		logMode = TLoggerOptions(ELogHTMLOnly);
		logLevel = RFileFlogger::TLogSeverity(ESevrAll);
		}
	Logger().SetLoggerOptions(logMode);
		
	// Initialise a handle to the file logger
	User::LeaveIfError(Logger().Connect());
	RFs fS;
	User::LeaveIfError(fS.Connect());
	CleanupClosePushL(fS);
	RFile file;
	TBuf<KMaxTestExecuteNameLength> xmlLogFile(xmlFilePath);
	TBuf<KMaxTestExecuteNameLength> logFile;
	TBuf<KMaxTestExecuteNameLength> logFileNameFile(resultFilePath);
	logFileNameFile.Append(KTestExecuteScheduleTestLogCompatibilityNameFile);
	if(file.Open(fS,logFileNameFile,EFileRead | EFileShareAny) != KErrNone)
		{
		// For the old flogger we have to create an individual file
		_LIT(KTxtLog,"TEIntSock.txt");
		logFile.Copy(KTxtLog);
		logMode = TLoggerOptions(0);
		Logger().SetLoggerOptions(logMode);
		}
	else
		{
		CleanupClosePushL(file);
		TBuf8<KMaxTestExecuteNameLength> logFile8;
		TInt fileSize;
		User::LeaveIfError(file.Size(fileSize));
		User::LeaveIfError(file.Read(logFile8,fileSize));
		logFile.Copy(logFile8);
		xmlLogFile.Append(logFile);
		_LIT(KXmlExtension,".xml");
		xmlLogFile.Append(KXmlExtension);
		_LIT(KHtmExtension,".htm");
		logFile.Append(KHtmExtension);
		CleanupStack::Pop(&file);
		file.Close();
		}
	TBuf<KMaxTestExecuteLogFilePath> logFilePath(resultFilePath);
	logFilePath.Append(logFile);
	CleanupStack::Pop(&fS);
	fS.Close();
	
	if (logMode == 0 || logMode == 2)
		{
		User::LeaveIfError(Logger().HtmlLogger().CreateLog(logFilePath,	RTestExecuteLogServ::ELogModeAppend));
		}
	if (logMode == 1 || logMode == 2)
		{
		User::LeaveIfError(Logger().XmlLogger().CreateLog(xmlLogFile,RFileFlogger::ELogModeAppend));
		Logger().XmlLogger().SetLogLevel(RFileFlogger::TLogSeverity(logLevel));
		}
	if (parseTestExecuteIni != NULL)
		{
		CleanupStack::PopAndDestroy(parseTestExecuteIni);
		}
	}
	

	
/** Called by the active scheduler in order to run the test 
 */
void CEIntSockTestBase::RunL()
	{
	iReschedule = EFalse;
	TVerdict res = RunTestL();
	if (!iReschedule)
		{
		Cleanup();
		FinishTest(res);
		delete this;
		return;
		}
	}

void CEIntSockTestBase::AboutToShutdown()
	{
	// Complete the client before the provider is deleted.
	FinishTest(EPass);
	iProvd = NULL;
	}
	
void CEIntSockTestBase::FinishTest(TVerdict aResult)
	{
	if(iProvd)
		{
		iProvd->TestComplete(aResult);
		}
	}
	
void CEIntSockTestBase::DoCancel()
	{
	Cleanup();
	}
	
//-------------------------------------------------------------------------------------------------
//	CEIntSockTest1
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest1::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest1(aProvd);
	return ret;
	}

CEIntSockTest1::CEIntSockTest1(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd)
	{}
	
TVerdict CEIntSockTest1::RunTestL()
	{
	
	TInt err;
    _LIT(KDescSillyProtocol, "trifle");
    iLogger.WriteFormat(_L("<i>Test 1 - Open and close socket to non-existent protocol</i>"));
	
	iLogger.WriteFormat(_L("<i>Open socket to protocol - name passed as parameter does not exist</i>"));
	err = iSocket.Open(KDescSillyProtocol);
	if (err != KErrNotFound)
		{
		iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
		return EFail; 
		}
	
	iLogger.WriteFormat(_L("<i>Open socket to protocol - address family does not exist</i>"));
	err = iSocket.Open(KDummyAddrFamily, KSockDatagram, 123411); //Some arbitary number
	if (err != KErrBadName)
		{
		iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
		return EFail;
		}
	return EPass;
	}

void CEIntSockTest1::Cleanup()
	{
	iSocket.Close();
	}
	
//-------------------------------------------------------------------------------------------------
//	CEIntSockTest2
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest2::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest2(aProvd);
	return ret;
	}

CEIntSockTest2::CEIntSockTest2(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd)
	{}
	
TVerdict CEIntSockTest2::RunTestL()
	{
	TInt err;
    iLogger.WriteFormat(_L("<i>Test 2 - Open and close socket</i>"));
	iLogger.WriteFormat(_L("<i>Open socket to protocol by name</i>"));
	err = iSocket.Open(KDummyTwoName);
	if (err != KErrNone)
		{
		iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
		return EFail;
		}
	iSocket.Close();
	iLogger.WriteFormat(_L("<i>Open socket to protocol supplying address family</i>"));
	err = iSocket.Open(KDummyAddrFamily, KSockDatagram, KDummyOne);
	if (err != KErrNone)
		{
		iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
		return EFail;
		}
	
	iSocket.Close();
	// this is a null socket for use with accept, can't specify protocol as no parameters
	iLogger.WriteFormat(_L("<i>Open null socket</i>"));
	err = iSocket.Open();
	if (err != KErrNone)
		{
		iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
		return EFail;
		}		
	return EPass;
	}

void CEIntSockTest2::Cleanup()
	{
	iSocket.Close();
	}

//-------------------------------------------------------------------------------------------------
//	CEIntSockTest3
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest3::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest3(aProvd);
	return ret;
	}

CEIntSockTest3::CEIntSockTest3(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd)
	{}
	
TVerdict CEIntSockTest3::RunTestL()
	{
	TSockAddr localAddr(KProxyAddrFamily);
	TInt err;
	iLogger.WriteFormat(_L("<i>Test 3 - Binding</i>"));
	iLogger.WriteFormat(_L("<i>Opening a socket to PDummy 2</i>"));
	err = iSocket.Open(KDummyTwoName);
	if (err != KErrNone)
		{
		iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
		return EFail;
		}
				
	iLogger.WriteFormat(_L("<i>Binding socket to TSockAddr</i>"));
	err = iSocket.Bind(localAddr);
	if (err != KErrNone)
		{
		iLogger.WriteFormat(_L("<i>Bind() failed with %d</i>"),err);
		return EFail;
		}

	iLogger.WriteFormat(_L("<i>Checking LocalName() returns the correct address following the bind</i>"));
	TSockAddr actualAddr;
	iSocket.LocalName(actualAddr);
	if (actualAddr != localAddr)
		{
		iLogger.WriteFormat(_L("<i>Name didn't match the one passed to Bind()</i>"));
		return EFail;
		}

	iLogger.WriteFormat(_L("<i>Closing the socket</i>"));
	iSocket.Close();
	
	iLogger.WriteFormat(_L("<i>Opening a socket to PDummy 2</i>"));
	err = iSocket.Open(KDummyTwoName);	
	if (err != KErrNone)
		{				
		iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
		return EFail;
		}
	iLogger.WriteFormat(_L("<i>Setting local port of socket</i>"));
	TUint localPort = 7;
	err = iSocket.SetLocalPort(localPort);
	if (err != KErrNone)
		{				
		iLogger.WriteFormat(_L("<i>SetLocalPort() failed with %d</i>"),err);
		return EFail;
		}

	iLogger.WriteFormat(_L("<i>Checking LocalPort() returns the correct port number following the bind</i>"));
	TUint actualPort;
	actualPort = iSocket.LocalPort();
	if (actualPort != localPort)
		{
		iLogger.WriteFormat(_L("<i>Port didn't match the one passed to SetLocalPort()</i>"));
		return EFail;
		}
	return EPass;
	}

void CEIntSockTest3::Cleanup()
	{
	iSocket.Close();
	}


//-------------------------------------------------------------------------------------------------
//	CEIntSockTest4
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest4::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest4(aProvd);
	return ret;
	}

CEIntSockTest4::CEIntSockTest4(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd)
	{}
	
TVerdict CEIntSockTest4::RunTestL()
	{
	TInt err;
	TProtocolDesc info;
	
	iLogger.WriteFormat(_L("<i>Test 4 - ProtocolInfo </i>"));
	iLogger.WriteFormat(_L("<i>Opening a socket to PDummy 2</i>"));
	err = iSocket.Open(KDummyTwoName);
	if (err != KErrNone)
		{
		iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
		return EFail;
		}
		
	iLogger.WriteFormat(_L("<i>Getting info about the protocol</i>"));
	err = iSocket.Info(info);
	if (err != KErrNone)
		{
		iLogger.WriteFormat(_L("<i>Info() failed with %d</i>"),err);
		return EFail;				
		}
	
	iLogger.WriteFormat(_L("<i>Checking that the address is of type PDummyAddrFamily</i>"));
	if (info.iAddrFamily != KDummyAddrFamily)
		{
		iLogger.WriteFormat(_L("<i>Address family in return unexpectedly was %X</i>"),info.iAddrFamily);
		return EFail;					
		}
	return EPass;
	}

void CEIntSockTest4::Cleanup()
	{
	iSocket.Close();
	}


//-------------------------------------------------------------------------------------------------
//	CEIntSockTest5
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest5::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest5(aProvd);
	return ret;
	}

CEIntSockTest5::CEIntSockTest5(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1), iRecvBufPtr(NULL, 0)
	{}
	
TVerdict CEIntSockTest5::RunTestL()
	{
	
	TInt err;
	_LIT8(KSendData,"1234567890");
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 5 - Send to and receive from a stream protocol (Descriptor API) </i>"));
			iLogger.WriteFormat(_L("<i>Opening a socket to PDummy 2</i>"));
			err = iSocket.Open(KDummyTwoName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
		
			iLogger.WriteFormat(_L("<i>Opening an internal socket (stream protocol)</i>"));
			
			iAddress.SetFamily(KProxyAddrFamily);
			iSocket.Connect(iAddress, iStatus);
			
			iState = EState2;
			Reschedule();
			return EPass;		
			}
		case EState2:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Connect() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			iBuf.Copy ( KSendData() );
			iLogger.WriteFormat(_L("<i>Sending data over an internal socket using the descriptor API</i>"));
			iSocket.Send(iBuf,0,iStatus);
			
			iState = EState3;
			Reschedule();
			return EPass;	
			}
		case EState3:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Send() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			
			iLogger.WriteFormat(_L("<i>Receiving data over an internal socket using the descriptor API</i>"));
			iBuf.Zero();
			iBuf.SetLength(KSendData().Length());
			iRecvBufPtr.Set(( TUint8* )iBuf.PtrZ(), KSendData().Length(), KSendData().Length());
			iSocket.Recv(iRecvBufPtr,0,iStatus);
			
			iState = EState4;
			Reschedule();
			return EPass;				
			}
		case EState4:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Recv() didn't return KErrNotSupported (returned %d)</i>"),iStatus.Int());
				return EFail;
				}

			iLogger.WriteFormat(_L("<i>Checking that the data was read into the buffer</i>"));
			if (iBuf != KSendData)
				{
				iLogger.WriteFormat(_L("<i>Incorrect data to recved</i>"));
				return EFail;
				}
				
			return EPass;
			}
		default: 
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 1));
			return EFail;
		}
	}

void CEIntSockTest5::Cleanup()
	{
	iSocket.Close();
	}
	

//-------------------------------------------------------------------------------------------------
//	CEIntSockTest6
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest6::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest6(aProvd);
	return ret;
	}

CEIntSockTest6::CEIntSockTest6(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
TVerdict CEIntSockTest6::RunTestL()
	{
	TInt err;
	_LIT8(KSendData,"1234567890");
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 6 - Send to and receive from a datagram protocol (Descriptor API)</i>"));
			
			iLogger.WriteFormat(_L("<i>Opening PDummy 1</i>"));
			err = iSocket.Open(KDummyOneName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
		
			iAddress.SetFamily (KProxyAddrFamily);
			iLogger.WriteFormat(_L("<i>Opening an internal socket (datagram protocol)</i>"));
			iSocket.Connect(iAddress, iStatus);
						
			iState = EState2;
			Reschedule();
			return EPass;				
			}
		case EState2:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Connect() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			
			iBuf.Copy ( KSendData() );
			iLogger.WriteFormat(_L("<i>Sending data over an internal socket using the descriptor API</i>"));
			iSocket.Send(iBuf,0,iStatus);
			
			iState = EState3;
			Reschedule();
			return EPass;				
			}
		case EState3:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Send() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			
			iLogger.WriteFormat(_L("<i>Receiving data over an internal socket using the descriptor API</i>"));
			iBuf.Zero();
			iBuf.SetLength(KSendData().Length());
			iSocket.Recv(iBuf,0,iStatus);
						
			iState = EState4;
			Reschedule();
			return EPass;	
			}
		case EState4:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Recv() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
				
			iLogger.WriteFormat(_L("<i>Checking that the data was read into the buffer</i>"));
			if (iBuf != KSendData)
				{
				iLogger.WriteFormat(_L("<i>Incorrect data to recved</i>"));
				return EFail;
				}
			return EPass;	
			}
		default: 
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 2));
			return EFail;
		}
	}

void CEIntSockTest6::Cleanup()
	{
	iSocket.Close();
	}
	
	
	
//-------------------------------------------------------------------------------------------------
//	CEIntSockTest7
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest7::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest7(aProvd);
	return ret;
	}

CEIntSockTest7::CEIntSockTest7(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
TVerdict CEIntSockTest7::RunTestL()
	{
	TInt err;
	_LIT8(KSendData,"1234567890");
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>	Test 7 - Send to and receive from a stream protocol (RMBufChain API)</i>"));
			iLogger.WriteFormat(_L("<i>Opening PDummy 2</i>"));
			err = iSocket.Open(KDummyTwoName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
			
			iAddress.SetFamily(KProxyAddrFamily);
			iLogger.WriteFormat(_L("<i>Opening an internal socket (stream protocol)</i>"));
			iSocket.Connect(iAddress, iStatus);
			
			iState = EState2;
			Reschedule();
			return EPass;	
			}
		case EState2:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Connect() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
				
			iLogger.WriteFormat(_L("<i>Sending data over an internal socket using the RMBufChain API</i>"));
			
			iSendChain.CreateL(KSendData());
			iSocket.Send(iSendChain,0,iStatus);
			
			
			iState = EState3;
			Reschedule();
			return EPass;				
			}
		case EState3:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Send() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			// dummy prt won't take ownership of this chain. we have still this data. but when we
			// do a receive we will get the "same" chain back. so we have to remove the chain that we sent.
			// Now data is sent. 
			iSendChain.Remove ();
		
			iLogger.WriteFormat(_L("<i>Receiving data over an internal socket using the RMBufChain API</i>"));
			
			iBuf.SetLength(KSendData().Length());
			iSocket.Recv(iRecvChain,0,iStatus);
			
			iState = EState4;
			Reschedule();
			return EPass;
			}
		case EState4:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Recv() failed returning %d</i>"),iStatus.Int());
				return EFail;
				}
			iRecvChain.CopyOut(iBuf);
			
			iRecvChain.Free();
			
			iLogger.WriteFormat(_L("<i>Checking that the data was read into the buffer</i>"));
			if (iBuf != KSendData)
				{
				iLogger.WriteFormat(_L("<i>Incorrect data recieved</i>"));
				return EFail;
				}

			return EPass;
			}


		default: 
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 3));
			return EFail;
		}
	}

void CEIntSockTest7::Cleanup()
	{
	iSocket.Close();
	__ASSERT_DEBUG(iSendChain.IsEmpty(), User::Panic(KSpecAssert_ESockTestptstcs, 4));
	__ASSERT_DEBUG(iRecvChain.IsEmpty(), User::Panic(KSpecAssert_ESockTestptstcs, 5));
	}	

//-------------------------------------------------------------------------------------------------
//	CEIntSockTest8
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest8::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest8(aProvd);
	return ret;
	}

CEIntSockTest8::CEIntSockTest8(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
TVerdict CEIntSockTest8::RunTestL()
	{
	TInt err;
	_LIT8(KSendData,"1234567890");
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>	Test 8 - Send to and receive from a datagram protocol (RMBufChain API)</i>"));
			iLogger.WriteFormat(_L("<i>Opening PDummy 1</i>"));
			err = iSocket.Open(KDummyOneName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
			
			iAddress.SetFamily(KProxyAddrFamily);
			iLogger.WriteFormat(_L("<i>Opening an internal socket (datagram protocol)</i>"));
			iSocket.Connect(iAddress, iStatus);
						
			iState = EState2;
			Reschedule();
			return EPass;								
			}
		case EState2:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Connect() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
				
			iLogger.WriteFormat(_L("<i>Sending data over an internal socket using the RMBufChain API</i>"));
			
			iSendChain.CreateL(KSendData());
			iSocket.Send(iSendChain,0,iStatus);
		
			iState = EState3;
			Reschedule();
			return EPass;					
			}
		case EState3:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Send() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			// dummy prt won't take ownership of this chain. we have still this data. but when we
			// do a receive we will get the "same" chain back. so we have to remove the chain that we sent.
			// Now we sent
			iSendChain.Remove ();
		
			iLogger.WriteFormat(_L("<i>Receiving data over an internal socket using the RMBufChain API</i>"));
			iSocket.Recv(iRecvChain,0,iStatus);
			
			iState = EState4;
			Reschedule();
			return EPass;		
			}
		case EState4:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Recv() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Checking that the data was read into the buffer</i>"));
			iBuf.SetLength(KSendData().Length());
			iRecvChain.CopyOut(iBuf);
			iRecvChain.Free();
			if(iBuf.Compare(KSendData()) == 0)
				{
				iLogger.WriteFormat(_L("<i>Recv() data matched that sent</i>"));
				}
			else
				{
				iLogger.WriteFormat(_L("<i>Recv() data != that send (%S recvd vs %S)</i>"), &iBuf, &KSendData());
				return EFail;
				}
				
			return EPass;					
			}

		default: 
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 6));
			return EFail;
		}
	}

void CEIntSockTest8::Cleanup()
	{
	iSocket.Close();
	__ASSERT_DEBUG(iSendChain.IsEmpty(), User::Panic(KSpecAssert_ESockTestptstcs, 7));
	__ASSERT_DEBUG(iRecvChain.IsEmpty(), User::Panic(KSpecAssert_ESockTestptstcs, 8));
	}	
	
//-------------------------------------------------------------------------------------------------
//	CEIntSockTest9
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest9::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest9(aProvd);
	return ret;
	}

CEIntSockTest9::CEIntSockTest9(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd)
	{}
	
TVerdict CEIntSockTest9::RunTestL()
	{
	TInt err;
	iLogger.WriteFormat(_L("<i>Test 9 - Get and Set Options</i>"));
	iLogger.WriteFormat(_L("<i>Opening to PDummy 1</i>"));
	err = iSocket.Open(KDummyOneName);
	if (err != KErrNone)
		{
		iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
		return EFail;
		}
	iLogger.WriteFormat(_L("<i>Setting Blocked connect to TRUE</i>"));
	err = iSocket.SetOpt(KDummyOptionSetBlockConnect,28,TRUE);
	if (err != KErrNone)
		{
		iLogger.WriteFormat(_L("<i>SetOpt() failed with %d</i>"),err);
		return EFail;
		}
	TPckgBuf<TBool> boolean;
	iLogger.WriteFormat(_L("<i>Getting Blocked connect Sock opt</i>"));
	err = iSocket.GetOpt(KDummyOptionSetBlockConnect,28, boolean);
	if (err != KErrNone)
		{
		iLogger.WriteFormat(_L("<i>SetOpt() failed with %d</i>"),err);
		return EFail;
		}
	iLogger.WriteFormat(_L("<i>Checking that Blocked connect Sock opt returns true</i>"));
	if (boolean() != TRUE)
		{
		iLogger.WriteFormat(_L("GetOpt did not return true - looks like the opt was not set"));
		return EFail;
		}
	iLogger.WriteFormat(_L("<i>Setting Blocked connect to FALSE</i>"));
	err = iSocket.SetOpt(KDummyOptionSetBlockConnect,28,FALSE);
	if (err != KErrNone)
		{
		iLogger.WriteFormat(_L("<i>SetOpt() failed with %d</i>"),err);
		return EFail;
		}
	iLogger.WriteFormat(_L("<i>Getting Blocked connect Sock opt</i>"));
	err = iSocket.GetOpt(KDummyOptionSetBlockConnect,28, boolean);
	if (err != KErrNone)
		{
		iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
		return EFail;
		}
	iLogger.WriteFormat(_L("<i>Checking Blocked connect is FALSE</i>"));
	if (boolean() != FALSE)
		{
		iLogger.WriteFormat(_L("GetOpt did not return false - looks like the opt was not set"));
		return EFail;
		}
	return EPass;	
	}

void CEIntSockTest9::Cleanup()
	{
	iSocket.Close();
	}	
	
//-------------------------------------------------------------------------------------------------
//	CEIntSockTest10
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest10::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest10(aProvd);
	return ret;
	}

CEIntSockTest10::CEIntSockTest10(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
TVerdict CEIntSockTest10::RunTestL()
	{
	TInt err;
	switch(iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 10 - Ioctl</i>"));
			iLogger.WriteFormat(_L("<i>Opening PDummy 3</i>"));
			err = iSocket.Open(KDummyThreeName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Sending an ioctl that does nothing</i>"));
			iSocket.Ioctl(KDummyIocltJustComplete, iStatus);

			iState  = EState2;
			Reschedule();
			return EPass;
			}
		case EState2:
			{
			iLogger.WriteFormat(_L("<i>Checking that the Ioctl() completes</i>"));
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Ioctl() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			return EPass;
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 9));
			return EFail;
		}
	}

void CEIntSockTest10::Cleanup()
	{
	iSocket.Close();
	}	

//-------------------------------------------------------------------------------------------------		
//	CEIntSockTest11
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest11::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest11(aProvd);
	return ret;
	}

CEIntSockTest11::CEIntSockTest11(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
TVerdict CEIntSockTest11::RunTestL()
	{	
	TInt err;
	switch(iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 11 - Cancel Ioctls</i>"));
			iLogger.WriteFormat(_L("<i>Opening PDummy 1</i>"));
			err = iSocket.Open(KDummyOneName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Sending an ioclt that does not complete</i>"));
			iSocket.Ioctl(KDummyIocltNonCompleting, iStatus);
			if (iStatus != KRequestPending)
				{
				iLogger.WriteFormat(_L("<i>Ioclt() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Completing the ioctl with a SocketOpt</i>"));
			err = iSocket.SetOpt(KDummyOptionSetIocltComplete,28);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>SetOpt() failed with %d</i>"),err);
				return EFail;
				}
			iState  = EState2;
			Reschedule();
			return EPass;
			}
		case EState2:
			{
			iLogger.WriteFormat(_L("<i>Checking the completion code for the ioclt</i>"));
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Ioctl() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Sending an ioclt that does not complete</i>"));
			iSocket.Ioctl(KDummyIocltNonCompleting, iStatus);
			iLogger.WriteFormat(_L("<i>Cancelling the Ioclt</i>"));
			iSocket.CancelIoctl();
			iState  = EState3;
			Reschedule();
			return EPass;
			}
		case EState3:
			{
			iLogger.WriteFormat(_L("<i>Checking the completion code for the ioclt in KErrCancel</i>"));
			if (iStatus != KErrCancel)
				{
				iLogger.WriteFormat(_L("<i>Ioclt() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Completing the ioctl with a SocketOpt</i>"));
			err = iSocket.SetOpt(KDummyOptionSetIocltComplete,28);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>SetOpt() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Checking the completion code for the ioclt in KErrCancel</i>"));
			if (iStatus != KErrCancel)
				{
				iLogger.WriteFormat(_L("<i>Ioclt() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			return EPass;
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 10));
			return EFail;
		}
	}

void CEIntSockTest11::Cleanup()
	{
	iSocket.Close();
	}
	
//-------------------------------------------------------------------------------------------------		
//	CEIntSockTest12
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest12::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest12(aProvd);
	return ret;
	}

CEIntSockTest12::CEIntSockTest12(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
TVerdict CEIntSockTest12::RunTestL()
	{
	TInt err;
	switch(iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 12 - Cancel Accept</i>"));
			iLogger.WriteFormat(_L("<i>Opening PDummy 3</i>"));
			err = iSocket.Open(KDummyThreeName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
			TSockAddr addr;
			iLogger.WriteFormat(_L("<i>Performing Bind</i>"));
			err =iSocket.Bind(addr);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Bind() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Performing Listen</i>"));
			err = iSocket.Listen(5);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Listen() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Opening a NULL socket</i>"));
			err = iNewSock.Open();
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
			
			iLogger.WriteFormat(_L("<i>Accepting a connection</i>"));
			iSocket.Accept(iNewSock,iStatus);
			
			iLogger.WriteFormat(_L("<i>Setting a sockopt to simulate a connect</i>"));
			err = iSocket.SetOpt(KDummyOptionSetConnectComplete,27);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>SetOpt() failed with %d</i>"),err);
				return EFail;
				}
			iState  = EState2;
			Reschedule();
			return EPass;
			}
			
		case EState2:
			{
			iLogger.WriteFormat(_L("<i>Checking that the accept completes</i>"));
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Accept() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Closing and then reopenning the new socket</i>"));	
			iNewSock.Close();
			err = iNewSock.Open();
			if(err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),iStatus.Int());
				return EFail;
				}

			iLogger.WriteFormat(_L("<i>Accepting a connection</i>"));
			iSocket.Accept(iNewSock,iStatus);
			
			iLogger.WriteFormat(_L("<i>Cancelling the accept</i>"));
			iSocket.CancelAccept();
			iState  = EState3;
			Reschedule();
			return EPass;
			}
		case EState3:
			{
			iLogger.WriteFormat(_L("<i>Checking that the accept completes with KErrCancel</i>"));
			if (iStatus != KErrCancel)
				{
				iLogger.WriteFormat(_L("<i>Accept() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Setting a sockopt to simulate a connect</i>"));
			err = iSocket.SetOpt(KDummyOptionSetConnectComplete,27);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>SetOpt() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Checking that the status code is still KErrCancel</i>"));
			if (iStatus != KErrCancel)
				{
				iLogger.WriteFormat(_L("<i>Accept() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			return EPass;
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 11));
			return EFail;
		}
	}	
	
void CEIntSockTest12::Cleanup()
	{
	iSocket.Close();
	iNewSock.Close();
	}
	
//-------------------------------------------------------------------------------------------------		
//	CEIntSockTest13
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest13::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest13(aProvd);
	return ret;
	}

CEIntSockTest13::CEIntSockTest13(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
TVerdict CEIntSockTest13::RunTestL()
	{
	TInt err;
	switch(iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 13 - Cancel Connect</i>"));
			iLogger.WriteFormat(_L("<i>Opening PDummy 3</i>"));
			err = iSocket.Open(KDummyThreeName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Setting sockopt not to complete connect</i>"));
			err = iSocket.SetOpt(KDummyOptionSetBlockConnect,28,TRUE); //Block connect
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>SetOpt() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Performing connect</i>"));
			iSocket.Connect(iAddress, iStatus);
			if (iStatus != KRequestPending)
				{
				iLogger.WriteFormat(_L("<i>Ioclt() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
				
			iLogger.WriteFormat(_L("<i>Setting sockopt to now complete the connect</i>"));
			err = iSocket.SetOpt(KDummyOptionSetBlockConnect,28,FALSE); //Unblock Connect
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>SetOpt() failed with %d</i>"),err);
				return EFail;
				}
		
			iState  = EState2;
			Reschedule();
			return EPass;
			}
		case EState2:
			{
			iLogger.WriteFormat(_L("<i>Checking that the connect completes</i>"));
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Ioclt() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Closing and Reopening socket</i>"));
			iSocket.Close();
			err = iSocket.Open(KDummyTwoName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Setting sockopt not to complete connect</i>"));
			err = iSocket.SetOpt(KDummyOptionSetBlockConnect,28,TRUE); //Block connect
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>SetOpt() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Performing connect</i>"));
			iSocket.Connect(iAddress, iStatus);
			if (iStatus != KRequestPending)
				{
				iLogger.WriteFormat(_L("<i>Ioclt() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			
			iLogger.WriteFormat(_L("<i>Cancelling connect</i>"));
			iSocket.CancelConnect();
			
			iState  = EState3;
			Reschedule();
			return EPass;
			}
		case EState3:
			{
			iLogger.WriteFormat(_L("<i>Checking that connect completes with KErrCancel</i>"));
			if (iStatus != KErrCancel)
				{
				iLogger.WriteFormat(_L("<i>Cancel() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
		
			iLogger.WriteFormat(_L("<i>Attepting to simulate the completion of the connection</i>"));
			err = iSocket.SetOpt(KDummyOptionSetBlockConnect,28,FALSE); //Unblock Connect
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>SetOpt() failed with %d</i>"),err);
				return EFail;
				}
			
			iLogger.WriteFormat(_L("<i>Checking that the status remains KErrCancel</i>"));	
			if (iStatus != KErrCancel)
				{
				iLogger.WriteFormat(_L("<i>iStatus is now %d</i>"),iStatus.Int());
				return EFail;
				}
				
			return EPass;
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 12));
			return EFail;
		}
	}	
	
void CEIntSockTest13::Cleanup()
	{
	iSocket.Close();
	}
	
//-------------------------------------------------------------------------------------------------		
//	CEIntSockTest14
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest14::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTest14* ret = new(ELeave) CEIntSockTest14(aProvd);
	CleanupStack::PushL(ret);
	ret->ConstructL();	
	CleanupStack::Pop(ret);
	return ret;
	}

CEIntSockTest14::CEIntSockTest14(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
CEIntSockTest14::~CEIntSockTest14()
	{
	delete iColl;
	}
	
void CEIntSockTest14::ConstructL()
	{
	iColl = CAggregateCollection::NewL(iStatus,CAggregateCollection::EAny);
	iSendItem = iColl->GetNewItemL();
	iIoctlItem = iColl->GetNewItemL();
	iConnectItem = iColl->GetNewItemL();
	}
	
TVerdict CEIntSockTest14::RunTestL()
	{
	_LIT8(KSendData,"1234567890");
	TInt err;
	switch(iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>	Test 14 - Transfer Socket</i>"));
			iLogger.WriteFormat(_L("<i>	Opening PDummy 1</i>"));
			err = iSocket.Open(KDummyOneName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Sending through a non completing ioctl</i>"));
			iIoctlItem->Start();
			iSocket.Ioctl(KDummyIocltNonCompleting, iIoctlItem->iStatus);
			iLogger.WriteFormat(_L("<i>Attempting to transfer the socket from one to another</i>"));
			err = iNewSock.Transfer(iSocket);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
			iSocket.CancelIoctl();
			if (iStatus == KErrCancel)
				{
				iLogger.WriteFormat(_L("<i>Ioclt() from parent socket returned %d</i>"),iStatus.Int());
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Connecting...</i>"));
			iConnectItem->Start();
			iNewSock.Connect(iAddress, iConnectItem->iStatus);
			iState = EState2;
			Reschedule();
			iColl->Kick();
			return EPass;
			}
		case EState2:
			{
			if (iIoctlItem->ReturnCode() != KRequestPending) //Since the socket was tranferred cancel should do nothing
				{
				iLogger.WriteFormat(_L("<i>Cancel) from parent socket returned %d</i>"),iIoctlItem->ReturnCode());
				return EFail;	
				}
			if(iConnectItem->iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Connect() failed with error code %d</i>"),iStatus.Int());
				return EFail;					
				}
			iSendItem->Start();
			iNewSock.Send(KSendData(),0,iSendItem->iStatus);
			iState = EState3;
			Reschedule();
			iColl->Kick();
			return EPass;			
			}	
		case EState3:
			{
			if (iSendItem->ReturnCode() != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Send() from new socket returned %d</i>"),iSendItem->iStatus.Int());
				return EFail;	
				}
			iLogger.WriteFormat(_L("<i>Receiving data over an internal socket using the descriptor API</i>"));
			iBuf.Zero();
			iNewSock.Recv(iBuf,0,iStatus);
			iState = EState4;
			Reschedule();
			return EPass;
			}
		case EState4:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Recv() from new socket returned %d</i>"),iStatus.Int());
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Checking that the data is recieved correctly</i>"));
			if (iBuf != KSendData())
				{
				iLogger.WriteFormat(_L("<i>Recv() did not return any data </i>"));
				return EFail;	
				}
			iNewSock.CancelIoctl();
			iState = EState5;
			iColl->Kick();
			Reschedule();
			return EPass;									
			}
		case EState5:
			{
			if (iIoctlItem->ReturnCode() != KErrCancel)
				{
				iLogger.WriteFormat(_L("<i>Ioclt() from new socket returned %d</i>"),iIoctlItem->iStatus.Int());
				return EFail;	
				}
			return EPass;
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 13));
			return EFail;
		}
	}	
	
void CEIntSockTest14::Cleanup()
	{
	iSocket.Close();
	iNewSock.Close();
	}
	
//-------------------------------------------------------------------------------------------------		
//	CEIntSockTest15
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest15::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest15(aProvd);
	return ret;
	}

CEIntSockTest15::CEIntSockTest15(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
TVerdict CEIntSockTest15::RunTestL()
	{
	TInt err;
	switch(iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>	Test 15 - Connect socket and close before completion</i>"));
			iLogger.WriteFormat(_L("<i>Opening socket to PDummy Two (stream protocol)</i>"));
			err = iSocket.Open(KDummyTwoName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}

			iLogger.WriteFormat(_L("<i>Connecting an internal socket (stream protocol)</i>"));
			iAddress.SetFamily(KProxyAddrFamily);
			iSocket.Connect(iAddress, iStatus);

			iLogger.WriteFormat(_L("<i>Closing the socket without waiting for Connect() to complete</i>"));
			// actually it probably is complete as it is pretty much synchronous with this protocol
			iSocket.Close();

			iState = EState2;
			Reschedule();
			return EPass;
			}
		case EState2:
			{
			iLogger.WriteFormat(_L("<i>Checking the status of the Connect()</i>"));
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Connect() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
				
			iLogger.WriteFormat(_L("<i>Opening socket to PDummy One (datagram protocol)</i>"));
			err = iSocket.Open(KDummyOneName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
				
			iLogger.WriteFormat(_L("<i>Connecting an internal socket (datagram protocol)</i>"));
			iAddress.SetFamily(KProxyAddrFamily);
			iSocket.Connect(iAddress, iStatus);

			iLogger.WriteFormat(_L("<i>Closing the socket without waiting for Connect() to complete</i>"));
			// actually it probably is complete as it is pretty much synchronous with this protocol
			iSocket.Close();
			
			iState = EState3;
			Reschedule();
			return EPass;
			}
		case EState3:
			{
			iLogger.WriteFormat(_L("<i>Checking the status of the Connect()</i>"));
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Connect() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			return EPass;
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 14));
			return EFail;
		}
	}	
	
void CEIntSockTest15::Cleanup()
	{
	iSocket.Close();
	}
	
	
//-------------------------------------------------------------------------------------------------		
//	CEIntSockTest16
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest16::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest16(aProvd);
	return ret;
	}

CEIntSockTest16::CEIntSockTest16(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
TVerdict CEIntSockTest16::RunTestL()
	{
	TInt err;
	switch(iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>	Test 16 - Send data and close socket without waiting for Send to complete</i>"));
			iLogger.WriteFormat(_L("<i>Opening socket to PDummy One (datagram protocol)</i>"));
			err = iSocket.Open(KDummyOneName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
		
			iLogger.WriteFormat(_L("<i>Connecting an internal socket (stream protocol)</i>"));
			iAddress.SetFamily(KProxyAddrFamily);
			iSocket.Connect(iAddress, iStatus);
			
			iState = EState2;
			Reschedule();
			return EPass;		
			}
		case EState2:
			{
			iLogger.WriteFormat(_L("<i>Checking the status of the Connect()</i>"));
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Connect() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
				
			iLogger.WriteFormat(_L("<i>Sending data over an internal socket using the descriptor API</i>"));
			TBuf8<20> buf = _L8("pancake");
			iSocket.Send(buf,0,iStatus);
			
			iLogger.WriteFormat(_L("<i>Close socket before Send() completes</i>"));
			iSocket.Close();
			
			iState = EState3;
			Reschedule();
			return EPass;
			}
		case EState3:
			{
			iLogger.WriteFormat(_L("<i>Checking the status of the Send()</i>"));
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Send() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			return EPass;	
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 15));
			return EFail;
		}
	}	
	
void CEIntSockTest16::Cleanup()
	{
	iSocket.Close();
	}
	
//-------------------------------------------------------------------------------------------------		
//	CEIntSockTest17
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest17::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTest17* ret = new(ELeave) CEIntSockTest17(aProvd);
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}
	
void CEIntSockTest17::ConstructL()
	{
	iColl = CAggregateCollection::NewL(iStatus,CAggregateCollection::EAny);
	iSendItem = iColl->GetNewItemL();
	iRecvItem = iColl->GetNewItemL();
	}

CEIntSockTest17::CEIntSockTest17(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
CEIntSockTest17::~CEIntSockTest17()
	{
	delete iColl;
	}
	
TVerdict CEIntSockTest17::RunTestL()
	{
	TInt err;
	_LIT8(KSendData,"1234567890");
	switch(iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 17 - Cancel Recv</i>"));
			iLogger.WriteFormat(_L("<i>Attempting to open socket on PDummy1</i>"));
			err = iSocket.Open(KDummyOneName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Attempting to perform a connect on PDummy1</i>"));
			iSocket.Connect(iAddress,iStatus);
			iState = EState2;
			Reschedule();
			return EPass;
			}
		case EState2:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Connect() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Attempting to perform a recv</i>"));
			iRecvItem->Start();
			TInt size = iBuf.MaxLength ();
			iBuf.Zero ();
			iSocket.Recv(iBuf,0,iRecvItem->iStatus);
			
			iLogger.WriteFormat(_L("<i>Attempting to perform a send</i>"));
			iSendItem->Start();
			iSocket.Send(KSendData(),0,iSendItem->iStatus);
			iState = EState3;
			iColl->Kick();
			Reschedule();
			return EPass;	
			}
		case EState3:
			{
			if (iSendItem->ReturnCode() != KRequestPending && iRecvItem->ReturnCode() != KRequestPending)
				{
				if(iSendItem->ReturnCode()!= KErrNone)
					{
					iLogger.WriteFormat(_L("<i>Send() failed with %d</i>"),iSendItem->ReturnCode());
					return EFail;
					}
				iLogger.WriteFormat(_L("<i>Checking that the recv returned correctly</i>"));
				if(iRecvItem->ReturnCode() != KErrNone)
					{
					iLogger.WriteFormat(_L("<i>Recv() failed with %d</i>"),iRecvItem->ReturnCode());
					return EFail;
					}
				if (iBuf != KSendData)
					{
					iLogger.WriteFormat(_L("<i>Recv did not return expected data</i>"));
					return EFail;
					}
				iLogger.WriteFormat(_L("<i>Attempting to perform a another recv</i>"));
				iBuf.Zero();
				iRecvItem->Start();
				iSocket.Recv(iBuf,0,iRecvItem->iStatus);
				
				iLogger.WriteFormat(_L("<i>Attempting to cancel the recv</i>"));
				iSocket.CancelRecv();
							
				iState =EState4;
				}
			//Else reschedule and wait for soemthing to come in
			iColl->Kick();
			Reschedule();
			return EPass;
			}
		case EState4:
			{
			iLogger.WriteFormat(_L("<i>Checking that the recv is cancelled</i>"));
			if(iRecvItem->ReturnCode() != KErrCancel)
				{
				iLogger.WriteFormat(_L("<i>Recv() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			
			iLogger.WriteFormat(_L("<i>Attempting to perform a send</i>"));
			iSendItem->Start();
			iSocket.Send(KSendData,0,iSendItem->iStatus);
			
			iState = EState5;
			iColl->Kick();
			Reschedule();
			return EPass;
			}
		case EState5:
			{
			if (iSendItem->ReturnCode() == KRequestPending || iRecvItem->ReturnCode() == KRequestPending)
				{
				//go back to active scheduler and wait for reschedule
				iColl->Kick();
				Reschedule();
				return EFail;
				}
			if(iSendItem->ReturnCode() != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Send() failed with %d</i>"),iSendItem->ReturnCode());
				return EFail;
				}
			
			iLogger.WriteFormat(_L("<i>Checking that the recv is still cancelled</i>"));
			if(iRecvItem->ReturnCode() != KErrCancel)
				{
				iLogger.WriteFormat(_L("<i>Recv() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
				
			return EPass;
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 16));
			return EFail;
		}
	}	
	
void CEIntSockTest17::Cleanup()
	{
	iSocket.Close();
	}

//-------------------------------------------------------------------------------------------------		
//	CEIntSockTest18
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest18::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest18(aProvd);
	return ret;
	}

CEIntSockTest18::CEIntSockTest18(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd)	
	{}
	
TVerdict CEIntSockTest18::RunTestL()
	{	
	TInt err;	
	iLogger.WriteFormat(_L("<i>Test 18 - Multiple opens and closes to a single protocol</i>"));
	
	TInt i;
	for (i=0; i<KTest18NumSockets; i++)
		{
		iLogger.WriteFormat(_L("<i>Opening socket %d to pdummy 2</i>"),i);
		err = iSockets[i].Open(KDummyTwoName);
		if (err != KErrNone)
			{
			iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
			return EFail;
			}	
		}
	
	for (i=(KTest18NumSockets-1); i>=0; i--)
		{
		iLogger.WriteFormat(_L("<i>Closing socket %d</i>"),i);
		iSockets[i].Close();
		}
	
	return EPass;
	}
	
void CEIntSockTest18::Cleanup()
	{
	for (TInt i=0; i<KTest18NumSockets; i++)
		{
		iSockets[i].Close();
		}
	}

//-------------------------------------------------------------------------------------------------		
//	CEIntSockTest19
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest19::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest19(aProvd);
	return ret;
	}

CEIntSockTest19::CEIntSockTest19(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd)	
	{}
	
TVerdict CEIntSockTest19::RunTestL()
	{	
	TInt err;	
	
	iLogger.WriteFormat(_L("<i>Test 19 - Multiple opens and closes to various protocols</i>"));

	iLogger.WriteFormat(_L("<i>Opening socket 0 (PDummyOne)</i>"));
	err = iSockets[0].Open(KDummyOneName);
	if (err != KErrNone)
		{
		iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
		return EFail;
		}
	
	iLogger.WriteFormat(_L("<i>Opening socket 1 (PDummyTwo)</i>"));
	err = iSockets[1].Open(KDummyTwoName);
	if (err != KErrNone)
		{
		iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
		return EFail;
		}
	
	iLogger.WriteFormat(_L("<i>Opening socket 2 (PDummyThree)</i>"));
	err = iSockets[2].Open(KDummyTwoName);
	if (err != KErrNone)
		{
		iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
		return EFail;
		}
	
	for (TInt i=(KTest18NumSockets-1); i>=0; i--)
		{
		iLogger.WriteFormat(_L("<i>Closing socket %d</i>"),i);
		iSockets[i].Close();
		}
	
	return EPass;
	}
	
void CEIntSockTest19::Cleanup()
	{
	for (TInt i=0; i<KTest18NumSockets; i++)
		{
		iSockets[i].Close();
		}
	}

//-------------------------------------------------------------------------------------------------		
//	CEIntSockTest20
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest20::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest20(aProvd);
	return ret;
	}

CEIntSockTest20::CEIntSockTest20(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd)	
	{}
	
TVerdict CEIntSockTest20::RunTestL()
	{	
	TInt err;	

	iLogger.WriteFormat(_L("<i>Test 20 - Repeated closes</i>"));

	iLogger.WriteFormat(_L("<i>Opening socket by name (PDummyOne)</i>"));
	err = iSocket.Open(KDummyOneName);
	if (err != KErrNone)
		{
		iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
		return EFail;
		}

	TInt i;
	for (i=0; i<KTest20NumCloses; i++)
		{
		iLogger.WriteFormat(_L("<i>Closing socket - iteration %d</i>"),i+1);
		iSocket.Close();
		}

	iLogger.WriteFormat(_L("<i>Reopening socket - using protocol family etc (PDummyOne)</i>"));
	err = iSocket.Open(KDummyAddrFamily, KSockDatagram, KDummyOne);
	if (err != KErrNone)
		{
		iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
		return EFail;
		}

	for (i=0; i<KTest20NumCloses; i++)
		{
		iLogger.WriteFormat(_L("<i>Closing socket - iteration %d</i>"),i+1);
		iSocket.Close();
		}
	
	return EPass;
	}
	
void CEIntSockTest20::Cleanup()
	{
	iSocket.Close();
	}

//-------------------------------------------------------------------------------------------------		
//	CEIntSockTest21
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest21::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest21(aProvd);
	return ret;
	}

CEIntSockTest21::CEIntSockTest21(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd)
	{}
	
TVerdict CEIntSockTest21::RunTestL()
	{
	TInt err;
	iLogger.WriteFormat(_L("<i>Test 21 - Repeated opens</i>"));
	
	iLogger.WriteFormat(_L("<i>Attempting to open a socket on pdummy 1</i>"));
	err = iSocket.Open(KDummyOneName);
	if (err != KErrNone)
		{
		iLogger.WriteFormat(_L("Open failed with %d"),err);
		return EFail;
		}

	iLogger.WriteFormat(_L("<i>Attempting to open a socket on the same protocol - Should Panic</i>"));	
	err = iSocket.Open(KDummyOneName);
	// Everything below here should never be run due to the panic, but has been left for completeness
	iLogger.WriteFormat(_L("Open returned %d"),err);
	if (err == KErrNone)
		{
		return EFail;
		}

	iSocket.Close();
	return EPass;
	}	
	
void CEIntSockTest21::Cleanup()
	{
	iSocket.Close();
	}
	
	
//-------------------------------------------------------------------------------------------------		
//	CEIntSockTest22
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest22::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest22(aProvd);
	return ret;
	}

CEIntSockTest22::CEIntSockTest22(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd)
	{}
	
TVerdict CEIntSockTest22::RunTestL()
	{
	iLogger.WriteFormat(_L("<i>Test 22 - Close without Open</i>"));
	
	iLogger.WriteFormat(_L("<i>Attempting to close a socket before openning it</i>"));
	iSocket.Close();
	return EPass;
	}	
	
void CEIntSockTest22::Cleanup()
	{
	}
	
//-------------------------------------------------------------------------------------------------		
//	CEIntSockTest23
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest23::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest23(aProvd);
	return ret;
	}

CEIntSockTest23::CEIntSockTest23(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{
	}
	
TVerdict CEIntSockTest23::RunTestL()
	{
	TInt err;
	_LIT8(KSendData,"1234567890");
	iOut = KSendData;
	
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 23 Connect with connectData</i>"));
			
			iLogger.WriteFormat(_L("<i>Opening socket to a datagram protocol (PDummyOne)</i>"));
			err = iSocket.Open(KDummyThreeName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
		
			iLogger.WriteFormat(_L("<i>Connecting the internal socket (datagram protocol) with the data in/data out overload</i>"));
			iAddress.SetFamily(KProxyAddrFamily);
			iOut.Copy (KSendData);
			iSocket.Connect(iAddress, iOut, iIn, iStatus);
						
			iState = EState2;
			Reschedule();
			return EPass;				
			}
		case EState2:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Connect() failed with %d</i>"),iStatus.Int());
				return EFail;
				}

			iLogger.WriteFormat(_L("<i>Connect() completed with %d</i>"),iStatus.Int()); // KErrNone presumably

			if (iIn.Compare(KSendData) == 0)
				{
				iLogger.WriteFormat(_L("<i>Data received matched data sent - pass!</i>"));
				}
			else
				{
				iLogger.WriteFormat(_L("<i>Data received did not match data sent - fail!</i>"));
				return EFail;
				}

			iSocket.Close();

			return EPass;			
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 17));
			return EFail;
		}
	}	
	
void CEIntSockTest23::Cleanup()
	{
	iSocket.Close();
	}
	
//-------------------------------------------------------------------------------------------------		
//	CEIntSockTest25A
//-------------------------------------------------------------------------------------------------	

CEIntSockTestBase* CEIntSockTest25A::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTest25A* ret = new(ELeave) CEIntSockTest25A(aProvd);
	CleanupStack::PushL(ret);
	ret->ConstructL();	
	CleanupStack::Pop(ret);
	return ret;
	}

CEIntSockTest25A::CEIntSockTest25A(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
CEIntSockTest25A::~CEIntSockTest25A()
	{
	delete iColl;
	}
	
void CEIntSockTest25A::ConstructL()
	{
	iColl = CAggregateCollection::NewL(iStatus,CAggregateCollection::EAny);
	iConnectItem = iColl->GetNewItemL();
	iIoctlItem = iColl->GetNewItemL();
	}

TVerdict CEIntSockTest25A::RunTestL()
	{
	TInt err;
	
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 25 CancelAll (Connect & Ioclt)</i>"));
			
			iLogger.WriteFormat(_L("<i>Opening socket to a datagram protocol (PDummyOne)</i>"));
			err = iSocket.Open(KDummyThreeName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
		
			iLogger.WriteFormat(_L("<i>Setting option to block connect</i>"));
			err = iSocket.SetOpt(KDummyOptionSetBlockConnect,28,TRUE);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>SetOpt() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Connecting the internal socket (datagram protocol) with the data in/data out overload</i>"));
			iAddress.SetFamily(KProxyAddrFamily);
			iConnectItem->Start();
			iSocket.Connect(iAddress, iConnectItem->iStatus);
			iLogger.WriteFormat(_L("<i>Sending through a non completing ioclt</i>"));
			iIoctlItem->Start();
			iSocket.Ioctl(KDummyIocltNonCompleting, iIoctlItem->iStatus);
			iLogger.WriteFormat(_L("<i>Performing CancelAll</i>"));
			iSocket.CancelAll();
			iState = EState2;
			iColl->Kick();
			Reschedule();
			return EPass;				
			}
		case EState2:
			{
			if(iConnectItem->ReturnCode() == KRequestPending || iIoctlItem->ReturnCode() == KRequestPending)
				{
				iColl->Kick();
				Reschedule();
				return EFail;
				}
			if (iConnectItem->ReturnCode() != KErrCancel)
				{
				iLogger.WriteFormat(_L("<i>Connect returned with %d (KErrCancel Expected)</i>"), iConnectItem->ReturnCode());
				return EFail;
				}
			if (iIoctlItem->ReturnCode() != KErrCancel)
				{
				iLogger.WriteFormat(_L("<i>Ioctl returned with %d (KErrCancel Expected)</i>"), iIoctlItem->ReturnCode());
				return EFail;
				}	
			return EPass;			
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 18));
			return EFail;
		}
	}
	
void CEIntSockTest25A::Cleanup()
	{
	iSocket.Close();
	}
	
//-------------------------------------------------------------------------------------------------		
//	CEIntSockTest25B
//-------------------------------------------------------------------------------------------------	

CEIntSockTestBase* CEIntSockTest25B::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTest25B* ret = new(ELeave) CEIntSockTest25B(aProvd);
	CleanupStack::PushL(ret);
	ret->ConstructL();	
	CleanupStack::Pop(ret);
	return ret;
	}

CEIntSockTest25B::CEIntSockTest25B(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
CEIntSockTest25B::~CEIntSockTest25B()
	{
	delete iColl;
	}
	
void CEIntSockTest25B::ConstructL()
	{
	iColl = CAggregateCollection::NewL(iStatus,CAggregateCollection::EAny);
	iAcceptItem = iColl->GetNewItemL();
	iIoctlItem = iColl->GetNewItemL();
	}
	
TVerdict CEIntSockTest25B::RunTestL()
	{
	TInt err;
	
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 25 CancelAll (Accept & Ioclt)</i>"));
			
			iLogger.WriteFormat(_L("<i>Opening socket to a datagram protocol (PDummyThree)</i>"));
		
			err = iSocket.Open(KDummyThreeName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
			TSockAddr addr;
			iLogger.WriteFormat(_L("<i>Performing Bind</i>"));
			err =iSocket.Bind(addr);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Bind() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Performing Listen</i>"));
			err = iSocket.Listen(5);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Listen() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Opening a NULL socket</i>"));
			err = iNewSock.Open();
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
			
			iLogger.WriteFormat(_L("<i>Accepting a connection</i>"));
			iAcceptItem->Start();
			iSocket.Accept(iNewSock,iAcceptItem->iStatus);
			iLogger.WriteFormat(_L("<i>Sending through a non completing ioclt</i>"));
			iIoctlItem->Start();
			iSocket.Ioctl(KDummyIocltNonCompleting, iIoctlItem->iStatus);
			iLogger.WriteFormat(_L("<i>Performing CancelAll</i>"));
			iSocket.CancelAll();
									
			iState = EState2;
			iColl->Kick();
			Reschedule();
			return EPass;				
			}
		case EState2:
			{
			if (iAcceptItem->ReturnCode() == KRequestPending || iIoctlItem->ReturnCode() == KRequestPending)
				{
				Reschedule(); //No change of status b/c we are waiting for a timeout
				iColl->Kick();
				return EFail;	
				}
			if (iAcceptItem->ReturnCode() != KErrCancel)
				{
				iLogger.WriteFormat(_L("<i>Accept returned with %d (KErrCancel Expected)</i>"), iAcceptItem->ReturnCode());
				return EFail;
				}
			if (iIoctlItem->ReturnCode() != KErrCancel)
				{
				iLogger.WriteFormat(_L("<i>Ioctl returned with %d (KErrCancel Expected)</i>"), iIoctlItem->ReturnCode());
				return EFail;
				}	
			return EPass;			
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 19));
			return EFail;
			
		}
	}
	
void CEIntSockTest25B::Cleanup()
	{
	iSocket.Close();
	}
	
//-------------------------------------------------------------------------------------------------		
//	CEIntSockTest25C
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest25C::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTest25C* ret = new(ELeave) CEIntSockTest25C(aProvd);
	CleanupStack::PushL(ret);
	ret->ConstructL();	
	CleanupStack::Pop(ret);
	return ret;
	}

CEIntSockTest25C::CEIntSockTest25C(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
CEIntSockTest25C::~CEIntSockTest25C()
	{
	delete iColl;
	}
	
void CEIntSockTest25C::ConstructL()
	{
	iColl = CAggregateCollection::NewL(iStatus,CAggregateCollection::EAny);
	iRecvItem = iColl->GetNewItemL();
	iIoctlItem = iColl->GetNewItemL();
	}

TVerdict CEIntSockTest25C::RunTestL()
	{
	TInt err;
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 25 CancelAll (Recv & Ioclt)</i>"));
			
			iLogger.WriteFormat(_L("<i>Opening socket to a datagram protocol (PDummyOne)</i>"));
			err = iSocket.Open(KDummyOneName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
		
			iLogger.WriteFormat(_L("<i>Peforming Connect</i>"));
			iAddress.SetFamily(KProxyAddrFamily);
			iSocket.Connect(iAddress, iStatus);
			iState = EState2;
			Reschedule();
			return EPass;
			}
		case EState2:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Connect() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Performing Recv that will not return</i>"));
			iRecvItem->Start();
			iSocket.Recv(iRecvBuf,0,iRecvItem->iStatus);
			iLogger.WriteFormat(_L("<i>Sending through a non completing ioclt</i>"));
			iIoctlItem->Start();
			iSocket.Ioctl(KDummyIocltNonCompleting, iIoctlItem->iStatus);
			iLogger.WriteFormat(_L("<i>Performing CancelAll</i>"));
			iSocket.CancelAll();
			iState = EState3;
			iColl->Kick();
			Reschedule();
			return EPass;				
			}
		case EState3:
			{
			if (iRecvItem->ReturnCode() == KRequestPending || iIoctlItem->ReturnCode() == KRequestPending)
				{
				iColl->Kick();
				Reschedule(); //No change of status b/c we are waiting for a timeout
				return EFail;	
				}
			
			if (iRecvItem->ReturnCode() != KErrCancel)
				{
				iLogger.WriteFormat(_L("<i>Connect returned with %d (KErrCancel Expected)</i>"), iRecvItem->ReturnCode());
				return EFail;
				}
			if (iIoctlItem->ReturnCode() != KErrCancel)
				{
				iLogger.WriteFormat(_L("<i>Ioctl returned with %d (KErrCancel Expected)</i>"), iIoctlItem->ReturnCode());
				return EFail;
				}	
			return EPass;			
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 20));
			return EFail;
		}
	}
	
void CEIntSockTest25C::Cleanup()
	{
	iSocket.Close();
	}
	
//-------------------------------------------------------------------------------------------------		
//	CEIntSockTest26
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest26::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest26(aProvd);
	return ret;
	}

CEIntSockTest26::CEIntSockTest26(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{
	}
	
TVerdict CEIntSockTest26::RunTestL()
	{
	TInt err;
	_LIT8(KSendData,"1234567890");
	
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 26 - GetDisconnectData Where there is no disconnect data</i>"));
			
			iLogger.WriteFormat(_L("<i>Opening a socket to PDummy 3</i>"));
			err = iSocket.Open(KDummyThreeName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
		
			iLogger.WriteFormat(_L("<i>Opening an internal socket (datagram protocol)</i>"));
			iAddress.SetFamily(KProxyAddrFamily);
			iSocket.Connect(iAddress, iStatus);
			
			iState = EState2;
			Reschedule();
			return EPass;		
			}
		case EState2:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Connect() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
				
			iLogger.WriteFormat(_L("<i>Sending data over an internal socket using the descriptor API and requesting amount sent</i>"));
			iSocket.Send(KSendData,0,iStatus);
			
			iState = EState3;
			Reschedule();
			return EPass;	
			}
		case EState3:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Send() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			
			iLogger.WriteFormat(_L("<i>Sending an Ioctl that simulates a remote disconnect</i>"));
			iSocket.Ioctl(KDummyIocltRemoteDisconnect,iStatus,NULL);
			
			iState = EState4;
			Reschedule();
			return EPass;
			}
		case EState4:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Ioctl() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Receiving data over an internal socket using the descriptor API</i>"));
			iBuf.Zero();
			iBuf.SetLength(KSendData().Length());
			iSocket.Recv(iBuf,0,iStatus);
			
			iState = EState5;
			Reschedule();
			return EPass;				
			}
		case EState5:
			{
			iLogger.WriteFormat(_L("<i>Recv() returned with %d</i>"),iStatus.Int());
			if (iStatus == KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Recv() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Getting Disconnect Data for the socket</i>"));
		
			TBuf8<40> disconnectData;
			err = iSocket.GetDisconnectData(disconnectData);
			if (err != KErrNotFound)
				{
				iLogger.WriteFormat(_L("<i>GetDisconnectData() failed with %d</i>"),iStatus.Int());
				return EFail;
				}	
			return EPass;	
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 21));
			return EFail;
		}
	}	
	
void CEIntSockTest26::Cleanup()
	{
	iSocket.Close();
	}
	
//-------------------------------------------------------------------------------------------------		
//	CEIntSockTest27
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest27::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest27(aProvd);
	return ret;
	}

CEIntSockTest27::CEIntSockTest27(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{
	}
	
TVerdict CEIntSockTest27::RunTestL()
	{
	TInt err;
	_LIT8(KSendData,"1234567890");
	_LIT8(KDisconnectData,"Socket has been disconnected");

	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 27 - GetDisconnectData Where there is disconnect data</i>"));
			
			iLogger.WriteFormat(_L("<i>Opening a socket to PDummy 3</i>"));
			err = iSocket.Open(KDummyThreeName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
		
			iLogger.WriteFormat(_L("<i>Opening an internal socket (datagram protocol)</i>"));
			iAddress.SetFamily(KProxyAddrFamily);
			iSocket.Connect(iAddress, iStatus);
			
			iState = EState2;
			Reschedule();
			return EPass;		
			}
		case EState2:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Connect() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
				
			iLogger.WriteFormat(_L("<i>Sending data over an internal socket using the descriptor API and requesting amount sent</i>"));
			iSocket.Send(KSendData,0,iStatus);
			
			iState = EState3;
			Reschedule();
			return EPass;	
			}
		case EState3:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Send() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			
			iLogger.WriteFormat(_L("<i>Sending an Ioctl that simulates a remote disconnect</i>"));
			iDisconnectBuf = KDisconnectData;
			iSocket.Ioctl(KDummyIocltRemoteDisconnect,iStatus,&iDisconnectBuf);
			
			iState = EState4;
			Reschedule();
			return EPass;
			}
		case EState4:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Ioctl() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Receiving data over an internal socket using the descriptor API</i>"));
			iBuf.Zero();
			iBuf.SetLength(KSendData().Length());
			iSocket.Recv(iBuf,0,iStatus);
			
			iState = EState5;
			Reschedule();
			return EPass;				
			}
		case EState5:
			{
			iLogger.WriteFormat(_L("<i>Recv() returned with %d</i>"),iStatus.Int());
			if (iStatus == KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Recv() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Getting Disconnect Data for the socket</i>"));
			TBuf8<1> tooShort;
			err = iSocket.GetDisconnectData(tooShort);
			if (err != KErrOverflow)
				{
				iLogger.WriteFormat(_L("<i>GetDisconnectData() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			TBuf8<40> disconnectData;
			err = iSocket.GetDisconnectData(disconnectData);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>GetDisconnectData() failed with %d</i>"),iStatus.Int());
				return EFail;
				}	
			if (disconnectData != KDisconnectData)
				{
				iLogger.WriteFormat(_L("<i>GetDisconnectData() returned %S. Should have returned %S</i>"),
					&disconnectData,&(KDisconnectData()));
				return EFail;
				}
			return EPass;	
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 22));
			return EFail;
		}
	}	
	
void CEIntSockTest27::Cleanup()
	{
	iSocket.Close();
	}
	
//-------------------------------------------------------------------------------------------------		
//	CEIntSockTest28A
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest28A::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest28A(aProvd);
	return ret;
	}

CEIntSockTest28A::CEIntSockTest28A(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{
	}
	
TVerdict CEIntSockTest28A::RunTestL()
	{
	TInt err;
	_LIT8(KConnectData,"1234567890");

	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 29 - Connect with connect data</i>"));
			
			iLogger.WriteFormat(_L("<i>Opening a socket to PDummy 3</i>"));
			err = iSocket.Open(KDummyThreeName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
		
			iLogger.WriteFormat(_L("<i>Performing a connect with connect data</i>"));
			iAddress.SetFamily(KProxyAddrFamily);
			iSocket.Connect(iAddress,KConnectData(),iBuf,iStatus);
			
			iState = EState2;
			Reschedule();
			return EPass;		
			}
		case EState2:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Connect() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
				
			if (iBuf != KConnectData)
				{
				iLogger.WriteFormat(_L("<i>Connect out is not equal to connect in. Connect in =%S</i>"),&iBuf);
				return EFail;
				}	
			return EPass;	
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 23));
			return EFail;
		}
	}	
	
void CEIntSockTest28A::Cleanup()
	{
	iSocket.Close();
	}	

//-------------------------------------------------------------------------------------------------		
//	CEIntSockTest28B
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest28B::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest28B(aProvd);
	return ret;
	}

CEIntSockTest28B::CEIntSockTest28B(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{
	}
	
TVerdict CEIntSockTest28B::RunTestL()	
	{
	TInt err;
	_LIT8(KConnectData,"1234567890");
	switch(iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 28B - Listen and Accept with connectdata</i>"));
			iLogger.WriteFormat(_L("<i>Opening PDummy 3</i>"));
			err = iSocket.Open(KDummyThreeName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
			TSockAddr addr;
			iLogger.WriteFormat(_L("<i>Performing Bind</i>"));
			err =iSocket.Bind(addr);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Bind() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Performing Listen with connect data</i>"));
			err = iSocket.Listen(5,KConnectData);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Listen() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Opening a NULL socket</i>"));
			err = iNewSock.Open();
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
			
			iLogger.WriteFormat(_L("<i>Accepting a connection</i>"));
			iSocket.Accept(iNewSock, iBuf, iStatus);
			
			iLogger.WriteFormat(_L("<i>Setting a sockopt to simulate a connect</i>"));
			err = iSocket.SetOpt(KDummyOptionSetConnectComplete,27);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>SetOpt() failed with %d</i>"),err);
				return EFail;
				}
			iState  = EState2;
			Reschedule();
			return EPass;
			}
			
		case EState2:
			{
			iLogger.WriteFormat(_L("<i>Checking that the accept completes</i>"));
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Accept() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			if (iBuf != KConnectData)
				{
				iLogger.WriteFormat(_L("<i>Connect out is not equal to connect in. Connect in =%S</i>"), &iBuf);
				return EFail;
				}	
			return EPass;
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 24));
			return EFail;
		}
	}
	
void CEIntSockTest28B::Cleanup()
	{
	iSocket.Close();
	}	
//	CEIntSockTest29
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest29::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest29(aProvd);
	return ret;
	}

CEIntSockTest29::CEIntSockTest29(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{
	}
	
TVerdict CEIntSockTest29::RunTestL()
	{
	TInt err;
	_LIT8(KSendData,"1234567890");

	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 29 - Send() with volume of data returned to client (datagram protocol, descriptor API)</i>"));
			
			iLogger.WriteFormat(_L("<i>Opening a socket to PDummy 1</i>"));
			err = iSocket.Open(KDummyOneName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
		
			iLogger.WriteFormat(_L("<i>Performing a connect</i>"));
			iAddress.SetFamily(KProxyAddrFamily);
			iSocket.Connect(iAddress, iStatus);
			
			iState = EState2;
			Reschedule();
			return EPass;		
			}
		case EState2:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Connect() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
				
			iLogger.WriteFormat(_L("<i>Sending data over an internal socket using the descriptor API and requesting amount sent</i>"));
			iSocket.Send(KSendData,0,iStatus,iVolume);
			
			iState = EState3;
			Reschedule();
			return EPass;	
			}
		case EState3:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Send() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			if (iVolume() != KSendData().Length())
				{
				iLogger.WriteFormat(_L("<i>Volume of datagrams sent was actually %d instead of 1</i>"),iVolume());
				return EFail;
				}
			
			iLogger.WriteFormat(_L("<i>Receiving data over an internal socket using the descriptor API</i>"));
			iBuf.Zero();
			iBuf.SetLength(KSendData().Length());
			iSocket.Recv(iBuf,0,iStatus);
			
			iState = EState4;
			Reschedule();
			return EPass;				
			}
		case EState4:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Recv() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
				
			iLogger.WriteFormat(_L("<i>Checking that the data was read into the buffer</i>"));
			if (iBuf != KSendData)
				{
				iLogger.WriteFormat(_L("<i>Incorrect data to recved</i>"));
				return EFail;
				}
			return EPass;	
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 25));
			return EFail;
		}
	}	
	
void CEIntSockTest29::Cleanup()
	{
	iSocket.Close();
	}
			
//-------------------------------------------------------------------------------------------------		
//	CEIntSockTest30
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest30::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest30(aProvd);
	return ret;
	}

CEIntSockTest30::CEIntSockTest30(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1), iRecvBufPtr(NULL, 0)
	{
	}
	
TVerdict CEIntSockTest30::RunTestL()
	{
	TInt err;
	_LIT8(KSendData,"1234567890");

	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 30 - Send() with volume of data returned to client (stream protocol, descriptor API)</i>"));
			
			iLogger.WriteFormat(_L("<i>Opening a socket to PDummy 2</i>"));
			err = iSocket.Open(KDummyTwoName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
		
			iLogger.WriteFormat(_L("<i>Opening an internal socket (datagram protocol)</i>"));
			iAddress.SetFamily(KProxyAddrFamily);
			iSocket.Connect(iAddress, iStatus);
			
			iState = EState2;
			Reschedule();
			return EPass;		
			}
		case EState2:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Connect() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
				
			iLogger.WriteFormat(_L("<i>Sending data over an internal socket using the descriptor API and requesting amount sent</i>"));
			iSocket.Send(KSendData,0,iStatus,iVolume);
			
			iState = EState3;
			Reschedule();
			return EPass;	
			}
		case EState3:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Send() didn't return KErrNone (returned %d)</i>"),iStatus.Int());
				return EFail;
				}
					
			iLogger.WriteFormat(_L("<i>Receiving data over an internal socket using the descriptor API</i>"));
			iBuf.Zero();
			iBuf.SetLength(KSendData().Length());
			iRecvBufPtr.Set( (TUint8*)iBuf.PtrZ(), KSendData().Length(), KSendData().Length() );
			iSocket.Recv(iRecvBufPtr,0,iStatus);
			
			iState = EState4;
			Reschedule();
			return EPass;				
			}
		case EState4:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Recv() didn't return KErrNone (returned %d)</i>"),iStatus.Int());
				return EFail;
				}
			return EPass;
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 26));
			return EFail;
		}
	}	
	
void CEIntSockTest30::Cleanup()
	{
	iSocket.Close();
	}
			
//-------------------------------------------------------------------------------------------------		
//	CEIntSockTest31
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest31::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest31(aProvd);
	return ret;
	}

CEIntSockTest31::CEIntSockTest31(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{
	}
	
TVerdict CEIntSockTest31::RunTestL()
	{
	TInt err;
	_LIT8(KSendData,"1234567890");

	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 31 - Send() with volume of data returned to client (datagram protocol, RMBufChain API)</i>"));
			
			iLogger.WriteFormat(_L("<i>Opening a socket to PDummy 1</i>"));
			err = iSocket.Open(KDummyOneName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
		
			iLogger.WriteFormat(_L("<i>Opening an internal socket (datagram protocol)</i>"));
			iAddress.SetFamily(KProxyAddrFamily);
			iSocket.Connect(iAddress, iStatus);
			
			iState = EState2;
			Reschedule();
			return EPass;		
			}
		case EState2:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Connect() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
				
			iLogger.WriteFormat(_L("<i>Sending data over an internal socket using the RMBufChain API and requesting amount sent</i>"));
			iSendChain.CreateL(KSendData());
			iSocket.Send(iSendChain,0,iStatus,iVolume);
			

			iState = EState3;
			Reschedule();
			return EPass;	
			}
		case EState3:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Send() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			if (iVolume() != KSendData().Length())
				{
				iLogger.WriteFormat(_L("<i>Volume of data sent was actually %d instead of 1</i>"),iVolume());
				return EFail;
				}
				
			iSendChain.Remove ();			
			
			iLogger.WriteFormat(_L("<i>Receiving data over an internal socket using the RMBufChain API</i>"));
			iBuf.Zero();
			
			iBuf.SetLength(KSendData().Length());
			iSocket.Recv(iRecvChain,0,iStatus);
						
			iState = EState4;
			Reschedule();
			return EPass;				
			}
		case EState4:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Recv() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			
			iRecvChain.CopyOut(iBuf);
			iRecvChain.Free();
			
			
			iLogger.WriteFormat(_L("<i>Checking that the data was read into the buffer</i>"));
			if (iBuf != KSendData)
				{
				iLogger.WriteFormat(_L("<i>Incorrect data to recved</i>"));
				return EFail;
				}
			return EPass;	
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 27));
			return EFail;
		}
	}

void CEIntSockTest31::Cleanup()
	{
	iSocket.Close();
	__ASSERT_DEBUG(iRecvChain.IsEmpty(), User::Panic(KSpecAssert_ESockTestptstcs, 28));
	__ASSERT_DEBUG(iSendChain.IsEmpty(), User::Panic(KSpecAssert_ESockTestptstcs, 29));
	}

//-------------------------------------------------------------------------------------------------		
//	CEIntSockTest32
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest32::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest32(aProvd);
	return ret;
	}

CEIntSockTest32::CEIntSockTest32(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{
	}
	
TVerdict CEIntSockTest32::RunTestL()
	{
	TInt err;
	_LIT8(KSendData,"1234567890");


	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 32 - Send() with volume of data returned to client (stream protocol, RMBufChain API)</i>"));
			
			iLogger.WriteFormat(_L("<i>Opening a socket to PDummy 2</i>"));
			err = iSocket.Open(KDummyTwoName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
		
			iLogger.WriteFormat(_L("<i>Opening an internal socket (datagram protocol)</i>"));
			iAddress.SetFamily(KProxyAddrFamily);
			iSocket.Connect(iAddress, iStatus);
			
			iState = EState2;
			Reschedule();
			return EPass;		
			}
		case EState2:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Connect() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
				
			iLogger.WriteFormat(_L("<i>Sending data over an internal socket using the descriptor API and requesting amount sent</i>"));
			iSendChain.CreateL(KSendData());
			iSocket.Send(iSendChain,0,iStatus,iVolume);

			
			iState = EState3;
			Reschedule();
			return EPass;	
			}
		case EState3:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Send() didn't return KErrNone (returned %d)</i>"),iStatus.Int());
				return EFail;
				}
		
			iSendChain.Remove ();			
			iLogger.WriteFormat(_L("<i>Receiving data over an internal socket using the descriptor API</i>"));
			iBuf.Zero();
			iBuf.SetLength(KSendData().Length());
			iSocket.Recv(iRecvChain,0,iStatus);
			
			iState = EState4;
			Reschedule();
			return EPass;				
			}
		case EState4:
			{
			iRecvChain.CopyOut(iBuf);
			iRecvChain.Free();

			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Recv() didn't return KErrNone (returned %d)</i>"),iStatus.Int());
				return EFail;
				}
			return EPass;
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 30));
			return EFail;
		}

	}	
	
void CEIntSockTest32::Cleanup()
	{
	iSocket.Close();
	__ASSERT_DEBUG(iSendChain.IsEmpty(), User::Panic(KSpecAssert_ESockTestptstcs, 31));
	__ASSERT_DEBUG(iRecvChain.IsEmpty(), User::Panic(KSpecAssert_ESockTestptstcs, 32));
	}

//-------------------------------------------------------------------------------------------------
//	CEIntSockTest33
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest33::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest33(aProvd);
	return ret;
	}

CEIntSockTest33::CEIntSockTest33(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1), iReadBufPtr(NULL, 0)
	{}
	
TVerdict CEIntSockTest33::RunTestL()

	{
	TInt err;
	_LIT8(KWriteData,"1234567890");
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 33 - Write to and read from a stream protocol (Descriptor API)</i>"));
			iLogger.WriteFormat(_L("<i>Opening a socket to PDummy 2</i>"));
			err = iSocket.Open(KDummyTwoName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
		
			iLogger.WriteFormat(_L("<i>Opening an internal socket (stream protocol)</i>"));
			iAddress.SetFamily(KProxyAddrFamily);
			iSocket.Connect(iAddress, iStatus);
			
			iState = EState2;
			Reschedule();
			return EPass;		
			}
		case EState2:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Connect() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
				
			iLogger.WriteFormat(_L("<i>Writing data to an internal socket using the descriptor API</i>"));
			iSocket.Write(KWriteData,iStatus);
			
			iState = EState3;
			Reschedule();
			return EPass;	
			}
		case EState3:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Write() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			
			iLogger.WriteFormat(_L("<i>Reading data from an internal socket using the descriptor API</i>"));
			iBuf.Zero();
			iBuf.SetLength(KWriteData().Length());
			iReadBufPtr.Set( (TUint8*)iBuf.PtrZ(), KWriteData().Length(), KWriteData().Length() );
			iSocket.Read(iReadBufPtr,iStatus);
			
			iState = EState4;
			Reschedule();
			return EPass;				
			}
		case EState4:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Read() didn't return KErrNone (returned %d)</i>"),iStatus.Int());
				return EFail;
				}
			return EPass;
			}
		default: __ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 33));
			return EFail;
		}
	}

void CEIntSockTest33::Cleanup()
	{
	iSocket.Close();
	}
	
//-------------------------------------------------------------------------------------------------
//	CEIntSockTest34
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest34::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest34(aProvd);
	return ret;
	}

CEIntSockTest34::CEIntSockTest34(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
TVerdict CEIntSockTest34::RunTestL()

	{
	TInt err;
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 34 - Shutdown without disconnect data</i>"));
			iLogger.WriteFormat(_L("<i>Opening a socket to PDummy 1</i>"));
			err = iSocket.Open(KDummyOneName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Performing a Connect</i>"));
			iAddress.SetFamily(KProxyAddrFamily);
			iSocket.Connect(iAddress, iStatus);
			
			iState = EState2;
			Reschedule();
			return EPass;		
			}
		case EState2:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Connect failed with</i>"),iStatus.Int());
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Performing a Shutdown</i>"));	
			iSocket.Shutdown(RSocket::ENormal,iStatus);
			iState = EState3;
			Reschedule();
			return EPass;
			}
		case EState3:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Shutdown failed with</i>"),iStatus.Int());
				return EFail;
				}
			return EPass;
			}
		default: __ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 34));
			return EFail;
		}
	}

void CEIntSockTest34::Cleanup()
	{
	iSocket.Close();
	}
	
	
//-------------------------------------------------------------------------------------------------
//	CEIntSockTest35
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest35::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest35(aProvd);
	return ret;
	}

CEIntSockTest35::CEIntSockTest35(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
TVerdict CEIntSockTest35::RunTestL()

	{
	TInt err;
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 35 - Error during connect</i>"));
			iLogger.WriteFormat(_L("<i>Opening a socket to PDummy 3</i>"));
			err = iSocket.Open(KDummyThreeName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("Set connect error code to %d"), KExpectedError);             
			err=iSocket.SetOpt(KDummyOptionSetErrorNextConnect, 0, KExpectedError);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Setopt() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("Performing Connect"));
			iSocket.Connect(iAddress,iStatus);  
			iState = EState2;
			Reschedule();
			return EPass;		
			}
		case EState2:
			{
			if (iStatus != KExpectedError)
				{
				iLogger.WriteFormat(_L("<i>Connect failed with %d should have returned %d</i>"),iStatus.Int(), KExpectedError);
				return EFail;
				}
			iLogger.WriteFormat(_L("Set blocked connect"));             
			err=iSocket.SetOpt(KDummyOptionSetBlockConnect, 0, TRUE);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Setopt() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("Set connect error code to %d"), KExpectedError);             
			err=iSocket.SetOpt(KDummyOptionSetErrorNextConnect, 0, KExpectedError);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Setopt() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("Performing Connect"));
			iSocket.Connect(iAddress,iStatus);  
			
			iState = EState3;
			Reschedule();
			return EPass;
			}
		case EState3:
			{
			if (iStatus != KExpectedError)
				{
				iLogger.WriteFormat(_L("<i>Connect failed with %d should have returned %d</i>"),iStatus.Int(), KExpectedError);
				return EFail;
				}
			iLogger.WriteFormat(_L("Set blocked connect"));             
			err=iSocket.SetOpt(KDummyOptionSetBlockConnect, 0, TRUE);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Setopt() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("Performing Connect"));
			iSocket.Connect(iAddress,iStatus);  
			
			iLogger.WriteFormat(_L("Set connect error code to %d"), KExpectedError);             
			err=iSocket.SetOpt(KDummyOptionSetErrorNextConnect, 0, KExpectedError);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Setopt() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("Unblock connect"));             
			err=iSocket.SetOpt(KDummyOptionSetBlockConnect, 0, FALSE);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Setopt() failed with %d</i>"),err);
				return EFail;
				}
			iState = EState4;
			Reschedule();
			return EPass;
			}
		case EState4:
			{
			if (iStatus != KExpectedError)
				{
				iLogger.WriteFormat(_L("<i>Connect failed with %d should have returned %d</i>"),iStatus.Int(), KExpectedError);
				return EFail;
				}
			return EPass;
			}
		default: __ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 35));
			return EFail;
		}
	}
	
void CEIntSockTest35::Cleanup()
	{
	iSocket.Close();
	}
	
	
//-------------------------------------------------------------------------------------------------
//	CEIntSockTest36
//-------------------------------------------------------------------------------------------------

CEIntSockTestBase* CEIntSockTest36::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest36(aProvd);
	return ret;
	}

CEIntSockTest36::CEIntSockTest36(CProxyProvd* aProvd)
	: CEIntSockTestBase(aProvd),
	  iState(EState1)
	{}

TVerdict CEIntSockTest36::RunTestL()
	{
	TInt err;
	_LIT8(KSendData,"1234567890");

	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 36 - Send to and receive from a datagram protocol that temporarily flows control off (Descriptor API)</i>"));

			iLogger.WriteFormat(_L("<i>Opening PDummy 1</i>"));
			err = iSocket.Open(KDummyOneName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}

			iLogger.WriteFormat(_L("<i>Opening an internal socket (datagram protocol)</i>"));
			iAddress.SetFamily(KProxyAddrFamily);
			iSocket.Connect(iAddress, iStatus);

			iState = EState2;
			Reschedule();
			return EPass;
			}

		case EState2:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Connect() failed with %d</i>"),iStatus.Int());
				return EFail;
				}

			iLogger.WriteFormat(_L("<i>Sending data over an internal socket using the descriptor API - temporarily flowing control off</i>"));
			iSocket.SetOpt(KDummyOptionSetTimedFlowOffWrite, 0, KDummyWriteFlowOffTimeout);
			iSocket.Send(KSendData,0,iStatus);
			iState = EState3;
			Reschedule();
			return EPass;
			}

		case EState3:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Send() failed with %d</i>"),iStatus.Int());
				iSocket.SetOpt(KDummyOptionSetTimedFlowOffWrite, 0, 0);
				return EFail;
				}

			iSocket.SetOpt(KDummyOptionSetTimedFlowOffWrite, 0, 0);
			iLogger.WriteFormat(_L("<i>Receiving data over an internal socket using the descriptor API</i>"));
			iBuf.Zero();
			iBuf.SetLength(KSendData().Length());
			iSocket.Recv(iBuf,0,iStatus);

			iState = EState4;
			Reschedule();
			return EPass;
			}

		case EState4:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Recv() failed with %d</i>"),iStatus.Int());
				return EFail;
				}

			iLogger.WriteFormat(_L("<i>Checking that the data was read into the buffer</i>"));
			if (iBuf != KSendData)
				{
				iLogger.WriteFormat(_L("<i>Incorrect data to recved</i>"));
				return EFail;
				}
			return EPass;
			}

		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 36));
			return EFail;
		}
	}

void CEIntSockTest36::Cleanup()
	{
	iSocket.Close();
	}


//-------------------------------------------------------------------------------------------------
//	CEIntSockTest37
//-------------------------------------------------------------------------------------------------

CEIntSockTestBase* CEIntSockTest37::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest37(aProvd);
	return ret;
	}

CEIntSockTest37::CEIntSockTest37(CProxyProvd* aProvd)
	: CEIntSockTestBase(aProvd),
	  iState(EState1)
	{}

TVerdict CEIntSockTest37::RunTestL()
	{
	TInt err;
	_LIT8(KSendData,"1234567890");

	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 37 - Send to and receive from a datagram protocol that temporarily flows control off (RMBufChain API)</i>"));

			iLogger.WriteFormat(_L("<i>Opening PDummy 1</i>"));
			err = iSocket.Open(KDummyOneName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}

			iLogger.WriteFormat(_L("<i>Opening an internal socket (datagram protocol)</i>"));
			iAddress.SetFamily(KProxyAddrFamily);
			iSocket.Connect(iAddress, iStatus);

			iState = EState2;
			Reschedule();
			return EPass;
			}

		case EState2:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Connect() failed with %d</i>"),iStatus.Int());
				return EFail;
				}

			iLogger.WriteFormat(_L("<i>Sending data over an internal socket using the RMBufChain API - temporarily flowing control off</i>"));

			iSendChain.CreateL(KSendData());

			iSocket.SetOpt(KDummyOptionSetTimedFlowOffWrite, 0, KDummyWriteFlowOffTimeout);
			iSocket.Send(iSendChain,0,iStatus);

			iState = EState3;
			Reschedule();
			return EPass;
			}

		case EState3:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Send() failed with %d</i>"),iStatus.Int());
				iSocket.SetOpt(KDummyOptionSetTimedFlowOffWrite, 0, 0);
				return EFail;
				}
			iSendChain.Remove();

			iSocket.SetOpt(KDummyOptionSetTimedFlowOffWrite, 0, 0);
			iLogger.WriteFormat(_L("<i>Receiving data over an internal socket using the RMBufChain API</i>"));

			iSocket.Recv(iRecvChain, 0, iStatus);

			iState = EState4;
			Reschedule();
			return EPass;
			}

		case EState4:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Recv() failed with %d</i>"),iStatus.Int());
				return EFail;
				}

			iLogger.WriteFormat(_L("<i>Checking that the data was read into the buffer</i>"));
			iBuf.SetLength(KSendData().Length());
			iRecvChain.CopyOut(iBuf);
			iRecvChain.Free();
			
			if (iBuf.Compare(KSendData()) != 0)
				{
				iLogger.WriteFormat(_L("<i>Recv() data != that send (%S recvd vs %S)</i>"), &iBuf, &KSendData());
				return EFail;
				}

			iLogger.WriteFormat(_L("<i>Recv() data matched that sent</i>"));
			return EPass;
			}

		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 37));
			return EFail;
		}
	}

void CEIntSockTest37::Cleanup()
	{
	iSocket.Close();
	__ASSERT_DEBUG(iSendChain.IsEmpty(), User::Panic(KSpecAssert_ESockTestptstcs, 38));
	__ASSERT_DEBUG(iRecvChain.IsEmpty(), User::Panic(KSpecAssert_ESockTestptstcs, 39));	
	}


//-------------------------------------------------------------------------------------------------
//	CEIntSockTest38
//-------------------------------------------------------------------------------------------------

CEIntSockTestBase* CEIntSockTest38::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest38(aProvd);
	return ret;
	}

CEIntSockTest38::CEIntSockTest38(CProxyProvd* aProvd)
	: CEIntSockTestBase(aProvd),
	  iState(EState1)
	{}

TVerdict CEIntSockTest38::RunTestL()
	{
	TInt err;
	_LIT8(KSendData,"1234567890");

	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 38 - Send to and receive from a datagram protocol that temporarily flows control off (Descriptor API)</i>"));

			iLogger.WriteFormat(_L("<i>Opening PDummy 1</i>"));
			err = iSocket.Open(KDummyOneName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}

			iLogger.WriteFormat(_L("<i>Opening an internal socket (datagram protocol)</i>"));
			iAddress.SetFamily(KProxyAddrFamily);
			iSocket.Connect(iAddress, iStatus);

			iState = EState2;
			Reschedule();
			return EPass;
			}

		case EState2:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Connect() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			
			iSocket.SetOpt(KDummyOptionSetFlowOffWrite, 0, ETrue);

			iLogger.WriteFormat(_L("<i>Sending data over an internal socket using the descriptor API - flowing control off</i>"));

			iSocket.Send(KSendData, 0, iStatus);

			iLogger.WriteFormat(_L("<i>Cancelling the Send request</i>"));
			iSocket.CancelSend();

			iState = EState3;
			Reschedule();
			return EPass;
			}

		case EState3:
			{

			iLogger.WriteFormat(_L("<i>Checking that the completion code for Send is KErrCancel</i>"));
			if (iStatus != KErrCancel)
				{
				iLogger.WriteFormat(_L("<i>Send() failed with %d</i>"),iStatus.Int());
				return EFail;
				}

			iSocket.SetOpt(KDummyOptionSetFlowOffWrite, 0, EFalse);

			iLogger.WriteFormat(_L("<i>Resending data over an internal socket using the descriptor API - will not flow control off</i>"));

			iSocket.Send(KSendData, 0, iStatus);

			iState = EState4;
			Reschedule();
			return EPass;
			}

		case EState4:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Send() failed with %d</i>"), iStatus.Int());
				return EFail;
				}

			iLogger.WriteFormat(_L("<i>Receiving data over an internal socket using the descriptor API</i>"));
			iBuf.Zero();
			iBuf.SetLength(KSendData().Length());
			iSocket.Recv(iBuf, 0, iStatus);

			iState = EState5;
			Reschedule();
			return EPass;
			}

		case EState5:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Recv() failed with %d</i>"),iStatus.Int());
				return EFail;
				}

			iLogger.WriteFormat(_L("<i>Checking that the data was read into the buffer</i>"));
			if (iBuf != KSendData)
				{
				iLogger.WriteFormat(_L("<i>Incorrect data to recved</i>"));
				return EFail;
				}
			return EPass;
			}

		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 40));
			return EFail;
		}
	}

void CEIntSockTest38::Cleanup()
	{
	iSocket.Close();
	}

//-------------------------------------------------------------------------------------------------
//	CEIntSockTest39
//-------------------------------------------------------------------------------------------------

CEIntSockTestBase* CEIntSockTest39::NewL( CProxyProvd* aProvd )
	{
	CEIntSockTestBase* ret = new ( ELeave ) CEIntSockTest39( aProvd );
	return ret;		
	}

CEIntSockTest39::CEIntSockTest39( CProxyProvd* aProvd )
: CEIntSockTestBase(aProvd),
iState ( EState1 )

	{
		
	}

TVerdict CEIntSockTest39::RunTestL()
	{
    iLogger.WriteFormat(_L("<i>Test 39 - Open (asynchronous) and close socket</i>"));
	iLogger.WriteFormat(_L("<i>Open socket to protocol by name</i>"));
	switch ( iState )
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Open socket by name</i>"));
			
			iSocket.Open(KDummyTwoName, iStatus);
			iState = EState2;
			Reschedule ();
			return EPass;			
			}	
		case EState2:
			{
			iLogger.WriteFormat(_L("<i>Open return with status code %d</i>"), iStatus.Int() );
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Close the socket</i>"));				
			iSocket.Close ();
			iLogger.WriteFormat(_L("<i>Open socket to protocol supplying address family</i>"));
			iSocket.Open(KDummyAddrFamily, KSockDatagram, KDummyOne, iStatus);						
			iState = EState3;		
			Reschedule ();	
			return EPass;				
			}
		case EState3:
			{
			iLogger.WriteFormat(_L("<i>Open with address family return with status code %d</i>"), iStatus.Int() );
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			return EPass;			
			}
		default:
			{
			 __ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 41));
			return EFail;			
			}
		}
	}

void CEIntSockTest39::Cleanup()
	{
	iSocket.Close ();		
	}

//-------------------------------------------------------------------------------------------------
//	CEIntSockTest40
//-------------------------------------------------------------------------------------------------
CEIntSockTestBase* CEIntSockTest40::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest40(aProvd);
	return ret;
	}

CEIntSockTest40::CEIntSockTest40(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
TVerdict CEIntSockTest40::RunTestL()
	{
	TInt err;
	switch(iState)
		{		
		case EState1:
			iLogger.WriteFormat(_L("<i>Test 40 - Asyn Open and cancel/close socket</i>"));
			iLogger.WriteFormat(_L("<i>Asyn Open socket to protocol by name</i>"));
			iSocket.Open(KDummyTwoName, iStatus);				
			iState  = EState2;
			Reschedule();
			return EPass;	
		case EState2:
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Async Open() failed with %d</i>"),iStatus.Int());
				return EFail;
				}			
			iSocket.Close();
		
			iLogger.WriteFormat(_L("<i>Async Open socket to protocol supplying address family</i>"));
			iSocket.Open(KDummyAddrFamily, KSockDatagram, KDummyOne, iStatus);
						
			iLogger.WriteFormat(_L("<i>Cancelling/Closing Async Open() </i>"));
			iSocket.Close();			
			
			iState  = EState3;
			Reschedule();
			return EPass;
		case EState3:
			if(iStatus == KErrNone)
				{
				iSocket.Close();
				}
			else if(iStatus != KErrAbort)
				{
				iLogger.WriteFormat(_L("<i>Cancelling Async Open failed with %d </i>"), iStatus.Int());
				return EFail;					
				}
							
			iLogger.WriteFormat(_L("<i>Open socket to protocol supplying address family</i>"));
			err = iSocket.Open(KDummyAddrFamily, KSockDatagram, KDummyOne);
			
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Async Open() failed with %d</i>"),err);
				return EFail;
				}
			iSocket.Close();			
			return EPass;		
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 42));
			return EFail;
		}
	}

void CEIntSockTest40::Cleanup()
	{
	iSocket.Close();
	}


//-------------------------------------------------------------------------------------------------
//	CEIntSockTest41
//-------------------------------------------------------------------------------------------------
CEIntSockTestBase* CEIntSockTest41::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest41(aProvd);
	return ret;
	}

CEIntSockTest41::CEIntSockTest41(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
TVerdict CEIntSockTest41::RunTestL()
	{
	_LIT8(KSendData,"1234567890");
	TInt err;
	switch(iState)
		{		
		case EState1:
			iLogger.WriteFormat(_L("<i>Test 41 - SendTo/RecvFrom with descriptior (datagram protocol)</i>"));			
			iLogger.WriteFormat(_L("<i>Opening a datagram socket.</i>"));
			err = iSocket.Open (KDummyOneName);
			if ( err != KErrNone )
				{
				iLogger.WriteFormat(_L("<i>Opening of datagram socket failed.</i>"));
				return EFail;	
				}
			iDestAddr.SetFamily(KProxyAddrFamily);
			iBuf.Copy (KSendData());
			iSocket.SendTo(iBuf, iDestAddr, 0, iStatus);
			iState  = EState2;
			Reschedule();
			return EPass;	
		case EState2:
			if(iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>SendTo failed with error code %d.</i>"), iStatus.Int() );
				return EFail;						
				}
			iBuf.Zero();
			iSocket.RecvFrom (iBuf, iRemoteAddr, 0, iStatus);
			iState  = EState3;
			Reschedule();
			return EPass;
		case EState3:
			{
			if(iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>RecvFrom failed with error code %d.</i>"), iStatus.Int() );
				return EFail;						
				}
			if (iDestAddr.Family() != iRemoteAddr.Family())
				{
				iLogger.WriteFormat(_L("<i>SendTo address is different from RecvFrom address.</i>"));
				return EFail;												
				}
			
			if ( iBuf != KSendData() )
				{
				iLogger.WriteFormat(_L("<i>SendTo data is different from RecvFrom data.</i>"));
				return EFail;																	
				}
			return EPass;						
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 43));
			return EFail;
		}
	}

void CEIntSockTest41::Cleanup()
	{
	iSocket.Close();
	}

//-------------------------------------------------------------------------------------------------
//	CEIntSockTest42
//-------------------------------------------------------------------------------------------------
CEIntSockTestBase* CEIntSockTest42::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest42(aProvd);
	return ret;
	}

CEIntSockTest42::CEIntSockTest42(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
TVerdict CEIntSockTest42::RunTestL()
	{
	_LIT8(KSendData,"1234567890");
	TInt err;
	switch(iState)
		{		
		case EState1:
			iLogger.WriteFormat(_L("<i>Test 42 - SendTo/RecvFrom with RMBufChain (datagram protocol)</i>"));			
			iLogger.WriteFormat(_L("<i>Opening a datagram socket.</i>"));
			err = iSocket.Open (KDummyOneName);
			if ( err != KErrNone )
				{
				iLogger.WriteFormat(_L("<i>Opening of datagram socket failed.</i>"));
				return EFail;	
				}
			iDestAddr.SetFamily(KProxyAddrFamily);
			iSendChain.CreateL(KSendData());
			
			iLogger.WriteFormat(_L("<i>SendTo data over an internal socket using the RMBufChain API</i>"));
			
			iSocket.SendTo(iSendChain, iDestAddr, 0, iStatus);
			iState  = EState2;
			Reschedule();
			return EPass;	
		case EState2:
			if(iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>SendTo failed with error code %d.</i>"), iStatus.Int() );
				return EFail;						
				}
			iSendChain.Remove ();

			
			iBuf.SetLength(KSendData().Length());
			iSocket.RecvFrom (iRecvChain, iRemoteAddr, 0, iStatus);
			iState  = EState3;
			Reschedule();
			return EPass;
		case EState3:
			{
			if(iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>RecvFrom failed with error code %d.</i>"), iStatus.Int() );
				return EFail;						
				}
			if (iDestAddr.Family() != iRemoteAddr.Family())
				{
				iLogger.WriteFormat(_L("<i>SendTo address is different from RecvFrom address.</i>"));
				return EFail;												
				}
			iRecvChain.CopyOut(iBuf);
			iRecvChain.Free ();
			if ( iBuf != KSendData )
				{
				iLogger.WriteFormat(_L("<i>SendTo data is different from RecvFrom data.</i>"));
				return EFail;																	
				}

			return EPass;						
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 44));
			return EFail;
		}
	}

void CEIntSockTest42::Cleanup()
	{
	iSocket.Close();
	__ASSERT_DEBUG(iRecvChain.IsEmpty(), User::Panic(KSpecAssert_ESockTestptstcs, 45));
	__ASSERT_DEBUG(iSendChain.IsEmpty(), User::Panic(KSpecAssert_ESockTestptstcs, 46));	
	}

//-------------------------------------------------------------------------------------------------
//	CEIntSockTest43
//-------------------------------------------------------------------------------------------------
CEIntSockTestBase* CEIntSockTest43::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest43(aProvd);
	return ret;
	}

CEIntSockTest43::CEIntSockTest43(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
TVerdict CEIntSockTest43::RunTestL()
	{
	_LIT8(KSendData,"1234567890");
	TInt err;
	switch(iState)
		{		
		case EState1:
			iLogger.WriteFormat(_L("<i>Test 43 - SendTo/RecvFrom with length --- descriptior (datagram protocol)</i>"));			
			iLogger.WriteFormat(_L("<i>Opening a datagram socket.</i>"));
			err = iSocket.Open (KDummyOneName);
			if ( err != KErrNone )
				{
				iLogger.WriteFormat(_L("<i>Opening of datagram socket failed.</i>"));
				return EFail;	
				}
			iDestAddr.SetFamily(KProxyAddrFamily);
			iBuf.Copy (KSendData());
			iSocket.SendTo(iBuf, iDestAddr, 0, iStatus, iLength);
			iState  = EState2;
			Reschedule();
			return EPass;	
		case EState2:
			if(iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>SendTo failed with error code %d.</i>"), iStatus.Int() );
				return EFail;						
				}
			
			if ( iLength() != KSendData().Length() )
				{
				iLogger.WriteFormat(_L("<i>SendTo didn't return the expected send length.</i>") );
				return EFail;											
				}
			iLength = 0;
			iBuf.Zero();
			iSocket.RecvFrom (iBuf, iRemoteAddr, 0, iStatus, iLength);
			iState  = EState3;
			Reschedule();
			return EPass;
		case EState3:
			{
			if(iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>RecvFrom failed with error code %d.</i>"), iStatus.Int() );
				return EFail;						
				}
			if ( iLength() != 0 )
				{
				iLogger.WriteFormat(_L("<i>RecvFrom didn't return the expected send length.</i>"));
				return EFail;											
				}
			if (iDestAddr.Family() != iRemoteAddr.Family())
				{
				iLogger.WriteFormat(_L("<i>SendTo address is different from RecvFrom address.</i>"));
				return EFail;												
				}
			
			if ( iBuf != KSendData() )
				{
				iLogger.WriteFormat(_L("<i>SendTo data is different from RecvFrom data.</i>"));
				return EFail;																	
				}

			return EPass;						
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 47));
			return EFail;
		}
	}

void CEIntSockTest43::Cleanup()
	{
	iSocket.Close();
	}

//-------------------------------------------------------------------------------------------------
//	CEIntSockTest44
//-------------------------------------------------------------------------------------------------
CEIntSockTestBase* CEIntSockTest44::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest44(aProvd);
	return ret;
	}

CEIntSockTest44::CEIntSockTest44(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
TVerdict CEIntSockTest44::RunTestL()
	{
	_LIT8(KSendData,"1234567890");
	TInt err;
	switch(iState)
		{		
		case EState1:
			iLogger.WriteFormat(_L("<i>Test 44 - SendTo/RecvFrom with RMBufChain (datagram protocol)</i>"));			
			iLogger.WriteFormat(_L("<i>Opening a datagram socket.</i>"));
			err = iSocket.Open (KDummyOneName);
			if ( err != KErrNone )
				{
				iLogger.WriteFormat(_L("<i>Opening of datagram socket failed.</i>"));
				return EFail;	
				}
			iDestAddr.SetFamily(KProxyAddrFamily);
			iSendChain.CreateL(KSendData());
			
			iLogger.WriteFormat(_L("<i>SendTo data over an internal socket using the RMBufChain API</i>"));
			
			iSocket.SendTo(iSendChain, iDestAddr, 0, iStatus, iLength);
			iState  = EState2;
			Reschedule();
			return EPass;	
		case EState2:
			if(iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>SendTo failed with error code %d.</i>"), iStatus.Int() );
				return EFail;						
				}
			if ( iLength() != KSendData().Length() )
				{
				iLogger.WriteFormat(_L("<i>SendTo didn't return the expected send length.</i>") );
				return EFail;											
				}

			iSendChain.Remove ();
			
			iLength = 0;
			iBuf.SetLength(KSendData().Length());
			iSocket.RecvFrom (iRecvChain, iRemoteAddr, 0, iStatus, iLength);
			iState  = EState3;
			Reschedule();
			return EPass;
		case EState3:
			{
			if(iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>RecvFrom failed with error code %d.</i>"), iStatus.Int() );
				return EFail;						
				}

			if ( iLength() != 0 )
				{
				iLogger.WriteFormat(_L("<i>RecvFrom didn't return the expected send length %d.</i>"), iLength() );
				return EFail;											
				}

			if (iDestAddr.Family() != iRemoteAddr.Family())
				{
				iLogger.WriteFormat(_L("<i>SendTo address is different from RecvFrom address.</i>"));
				return EFail;												
				}
			iRecvChain.CopyOut(iBuf);
			iRecvChain.Free ();
			
			if ( iBuf != KSendData )
				{
				iLogger.WriteFormat(_L("<i>SendTo data is different from RecvFrom data.</i>"));
				return EFail;																	
				}

			return EPass;						
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 48));
			return EFail;
		}
	}

void CEIntSockTest44::Cleanup()
	{
	iSocket.Close();
	__ASSERT_DEBUG(iSendChain.IsEmpty(), User::Panic(KSpecAssert_ESockTestptstcs, 49));
	__ASSERT_DEBUG(iRecvChain.IsEmpty(), User::Panic(KSpecAssert_ESockTestptstcs, 50));
	}

//-------------------------------------------------------------------------------------------------
//	CEIntSockTest45
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest45::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest45(aProvd);
	return ret;
	}

CEIntSockTest45::CEIntSockTest45(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
TVerdict CEIntSockTest45::RunTestL()
	{
	TInt err;
	_LIT8(KSendData,"1234567890");
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>	Test 45 - Send and receive a stream protocol that temporarily flows control off(RMBufChain API)</i>"));
			iLogger.WriteFormat(_L("<i>Opening PDummy 2</i>"));
			err = iSocket.Open(KDummyTwoName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
			
			iDestAddr.SetFamily(KProxyAddrFamily);
			iLogger.WriteFormat(_L("<i>Opening an internal socket (stream protocol)</i>"));
			iSocket.Connect(iDestAddr, iStatus);
			
			iState = EState2;
			Reschedule();
			return EPass;	
			}
		case EState2:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Connect() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
				
			iLogger.WriteFormat(_L("<i>Sending data over an internal socket using the RMBufChain API</i>"));
			
			iSendChain.CreateL(KSendData());
			iSocket.SetOpt(KDummyOptionSetTimedFlowOffWrite, 0, KDummyWriteFlowOffTimeout);			
			iSocket.Send(iSendChain,0,iStatus);

			iState = EState3;
			Reschedule();
			return EPass;				
			}
		case EState3:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Send() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			
			iSendChain.Remove();
		
			iLogger.WriteFormat(_L("<i>Receiving data over an internal socket using the RMBufChain API</i>"));
			iSocket.SetOpt(KDummyOptionSetTimedFlowOffWrite, 0, 0);			
			iBuf.SetLength(KSendData().Length());
			iSocket.Recv(iRecvChain,0,iStatus);
			
			iState = EState4;
			Reschedule();
			return EPass;
			}
		case EState4:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Recv() failed returning %d</i>"),iStatus.Int());
				return EFail;
				}
			iRecvChain.CopyOut(iBuf);
			iRecvChain.Free();
						
			iLogger.WriteFormat(_L("<i>Checking that the data was read into the buffer</i>"));
			if (iBuf != KSendData())
				{
				iLogger.WriteFormat(_L("<i>Incorrect data to recved</i>"));
				return EFail;
				}

			return EPass;
			}


		default: 
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 51));
			return EFail;
		}
	}

void CEIntSockTest45::Cleanup()
	{
	iSocket.Close();
	__ASSERT_DEBUG(iSendChain.IsEmpty(), User::Panic(KSpecAssert_ESockTestptstcs, 52));
	__ASSERT_DEBUG(iRecvChain.IsEmpty(), User::Panic(KSpecAssert_ESockTestptstcs, 53));
	}	

//-------------------------------------------------------------------------------------------------
//	CEIntSockTest46
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest46::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest46(aProvd);
	return ret;
	}

CEIntSockTest46::CEIntSockTest46(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
TVerdict CEIntSockTest46::RunTestL()
	{
	
	TInt err;
	_LIT8(KSendData,"1234567890");
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 46 - Send to and receive (with RecvOneOrMore )from a stream protocol (Descriptor API) </i>"));
			iLogger.WriteFormat(_L("<i>Opening a socket to PDummy 2</i>"));
			err = iSocket.Open(KDummyTwoName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
		
			iLogger.WriteFormat(_L("<i>Opening an internal socket (stream protocol)</i>"));
			
			iDestAddr.SetFamily(KProxyAddrFamily);
			iSocket.Connect(iDestAddr, iStatus);
			
			iState = EState2;
			Reschedule();
			return EPass;		
			}
		case EState2:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Connect() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			iBuf.Copy ( KSendData() );
			iLogger.WriteFormat(_L("<i>Sending data over an internal socket using the descriptor API</i>"));
			iSocket.Send(iBuf,0,iStatus);
			
			iState = EState3;
			Reschedule();
			return EPass;	
			}
		case EState3:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Send() failed with %d</i>"),iStatus.Int());
				return EFail;
				}
			
			iLogger.WriteFormat(_L("<i>Receiving data over an internal socket using the descriptor API</i>"));
			iBuf.Zero();
			iBuf.SetLength(KSendData().Length());
			iSocket.RecvOneOrMore(iBuf,0,iStatus, iLength);
			
			iState = EState4;
			Reschedule();
			return EPass;				
			}
		case EState4:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Recv() didn't return KErrNotSupported (returned %d)</i>"),iStatus.Int());
				return EFail;
				}

			iLogger.WriteFormat(_L("<i>Checking that the data was read into the buffer</i>"));
			if (iBuf != KSendData)
				{
				iLogger.WriteFormat(_L("<i>Incorrect data to recved</i>"));
				return EFail;
				}

			if ( iLength() != KSendData().Length() )
				{
				iLogger.WriteFormat(_L("<i>RecvOneOrMore didn't return the expected send length.</i>") );
				return EFail;											
				}
				
			return EPass;
			}
		default: 
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 54));
			return EFail;
		}
	}

void CEIntSockTest46::Cleanup()
	{
	iSocket.Close();
	}
	
//-------------------------------------------------------------------------------------------------
//	CEIntSockTest47
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest47::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest47(aProvd);
	return ret;
	}

CEIntSockTest47::CEIntSockTest47(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
TVerdict CEIntSockTest47::RunTestL()
	{
	
	TInt err;
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 47 - Ioctl returns not supported on null socket </i>"));
			iLogger.WriteFormat(_L("<i>Opening a null socket</i>"));
			err = iSocket.Open();
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
		
			iLogger.WriteFormat(_L("<i>Perform Ioctl on the null socket</i>"));
			
			iSocket.Ioctl(KDummyIoctlCheckStarted, iStatus);			
			iState = EState2;
			Reschedule();
			return EPass;		
			}
		case EState2:
			{
			if (iStatus != KErrNotSupported)	
				{
				iLogger.WriteFormat(_L("<i>Ioctl operation on null socket returns %d != KErrNotSupported</i>"),iStatus.Int());
				return EFail;
				}
			return EPass;	
			}
		default: 
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 55));
			return EFail;
		}
	}

void CEIntSockTest47::Cleanup()
	{
	iSocket.Close();
	}
	

//-------------------------------------------------------------------------------------------------
//	CEIntSockTest48
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest48::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest48(aProvd);
	return ret;
	}

CEIntSockTest48::CEIntSockTest48(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}

TVerdict CEIntSockTest48::RunTestL()
	{	
	TInt err = KErrNone;
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 48 - Ioctl observes InternalOptionBit </i>"));
			iLogger.WriteFormat(_L("<i>Opening a Dummy Protocol 1 socket</i>"));
			iSocket.Open(KDummyOneName, iStatus);
		
			iState = EState2;
			Reschedule();
			return EPass;		
			}
		case EState2:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Open on Dummy Protocol 1 socket returns %d</i>"),iStatus.Int());
				return EFail;
				}

			iLogger.WriteFormat(_L("<i>Perform Ioctl with InternalIoctlBit</i>"));			
			iSocket.Ioctl(KInternalIoctlBit, iStatus);			
			
			iState = EState3;
			Reschedule();
			return EPass;					
			}
		case EState3:
			{
			if (iStatus != KErrAccessDenied)
				{
				iLogger.WriteFormat(_L("<i>Ioctl option with KInternalIoctlBit failed with error = %d</i>"),iStatus.Int());
				return EFail;
				}

			iLogger.WriteFormat(_L("<i>SetOpt with internal options bit</i>"));
			err = iSocket.SetOpt(KSocketInternalOptionBit, KSOLSocket);
			if (err != KErrAccessDenied)
				{
				iLogger.WriteFormat(_L("<i>Setopt with internal options bit returned with error = %d != KErrAccessDenied</i>"),iStatus.Int());									
				return EFail;
				}

			iLogger.WriteFormat(_L("Trying to GetOpt internal options bit"));
			TPckgBuf<TBool> boolean;
			err = iSocket.GetOpt(KSocketInternalOptionBit, KSOLSocket, boolean);
			if (err != KErrAccessDenied)
				{
				iLogger.WriteFormat(_L("<i>Getopt with internal options bit returned with error = %d != KErrAccessDenied</i>"),iStatus.Int());									
				return EFail;
				}

			iLogger.WriteFormat(_L("<i>Testing StartL() with Ioctl KDummyIoctlCheckStarted</i>"));
			iSocket.Ioctl(KDummyIoctlCheckStarted, iStatus, NULL, KIoctlDummyLevel);

			iState = EState4;
			Reschedule();
			return EPass;									
			}
		case EState4:
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Ioctl with KDummIoctlCheckStarted returned with error = %d != KErrNone</i>"),iStatus.Int());	
				return EFail;				
				}
			return EPass;
		default: 
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 56));
			return EFail;
		}
	}

void CEIntSockTest48::Cleanup()
	{
	iSocket.Close();
	}

//-------------------------------------------------------------------------------------------------
//	CEIntSockTest49
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest49::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest49(aProvd);
	return ret;
	}

CEIntSockTest49::CEIntSockTest49(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}

TVerdict CEIntSockTest49::RunTestL()
	{	
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 49 - Ioctl auto bind and check started </i>"));
			iLogger.WriteFormat(_L("<i>Opening a Dummy Protocol 1 socket</i>"));
			iSocket.Open(KDummyOneName, iStatus);
		
			iState = EState2;
			Reschedule();
			return EPass;		
			}
		case EState2:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Open on Dummy Protocol 1 socket returns %d</i>"),iStatus.Int());
				return EFail;
				}

			iLogger.WriteFormat(_L("<i>Perform Ioctl with KDummyIoctlCheckStarted</i>"));			
			iSocket.Ioctl(KDummyIoctlCheckStarted, iStatus, NULL, KIoctlDummyLevel);
			
			iState = EState3;
			Reschedule();
			return EPass;					
			}
		case EState3:
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Ioctl option with KInternalIoctlBit failed with error = %d</i>"),iStatus.Int());
				return EFail;
				}

			iLogger.WriteFormat(_L("<i>Auto bind -- Connect the socket</i>"));
			iSocket.Connect(iAddress, iStatus);

			iState = EState4;
			Reschedule();
			return EPass;					
		case EState4:
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Connect failed with = %d != KErrNone</i>"),iStatus.Int());
				return EFail;					
				}
			iLogger.WriteFormat(_L("<i>Checking bound status with Ioctl</i>"));
			iSocket.Ioctl(KDummyIoctlCheckBound, iStatus, NULL, KIoctlDummyLevel);		
				
			iState = EState5;
			Reschedule();
			return EPass;		
		case EState5:
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Checking bound status with Ioctl failed with = %d != KErrNone</i>"),iStatus.Int());
				return EFail;										
				}
			return EPass;
		default: 
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 57));
			return EFail;
		}
	}

void CEIntSockTest49::Cleanup()
	{
	iSocket.Close();
	}

//-------------------------------------------------------------------------------------------------
//	CEIntSockTest50
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest50::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest50(aProvd);
	return ret;
	}

CEIntSockTest50::CEIntSockTest50(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}

TVerdict CEIntSockTest50::RunTestL()
	{	
	TInt err = KErrNone;
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 50 - Get and Set options </i>"));
			iLogger.WriteFormat(_L("<i>Opening a Dummy Protocol 1 socket</i>"));
			iSocket.Open(KDummyOneName, iStatus);
		
			iState = EState2;
			Reschedule();
			return EPass;		
			}
		case EState2:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Open on Dummy Protocol 1 socket returns %d</i>"),iStatus.Int());
				return EFail;
				}
			
			const TInt KPort = 666;
			iLogger.WriteFormat(_L("Set local port as %d"), KPort);
			err = iSocket.SetLocalPort(KPort);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Set local port as 666 failed with error = %d</i>"),iStatus.Int());
				return EFail;					
				}

			iLogger.WriteFormat(_L("Get local port that is set. should be %d"), KPort);
			TInt port = iSocket.LocalPort();
			if (port != KPort)
				{
				iLogger.WriteFormat(_L("<i>Get local port failed Got = %d Expected = %d</i>"), port, KPort);
				return EFail;					
				}
					
			iLogger.WriteFormat(_L("SetOpt with KSONonBlockingIO"));
			err = iSocket.SetOpt(KSONonBlockingIO, KSOLSocket);
			if (err != KErrNone)	
				{
				iLogger.WriteFormat(_L("SetOpt with KSONonBlockingIO failed with error = %d"), err);
				return EFail;				
				}
			
			
			iLogger.WriteFormat(_L("GetOpt with KSONonBlockingIO"));
			TPckgBuf<TBool> boolean;
			err = iSocket.GetOpt(KSONonBlockingIO, KSOLSocket, boolean);
			if (err != KErrNone)	
				{
				iLogger.WriteFormat(_L("GetOpt with KSONonBlockingIO failed with error = %d"), err);					
				return EFail;
				}

			if (boolean() != TRUE)	
				{
				iLogger.WriteFormat(_L("The value returned on GetOpt with KSONonBlockingIO failed"));					
				return EFail;
				}

			iLogger.WriteFormat(_L("GetOpt with KSOBlockingIO"));
			err = iSocket.GetOpt(KSOBlockingIO, KSOLSocket, boolean);
			if (err != KErrNone)	
				{
				iLogger.WriteFormat(_L("GetOpt with KSOBlockingIO failed with error = %d"), err);					
				return EFail;
				}

			if (boolean() != FALSE)	
				{
				iLogger.WriteFormat(_L("The value returned on GetOpt with KSOBlockingIO failed"));					
				return EFail;
				}


			iLogger.WriteFormat(_L("SetOpt with KSONonBlockingIO"));
			err = iSocket.SetOpt(KSONonBlockingIO, KSOLSocket);
			if (err != KErrNone)	
				{
				iLogger.WriteFormat(_L("SetOpt with KSONonBlockingIO failed with error = %d"), err);
				return EFail;					
				}

			iLogger.WriteFormat(_L("GetOpt with KSONonBlockingIO"));
			err = iSocket.GetOpt(KSONonBlockingIO, KSOLSocket, boolean);
			if (err != KErrNone)	
				{
				iLogger.WriteFormat(_L("GetOpt with KSONonBlockingIO failed with error = %d"), err);					
				return EFail;
				}

			if (boolean() != TRUE)	
				{
				iLogger.WriteFormat(_L("The value returned on GetOpt with KSONonBlockingIO failed"));					
				return EFail;
				}
			
			iLogger.WriteFormat(_L("GetOpt with KSOBlockingIO"));
			err = iSocket.GetOpt(KSONonBlockingIO, KSOLSocket, boolean);
			if (err != KErrNone)	
				{
				iLogger.WriteFormat(_L("GetOpt with KSOBlockingIO failed with error = %d"), err);					
				return EFail;
				}

			if (boolean() != TRUE)	
				{
				iLogger.WriteFormat(_L("The value returned on GetOpt with KSOBlockingIO failed"));					
				return EFail;
				}
			return EPass;
			}
		default: 
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 58));
			return EFail;
		}
	}

void CEIntSockTest50::Cleanup()
	{
	iSocket.Close();
	}

//-------------------------------------------------------------------------------------------------
//	CEIntSockTest51
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest51::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest51(aProvd);
	return ret;
	}

CEIntSockTest51::CEIntSockTest51(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}

TVerdict CEIntSockTest51::RunTestL()
	{	
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 51 - Ioctl non-completion tests</i>"));
			iLogger.WriteFormat(_L("<i>Opening a Dummy Protocol 1 socket</i>"));
			iSocket.Open(KDummyOneName, iStatus);
		
			iState = EState2;
			Reschedule();
			return EPass;		
			}
		case EState2:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Open on Dummy Protocol 1 socket returns %d</i>"),iStatus.Int());
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Perform Ioctl with KDummyIoctlIgnore.</i>"));
			iSocket.Ioctl(KDummyIoctlIgnore, iStatus);
			iSocket.CancelAll();

			iState = EState3;
			Reschedule();
			return EPass;		
			}
		case EState3:
			{
			if (iStatus != KErrCancel)
				{
				iLogger.WriteFormat(_L("<i>Cancelling of Ioctl returns with err = %d != KErrCancel</i>"), iStatus.Int());
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Query the status of previous of Ioctl call.</i>"));
			iSocket.Ioctl(KDummyIoctlCancelled, iStatus);
			
			iState = EState4;
			Reschedule();
			return EPass;
			}	
		case EState4:
			if (iStatus == KErrNotFound)
				{
				iLogger.WriteFormat(_L("<i>Ioctl returns with err = %d CancellAll failed to cancel the previous Ioctl</i>"), iStatus.Int());
				return EFail;					
				}			
			return EPass;
		default: 
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 59));
			return EFail;
		}
	}

void CEIntSockTest51::Cleanup()
	{
	iSocket.Close();
	}

//-------------------------------------------------------------------------------------------------
//	CEIntSockTest52
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest52::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest52(aProvd);
	return ret;
	}

CEIntSockTest52::CEIntSockTest52(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}

TVerdict CEIntSockTest52::RunTestL()
	{	
	const TUint KBufferSize = 1024;	
	TInt err = KErrNone;
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 52 - Retention of socket options from before Accept to after Accept</i>"));
			iLogger.WriteFormat(_L("<i>Opening a Dummy Protocol 2 socket</i>"));
			iAcceptingSocket.Open(KDummyThreeName, iStatus);
		
			iState = EState2;
			Reschedule();
			return EPass;		
			}
		case EState2:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Open on Dummy Protocol 1 socket returns %d</i>"),iStatus.Int());
				return EFail;
				}

			iLogger.WriteFormat(_L("<i>Bind the socket</i>"));
			TSockAddr addr;
			err = iAcceptingSocket.Bind(addr);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Bind failed with error %d</i>"), err);
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Start listening on the socket</i>"));
			err = iAcceptingSocket.Listen(5);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Listening failed with error %d</i>"), err);
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Opening a null socket</i>"));
			err = iSocket.Open();
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Opening a null socket returned with error %d</i>"), err);
				return EFail;
				}

			iLogger.WriteFormat(_L("<i>Setting a whole bunch of KSOLSocket options on the accepting socket</i>"));
			err = iAcceptingSocket.SetOpt(KSOSendBuf, KSOLSocket, KBufferSize);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>SetOpt with KSOSendBuf returned with error %d</i>"), err);
				return EFail;
				}			
			err = iAcceptingSocket.SetOpt(KSORecvBuf, KSOLSocket, KBufferSize);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>SetOpt with KSORecvBuf returned with error %d</i>"), err);
				return EFail;
				}			
			err = iAcceptingSocket.SetOpt(KSODebug, KSOLSocket, ETrue);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>SetOpt with KSODebug returned with error %d</i>"), err);
				return EFail;
				}			
			err = iAcceptingSocket.SetOpt(KSONonBlockingIO, KSOLSocket);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>SetOpt with KSONonBlockingIO returned with error %d</i>"), err);
				return EFail;
				}			
			
			iLogger.WriteFormat(_L("<i>Now simulating a remote connection to complete the accept</i>"));
			err = iAcceptingSocket.SetOpt(KDummyOptionSetConnectComplete, KIoctlDummyLevel);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>SetOpt with KDummyOptionSetConnectComplete returned with error %d</i>"), err);
				return EFail;
				}			

			// We set the connect complete option. Pdummy needs to run AO to complete the accept. 
			// So we cannot do the accept here. we move into the next state and do the accept there
			// Here just do a self complete and keep going
			iStatus = KRequestPending;
			iState = EState3;
			Reschedule();
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			return EPass;		
			}
		case EState3:
			{
			iLogger.WriteFormat(_L("Perform an accept"));
			iAcceptingSocket.Accept(iSocket, iStatus);		

			iState = EState4;
			Reschedule();
			return EPass;									
			}
		case EState4:
			{
			if (iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Accept failed with  error %d</i>"), iStatus.Int());
				return EFail;					
				}
				
			iLogger.WriteFormat(_L("<i>Retrieve the whole bunch of KSOLSocket options that is set</i>"));

			TInt retrievedOpt;
			err = iSocket.GetOpt(KSOSendBuf, KSOLSocket, retrievedOpt);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>GetOpt with KSOSendBuf returned with error %d</i>"), err);
				return EFail;
				}			

			if (retrievedOpt != KBufferSize)
				{
				iLogger.WriteFormat(_L("Opt KSOSendBuf did not return the correct value"));
				return EFail;
				}	
				
			err = iSocket.GetOpt(KSORecvBuf, KSOLSocket, retrievedOpt);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>GetOpt with KSORecvBuf returned with error %d</i>"), err);
				return EFail;
				}			

			if (retrievedOpt != KBufferSize)
				{
				iLogger.WriteFormat(_L("Opt KSORecvBuf did not return the correct value"));
				return EFail;
				}	

			err = iSocket.GetOpt(KSODebug, KSOLSocket, retrievedOpt);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>GetOpt with KSODebug returned with error %d</i>"), err);
				return EFail;
				}			
			if (!retrievedOpt)
				{
				iLogger.WriteFormat(_L("Opt KSODebug did not return the correct value"));
				return EFail;
				}	

			err = iSocket.GetOpt(KSONonBlockingIO, KSOLSocket, retrievedOpt);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>GetOpt with KSONonBlockingIO returned with error %d</i>"), err);
				return EFail;
				}			
			if (!retrievedOpt)
				{
				iLogger.WriteFormat(_L("<i>Opt KSONonBlockingIO did not return the correct value</i>"));
				return EFail;
				}	
			return EPass;
			}	
		default: 
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 60));
			return EFail;
		}
	}

void CEIntSockTest52::Cleanup()
	{
	iAcceptingSocket.Close();
	iSocket.Close();
	}

//-------------------------------------------------------------------------------------------------		
//	CEIntSockTest53
//-------------------------------------------------------------------------------------------------	

CEIntSockTestBase* CEIntSockTest53::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTest53* ret = new(ELeave) CEIntSockTest53(aProvd);
	CleanupStack::PushL(ret);
	ret->ConstructL();	
	CleanupStack::Pop(ret);
	return ret;
	}

CEIntSockTest53::CEIntSockTest53(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
CEIntSockTest53::~CEIntSockTest53()
	{
	delete iColl;
	}
	
void CEIntSockTest53::ConstructL()
	{
	iColl = CAggregateCollection::NewL(iStatus,CAggregateCollection::EAny);
	iRecvItem = iColl->GetNewItemL();
	iSendItem = iColl->GetNewItemL();
	}
	
TVerdict CEIntSockTest53::RunTestL()
	{
	TInt err;
	_LIT8(KSendData,"abcdefghij");
	_LIT8(KSendData2,"KLMNOPQRST");	

	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 53 - Partial read with descriptor</i>"));
			iLogger.WriteFormat(_L("<i>Opening a socket on Dummy protocol 2</i>"));
			err = iSocket.Open(KDummyTwoName);
			if (err != KErrNone)			
				{
				iLogger.WriteFormat(_L("<i>Open on Dummy Protocol 2 failed with error %d</i>"), err);
				return EFail;					
				}
				
			iLogger.WriteFormat(_L("<i>Attempting a connection</i>"));
			iSocket.Connect(iAddress, iStatus);
			
			iState = EState2;
			Reschedule();
			return EPass;				
			}
		case EState2:
			{
			if (iStatus != KErrNone)			
				{
				iLogger.WriteFormat(_L("<i>Connect failed with error %d</i>"), iStatus.Int());
				return EFail;					
				}			
						
			iLogger.WriteFormat(_L("<i>Sending 20 octets and reciving 10</i>"));
			iSendBuf.Copy(KSendData());
			iSendBuf.Append(KSendData2());
			
			iRecvItem->Start();
			iSendItem->Start();		
			iSocket.Recv(iRecvBuf, 0, iRecvItem->iStatus);
			iSocket.Send(iSendBuf, 0, iSendItem->iStatus);
			
			iState = EState3;
			iColl->Kick();
			Reschedule();
			return EPass;			
			}
		case EState3:
			{
			// Check which operation completed first (no order can be enforced) and check to
			// see if it was successful.
			if ((iSendItem->ReturnCode() != KErrNone) && (iRecvItem->ReturnCode() != KErrNone))
				{
				if(iSendItem->ReturnCode() != KRequestPending)
					{
					iLogger.WriteFormat(_L("<i>Send failed with error %d</i>"), iSendItem->ReturnCode());
					}
				else
					{
					iLogger.WriteFormat(_L("<i>Recv failed with error %d</i>"), iRecvItem->ReturnCode());
					}				
				return EFail;														
				}
				
			iState = EState4;
			iColl->Kick();
			Reschedule();
			return EPass;
			}
		case EState4:
			{
			// Check which operation completed second (no order can be enforced) and check to
			// see if it was successful.
			if ((iSendItem->ReturnCode() != KErrNone) && (iRecvItem->ReturnCode() != KErrNone))
				{
				if(iSendItem->ReturnCode() != KRequestPending)
					{
					iLogger.WriteFormat(_L("<i>Send failed with error %d</i>"), iSendItem->ReturnCode());
					}
				else
					{
					iLogger.WriteFormat(_L("<i>Recv failed with error %d</i>"), iRecvItem->ReturnCode());
					}				
				return EFail;														
				}				
			
			iLogger.WriteFormat(_L("<i>Check whether the Recv gave the first 10octets</i>"));			
			if (iRecvBuf != KSendData())
				{
				iLogger.WriteFormat(_L("<i>Recv data is not matching the first 10octets that is sent</i>"));
				return EFail;																			
				}
			
			iLogger.WriteFormat(_L("<i>Issue another recv to get the rest 10octets</i>"));			
			iRecvBuf.SetLength(0);
			iRecvItem->Start();
			iSocket.Recv(iRecvBuf, 0, iRecvItem->iStatus);

			iState = EState5;
			iColl->Kick();
			Reschedule();
			return EPass;
			}
		case EState5:
			{
			if (iRecvItem->ReturnCode() != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Recv failed with error %d</i>"), iRecvItem->ReturnCode());
				return EFail;																			
				}
			iLogger.WriteFormat(_L("<i>Check whether the Recv gave the last 10octets</i>"));			
			if (iRecvBuf != KSendData2())
				{
				iLogger.WriteFormat(_L("<i>Recv data is not matching the last 10octets that is sent</i>"));
				return EFail;																			
				}
			return EPass;
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 61));
			return EFail;
			
		}
	}
	
void CEIntSockTest53::Cleanup()
	{
	iSocket.Close();
	}


//-------------------------------------------------------------------------------------------------
//	CEIntSockTest54
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest54::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest54(aProvd);
	return ret;
	}

CEIntSockTest54::CEIntSockTest54(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}

TVerdict CEIntSockTest54::RunTestL()
	{	
	TInt err = KErrNone;
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 54 - RecvFrom not supported on ConnectionOriented socket</i>"));
			iLogger.WriteFormat(_L("<i>Opening a Dummy Protocol 2 socket</i>"));
			err = iSocket.Open(KDummyTwoName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
			
			iLogger.WriteFormat(_L("<i>Attempting a Connect</i>"));
			iSocket.Connect(iAddress, iStatus);
		
			iState = EState2;
			Reschedule();
			return EPass;		
			}
		case EState2:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Connect failed with error %d</i>"),iStatus.Int());
				return EFail;
				}

			iLogger.WriteFormat(_L("<i>Attempt a RecvFrom</i>"));
			iSocket.RecvFrom(iRecvBuf,iAddress,0, iStatus,iLen);
			
			iState = EState3;
			Reschedule();
			return EPass;		
			}
		case EState3:
			{
			if (iStatus != KErrNotSupported)
				{
				iLogger.WriteFormat(_L("<i>RecvFrom on a connection oriented failed with error %d</i>"), iStatus.Int());
				return EFail;					
				}
				
			return EPass;
			}	
		default: 
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 62));
			return EFail;
		}
	}

void CEIntSockTest54::Cleanup()
	{
	iSocket.Close();
	}

//-------------------------------------------------------------------------------------------------
//	CEIntSockTest55
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest55::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest55(aProvd);
	return ret;
	}

CEIntSockTest55::CEIntSockTest55(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}

TVerdict CEIntSockTest55::RunTestL()
	{	
	TInt err = KErrNone;
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 55 - Fail urgent data on a non supported socket</i>"));
			iLogger.WriteFormat(_L("<i>Opening a Dummy Protocol 1 socket</i>"));
			err = iSocket.Open(KDummyOneName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
			
			iLogger.WriteFormat(_L("<i>Send urgent data</i>"));
			iSendBuf.SetLength(5);
			iSocket.SendTo(iSendBuf, iAddress, KSockWriteUrgent, iStatus);
		
			iState = EState2;
			Reschedule();
			return EPass;		
			}
		case EState2:
			{
			if (iStatus != KErrNotSupported)	
				{
				iLogger.WriteFormat(_L("<i>Send urgent data oepration  failed with error %d Expected: KErrNotSupported</i>"),iStatus.Int());
				return EFail;
				}

			return EPass;		
			}
		default: 
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 63));
			return EFail;
		}
	}

void CEIntSockTest55::Cleanup()
	{
	iSocket.Close();
	}

//-------------------------------------------------------------------------------------------------
//	CEIntSockTest56
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest56::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest56(aProvd);
	return ret;
	}

CEIntSockTest56::CEIntSockTest56(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}

TVerdict CEIntSockTest56::RunTestL()
	{	
	TInt err = KErrNone;
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 56 - Send to an unconnected socket</i>"));
			iLogger.WriteFormat(_L("<i>Opening a Dummy Protocol 3 socket</i>"));
			err = iSocket.Open(KDummyThreeName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
			
			iLogger.WriteFormat(_L("<i>Send urgent data</i>"));
			iSendBuf.SetLength(5);
			iSocket.Write(iSendBuf, iStatus);
		
			iState = EState2;
			Reschedule();
			return EPass;		
			}
		case EState2:
			{
			if (iStatus != KErrNotReady)	
				{
				iLogger.WriteFormat(_L("<i>Write operation failed with error %d Expected: KErrNotReady</i>"),iStatus.Int());
				return EFail;
				}

			return EPass;		
			}
		default: 
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 64));
			return EFail;
		}
	}

void CEIntSockTest56::Cleanup()
	{
	iSocket.Close();
	}

//-------------------------------------------------------------------------------------------------
//	CEIntSockTest57
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest57::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTest57* ret = new(ELeave) CEIntSockTest57(aProvd);
	CleanupStack::PushL (ret);
	ret->ConstructL();
	CleanupStack::Pop();
	return ret;
	}

CEIntSockTest57::CEIntSockTest57(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}

void CEIntSockTest57::ConstructL()
	{
	iColl = CAggregateCollection::NewL(iStatus,CAggregateCollection::EAny);
	iShutdownItem = iColl->GetNewItemL();		
	iRecvItem = iColl->GetNewItemL();			
	iSendItem = iColl->GetNewItemL();				
	}
	
CEIntSockTest57::~CEIntSockTest57()
	{
	delete iColl;		
	}

TVerdict CEIntSockTest57::RunTestL()
	{	
	TInt err = KErrNone;
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 57 - Shoutdown with EStopOutput</i>"));
			iLogger.WriteFormat(_L("<i>Opening a Dummy Protocol 2 socket</i>"));
			err = iSocket.Open(KDummyTwoName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Attempting a shutdown</i>"));	
			iShutdownItem->Start();			
			iSocket.Shutdown(RSocket::EStopOutput, iShutdownItem->iStatus);
			if (iShutdownItem->iStatus != KRequestPending)
				{
				iLogger.WriteFormat(_L("<i>Shutdown failed with %d Expected: KRequestPending</i>"),err);
				return EFail;	
				}
			
			iLogger.WriteFormat(_L("<i>Attempting a recv</i>"));
			iRecvItem->Start();
			iSocket.Recv(iRecvBuf, 0, iRecvItem->iStatus, iRecvLen);
								
			iState = EState2;
			iColl->Kick();
			Reschedule();
			return EPass;		
			}
		case EState2:
			{
			if (iShutdownItem->ReturnCode() != KRequestPending || iRecvItem->ReturnCode() != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Shutdown with EStopInput and Recv operation failed with error Shutdown status:%d Recv status: %d</i>"),iShutdownItem->ReturnCode(), iRecvItem->ReturnCode());
				return EFail;
				}
			
			if (iRecvLen() != 10)
				{
				iLogger.WriteFormat(_L("<i>Recv length matching failed</i>"));
				return EFail;					
				}

			iLogger.WriteFormat(_L("<i>Attempting a Send</i>"));
			iSendBuf.SetLength(1);
			iSendItem->Start ();
			iSocket.Send(iSendBuf, 0, iSendItem->iStatus);
			
			iState = EState3;
			iColl->Kick();
			Reschedule();
			return EPass;		
			}
		
		case EState3:
			{
			if (iSendItem->ReturnCode() != KErrNotReady)	
				{
				iLogger.WriteFormat(_L("<i>Send on a shutdowned socket failed with error %d Expected: KErrNotReady</i>"), iSendItem->ReturnCode());					
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Attempting to read more data</i>"));
			iRecvItem->Start();
			iSocket.Recv(iRecvBuf, 0, iRecvItem->iStatus, iRecvLen);

			iState = EState4;
			iColl->Kick();
			Reschedule();
			return EPass;					
			}
		case EState4:
			{
			if (iRecvItem->ReturnCode() != KErrEof)	
				{
				iLogger.WriteFormat(_L("<i>Recv on a shutdowned socket failed with error %d Expected: KErrEof</i>"), iSendItem->ReturnCode());					
				return EFail;
				}
			if (iRecvLen() != 1)
				{
				iLogger.WriteFormat(_L("<i>Recv length matching failed</i>"));
				return EFail;					
				}
			iRecvLen = 0;
			iLogger.WriteFormat(_L("<i>Attempting to read more data</i>"));
			iRecvItem->Start();
			iSocket.Recv(iRecvBuf, 0, iRecvItem->iStatus, iRecvLen);

			iState = EState5;
			iColl->Kick();
			Reschedule();
			return EPass;
			}
		case EState5:
			{
			if (iRecvItem->ReturnCode() != KErrEof)	
				{
				iLogger.WriteFormat(_L("<i>Recv on a shutdowned socket failed with error %d Expected: KErrEof</i>"), iRecvItem->ReturnCode());					
				return EFail;
				}
			if (iRecvLen() != 0)
				{
				iLogger.WriteFormat(_L("<i>Recv length matching failed</i>"));
				return EFail;					
				}
			iLogger.WriteFormat(_L("<i>Closing the socket</i>"));
			iSocket.Close();

			iState = EState6;
			iColl->Kick();
			Reschedule();
			return EPass;
			}
		case EState6:
			{
			if (iShutdownItem->ReturnCode() != KErrCancel)
				{
				iLogger.WriteFormat(_L("<i>Shutdown status returned %d Expected: KErrCancel</i>"), iShutdownItem->ReturnCode());
				return EFail;										
				}
			return EPass;
			}
			
		default: 
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 65));
			return EFail;
		}
	}

void CEIntSockTest57::Cleanup()
	{
	iSocket.Close();
	}
	
//-------------------------------------------------------------------------------------------------
//	CEIntSockTest52
//-------------------------------------------------------------------------------------------------	
	
CEIntSockTestBase* CEIntSockTest58::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest58(aProvd);
	return ret;
	}

CEIntSockTest58::CEIntSockTest58(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}

TVerdict CEIntSockTest58::RunTestL()
	{	
	TInt err = KErrNone;
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 58 - Close before completion of the accept</i>"));
			iLogger.WriteFormat(_L("<i>Opening a Dummy Protocol 2 accepting socket</i>"));
			err = iAcceptingSocket.Open(KDummyTwoName);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
			
			iLogger.WriteFormat(_L("<i>Open a null socket</i>"));
			err = iSocket.Open();
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}

			iLogger.WriteFormat(_L("<i>Bind the socket</i>"));
			TSockAddr addr;
			err = iAcceptingSocket.Bind(addr);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Bind failed with error %d</i>"), err);
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Start listening on the socket</i>"));
			err = iAcceptingSocket.Listen(5);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Listening failed with error %d</i>"), err);
				return EFail;
				}

			iLogger.WriteFormat(_L("Perform an accept"));
			iAcceptingSocket.Accept(iSocket, iStatus);		
					

			iLogger.WriteFormat(_L("<i>Now simulating a remote connection to complete the accept</i>"));
			err = iAcceptingSocket.SetOpt(KDummyOptionSetConnectComplete, KIoctlDummyLevel);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>SetOpt with KDummyOptionSetConnectComplete returned with error %d</i>"), err);
				return EFail;
				}			
			
			iLogger.WriteFormat(_L("<i>Close the null socket</i>"));
			iSocket.Close();
		
			iState = EState2;
			Reschedule();
			return EPass;		
			}
		case EState2:
			{
			if ((iStatus != KErrCancel) && (iStatus != KErrNone))
				{
				iLogger.WriteFormat(_L("<i>Accept failed on a closed null socket with error %d: Expected: KErrCancel or KErrNone</i>"),iStatus.Int());
				return EFail;
				}

			iLogger.WriteFormat(_L("<i>Open a null socket again</i>"));
			err = iSocket.Open();
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
			
			iLogger.WriteFormat(_L("Perform an accept"));
			iAcceptingSocket.Accept(iSocket, iStatus);		
					

			iLogger.WriteFormat(_L("<i>Now simulating a remote connection to complete the accept</i>"));
			err = iAcceptingSocket.SetOpt(KDummyOptionSetConnectComplete, KIoctlDummyLevel);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>SetOpt with KDummyOptionSetConnectComplete returned with error %d</i>"), err);
				return EFail;
				}			


			iState = EState3;
			Reschedule();
			return EPass;		
			}
		case EState3:
			{
			if (iStatus != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>Accept failed with error %d</i>"),iStatus.Int());
				return EFail;
				}

			return EPass;									
			}
		default: 
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 66));
			return EFail;
		}
	}

void CEIntSockTest58::Cleanup()
	{
	iAcceptingSocket.Close();
	iSocket.Close();
	}


//-------------------------------------------------------------------------------------------------
//	CEIntSockTest59
//-------------------------------------------------------------------------------------------------
CEIntSockTestBase* CEIntSockTest59::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTestBase* ret = new(ELeave) CEIntSockTest59(aProvd);
	return ret;
	}

CEIntSockTest59::CEIntSockTest59(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1)
	{}
	
TVerdict CEIntSockTest59::RunTestL()
	{
	_LIT8(KSendData,"1234567890");
	TInt err;
	switch(iState)
		{		
		case EState1:
			iLogger.WriteFormat(_L("<i>Test 59 - SendTo/RecvFrom with RMBufChain (datagram protocol) with KSockReadPeek</i>"));			
			iLogger.WriteFormat(_L("<i>Opening a datagram socket.</i>"));
			err = iSocket.Open (KDummyOneName);
			if ( err != KErrNone )
				{
				iLogger.WriteFormat(_L("<i>Opening of datagram socket failed.</i>"));
				return EFail;	
				}
			iDestAddr.SetFamily(KProxyAddrFamily);
			iSendChain.CreateL(KSendData());
			
			iLogger.WriteFormat(_L("<i>SendTo data over an internal socket using the RMBufChain API</i>"));
			
			iSocket.SendTo(iSendChain, iDestAddr, 0, iStatus, iLength);
			iState  = EState2;
			Reschedule();
			return EPass;	
		case EState2:
			if(iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>SendTo failed with error code %d.</i>"), iStatus.Int() );
				return EFail;						
				}
			if ( iLength() != KSendData().Length() )
				{
				iLogger.WriteFormat(_L("<i>SendTo didn't return the expected send length.</i>") );
				return EFail;											
				}

			iSendChain.Remove ();
			
			iLength = 0;
			iBuf.SetLength(KSendData().Length());
			iSocket.RecvFrom (iRecvChain, iRemoteAddr, KSockReadPeek, iStatus, iLength);
			iState  = EState3;
			Reschedule();
			return EPass;
		case EState3:
			{
			if(iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>RecvFrom failed with error code %d.</i>"), iStatus.Int() );
				return EFail;						
				}
			TInt len = iLength();
			if ( iLength() != 0 )
				{
				iLogger.WriteFormat(_L("<i>RecvFrom didn't return the expected send length %d.</i>"), iLength() );
				return EFail;											
				}
			if (iDestAddr.Family() != iRemoteAddr.Family())
				{
				iLogger.WriteFormat(_L("<i>SendTo address is different from RecvFrom address.</i>"));
				return EFail;												
				}
			iRecvChain.CopyOut(iBuf);
			
			if ( iBuf != KSendData )
				{
				iLogger.WriteFormat(_L("<i>SendTo data is different from RecvFrom data.</i>"));
				return EFail;																	
				}

			iLength = 0;
			iBuf.SetLength(KSendData().Length());
			iSocket.RecvFrom (iRecvChain, iRemoteAddr, 0, iStatus, iLength);
			iState  = EState4;
			Reschedule();
			return EPass;
			}
		case EState4:
			{
			if(iStatus != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>RecvFrom failed with error code %d.</i>"), iStatus.Int() );
				return EFail;						
				}

			if (iLength() != 0)
				{
				iLogger.WriteFormat(_L("<i>RecvFrom didn't return the expected send length %d.</i>"), iLength() );
				return EFail;											
				}

			if (iDestAddr.Family() != iRemoteAddr.Family())
				{
				iLogger.WriteFormat(_L("<i>SendTo address is different from RecvFrom address.</i>"));
				return EFail;												
				}
			iRecvChain.CopyOut(iBuf);
			iRecvChain.Free ();
			
			if ( iBuf != KSendData )
				{
				iLogger.WriteFormat(_L("<i>SendTo data is different from RecvFrom data.</i>"));
				return EFail;																	
				}

			return EPass;				
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 67));
			return EFail;
		}
	}

void CEIntSockTest59::Cleanup()
	{
	iSocket.Close();
	__ASSERT_DEBUG(iSendChain.IsEmpty(), User::Panic(KSpecAssert_ESockTestptstcs, 68));
	__ASSERT_DEBUG(iRecvChain.IsEmpty(), User::Panic(KSpecAssert_ESockTestptstcs, 69));
	}

//-------------------------------------------------------------------------------------------------
//	CEIntSockTest60
//-------------------------------------------------------------------------------------------------	
CEIntSockTestBase* CEIntSockTest60::NewL(CProxyProvd* aProvd)
	{
	CEIntSockTest60* ret = new(ELeave) CEIntSockTest60(aProvd);
	CleanupStack::PushL (ret);
	ret->ConstructL();
	CleanupStack::Pop();
	return ret;
	}

CEIntSockTest60::CEIntSockTest60(CProxyProvd* aProvd) :
	CEIntSockTestBase(aProvd),
	iState(EState1),
	iInetAddrTo(INET_ADDR(127,0,0,1),7)
	{}

void CEIntSockTest60::ConstructL()
	{
	iColl = CAggregateCollection::NewL(iStatus,CAggregateCollection::EAny);
	iRecvItem = iColl->GetNewItemL();			
	iSendItem = iColl->GetNewItemL();				
	}
	
CEIntSockTest60::~CEIntSockTest60()
	{
	delete iColl;		
	}

TVerdict CEIntSockTest60::RunTestL()
	{	
    const  TInt     KIPHeaderSize = 20; //-- IP header size
    const  TInt     KUdpHeaderSize= 8;  //-- UDP header size

  _LIT8(KSendData, "ABCDEFGHIJKLMNOPQRSTUVWXYZ"); 

	TInt err = KErrNone;
	switch (iState)
		{
		case EState1:
			{
			iLogger.WriteFormat(_L("<i>Test 60 - Read from socket with KSockReadPeek flag set in different situations</i>"));
			iLogger.WriteFormat(_L("<i>Opening a UDP input socket</i>"));
			err = iSocketIn.Open(KAfInet, KSockDatagram, KProtocolInetUdp);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
			iLogger.WriteFormat(_L("<i>Opening a UDP output socket</i>"));
			err = iSocketOut.Open(KAfInet, KSockDatagram, KProtocolInetUdp);
			if (err != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>Open() failed with %d</i>"),err);
				return EFail;
				}
				
			iLogger.WriteFormat(_L("<i>Bind the address</i>"));				
			iSocketIn.Bind (iInetAddrTo);

			iLogger.WriteFormat(_L("<i>Send the data</i>"));	
			iSendBuf.Copy(KSendData());
			iSendItem->Start();
			iSocketOut.SendTo(iSendBuf, iInetAddrTo, 0, iSendItem->iStatus);
			
			iState = EState2;
			Reschedule();
			iColl->Kick();
			return EPass;		
			}
		case EState2:
			{
			if (iSendItem->ReturnCode() != KErrNone)				
				{
				iLogger.WriteFormat(_L("<i>SendTo failed with %d</i>"),iSendItem->ReturnCode());
				return EFail;					
				}
			iLogger.WriteFormat(_L("<i>Peek data from socket and check the size</i>"));	
			iRecvItem->Start();
		    iSocketIn.RecvFrom(iRecvBuf, iInetAddrFrom, KIpHeaderIncluded|KSockReadPeek, iRecvItem->iStatus);
			
			iState = EState3;
			Reschedule();
			iColl->Kick();
			return EPass;					
			}
		case EState3:	
			{
			if (iRecvItem->ReturnCode() != KErrNone)				
				{
				iLogger.WriteFormat(_L("<i>RecvFrom failed with %d</i>"),iRecvItem->ReturnCode());
				return EFail;					
				}
			
			if (iRecvBuf.Length() != (KIPHeaderSize+KUdpHeaderSize+KSendData().Length()))
				{
				iLogger.WriteFormat(_L("<i>RecvFrom failed to deliver the expected data %d</i>"),err);
				return EFail;														
				}
			iLogger.WriteFormat(_L("<i>Peek data from socket and check the size</i>"));	
			iRecvItem->Start();
		    iSocketIn.RecvFrom(iRecvBuf2, iInetAddrFrom, KIpHeaderIncluded|KSockReadPeek, iRecvItem->iStatus);
			
			iState = EState4;
			Reschedule();
			iColl->Kick();
			return EPass;								
			}
		case EState4:
			{
			if (iRecvItem->ReturnCode() != KErrNone)				
				{
				iLogger.WriteFormat(_L("<i>RecvFrom failed with %d</i>"),iRecvItem->ReturnCode());
				return EFail;					
				}
			if (iRecvBuf != iRecvBuf2)
				{
				iLogger.WriteFormat(_L("<i>Received data is not same</i>"));
				return EFail;														
				}
			iLogger.WriteFormat(_L("<i>Recv data from socket and check the size</i>"));	
			iRecvBuf.SetLength(0);
			iRecvItem->Start();
			iSocketIn.RecvFrom(iRecvBuf, iInetAddrFrom, 0, iRecvItem->iStatus);
			
			iState = EState5;
			Reschedule();
			iColl->Kick();
			return EPass;											
			}
		case EState5:
			{
			if (iRecvItem->ReturnCode() != KErrNone)				
				{
				iLogger.WriteFormat(_L("<i>RecvFrom failed with %d</i>"),iRecvItem->ReturnCode());
				return EFail;					
				}
			if (iRecvBuf.Length() != KSendData().Length())
				{
				iLogger.WriteFormat(_L("<i>Received data is not same</i>"));
				return EFail;														
				}
			
			iLogger.WriteFormat(_L("<i>Peek data from socket and Cancel the recv</i>"));
			iRecvItem->Start();
			iRecvBuf.SetLength(0);			
			iSocketIn.RecvFrom(iRecvBuf, iInetAddrFrom, KIpHeaderIncluded|KSockReadPeek, iRecvItem->iStatus);
			iSocketIn.CancelRecv();

			iLogger.WriteFormat(_L("<i>Send data to the socket</i>"));
			iSendItem->Start();			
			iSocketOut.SendTo(iSendBuf, iInetAddrTo, 0, iSendItem->iStatus);

			iState = EState6;
			Reschedule();
			iColl->Kick();
			return EPass;			
			}
		case EState6:
			{
			if (iRecvItem->ReturnCode() != KErrCancel)
				{
				iLogger.WriteFormat(_L("<i>CancelRecv failed with %d</i>"),iRecvItem->ReturnCode());
				return EFail;															
				}

			iState = EState7;
			Reschedule();
			iColl->Kick();
			return EPass;						
			}
		case EState7:
			{
			if (iSendItem->ReturnCode() != KErrNone)
				{
				iLogger.WriteFormat(_L("<i>SendTo failed with %d</i>"),iSendItem->ReturnCode());
				return EFail;															
				}

			iLogger.WriteFormat(_L("<i>Peek data from socket</i>"));
			iRecvBuf.SetLength(0);
			iRecvItem->Start();
			iSocketIn.RecvFrom(iRecvBuf, iInetAddrFrom, KIpHeaderIncluded|KSockReadPeek, iRecvItem->iStatus);

			iState = EState8;
			Reschedule();
			iColl->Kick();
			return EPass;						
			}
		case EState8:
			{
			if (iRecvItem->ReturnCode() != KErrNone)	
				{
				iLogger.WriteFormat(_L("<i>RecvFrom failed with %d</i>"),iRecvItem->ReturnCode());
				return EFail;					
				}

			if (iRecvBuf.Length() != KIPHeaderSize + KUdpHeaderSize + KSendData().Length())
				{
				iLogger.WriteFormat(_L("<i>Received data is not same</i>"));
				return EFail;									
				}
			return EPass;	
			}
		default: 
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestptstcs, 70));
			return EFail;
		}
	}

void CEIntSockTest60::Cleanup()
	{
	iSocketIn.Close();
	iSocketOut.Close();	
	}



//-------------------------------------------------------------------------------------------------
//  CEIntSockTest61
//------------------------------------------------------------------------------------------------- 
    
CEIntSockTestBase* CEIntSockTest61::NewL(CProxyProvd* aProvd)
    {
    CEIntSockTestBase* ret = new(ELeave) CEIntSockTest61(aProvd);
    return ret;
    }

CEIntSockTest61::CEIntSockTest61(CProxyProvd* aProvd) :
    CEIntSockTestBase(aProvd)
    {}
    
TVerdict CEIntSockTest61::RunTestL()
    {
    Messages::TNodeId sourceCookie;
    Messages::TRuntimeCtxIdOp sourceCookieOp(sourceCookie);
    Messages::TNodeIdOp sourceNodeIdOp(sourceCookie); 
    
    // Arbitrary numbers to test with
    const TUint16 KSomeWorkerId = 0xf0f0;
    const TUint8 KSomeSalt = 127;
    TAny* KSomePtr = (TAny*)0xff0000ff; 
    sourceCookieOp.Set(KSomeSalt, KSomeWorkerId);
    sourceNodeIdOp.SetPtr(KSomePtr);
    
    TEIntSockOpMsgWrapper opMsgWrapper;
    CInternalSocketImpl* dummyImplPtr = NULL;
    CommsFW::MCommsTransportSender* dummyTransportSender = NULL;
    
    TBuf8<sizeof(TEIntSockMsgExt)> msgBuff;
    TEIntSockMsgExt* msgPtr = const_cast<TEIntSockMsgExt*>(reinterpret_cast<const TEIntSockMsgExt*>(msgBuff.Ptr()));
    
    // Test part 1
    iLogger.WriteFormat(_L("Corrupting TEIntSockMsgExt memory with 0xcc"));
    Mem::Fill(msgPtr, sizeof(TEIntSockMsgExt), TChar(0xcc));

    iLogger.WriteFormat(_L("Constructing TEIntSockMsgExt"));
    new (msgPtr)TEIntSockMsgExt(ESoCreate, opMsgWrapper, dummyImplPtr, dummyTransportSender, sourceCookie);

    TInt result = Mem::Compare(reinterpret_cast<const TUint8*>(&sourceCookie), (TInt)sizeof(Messages::TNodeId),
        reinterpret_cast<const TUint8*>(&msgPtr->ImplCookie()), (TInt)sizeof(Messages::TNodeId)); 
    if (result == 0)
        {
        iLogger.WriteFormat(_L("Cookie comparison result = 0 as expected"));
        }
    else
        {
        iLogger.WriteFormat(_L("Cookie comparison result = %d"), result);
        return EFail;
        }
    
    // Test part 2
    iLogger.WriteFormat(_L("Corrupting TEIntSockMsgExt memory with 0xaa"));
    Mem::Fill(msgPtr, sizeof(TEIntSockMsgExt), TChar(0xaa));

    iLogger.WriteFormat(_L("Constructing TEIntSockMsgExt"));
    new (msgPtr)TEIntSockMsgExt(ESoCreate, opMsgWrapper, dummyImplPtr, dummyTransportSender, sourceCookie);

    result = Mem::Compare(reinterpret_cast<const TUint8*>(&sourceCookie), sizeof(Messages::TNodeId),
        reinterpret_cast<const TUint8*>(&msgPtr->ImplCookie()), sizeof(Messages::TNodeId)); 
    if (result == 0)
        {
        iLogger.WriteFormat(_L("Cookie comparison result = 0 as expected"));
        }
    else
        {
        iLogger.WriteFormat(_L("Cookie comparison result = %d"), result);
        return EFail;
        }

    return EPass;
    }

void CEIntSockTest61::Cleanup()
    {
    }
