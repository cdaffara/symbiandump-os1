// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains the implementation of the test cases that test Napt.
// Creates a TCP conection to the NaptTestSerever.Send gateway address and 
// test case name to NaptTestSerever. The test cases are executed from 
// NaptTestSerever.The test case result is send back from the NaptTestSerever. 
// 
//

/**
 @file
*/

#include "TE_RConnectionNaptTestSteps.h"
#include <nifman.h>

TE_RConnectionTest950::~TE_RConnectionTest950(){}

enum TVerdict TE_RConnectionTest950::doTestStepL(void)
{
	SetTestStepResult(EFail);
	
	INFO_PRINTF1(_L("Send data over TCP socket to Echo Server running in Test Network"));
    
    const int resultLength   = 2; 
	const int addressLength  = 20;
	const int testNameLength = 10;
	
	RSocketServ socketServNapt;
	RConnection connNapt,connNapt1;
	RSocket sockTestServ,socketNapt,socketGloabal;
	HBufC8* sendEthAddress;
	
	TInt err;
	err = OpenSocketServer(socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServNapt);
    
	err = OpenConnection(connNapt, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt);

	err = OpenConnection(connNapt1, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt1);
	
	//start a connection on private interface
	err = StartConnectionWithOverrides(connNapt, iNaptPrivateIap);
	TESTEL(KErrNone == err, err);
	connNapt.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	//start a connection on public interface
	err = StartConnectionWithOverrides(connNapt1, iNaptPublicIap);
	TESTEL(KErrNone == err, err);
	connNapt1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	
	err = OpenTcpSocketExplicit(sockTestServ, socketServNapt, connNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sockTestServ);
			
	_LIT(KInterfaceName1,"eth");
	TInetAddr ethAddress;
	TName ethString;
	
	//get eth interface address
	TUint32 testAddr1 = GetInterfaceAddress(sockTestServ,KInterfaceName1());
	if(testAddr1 <= KErrNone)
		{
		INFO_PRINTF1(_L("Failed to get eth address"));
		return TestStepResult();
		}
	
	ethAddress.SetAddress(testAddr1);
	ethAddress.Output(ethString);
	
	//Concatenate eth ip address with the sring ':ethaddress\n'
	sendEthAddress = HBufC8::NewL(ethString.Length()+addressLength);
	sendEthAddress->Des().Copy(ethString); 
	sendEthAddress->Des().Append(_L8(":ethaddress\n"));
	
	//Load NAPT   
    err = LoadNapt(socketNapt,sockTestServ,socketServNapt);
    TESTEL(KErrNone == err, err);
    
    //Connect to NaptTestServer
  	err = ConnectTcpSocket(sockTestServ, iNaptTestServAddr);
    TESTEL(KErrNone == err, err);
                  
	_LIT8(KTest,"test1\n");
	TBufC8 <testNameLength> tst(KTest);
	TPtrC8 testCaseName(tst);
	         
    //Need to add route in NaptTestServer.send gateway IP address to the NaptTestServer.
    TRequestStatus status;
    sockTestServ.Send(*sendEthAddress, 0, status);
	User::WaitForRequest(status);
    err = status.Int();
	
	//send test case name to NaptTestServer
	sockTestServ.Send(testCaseName, 0, status);
	User::WaitForRequest(status);
    err = status.Int();

    TSockXfrLength length;
 	TBuf8<resultLength> result;
	result.Zero();

	//receive test result from NaptTestServer
	sockTestServ.RecvOneOrMore(result, 0, status,length);
	User::WaitForRequest(status);
	err = status.Int();
	
	if(err == 0)
		{
		//pass = 1,fail = 0
		if(result[0] == '1')
			{
	    	SetTestStepResult(EPass);
			}
		else if(result[0] == '0')
			{
	    	INFO_PRINTF1(_L("Test failed.Returned fail from NaptTestServer "));
	    	SetTestStepResult(EFail);
			}
		}

	delete sendEthAddress;

	socketNapt.Close();

	err = DestroyTcpSocket(sockTestServ);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();

	CloseConnection(connNapt);
	CleanupStack::Pop();

	CloseConnection(connNapt);
	CleanupStack::Pop();

	CloseSocketServer(socketServNapt);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest950


TE_RConnectionTest951::~TE_RConnectionTest951(){}

enum TVerdict TE_RConnectionTest951::doTestStepL(void)
{
	SetTestStepResult(EFail);
	
	INFO_PRINTF1(_L("Send data over UDP Socket to Echo Server running in Test Network"));

	const int resultLength   = 2; 
	const int addressLength  = 20;
	const int testNameLength = 10;
	
	RSocketServ socketServNapt;
	RConnection connNapt,connNapt1;
	RSocket sockTestServ,socketNapt,socketGloabal;
	HBufC8* sendEthAddress;
            
    TInt err;
	err = OpenSocketServer(socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServNapt);
  	    
	err = OpenConnection(connNapt, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt);
	
	err = OpenConnection(connNapt1, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt1);
	
	//start a connection on private interface
	err = StartConnectionWithOverrides(connNapt, iNaptPrivateIap);
	TESTEL(KErrNone == err, err);
	connNapt.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	//start a connection on public interface
	err = StartConnectionWithOverrides(connNapt1, iNaptPublicIap);
	TESTEL(KErrNone == err, err);
	connNapt1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	
	//Connect to NaptTestServer
	err = OpenTcpSocketExplicit(sockTestServ, socketServNapt, connNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sockTestServ);
	
	_LIT(KInterfaceName1,"eth");
	TInetAddr ethAddress;
	TName ethString;
	
	//get eth interface address
	TUint32 testAddr1 = GetInterfaceAddress(sockTestServ,KInterfaceName1());
	if(testAddr1 <= KErrNone)
		{
		INFO_PRINTF1(_L("Failed to get eth address"));
		return TestStepResult();
		}
	
	ethAddress.SetAddress(testAddr1);
	ethAddress.Output(ethString);
	
	//Concatenate eth ip address with the sring ':ethaddress\n'
	sendEthAddress = HBufC8::NewL(ethString.Length()+addressLength);
	sendEthAddress->Des().Copy(ethString); 
	sendEthAddress->Des().Append(_L8(":ethaddress\n"));
	
	//Load NAPT   
    err = LoadNapt(socketNapt,sockTestServ,socketServNapt);
    TESTEL(KErrNone == err, err);
    
    //Connect to NaptTestServer
  	err = ConnectTcpSocket(sockTestServ, iNaptTestServAddr);
    TESTEL(KErrNone == err, err);

	_LIT8(KTest,"test2\n");
	TBufC8 <testNameLength> tst(KTest);
	TPtrC8 testCaseName(tst);
	 
	//Need to add route in NaptTestServer.send gateway IP address to the NaptTestServer.
    TRequestStatus status;
    sockTestServ.Send(*sendEthAddress, 0, status);
	User::WaitForRequest(status);
    err = status.Int();
    
	//send test case name to NaptTestServer.
	sockTestServ.Send(testCaseName, 0, status);
	User::WaitForRequest(status);
     err = status.Int();

    TSockXfrLength length;
 	TBuf8<resultLength> result;
	result.Zero();

	//receive test result from NaptTestServer
	sockTestServ.RecvOneOrMore(result, 0, status,length);
	User::WaitForRequest(status);
	err = status.Int();
	
	if(err == 0)
		{
		//pass = 1,fail = 0
		if(result[0] == '1')
			{
    		SetTestStepResult(EPass);
			}
		else if(result[0] == '0')
			{
    		INFO_PRINTF1(_L("Test failed.Returned fail from NaptTestServer "));
    		SetTestStepResult(EFail);
			}
		}
	
	delete sendEthAddress;

	socketNapt.Close();

	err = DestroyTcpSocket(sockTestServ);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();
		
	CloseConnection(connNapt);
	CleanupStack::Pop();

	CloseConnection(connNapt);
	CleanupStack::Pop();

	CloseSocketServer(socketServNapt);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest951
TE_RConnectionTest952::~TE_RConnectionTest952(){}

enum TVerdict TE_RConnectionTest952::doTestStepL(void)
{
    SetTestStepResult(EFail);
    
    INFO_PRINTF1(_L("Send data over UDP Socket to invalid port."));
          
    const int resultLength   = 2; 
	const int addressLength  = 20;
	const int testNameLength = 10;

	RSocketServ socketServNapt;
	RConnection connNapt,connNapt1;
	RSocket sockTestServ,socketNapt,socketGloabal;
	HBufC8* sendEthAddress;
        
    TInt err;
	err = OpenSocketServer(socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServNapt);
  	    
	err = OpenConnection(connNapt, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt);
	
	err = OpenConnection(connNapt1, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt1);
	
	//start a connection on private interface
	err = StartConnectionWithOverrides(connNapt, iNaptPrivateIap);
	TESTEL(KErrNone == err, err);
	connNapt.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	//start a connection on public interface
	err = StartConnectionWithOverrides(connNapt1, iNaptPublicIap);
	TESTEL(KErrNone == err, err);
	connNapt1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);

	err = OpenTcpSocketExplicit(sockTestServ, socketServNapt, connNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sockTestServ);
	
	_LIT(KInterfaceName1,"eth");
	TInetAddr ethAddress;
	TName ethString;
	
	//get eth interface address
	TUint32 testAddr1 = GetInterfaceAddress(sockTestServ,KInterfaceName1());
	if(testAddr1 <= KErrNone)
		{
		INFO_PRINTF1(_L("Failed to get eth address"));
		return TestStepResult();
		}
	
	ethAddress.SetAddress(testAddr1);
	ethAddress.Output(ethString);
	
	//Concatenate eth ip address with the sring ':ethaddress\n'
	sendEthAddress = HBufC8::NewL(ethString.Length()+addressLength);
	sendEthAddress->Des().Copy(ethString); 
	sendEthAddress->Des().Append(_L8(":ethaddress\n"));
	
	//Load NAPT   
    err = LoadNapt(socketNapt,sockTestServ,socketServNapt);
    TESTEL(KErrNone == err, err);
    
    //Connect to NaptTestServer
  	err = ConnectTcpSocket(sockTestServ, iNaptTestServAddr);
    TESTEL(KErrNone == err, err);
                     
	_LIT8(KTest,"test3\n");
	TBufC8 <testNameLength> tst(KTest);
	TPtrC8 testCaseName(tst);
	 
    //Need to add route in NaptTestServer.send gateway IP address to the NaptTestServer.
    TRequestStatus status;
	sockTestServ.Send(*sendEthAddress, 0, status);
	User::WaitForRequest(status);
    err = status.Int();
	
	//send test case name to NaptTestServer.
	sockTestServ.Send(testCaseName, 0, status);
	User::WaitForRequest(status);
    err = status.Int();

    TSockXfrLength length;
 	TBuf8<resultLength> result;
	result.Zero();

	//receive test result from NaptTestServer
	sockTestServ.RecvOneOrMore(result, 0, status,length);
	User::WaitForRequest(status);
	err = status.Int();
	
	if(err == 0)
		{
		//pass = 1,fail = 0
		if(result[0] == '1')
			{
    		SetTestStepResult(EPass);
			}
		else if(result[0] == '0')
			{
    		INFO_PRINTF1(_L("Test failed.Returned fail from NaptTestServer "));
    		SetTestStepResult(EFail);
			}
		}
	delete sendEthAddress;

	socketNapt.Close();

	err = DestroyTcpSocket(sockTestServ);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();

	CloseConnection(connNapt);
	CleanupStack::Pop();

	CloseConnection(connNapt);
	CleanupStack::Pop();

	CloseSocketServer(socketServNapt);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest952
TE_RConnectionTest953::~TE_RConnectionTest953(){}

enum TVerdict TE_RConnectionTest953::doTestStepL(void)
{
    SetTestStepResult(EFail);
    
    INFO_PRINTF1(_L(" Resolve host name from IP addr "));
     
    const int resultLength   = 2; 
	const int addressLength  = 20;
	const int testNameLength = 10;

	RSocketServ socketServNapt;
	RConnection connNapt,connNapt1;
	RSocket sockTestServ,socketNapt,socketGloabal;
	HBufC8* sendEthAddress;
        
    TInt err;
	err = OpenSocketServer(socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServNapt);
  	    
	err = OpenConnection(connNapt, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt);
	
	err = OpenConnection(connNapt1, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt1);

	//start a connection on private interface
	err = StartConnectionWithOverrides(connNapt, iNaptPrivateIap);
	TESTEL(KErrNone == err, err);
	connNapt.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	//start a connection on public interface
	err = StartConnectionWithOverrides(connNapt1, iNaptPublicIap);
	TESTEL(KErrNone == err, err);
	connNapt1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);

	err = OpenTcpSocketExplicit(sockTestServ, socketServNapt, connNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sockTestServ);
		
	_LIT(KInterfaceName1,"eth");
	TInetAddr ethAddress;
	TName ethString;

	//get eth interface address
	TUint32 testAddr1 = GetInterfaceAddress(sockTestServ,KInterfaceName1());
	if(testAddr1 <= KErrNone)
		{
		INFO_PRINTF1(_L("Failed to get eth address"));
		return TestStepResult();
		}

	ethAddress.SetAddress(testAddr1);
	ethAddress.Output(ethString);
	
	//Concatenate eth ip address with the sring ':ethaddress\n'
	sendEthAddress = HBufC8::NewL(ethString.Length()+addressLength);
	sendEthAddress->Des().Copy(ethString); 
	sendEthAddress->Des().Append(_L8(":storeaddr\n"));
	
	//Load NAPT   
    err = LoadNapt(socketNapt,sockTestServ,socketServNapt);
    TESTEL(KErrNone == err, err);
    
    //Connect to NaptTestServer
  	err = ConnectTcpSocket(sockTestServ, iNaptTestServAddr);
    TESTEL(KErrNone == err, err);  
                       
	_LIT8(KTest,"test4\n");
	TBufC8 <testNameLength> tst(KTest);
	TPtrC8 testCaseName(tst);
	
	//Need to add route in NaptTestServer.send gateway IP address to the NaptTestServer. 
    TRequestStatus status;
    sockTestServ.Send(*sendEthAddress, 0, status);
	User::WaitForRequest(status);
    err = status.Int();
	
	//send test case name to NaptTestServer.
	sockTestServ.Send(testCaseName, 0, status);
	User::WaitForRequest(status);
    err = status.Int();

    TSockXfrLength length;
 	TBuf8<resultLength> result;
	result.Zero();

	//receive test result from NaptTestServer
	sockTestServ.RecvOneOrMore(result, 0, status,length);
	User::WaitForRequest(status);
	err = status.Int();
	
	if(err == 0)
		{
		//pass = 1,fail = 0
		if(result[0] == '1')
			{
    		SetTestStepResult(EPass);
			}
		else if(result[0] == '0')
			{
    		INFO_PRINTF1(_L("Test failed.Returned fail from NaptTestServer "));
    		SetTestStepResult(EFail);
			}
		}
	delete sendEthAddress;

	socketNapt.Close();

	err = DestroyTcpSocket(sockTestServ);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();
	
	CloseConnection(connNapt);
	CleanupStack::Pop();

	CloseConnection(connNapt);
	CleanupStack::Pop();

	CloseSocketServer(socketServNapt);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest953
TE_RConnectionTest954::~TE_RConnectionTest954()
{
}

enum TVerdict TE_RConnectionTest954::doTestStepL(void)
{
 	SetTestStepResult(EFail);
 	
 	INFO_PRINTF1(_L("Resolve IP addr from host name"));
        
    const int resultLength   = 2; 
	const int addressLength  = 20;
	const int testNameLength = 10;
	
	RSocketServ socketServNapt;
	RConnection connNapt,connNapt1;
	RSocket sockTestServ,socketNapt,socketGloabal;
	HBufC8* sendEthAddress;
        
    TInt err;
	err = OpenSocketServer(socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServNapt);
  	    
	err = OpenConnection(connNapt, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt);
	
	err = OpenConnection(connNapt1, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt1);

	//start a connection on private interface
	err = StartConnectionWithOverrides(connNapt, iNaptPrivateIap);
	TESTEL(KErrNone == err, err);
	connNapt.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	//start a connection on public interface
	err = StartConnectionWithOverrides(connNapt1, iNaptPublicIap);
	TESTEL(KErrNone == err, err);
	connNapt1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);

 	err = OpenTcpSocketExplicit(sockTestServ, socketServNapt, connNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sockTestServ);
		
	_LIT(KInterfaceName1,"eth");
	TInetAddr ethAddress;
	TName ethString;
	
	//get eth interface address
	TUint32 testAddr1 = GetInterfaceAddress(sockTestServ,KInterfaceName1());
	if(testAddr1 <= KErrNone)
		{
		INFO_PRINTF1(_L("Failed to get eth address"));
		return TestStepResult();
		}
	
	ethAddress.SetAddress(testAddr1);
	ethAddress.Output(ethString);
	
	//Concatenate eth ip address with the sring ':ethaddress\n'
	sendEthAddress = HBufC8::NewL(ethString.Length()+addressLength);
	sendEthAddress->Des().Copy(ethString); 
	sendEthAddress->Des().Append(_L8(":storeaddr\n"));
	
	//Load NAPT	   
    err = LoadNapt(socketNapt,sockTestServ,socketServNapt);
    TESTEL(KErrNone == err, err);
    
    //Connect to NaptTestServer
  	err = ConnectTcpSocket(sockTestServ, iNaptTestServAddr);
    TESTEL(KErrNone == err, err);
                  
	_LIT8(KTest,"test5\n");
	TBufC8 <testNameLength> tst(KTest);
	TPtrC8 testCaseName(tst);
	 
	//Need to add route in NaptTestServer.send gateway IP address  to the NaptTestServer. 
    TRequestStatus status;
	sockTestServ.Send(*sendEthAddress, 0, status);
	User::WaitForRequest(status);
     err = status.Int();
	
	//send test case name to NaptTestServer.
	sockTestServ.Send(testCaseName, 0, status);
	User::WaitForRequest(status);
    err = status.Int();

    TSockXfrLength length;
 	TBuf8<resultLength> result;
	result.Zero();

	//receive test result from NaptTestServer
	sockTestServ.RecvOneOrMore(result, 0, status,length);
	User::WaitForRequest(status);
	err = status.Int();
		
	if(err == 0)
		{
		//pass = 1,fail = 0
		if(result[0] == '1')
			{
	    	SetTestStepResult(EPass);
			}
		else if(result[0] == '0')
			{
	    	INFO_PRINTF1(_L("Test failed.Returned fail from NaptTestServer "));
	    	SetTestStepResult(EFail);
			}
		}
	delete sendEthAddress;
	
	socketNapt.Close();
	
	err = DestroyTcpSocket(sockTestServ);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();
	
	CloseConnection(connNapt);
	CleanupStack::Pop();
	
	CloseConnection(connNapt);
	CleanupStack::Pop();

	CloseSocketServer(socketServNapt);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest954	
TE_RConnectionTest955::~TE_RConnectionTest955()
{
}

enum TVerdict TE_RConnectionTest955::doTestStepL(void)
{
    SetTestStepResult(EFail);
    
    INFO_PRINTF1(_L(" Ping echo server in Test Network"));
         
	const int resultLength   = 2; 
	const int addressLength  = 20;
	const int testNameLength = 10;

	RSocketServ socketServNapt;
	RConnection connNapt,connNapt1;
	RSocket sockTestServ,socketNapt,socketGloabal;
	HBufC8* sendEthAddress;
        
    TInt err;
	err = OpenSocketServer(socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServNapt);
  	    
	err = OpenConnection(connNapt, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt);
	
	err = OpenConnection(connNapt1, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt1);

	//start a connection on private interface
	err = StartConnectionWithOverrides(connNapt, iNaptPrivateIap);
	TESTEL(KErrNone == err, err);
	connNapt.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	//start a connection on public interface
	err = StartConnectionWithOverrides(connNapt1, iNaptPublicIap);
	TESTEL(KErrNone == err, err);
	connNapt1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);

 	err = OpenTcpSocketExplicit(sockTestServ, socketServNapt, connNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sockTestServ);
			
	_LIT(KInterfaceName1,"eth");
	TInetAddr ethAddress;
	TName ethString;
	
	//get eth interface address	
	TUint32 testAddr1 = GetInterfaceAddress(sockTestServ,KInterfaceName1());
	if(testAddr1 <= KErrNone)
		{
		INFO_PRINTF1(_L("Failed to get eth address"));
		return TestStepResult();
		}
	
	ethAddress.SetAddress(testAddr1);
	ethAddress.Output(ethString);
	
	//Concatenate eth ip address with the sring ':ethaddress\n'
	sendEthAddress = HBufC8::NewL(ethString.Length()+addressLength);
	sendEthAddress->Des().Copy(ethString); 
	sendEthAddress->Des().Append(_L8(":ethaddress\n"));
	
	//Load NAPT	   
	err = LoadNapt(socketNapt,sockTestServ,socketServNapt);
	TESTEL(KErrNone == err, err);
	
	//Connect to NaptTestServer
	err = ConnectTcpSocket(sockTestServ, iNaptTestServAddr);
	TESTEL(KErrNone == err, err);
	              
	_LIT8(KTest,"test6\n");
	TBufC8 <testNameLength> tst(KTest);
	TPtrC8 testCaseName(tst);
	 
	//Need to add route in NaptTestServer.send gateway IP address to the NaptTestServer.
	TRequestStatus status;
	sockTestServ.Send(*sendEthAddress, 0, status);
	User::WaitForRequest(status);
	 err = status.Int();

	//send test case name to NaptTestServer.
	sockTestServ.Send(testCaseName, 0, status);
	User::WaitForRequest(status);
	err = status.Int();

	TSockXfrLength length;
	TBuf8<resultLength> result;
	result.Zero();

	//receive test result from NaptTestServer
	sockTestServ.RecvOneOrMore(result, 0, status,length);
	User::WaitForRequest(status);
	err = status.Int();

	if(err == 0)
		{
		//pass = 1,fail = 0
		if(result[0] == '1')
			{
			SetTestStepResult(EPass);
			}
		else if(result[0] == '0')
			{
			INFO_PRINTF1(_L("Test failed.Returned fail from NaptTestServer "));
			SetTestStepResult(EFail);
			}
		}
	delete sendEthAddress;

	socketNapt.Close();

	err = DestroyTcpSocket(sockTestServ);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();

	CloseConnection(connNapt);
	CleanupStack::Pop();

	CloseConnection(connNapt);
	CleanupStack::Pop();

	CloseSocketServer(socketServNapt);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest955	
TE_RConnectionTest956::~TE_RConnectionTest956()
{
}

enum TVerdict TE_RConnectionTest956::doTestStepL(void)
{
	SetTestStepResult(EFail);
	
	INFO_PRINTF1(_L(" Open many TCP Sockets and Send data to Echo Server over each socket"));
	 
	const int resultLength   = 2; 
	const int addressLength  = 20;
	const int testNameLength = 10;

	RSocketServ socketServNapt;
	RConnection connNapt,connNapt1;
	RSocket sockTestServ,socketNapt,socketGloabal;
	HBufC8* sendEthAddress;

	TInt err;
	err = OpenSocketServer(socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServNapt);
	    
	err = OpenConnection(connNapt, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt);

	err = OpenConnection(connNapt1, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt1);

	//start a connection on private interface
	err = StartConnectionWithOverrides(connNapt, iNaptPrivateIap);
	TESTEL(KErrNone == err, err);
	connNapt.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	//start a connection on public interface
	err = StartConnectionWithOverrides(connNapt1, iNaptPublicIap);
	TESTEL(KErrNone == err, err);
	connNapt1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
		
	err = OpenTcpSocketExplicit(sockTestServ, socketServNapt, connNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sockTestServ);
			
	_LIT(KInterfaceName1,"eth");
	TInetAddr ethAddress;
	TName ethString;

	//get eth interface address
	TUint32 testAddr1 = GetInterfaceAddress(sockTestServ,KInterfaceName1());
	if(testAddr1 <= KErrNone)
		{
		INFO_PRINTF1(_L("Failed to get eth address"));
		return TestStepResult();
		}

	ethAddress.SetAddress(testAddr1);
	ethAddress.Output(ethString);
	
	//Concatenate eth ip address with the sring ':ethaddress\n'
	sendEthAddress = HBufC8::NewL(ethString.Length()+addressLength);
	sendEthAddress->Des().Copy(ethString); 
	sendEthAddress->Des().Append(_L8(":ethaddress\n"));
	
	//Load NAPT   
	err = LoadNapt(socketNapt,sockTestServ,socketServNapt);
	TESTEL(KErrNone == err, err);
	
	//Connect to NaptTestServer
	err = ConnectTcpSocket(sockTestServ, iNaptTestServAddr);
	TESTEL(KErrNone == err, err);
	              
	_LIT8(KTest,"test7\n");
	TBufC8 <testNameLength> tst(KTest);
	TPtrC8 testCaseName(tst);
	 
	//Need to add route in NaptTestServer.send gateway IP address to the NaptTestServer.
	TRequestStatus status;
	sockTestServ.Send(*sendEthAddress, 0, status);
	User::WaitForRequest(status);
	 err = status.Int();

	//send test case name to NaptTestServer.
	sockTestServ.Send(testCaseName, 0, status);
	User::WaitForRequest(status);
	err = status.Int();

	TSockXfrLength length;
	TBuf8<resultLength> result;
	result.Zero();

	//receive test result from NaptTestServer
	sockTestServ.RecvOneOrMore(result, 0, status,length);
	User::WaitForRequest(status);
	err = status.Int();
		
	if(err == 0)
		{
		//pass = 1,fail = 0
		if(result[0] == '1')
			{
	    	SetTestStepResult(EPass);
			}
		else if(result[0] == '0')
			{
	    	INFO_PRINTF1(_L("Test failed.Returned fail from NaptTestServer "));
	    	SetTestStepResult(EFail);
			}
		}
	delete sendEthAddress;
	
	socketNapt.Close();
	
	err = DestroyTcpSocket(sockTestServ);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();
	
	CloseConnection(connNapt);
	CleanupStack::Pop();
	
	CloseConnection(connNapt);
	CleanupStack::Pop();

	CloseSocketServer(socketServNapt);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest956	
TE_RConnectionTest957::~TE_RConnectionTest957()
{
}

enum TVerdict TE_RConnectionTest957::doTestStepL(void)
{
    SetTestStepResult(EFail);
    
	INFO_PRINTF1(_L("Send bulk data over TCP socket"));
	 
	const int resultLength   = 2; 
	const int addressLength  = 20;
	const int testNameLength = 10;

	RSocketServ socketServNapt;
	RConnection connNapt,connNapt1;
	RSocket sockTestServ,socketNapt,socketGloabal;
	HBufC8* sendEthAddress;
   
    TInt err;
	err = OpenSocketServer(socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServNapt);
	    
	err = OpenConnection(connNapt, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt);

	err = OpenConnection(connNapt1, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt1);

	//start a connection on private interface
	err = StartConnectionWithOverrides(connNapt, iNaptPrivateIap);
	TESTEL(KErrNone == err, err);
	connNapt.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	//start a connection on public interface
	err = StartConnectionWithOverrides(connNapt1, iNaptPublicIap);
	TESTEL(KErrNone == err, err);
	connNapt1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);

	err = OpenTcpSocketExplicit(sockTestServ, socketServNapt, connNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sockTestServ);

	_LIT(KInterfaceName1,"eth");
	TInetAddr ethAddress;
	TName ethString;
	
	//get eth interface address
	TUint32 testAddr1 = GetInterfaceAddress(sockTestServ,KInterfaceName1());
	if(testAddr1 <= KErrNone)
		{
		INFO_PRINTF1(_L("Failed to get eth address"));
		return TestStepResult();
		}

	ethAddress.SetAddress(testAddr1);
	ethAddress.Output(ethString);

	//Concatenate eth ip address with the sring ':ethaddress\n'
	sendEthAddress = HBufC8::NewL(ethString.Length()+addressLength);
	sendEthAddress->Des().Copy(ethString); 
	sendEthAddress->Des().Append(_L8(":ethaddress\n"));
	
	//Load NAPT   
	err = LoadNapt(socketNapt,sockTestServ,socketServNapt);
	TESTEL(KErrNone == err, err);
	
	//Connect to NaptTestServer
	err = ConnectTcpSocket(sockTestServ, iNaptTestServAddr);
	TESTEL(KErrNone == err, err);
	              
	_LIT8(KTest,"test8\n");
	TBufC8 <testNameLength> tst(KTest);
	TPtrC8 testCaseName(tst);
	
	//Need to add route in NaptTestServer.send gateway IP address to the NaptTestServer.
	TRequestStatus status;
	sockTestServ.Send(*sendEthAddress, 0, status);
	User::WaitForRequest(status);
	 err = status.Int();

	//send test case name to NaptTestServer.
	sockTestServ.Send(testCaseName, 0, status);
	User::WaitForRequest(status);
	err = status.Int();

	TSockXfrLength length;
	TBuf8<resultLength> result;
	result.Zero();

	//receive test result from NaptTestServer
	sockTestServ.RecvOneOrMore(result, 0, status,length);
	User::WaitForRequest(status);
	err = status.Int();
		
	if(err == 0)
		{
		//pass = 1,fail = 0
		if(result[0] == '1')
			{
	    	SetTestStepResult(EPass);
			}
		else if(result[0] == '0')
			{
	    	INFO_PRINTF1(_L("Test failed.Returned fail from NaptTestServer "));
	    	SetTestStepResult(EFail);
			}
		}
	delete sendEthAddress;

	socketNapt.Close();

	err = DestroyTcpSocket(sockTestServ);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();

	CloseConnection(connNapt);
	CleanupStack::Pop();

	CloseConnection(connNapt);
	CleanupStack::Pop();

	CloseSocketServer(socketServNapt);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest958	
TE_RConnectionTest958::~TE_RConnectionTest958()
{
}

enum TVerdict TE_RConnectionTest958::doTestStepL(void)
{
	SetTestStepResult(EFail);
	
	INFO_PRINTF1(_L("Open,Send and Close TCP Sockets in different order"));

	const int resultLength   = 2; 
	const int addressLength  = 20;
	const int testNameLength = 10;

	RSocketServ socketServNapt;
	RConnection connNapt,connNapt1;
	RSocket sockTestServ,socketNapt,socketGloabal;
	HBufC8* sendEthAddress;

	TInt err;
	err = OpenSocketServer(socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServNapt);

	err = OpenConnection(connNapt, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt);

	err = OpenConnection(connNapt1, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt1);

	//start a connection on private interface
	err = StartConnectionWithOverrides(connNapt, iNaptPrivateIap);
	TESTEL(KErrNone == err, err);
	connNapt.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	//start a connection on public interface
	err = StartConnectionWithOverrides(connNapt1, iNaptPublicIap);
	TESTEL(KErrNone == err, err);
	connNapt1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);

	err = OpenTcpSocketExplicit(sockTestServ, socketServNapt, connNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sockTestServ);

	_LIT(KInterfaceName1,"eth");
	TInetAddr ethAddress;
	TName ethString;

	//get eth interface address
	TUint32 testAddr1 = GetInterfaceAddress(sockTestServ,KInterfaceName1());
	if(testAddr1 <= KErrNone)
		{
		INFO_PRINTF1(_L("Failed to get eth address"));
		return TestStepResult();
		}

	ethAddress.SetAddress(testAddr1);
	ethAddress.Output(ethString);

	sendEthAddress = HBufC8::NewL(ethString.Length()+addressLength);
	sendEthAddress->Des().Copy(ethString); 
	sendEthAddress->Des().Append(_L8(":ethaddress\n"));
    
    //Load NAPT
	err = LoadNapt(socketNapt,sockTestServ,socketServNapt);
	TESTEL(KErrNone == err, err);
	
	//Connect to NaptTestServer
	err = ConnectTcpSocket(sockTestServ, iNaptTestServAddr);
	TESTEL(KErrNone == err, err);
	          
	_LIT8(KTest,"test9\n");
	TBufC8 <testNameLength> tst(KTest);
	TPtrC8 testCaseName(tst);

	//Need to add route in NaptTestServer.send gateway IP address to the NaptTestServer.
	TRequestStatus status;
	sockTestServ.Send(*sendEthAddress, 0, status);
	User::WaitForRequest(status);
	err = status.Int();

	//send test case name to NaptTestServer.
	sockTestServ.Send(testCaseName, 0, status);
	User::WaitForRequest(status);
	err = status.Int();

	TSockXfrLength length;
	TBuf8<resultLength> result;
	result.Zero();

	//receive test result from NaptTestServer.
	sockTestServ.RecvOneOrMore(result, 0, status,length);
	User::WaitForRequest(status);
	err = status.Int();
		
	if(err == 0)
		{
		//pass = 1,fail = 0
		if(result[0] == '1')
			{
	    	SetTestStepResult(EPass);
			}
		else if(result[0] == '0')
			{
	    	INFO_PRINTF1(_L("Test failed.Returned fail from NaptTestServer "));
	    	SetTestStepResult(EFail);
			}
		}
	delete sendEthAddress;
	
	socketNapt.Close();
	
	err = DestroyTcpSocket(sockTestServ);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();
			
	CloseConnection(connNapt);
	CleanupStack::Pop();
	
	CloseConnection(connNapt);
	CleanupStack::Pop();

	CloseSocketServer(socketServNapt);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest958	
TE_RConnectionTest959::~TE_RConnectionTest959()
{
}

enum TVerdict TE_RConnectionTest959::doTestStepL(void)
{
	SetTestStepResult(EFail);
	
	INFO_PRINTF1(_L("NAPT Timer Expires."));

	const int resultLength   = 2; 
	const int addressLength  = 20;
	const int testNameLength = 10;

	RSocketServ socketServNapt;
	RConnection connNapt,connNapt1;
	RSocket sockTestServ,socketNapt,socketGloabal;
	HBufC8* sendEthAddress;

	TInt err;
	err = OpenSocketServer(socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServNapt);

	err = OpenConnection(connNapt, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt);

	err = OpenConnection(connNapt1, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt1);

	//start a connection on private interface
	err = StartConnectionWithOverrides(connNapt, iNaptPrivateIap);
	TESTEL(KErrNone == err, err);
	connNapt.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	//start a connection on public interface
	err = StartConnectionWithOverrides(connNapt1, iNaptPublicIap);
	TESTEL(KErrNone == err, err);
	connNapt1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	
	err = OpenTcpSocketExplicit(sockTestServ, socketServNapt, connNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sockTestServ);

	_LIT(KInterfaceName1,"eth");
	TInetAddr ethAddress;
	TName ethString;

	//get eth interface address
	TUint32 testAddr1 = GetInterfaceAddress(sockTestServ,KInterfaceName1());
	if(testAddr1 <= KErrNone)
		{
		INFO_PRINTF1(_L("Failed to get eth address"));
		return TestStepResult();
		}

	ethAddress.SetAddress(testAddr1);
	ethAddress.Output(ethString);
	
	//Concatenate eth ip address with the sring ':ethaddress\n'
	sendEthAddress = HBufC8::NewL(ethString.Length()+addressLength);
	sendEthAddress->Des().Copy(ethString); 
	sendEthAddress->Des().Append(_L8(":ethaddress\n"));
	
	//Load NAPT
	err = LoadNapt(socketNapt,sockTestServ,socketServNapt);
	TESTEL(KErrNone == err, err);
	
	//Connect to NaptTestServer
	err = ConnectTcpSocket(sockTestServ, iNaptTestServAddr);
	TESTEL(KErrNone == err, err);
	     
	_LIT8(KTest,"test10\n");
	TBufC8 <testNameLength> tst(KTest);
	TPtrC8 testCaseName(tst);

	//Need to add route in NaptTestServer.send gateway IP address to the NaptTestServer.
	TRequestStatus status;
	sockTestServ.Send(*sendEthAddress, 0, status);
	User::WaitForRequest(status);
	err = status.Int();

	//send test case name to NaptTestServer
	sockTestServ.Send(testCaseName, 0, status);
	User::WaitForRequest(status);
	err = status.Int();

	TSockXfrLength length;
	TBuf8<resultLength> result;
	result.Zero();

	//receive test result from NaptTestServer
	sockTestServ.RecvOneOrMore(result, 0, status,length);
	User::WaitForRequest(status);
	err = status.Int();
		
	if(err == 0)
		{
		//pass = 1,fail = 0
		if(result[0] == '1')
			{
	    	SetTestStepResult(EPass);
			}
		else if(result[0] == '0')
			{
	    	INFO_PRINTF1(_L("Test failed.Returned fail from NaptTestServer "));
	    	SetTestStepResult(EFail);
			}
		}
	delete sendEthAddress;
	
	socketNapt.Close();
	
	err = DestroyTcpSocket(sockTestServ);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();
			
	CloseConnection(connNapt);
	CleanupStack::Pop();
	
	CloseConnection(connNapt);
	CleanupStack::Pop();

	CloseSocketServer(socketServNapt);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest959	
TE_RConnectionTest960::~TE_RConnectionTest960()
{
}

enum TVerdict TE_RConnectionTest960::doTestStepL(void)
{
	SetTestStepResult(EFail);
	
	INFO_PRINTF1(_L("Send data over gloabal socket and Napt socket "));
	 
	const int resultLength   = 2; 
	const int addressLength  = 20;
	const int testNameLength = 10;

	RSocketServ socketServNapt;
	RConnection connNapt,connNapt1;
	RSocket sockTestServ,socketNapt,socketGloabal;
	HBufC8* sendEthAddress;

	TInt err;
	err = OpenSocketServer(socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServNapt);
	    
	err = OpenConnection(connNapt, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt);

	err = OpenConnection(connNapt1, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt1);

	//start a connection on private interface
	err = StartConnectionWithOverrides(connNapt, iNaptPrivateIap);
	TESTEL(KErrNone == err, err);
	connNapt.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	//start a connection on public interface
	err = StartConnectionWithOverrides(connNapt1, iNaptPublicIap);
	TESTEL(KErrNone == err, err);
	connNapt1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);

	err = OpenTcpSocketExplicit(sockTestServ, socketServNapt, connNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sockTestServ);

	_LIT(KInterfaceName1,"eth");
	TInetAddr ethAddress;
	TName ethString;
	
	//get eth interface address
	TUint32 testAddr1 = GetInterfaceAddress(sockTestServ,KInterfaceName1());
	if(testAddr1 <= KErrNone)
		{
		INFO_PRINTF1(_L("Failed to get eth address"));
		return TestStepResult();
		}

	ethAddress.SetAddress(testAddr1);
	ethAddress.Output(ethString);

	sendEthAddress = HBufC8::NewL(ethString.Length()+addressLength);
	sendEthAddress->Des().Copy(ethString); 
	sendEthAddress->Des().Append(_L8(":ethaddress\n"));
	
	//Load NAPT   
	err = LoadNapt(socketNapt,sockTestServ,socketServNapt);
	TESTEL(KErrNone == err, err);
	
	//Connect to NaptTestServer
	err = ConnectTcpSocket(sockTestServ, iNaptTestServAddr);
	TESTEL(KErrNone == err, err);
	              
	_LIT8(KTest,"test11\n");
	TBufC8 <testNameLength> tst(KTest);
	TPtrC8 testCaseName(tst);
		 
	//Need to add route in NaptTestServer.send gateway IP address to the NaptTestServer.
  	TRequestStatus status;
	sockTestServ.Send(*sendEthAddress, 0, status);
	User::WaitForRequest(status);
	 err = status.Int();

	//send test case name to NaptTestServer
	sockTestServ.Send(testCaseName, 0, status);
	User::WaitForRequest(status);
	err = status.Int();

	err = OpenTcpSocketExplicit(socketGloabal, socketServNapt, connNapt1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketGloabal);
	
	err = ConnectTcpSocket(socketGloabal, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	err = TestTcpDataPathL(socketGloabal);
	TESTEL(KErrNone == err, err);

	TSockXfrLength length;
	TBuf8<resultLength> result;
	result.Zero();

	//receive test result from NaptTestServer
	sockTestServ.RecvOneOrMore(result, 0, status,length);
	User::WaitForRequest(status);
	err = status.Int();
		
	if(err == 0)
		{
		//pass = 1,fail = 0
		if(result[0] == '1')
			{
	    	SetTestStepResult(EPass);
			}
		else if(result[0] == '0')
			{
	    	INFO_PRINTF1(_L("Test failed.Returned fail from NaptTestServer "));
	    	SetTestStepResult(EFail);
			}
		}
		
	err = DestroyTcpSocket(socketGloabal);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();
	
	delete sendEthAddress;
	
	socketNapt.Close();
	
	err = DestroyTcpSocket(sockTestServ);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();
			
	CloseConnection(connNapt);
	CleanupStack::Pop();
	
	CloseConnection(connNapt);
	CleanupStack::Pop();

	CloseSocketServer(socketServNapt);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest960	

TE_RConnectionTest961::~TE_RConnectionTest961()
{
}

enum TVerdict TE_RConnectionTest961::doTestStepL(void)
{
	SetTestStepResult(EFail);
	
	INFO_PRINTF1(_L("Ping Global Interface"));

    const int addressLength = 30;
	const int resultLength 	= 2;

	RSocketServ socketServNapt;
	RConnection connNapt,connNapt1;
	RSocket sockTestServ,socketNapt,socketGloabal;
	HBufC8* sendEthAddress,*sendIPCPAddress;
	
	TInt err;
	err = OpenSocketServer(socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServNapt);
	    
	err = OpenConnection(connNapt, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt);

	err = OpenConnection(connNapt1, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt1);

	//start a connection on private interface
	err = StartConnectionWithOverrides(connNapt, iNaptPrivateIap);
	TESTEL(KErrNone == err, err);
	connNapt.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	//start a connection on public interface
	err = StartConnectionWithOverrides(connNapt1, iNaptPublicIap);
	TESTEL(KErrNone == err, err);
	connNapt1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);

	err = OpenTcpSocketExplicit(sockTestServ, socketServNapt, connNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sockTestServ);
	
	_LIT(KInterfaceName1,"eth");
	TInetAddr ethAddress;
	TName ethString;
	
	//get eth interface address
	TUint32 ethVal = GetInterfaceAddress(sockTestServ,KInterfaceName1());
	if(ethVal <= KErrNone)
		{
		INFO_PRINTF1(_L("Failed to get eth address"));
		return TestStepResult();
		}

	ethAddress.SetAddress(ethVal);
	ethAddress.Output(ethString);

	sendEthAddress = HBufC8::NewL(ethString.Length()+addressLength);
	sendEthAddress->Des().Copy(ethString); 
	sendEthAddress->Des().Append(_L8(":storeaddr\n"));
	
	_LIT(KInterfaceName2,"ipcp::comm::0");	   
	TInetAddr ipcpAddress;
	TName ipcpString;
	
	//get ipcp interface address
	TUint32	 ipcpVal = GetInterfaceAddress(sockTestServ,KInterfaceName2());

	ipcpAddress.SetAddress(ipcpVal);
	ipcpAddress.Output(ipcpString);

	sendIPCPAddress = HBufC8::NewL(ipcpString.Length()+addressLength);
	sendIPCPAddress->Des().Copy(ipcpString); 
	sendIPCPAddress->Des().Append(_L8(":pingglobal\n"));
	
	//Load NAPT
	err = LoadNapt(socketNapt,sockTestServ,socketServNapt);
	TESTEL(KErrNone == err, err);
	
	//Connect to NaptTestServer
	err = ConnectTcpSocket(sockTestServ, iNaptTestServAddr);
	TESTEL(KErrNone == err, err);
	
	//Need to add route in NaptTestServer.send gateway IP address to the NaptTestServer.
	TRequestStatus status;
	sockTestServ.Send(*sendEthAddress, 0, status);
	User::WaitForRequest(status);
	err = status.Int();
    
    //send concatenated string  to the NaptTestServer. 
	sockTestServ.Send(*sendIPCPAddress, 0, status);
	User::WaitForRequest(status);
	 err = status.Int();

	TSockXfrLength length;
	TBuf8<resultLength> result;
	result.Zero();

	//receive test result from NaptTestServer
	sockTestServ.RecvOneOrMore(result, 0, status,length);
	User::WaitForRequest(status);
	err = status.Int();

	if(err == 0)
	{
	//pass = 1,fail = 0
	if(result[0] == '1')
		{
		SetTestStepResult(EPass);
		}
	else if(result[0] == '0')
		{
		INFO_PRINTF1(_L("Test failed.Returned fail from NaptTestServer "));
		SetTestStepResult(EFail);
		}
	}
	delete sendEthAddress;
	delete sendIPCPAddress;

	socketNapt.Close();

	err = DestroyTcpSocket(sockTestServ);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();

	CloseConnection(connNapt);
	CleanupStack::Pop();

	CloseConnection(connNapt);
	CleanupStack::Pop();

	CloseSocketServer(socketServNapt);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest961

TE_RConnectionTest962::~TE_RConnectionTest962()
{
}

enum TVerdict TE_RConnectionTest962::doTestStepL(void)
{
	SetTestStepResult(EFail);
    
    INFO_PRINTF1(_L("ICMP Tests"));
     
    const int resultLength   = 2; 
    const int addressLength  = 20;
    const int testNameLength = 10;

    RSocketServ socketServNapt;
    RConnection connNapt,connNapt1;
    RSocket sockTestServ,socketNapt,socketGloabal;
    HBufC8* sendEthAddress;

    TInt err;
    err = OpenSocketServer(socketServNapt);
    TESTEL(KErrNone == err, err);
    CleanupClosePushL(socketServNapt);
        
    err = OpenConnection(connNapt, socketServNapt);
    TESTEL(KErrNone == err, err);
    CleanupClosePushL(connNapt);

    err = OpenConnection(connNapt1, socketServNapt);
    TESTEL(KErrNone == err, err);
    CleanupClosePushL(connNapt1);

    //start a connection on private interface
    err = StartConnectionWithOverrides(connNapt, iNaptPrivateIap);
    TESTEL(KErrNone == err, err);
    connNapt.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
    //start a connection on public interface
    err = StartConnectionWithOverrides(connNapt1, iNaptPublicIap);
    TESTEL(KErrNone == err, err);
    connNapt1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);

    err = OpenTcpSocketExplicit(sockTestServ, socketServNapt, connNapt);
    TESTEL(KErrNone == err, err);
    CleanupClosePushL(sockTestServ);

    _LIT(KInterfaceName1,"eth");
    TInetAddr ethAddress;
    TName ethString;
    
    //get eth interface address
    TUint32 testAddr1 = GetInterfaceAddress(sockTestServ,KInterfaceName1());
    if(testAddr1 <= KErrNone)
        {
        INFO_PRINTF1(_L("Failed to get eth address"));
        return TestStepResult();
        }

    ethAddress.SetAddress(testAddr1);
    ethAddress.Output(ethString);

    sendEthAddress = HBufC8::NewL(ethString.Length()+addressLength);
    sendEthAddress->Des().Copy(ethString); 
    sendEthAddress->Des().Append(_L8(":ethaddress\n"));
    
    //Load NAPT   
    err = LoadNapt(socketNapt,sockTestServ,socketServNapt);
    TESTEL(KErrNone == err, err);
    
    //Connect to NaptTestServer
    err = ConnectTcpSocket(sockTestServ, iNaptTestServAddr);
    TESTEL(KErrNone == err, err);
    
    TRequestStatus status;
         
    _LIT8(KTest,"test12\n");
    TBufC8 <testNameLength> tst(KTest);
    TPtrC8 testCaseName(tst);
         
    //Need to add route in NaptTestServer.send gateway IP address to the NaptTestServer.
    sockTestServ.Send(*sendEthAddress, 0, status);
    User::WaitForRequest(status);
     err = status.Int();

    //send test case name to NaptTestServer
    sockTestServ.Send(testCaseName, 0, status);
    User::WaitForRequest(status);
    err = status.Int();

    err = OpenTcpSocketExplicit(socketGloabal, socketServNapt, connNapt1);
    TESTEL(KErrNone == err, err);
    CleanupClosePushL(socketGloabal);
    
    err = ConnectTcpSocket(socketGloabal, iEchoServerAddr);
    TESTEL(KErrNone == err, err);

    TSockXfrLength length;
    TBuf8<resultLength> result;
    result.Zero();

    //receive test result from NaptTestServer
    sockTestServ.RecvOneOrMore(result, 0, status,length);
    User::WaitForRequest(status);
    err = status.Int();
        
    if(err == 0)
        {
        //pass = 1,fail = 0
        if(result[0] == '1')
            {
            SetTestStepResult(EPass);
            }
        else if(result[0] == '0')
            {
            INFO_PRINTF1(_L("Test failed.Returned fail from NaptTestServer "));
            SetTestStepResult(EFail);
            }
        }
        
    err = DestroyTcpSocket(socketGloabal);
    TESTEL(KErrNone == err, err);
    CleanupStack::Pop();
    
    delete sendEthAddress;
    
    socketNapt.Close();
    
    err = DestroyTcpSocket(sockTestServ);
    TESTEL(KErrNone == err, err);
    CleanupStack::Pop();
            
    CloseConnection(connNapt);
    CleanupStack::Pop();
    
    CloseConnection(connNapt);
    CleanupStack::Pop();

    CloseSocketServer(socketServNapt);
    CleanupStack::Pop();

    return TestStepResult();
} // TE_RConnectionTest962


TE_RConnectionTest966::~TE_RConnectionTest966()
{
}
//Napt Cap Tests
enum TVerdict TE_RConnectionTest966::doTestStepL(void)
{
	SetTestStepResult(EFail);
	
	INFO_PRINTF1(_L("Napt Capability tests"));
	
	RSocketServ socketServ;
	socketServ.Connect();
	CleanupClosePushL(socketServ);

	RSocket naptSocket;
	TInt err = naptSocket.Open(socketServ ,_L("napt"));
	CleanupClosePushL(naptSocket);
	
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("Load NAPT failed with error: %d."),err);

		}
	if(RProcess().HasCapability(ECapabilityNetworkControl))
		{
		if(err == KErrPermissionDenied)
			{
			SetTestStepResult(EFail);
			return TestStepResult();
			}
		}
		
	naptSocket.Close();
	CleanupStack::Pop();
	socketServ.Close();
	CleanupStack::Pop();
	SetTestStepResult(EPass);

	return TestStepResult();
} // TE_RConnectionTest966	

	
TE_RConnectionTest967::~TE_RConnectionTest967()
{
}
//Napt socket open test Tests
enum TVerdict TE_RConnectionTest967::doTestStepL(void)
{
	SetTestStepResult(EFail);
	
	INFO_PRINTF1(_L("Reconfigure napt"));
    
    const int resultLength   = 2; 
	const int addressLength  = 20;
	const int testNameLength = 10;
	
	RSocketServ socketServNapt;
	RConnection connNapt,connNapt1;
	RSocket sockTestServ,socketNapt,socketGloabal;
	HBufC8* sendEthAddress;
	
	TInt err;
	err = OpenSocketServer(socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServNapt);
    
	err = OpenConnection(connNapt, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt);

	err = OpenConnection(connNapt1, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt1);
	
	//start a connection on private interface
	err = StartConnectionWithOverrides(connNapt, iNaptPrivateIap);
	TESTEL(KErrNone == err, err);
	connNapt.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	//start a connection on public interface
	err = StartConnectionWithOverrides(connNapt1, iNaptPublicIap);
	TESTEL(KErrNone == err, err);
	connNapt1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	
	err = OpenTcpSocketExplicit(sockTestServ, socketServNapt, connNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sockTestServ);
			
	_LIT(KInterfaceName1,"eth");
	TInetAddr ethAddress;
	TName ethString;
	
	//get eth interface address
	TUint32 testAddr1 = GetInterfaceAddress(sockTestServ,KInterfaceName1());
	if(testAddr1 <= KErrNone)
		{
		INFO_PRINTF1(_L("Failed to get eth address"));
		return TestStepResult();
		}
	
	ethAddress.SetAddress(testAddr1);
	ethAddress.Output(ethString);
	
	//Concatenate eth ip address with the sring ':ethaddress\n'
	sendEthAddress = HBufC8::NewL(ethString.Length()+addressLength);
	sendEthAddress->Des().Copy(ethString); 
	sendEthAddress->Des().Append(_L8(":ethaddress\n"));
	
    //Connect to NaptTestServer
	err = ConnectTcpSocket(sockTestServ, iNaptTestServAddr);
    TESTEL(KErrNone == err, err);
	//Load NAPT   
    err = LoadNapt(socketNapt,sockTestServ,socketServNapt);
    TESTEL(KErrNone == err, err);
	
	//closing socket    
    socketNapt.Close();
	INFO_PRINTF1(_L("Close first socket"));
	//again loading napt
    INFO_PRINTF1(_L("Open second socket and reconfigure napt"));
    err = LoadNapt(socketNapt,sockTestServ,socketServNapt);
    TESTEL(KErrNone == err, err);

                  
	_LIT8(KTest,"test1\n");
	TBufC8 <testNameLength> tst(KTest);
	TPtrC8 testCaseName(tst);
	         
    //Need to add route in NaptTestServer.send gateway IP address to the NaptTestServer.
    TRequestStatus status;
    sockTestServ.Send(*sendEthAddress, 0, status);
	User::WaitForRequest(status);
    err = status.Int();
	
	//send test case name to NaptTestServer
	sockTestServ.Send(testCaseName, 0, status);
	User::WaitForRequest(status);
    err = status.Int();

    TSockXfrLength length;
 	TBuf8<resultLength> result;
	result.Zero();

	//receive test result from NaptTestServer
	sockTestServ.RecvOneOrMore(result, 0, status,length);
	User::WaitForRequest(status);
	err = status.Int();
	
	if(err == 0)
		{
		//pass = 1,fail = 0
		if(result[0] == '1')
			{
	    	SetTestStepResult(EPass);
			}
		else if(result[0] == '0')
			{
	    	INFO_PRINTF1(_L("Test failed.Returned fail from NaptTestServer "));
	    	SetTestStepResult(EFail);
			}
		}

	delete sendEthAddress;

	socketNapt.Close();

	err = DestroyTcpSocket(sockTestServ);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();

	CloseConnection(connNapt);
	CleanupStack::Pop();

	CloseConnection(connNapt);
	CleanupStack::Pop();

	CloseSocketServer(socketServNapt);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest967	

TE_RConnectionTest968::~TE_RConnectionTest968()
{
}

enum TVerdict TE_RConnectionTest968::doTestStepL(void)
{
	SetTestStepResult(EFail);
	
	INFO_PRINTF1(_L(" Open many TCP Sockets and Send data to Echo Server over each socket"));
	 
	const int resultLength   = 2; 
	const int addressLength  = 20;
	const int testNameLength = 10;

	RSocketServ socketServNapt;
	RConnection connNapt,connNapt1;
	RSocket sockTestServ,socketNapt,socketGloabal;
	HBufC8* sendEthAddress;

	TInt err;
	err = OpenSocketServer(socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServNapt);
	    
	err = OpenConnection(connNapt, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt);

	err = OpenConnection(connNapt1, socketServNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connNapt1);

	//start a connection on private interface
	err = StartConnectionWithOverrides(connNapt, iNaptPrivateIap);
	TESTEL(KErrNone == err, err);
	connNapt.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	//start a connection on public interface
	err = StartConnectionWithOverrides(connNapt1, iNaptPublicIap);
	TESTEL(KErrNone == err, err);
	connNapt1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
		
	err = OpenTcpSocketExplicit(sockTestServ, socketServNapt, connNapt);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sockTestServ);
			
	_LIT(KInterfaceName1,"eth");
	TInetAddr ethAddress;
	TName ethString;

	//get eth interface address
	TUint32 testAddr1 = GetInterfaceAddress(sockTestServ,KInterfaceName1());
	if(testAddr1 <= KErrNone)
		{
		INFO_PRINTF1(_L("Failed to get eth address"));
		return TestStepResult();
		}

	ethAddress.SetAddress(testAddr1);
	ethAddress.Output(ethString);
	
	//Concatenate eth ip address with the sring ':ethaddress\n'
	sendEthAddress = HBufC8::NewL(ethString.Length()+addressLength);
	sendEthAddress->Des().Copy(ethString); 
	sendEthAddress->Des().Append(_L8(":ethaddress\n"));
	
	//Load NAPT   
	err = LoadNapt(socketNapt,sockTestServ,socketServNapt);
	TESTEL(KErrNone == err, err);
	
	//Connect to NaptTestServer
	err = ConnectTcpSocket(sockTestServ, iNaptTestServAddr);
	TESTEL(KErrNone == err, err);
	              
	_LIT8(KTest,"test13\n");
	TBufC8 <testNameLength> tst(KTest);
	TPtrC8 testCaseName(tst);
	 
	//Need to add route in NaptTestServer.send gateway IP address to the NaptTestServer.
	TRequestStatus status;
	sockTestServ.Send(*sendEthAddress, 0, status);
	User::WaitForRequest(status);
	 err = status.Int();

	//send test case name to NaptTestServer.
	sockTestServ.Send(testCaseName, 0, status);
	User::WaitForRequest(status);
	err = status.Int();

	TSockXfrLength length;
	TBuf8<resultLength> result;
	result.Zero();

	//receive test result from NaptTestServer
	sockTestServ.RecvOneOrMore(result, 0, status,length);
	User::WaitForRequest(status);
	err = status.Int();
		
	if(err == 0)
		{
		//pass = 1,fail = 0
		if(result[0] == '1')
			{
	    	SetTestStepResult(EPass);
			}
		else if(result[0] == '0')
			{
	    	INFO_PRINTF1(_L("Test failed.Returned fail from NaptTestServer "));
	    	SetTestStepResult(EFail);
			}
		}
	delete sendEthAddress;
	
	socketNapt.Close();
	
	err = DestroyTcpSocket(sockTestServ);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();
	
	CloseConnection(connNapt);
	CleanupStack::Pop();
	
	CloseConnection(connNapt);
	CleanupStack::Pop();

	CloseSocketServer(socketServNapt);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest968	

