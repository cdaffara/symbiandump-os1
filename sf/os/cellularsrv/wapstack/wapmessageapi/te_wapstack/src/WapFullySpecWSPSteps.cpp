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
//

/**
 @file
 @internalComponent
*/

#include "WapFullySpecWSPSteps.h"
#include "WapStackSuiteDefs.h"
#include <simtsy.h>
#include <ecom/ecom.h>

CFullySpecWSPStepBase::CFullySpecWSPStepBase()
/**
* Constructor
*/
	{
	}

CFullySpecWSPStepBase::~CFullySpecWSPStepBase()
/**
* Destructor
*/
	{
	if(iImp) 
		{
		UnloadInterface();
		}
	}

// Load CWapBoundDatagramService Interface 
CWapFullySpecCLWSPService* CFullySpecWSPStepBase::LoadInterface()
	{
	TInt trapValue;
	CWapFullySpecCLWSPService* implementation = NULL;
	// UID {0x101FBB3E} - SWS implementation
	TUid KInterfaceUid = {0x101FBB3E}; 	
	INFO_PRINTF2(_L("Calling new NewL(0x%08X) method"), KInterfaceUid);
	TRAP(trapValue, implementation = CWapFullySpecCLWSPService::NewL(KInterfaceUid));
	if(!trapValue)
		{
		INFO_PRINTF2(_L("Interface is loaded: implementation.addr = 0x%08x"), implementation);
		return implementation;
		}
	else return NULL;
	}

void CFullySpecWSPStepBase::UnloadInterface()
	{
	INFO_PRINTF2(_L("Interface at implementation.addr = 0x%08x is deleted"), iImp);
	delete iImp;
	iImp = NULL;
	REComSession::FinalClose();	
	}

CFullySpecWSPStep_1::~CFullySpecWSPStep_1()
/**
* Destructor
*/
	{
	}

CFullySpecWSPStep_1::CFullySpecWSPStep_1()
/**
* Constructor
*/
	{
	}

TVerdict CFullySpecWSPStep_1::doTestStepL()
/**
* @return - TVerdict code
* Override of base class pure virtual
* Our implementation only gets called if the base class doTestStepPreambleL() did
* not leave. That being the case, the current test result value will be EPass.
*/
	{
	//
	// CWapFullySpecCLWSPService
	// 
	INFO_PRINTF1(_L("CreateImplementation - CWapFullySpecCLWSPService"));
	INFO_PRINTF1(_L("Calling old NewL() method"));
	
	TInt trapValue; 
	
	SetTestStepResult(EFail);
	TRAP(trapValue, iImp = CWapFullySpecCLWSPService::NewL());
	if(!trapValue)
		{
		// interface is loaded
		INFO_PRINTF2(_L("Interface is loaded: implementation.addr = 0x%08x"), iImp);
		UnloadInterface();
		iImp = LoadInterface();
		if(iImp)
			{
			INFO_PRINTF1(_L("CreateImplementation - OK"));
			SetTestStepResult(EPass);
			UnloadInterface();
			}
		}
	return TestStepResult();
	}

CFullySpecWSPStep_2::CFullySpecWSPStep_2()
/**
* Constructor
*/
	{
	}

CFullySpecWSPStep_2::~CFullySpecWSPStep_2() 
/**
* Destructor
*/
	{
	}

TVerdict CFullySpecWSPStep_2::doTestStepL()
/** 
* Test Connect methoths 
* @return - TVerdict code
* 
*/
	{
	INFO_PRINTF1(_L("FULLYSPEC-WSP: Testing Connect"));
	if(TestStepResult() == EPass )
		{
		SetTestStepResult(EFail);
		iImp = LoadInterface();
		if(iImp)
			{
			TInt bearer	= 0;
			TInt error	= KErrNone;
			TBool secure= EFalse;
			TBool reconnect= EFalse;
			SetLastError(KErrNone);
			
			if(GetIntFromConfig(ConfigSection(),KWapBearer, bearer)) iBearer = (Wap::TBearer)bearer;
			if(GetIntFromConfig(ConfigSection(), KWapExError, error)) SetExpectedError(error);
			if(GetBoolFromConfig(ConfigSection(), KWapSecure, secure)) iSecure = secure;
			if(GetBoolFromConfig(ConfigSection(),_L("Reconnect"), reconnect)) iReconnect = reconnect;
			
			if(UseSimTsy())
				{
				GetIntFromConfig(ConfigSection(), KWapSIMTSYState, iSimTsyState);
				User::LeaveIfError(RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, iSimTsyState));
				}
			User::LeaveIfError(RProperty::Set(KUidSystemCategory, KUidPhonePwr.iUid, ESAPhoneOn));

			SetState(EConnect);
			iControl->ReStart();
			ShowConnectionInfo();
			// Start state machine
			StartScheduler();
			UnloadInterface();
			}
		}
	return TestStepResult();
	}

TInt CFullySpecWSPStep_2::CallStateMachine()
	{
	TInt err = KErrNone;
	INFO_PRINTF2(_L("CallStateMachine: state [%d]"), State());
	switch(State())
		{
		case EConnect:
			INFO_PRINTF1(_L("EConnect"));
			err = iImp->Connect(iRemoteHost, iRemotePort, iBearer, iSecure);
			INFO_PRINTF2(_L("Connection completed with code = %d"), err);
			if( err == KErrNone)
				{
				if(iReconnect)
					{ 
					INFO_PRINTF1(_L("Reconnection"));
					iControl->ReStart();
					break;
					}
				}
			SetState(EFinish);
			SetLastError(err);
			iControl->ReStart();
			break;
			
		case EFinish:
			INFO_PRINTF1(_L("EFinish"));
			if(GetLastError() == GetExpectedError()) 
				{
				SetTestStepResult(EPass);
				}
			else
				{
				SetTestStepResult(EFail);
				}
			SetState(EIdle);
			StopScheduler();
			break;
			
		case EIdle:
		default:
			INFO_PRINTF2(_L("Error: EIdle or unknown state %d"), State());
			SetTestStepResult(EFail);
			StopScheduler();
			break;
		}
	return (0);
	}

CFullySpecWSPStep_3::CFullySpecWSPStep_3()
/**
* Constructor
*/
	{
	}

CFullySpecWSPStep_3::~CFullySpecWSPStep_3() 
/**
* Destructor
*/
	{
	}

TVerdict CFullySpecWSPStep_3::doTestStepL()
/** 
* Test Connect methoths 
* @return - TVerdict code
* 
*/
	{
	INFO_PRINTF1(_L("FullySpecWSP. Requests without connection"));
	// check preamble result
	TVerdict verdict = TestStepResult();
	if (verdict==EPass)
		{
		TInt err, ret;
		iImp = LoadInterface();
		if (!iImp)
			{
			ERR_PRINTF1(_L("Unable to load FullySpecWSP Interface"));
			verdict = EFail;
			}
		else
			{
			GetIntFromConfig(ConfigSection(),KWapExError,(TInt&)iExError);
			ShowConnectionInfo();
			TBuf8<100> header, body;
			TPckgBuf<TUint8> idPckg;
			TWSPStatus wspStatus;
			TRequestStatus reqStatus;
			TUint32 timeout = 0;
			ShowConnectionInfo();
			iImp->MethodResult(header, body, idPckg, wspStatus, reqStatus, timeout);
			TRAP(ret, TEST_CHECKL(reqStatus.Int(), iExError, _L("MethodResult Fails not as expected")))
				if (ret) 
					verdict = EFail;
				else
					{
					Wap::TPort port;
					err=iImp->GetLocalPort(port);
					TRAP(ret, TEST_CHECKL(err, iExError, _L("GetLocalPort Fails not as expected")))
						if (ret)
							verdict = EFail;
						else
							{
							HBufC8* localHost;					
							err=iImp->GetLocalAddress(localHost);
							TRAP(ret, TEST_CHECKL(err, iExError, _L("GetLocalAddress Fails not as expected")))
								if (ret) 
									verdict = EFail;
								else
									{
									TBuf<100> uri;
									TUint8 id = 0;
									err=iImp->MethodInvoke(0x40, uri, header, body, id);
									TRAP(ret, TEST_CHECKL(err, iExError, _L("MethodInvoke Fails not as expected")))
										if (ret) 
											verdict = EFail;
									}
							}
					}
				UnloadInterface();
			}
		}
	if (verdict==EPass)
		{
		INFO_PRINTF1(_L("FullySpecWSP. Requests Fail as expected"));
		}
	SetTestStepResult(verdict);
	return TestStepResult();
	}

TInt CFullySpecWSPStep_3::CallStateMachine()
	{
	return (0);
	}

CFullySpecWSPStep_4::CFullySpecWSPStep_4()
/**
* Constructor
*/
	{
	}

CFullySpecWSPStep_4::~CFullySpecWSPStep_4() 
/**
* Destructor
*/
	{
	}

TVerdict CFullySpecWSPStep_4::doTestStepL()
/** 
* Test Connect methoths 
* @return - TVerdict code
* 
*/
	{
	INFO_PRINTF1(_L("FULLYSPEC-WSP: UnitInvoke-UnitResult"));
	if(TestStepResult() == EPass )
		{
		SetTestStepResult(EFail);
		iImp = LoadInterface();
		if(iImp)
			{
			SetLastError(KErrNone);
			
			TInt bearer, port, repeat, method;
			TInt error = KErrNone;
			TPtrC headers;
			TPtrC body;
			TPtrC uri;
			
			if(GetIntFromConfig(ConfigSection(),KWapBearer, bearer)) iBearer = (Wap::TBearer)bearer;
			if(GetIntFromConfig(ConfigSection(),KWapLocalPort, port)) iLocalPort = (Wap::TPort)port;
			if(GetIntFromConfig(ConfigSection(),KWapExError, error)) SetExpectedError(error);
			if(GetIntFromConfig(ConfigSection(),KWapWSPMethod, method)) iMethod = method;
			if(GetIntFromConfig(ConfigSection(), KWapRepeat, repeat)) iRepeat = repeat;
			
			if(!GetStringFromConfig(ConfigSection(),KWapURI, uri)) 
				{
				User::Leave(KErrNoMemory);
				}
			GetStringFromConfig(ConfigSection(),KWapWSPHeaders, headers);
			GetStringFromConfig(ConfigSection(),KWapWSPBody, body);
			
			User::LeaveIfError(RProperty::Set(KUidSystemCategory, KUidPhonePwr.iUid, ESAPhoneOn));
			
			if(UseSimTsy())
				{
				User::LeaveIfError(RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, 41));
				}
				
			
			iURI = HBufC16::NewL(uri.Length());
			
			iSendHeaders = HBufC8::NewL(headers.Length());
			
			iSendBody = HBufC8::NewL(body.Length());
			
			iRecvHeaders = HBufC8::NewL(KMaxWapWspHeaderBufferLength);
			iRecvBody = HBufC8::NewL(KMaxWapWspDataBufferLength);

			TPtr16 tmp16 = iURI->Des();
			tmp16.Copy(uri);
			TPtr8 tmp1 = iSendHeaders->Des();
			tmp1.Copy(headers);
			TPtr8 tmp2 = iSendBody->Des();
			tmp2.Copy(body);

			iRecvHeadersBuf.Set(iRecvHeaders->Des());
			iRecvHeadersBuf.Zero();
			iRecvBodyBuf.Set(iRecvBody->Des());
			iRecvBodyBuf.Zero();
			
			SetState(EConnect);
			iControl->ReStart();
			ShowConnectionInfo();

			StartScheduler();
			
			delete iRecvBody;
			iRecvBody=NULL;
			delete iRecvHeaders;
			iRecvHeaders=NULL;

			delete iSendBody;
			iSendBody=NULL;
			delete iSendHeaders;
			iSendHeaders=NULL;
			delete iURI;
			iURI=NULL;

			UnloadInterface();
			}
		}
	return TestStepResult();
	}

TInt CFullySpecWSPStep_4::CallStateMachine()
	{
	TVerdict verdict = EFail;
	TInt err = KErrNone;
	TInt activate = 0;
	TPtr8 tmp = iSendBody->Des();
		
	static TUint total_headers = 0;		
	static TUint total_body = 0;		
	static TUint repeat = 0;		
	
	INFO_PRINTF2(_L("CallStateMachine: state [%d]"), State());

	switch(iState)
		{
		case EConnect:
			INFO_PRINTF1(_L("EConnect"));
			total_headers = 0;
			total_body = 0;
			err = iImp->Connect(iRemoteHost, iRemotePort, iBearer, EFalse);
			INFO_PRINTF2(_L("Connection completed with code = %d"), err);
			if( err == KErrNone)
				{
				SetState(EUnitInvoke);
				}
			else
				{
				SetState(EFinish);
				}
			SetLastError(err);
			INFO_PRINTF2(_L("Connection completed with code = %d"), err);
			iControl->ReStart();
			break;

		case EUnitInvoke:
			INFO_PRINTF2(_L("EUnitInvoke: Method - 0x%02x"), iMethod);
			if(iBearer != Wap::EIP)
			{
				TRAP(err, WaitForInitializeL());
			}
			if(tmp.Length() != 0)
				{
				INFO_PRINTF1(_L("Data to send:"));
				DumpBuf(tmp);
				}
			err = iImp->MethodInvoke(iMethod, iURI->Des(), iSendHeaders->Des(), iSendBody->Des(), iTransactionId);
			INFO_PRINTF3(_L("MethodInvoke() retcode: - dec: %d hex: 0x%08x"), err, err);
			if(err == KErrNone) 
				{	
				SetState(EUnitResult);
				}
			else 
				{
				SetState(EFinish);
				}
			SetLastError(err);
			iControl->ReStart();
			break;

		case EUnitResult:

			iRecvHeadersBuf.Set(iRecvHeaders->Des());
			iRecvHeadersBuf.Zero();
			iRecvBodyBuf.Set(iRecvBody->Des());
			iRecvBodyBuf.Zero();
			iImp->MethodResult(iRecvHeadersBuf, iRecvBodyBuf, iTransactionIdPckg, iWspStatus, *iStatus, iTimeout);
			INFO_PRINTF2(_L("EUnitResult: status 0x%08x"), iStatus->Int());
			activate = 1;
			SetLastError(err);
			SetState(EFinish);
			break;
			
		case EFinish:
			
			INFO_PRINTF4(_L("EFinish: Status: %d, WSP status: %d (0x%02x)"), iStatus->Int(), iWspStatus, iWspStatus);

			if(iRecvHeadersBuf.Length() != 0)
				{
				INFO_PRINTF1(_L("Headers"));
				total_headers += iRecvHeadersBuf.Length();
				DumpBuf(iRecvHeadersBuf);
				}

			if(iRecvBodyBuf.Length() != 0)
				{
				INFO_PRINTF1(_L("Body"));
				total_body += iRecvBodyBuf.Length();
				DumpBuf(iRecvBodyBuf);
				}

			if(iStatus->Int() != KErrNone) 
				{
				SetLastError(iStatus->Int());
				if(iStatus->Int() == Wap::EMoreData)
					{
					INFO_PRINTF1(_L("NOTE: More data... calling MethodResult()"));
					SetState(EUnitResult);
					iControl->ReStart();
					break;
					}
				}

			INFO_PRINTF3(_L("Data received: headers - %d bytes, body - %d bytes"), total_headers, total_body);
			total_headers = 0;
			total_body = 0;
			if((--iRepeat != 0) && (iStatus->Int() == KErrNone))
				{
				++repeat;
				INFO_PRINTF2(_L("REPEAT TEST - %d"), repeat);
				if(err == KErrNone) 
					{	
					SetState(EUnitInvoke);
					iControl->ReStart();
					break;
					}
				}

			verdict = (GetLastError() == GetExpectedError())?EPass:EFail;
			SetTestStepResult(verdict);
			SetState(EIdle);
			StopScheduler();
			break;
			
		case EIdle:
		default:
			INFO_PRINTF2(_L("Error: EIdle or unknown state %d"), State());
			SetTestStepResult(verdict);
			StopScheduler();
			break;
		}
	return (activate);
	}


CFullySpecWSPStep_5::CFullySpecWSPStep_5()
/**
* Constructor
*/
	{
	}

CFullySpecWSPStep_5::~CFullySpecWSPStep_5() 
/**
* Destructor
*/
	{
	}

TVerdict CFullySpecWSPStep_5::doTestStepL()
/** 
* Test Cancel Request 
* @return - TVerdict code
* 
*/
	{
	INFO_PRINTF1(_L("BoundCLPush. Testing Cancel Request"));
	if(TestStepResult() == EPass )
		{
		SetTestStepResult(EFail);
		iImp = LoadInterface();
		if(iImp)
			{
			TInt bearer	= 0;
			TInt error	= KErrNone;
			TBool secure= EFalse;
			TBool reconnect= EFalse;
			SetLastError(KErrNone);
			
			if(GetIntFromConfig(ConfigSection(),KWapBearer, bearer)) iBearer = (Wap::TBearer)bearer;
			if(GetIntFromConfig(ConfigSection(), KWapExError, error)) SetExpectedError(error);
			if(GetBoolFromConfig(ConfigSection(), KWapSecure, secure)) iSecure = secure;
			if(GetBoolFromConfig(ConfigSection(),_L("Reconnect"), reconnect)) iReconnect = reconnect;
			
			ShowConnectionInfo();			

			error = iImp->Connect(iRemoteHost, iRemotePort, iBearer, iSecure);
			INFO_PRINTF2(_L("Connection completed with code = %d"), error);
			if( error == KErrNone)
				{
				TBuf8<100> header, body;
				TPckgBuf<TUint8> idPckg;
				TWSPStatus wspStatus;
				TRequestStatus reqStatus;
				TUint32 timeout=0;
				iImp->MethodResult(header, body, idPckg, wspStatus, reqStatus, timeout);
				SetState(EUnitResult);
				StartScheduler();
				iControl->ReStart();
				if(reqStatus==KErrCancel)
					{
					SetTestStepResult(EPass);
					}
				
				}
			}
		}
	return TestStepResult();
	}

TInt CFullySpecWSPStep_5::CallStateMachine()
	{
	INFO_PRINTF2(_L("CallStateMachine: step [%d]"), State());
	switch(iState)
		{
		case EUnitResult:
			INFO_PRINTF1(_L("EUnitResult"));
			iImp->CancelReq();
			SetState(EFinish);
			iControl->ReStart();
			break;
			
		case EFinish:
			INFO_PRINTF1(_L("EFinish"));
			SetState(EIdle);
			StopScheduler();
			break;
			
		case EIdle:
		default:
			INFO_PRINTF2(_L("Error: EIdle or unknown state %d"), State());
			SetTestStepResult(EFail);
			StopScheduler();
			break;
		}
	return (0);
	}


CFullySpecWSPStep_6::CFullySpecWSPStep_6() 
	{
	}

CFullySpecWSPStep_6::~CFullySpecWSPStep_6()
	{
	}

TVerdict CFullySpecWSPStep_6::doTestStepL()
/*
* Test receive time out
*/
	{
	INFO_PRINTF1(_L("BOUND-WSP: Connect-UnitResult-TimeOut"));
	if(TestStepResult() == EPass )
		{
		SetTestStepResult(EFail);
		iImp = LoadInterface();
		if(iImp)
			{
			SetLastError(KErrNone);
			TInt bearer, port;
			TInt error = KErrNone;
			if(GetIntFromConfig(ConfigSection(),KWapBearer, bearer))
				{
				iBearer = static_cast<Wap::TBearer>(bearer);
				}
			if(GetIntFromConfig(ConfigSection(),KWapLocalPort, port))
				{
				iLocalPort = static_cast<Wap::TPort>(port);
				}
			if(GetIntFromConfig(ConfigSection(),KWapExError, error))
				{
				SetExpectedError(error);
				}
			TInt headerLength, bufLength, timeOut; 
			if(GetIntFromConfig(ConfigSection(),KWapWSPRecvHeaderLength, headerLength))
				{
				iHeaderRecvBufLength=static_cast<TUint16>(headerLength);
				}
			if(GetIntFromConfig(ConfigSection(),KWapWSPRecvBodyLength, bufLength))
				{
				iBodyRecvBufLength=static_cast<TUint16>(bufLength);
				}
			if(GetIntFromConfig(ConfigSection(),KWapTimeOut, timeOut))
				{
				iTimeout=timeOut;
				}
			iRecvHeaders = HBufC8::NewL(iHeaderRecvBufLength);
			iRecvBody = HBufC8::NewL(iBodyRecvBufLength);
			SetState(EConnect);
			iControl->ReStart();
			ShowConnectionInfo();						
			// Start state machine
			StartScheduler();
			delete iRecvBody;
			iRecvBody=NULL;
			delete iRecvHeaders;
			iRecvHeaders=NULL;
			UnloadInterface();
			}
		}
	return TestStepResult();
	}

TInt CFullySpecWSPStep_6::CallStateMachine()
	{
	TVerdict verdict = EFail;
	TInt err = KErrNone;
	TInt activate = 0;
	
	INFO_PRINTF2(_L("CallStateMachine: state [%d]"), State());
	switch(iState)
		{
		case EConnect:
			INFO_PRINTF1(_L("EConnect"));
			err = iImp->Connect(iRemoteHost, iRemotePort, iBearer, EFalse);
			INFO_PRINTF2(_L("Connection completed with code = %d"), err);
			if( err == KErrNone)
				{
				//				WaitForInitializeL();
				iRecvHeadersBuf.Set(iRecvHeaders->Des());
				iRecvBodyBuf.Set(iRecvBody->Des());
				iImp->MethodResult(iRecvHeadersBuf, iRecvBodyBuf, iTransactionIdPckg, iWspStatus, *iStatus, iTimeout);
				INFO_PRINTF1(_L("MethodResult"));
				activate = 1;
				}
			else
				{
				iControl->ReStart();
				}
			SetLastError(err);
			SetState(EFinish);
			break;
			
		case EFinish:
			INFO_PRINTF1(_L("EFinish"));
			if(iStatus->Int() != KErrNone) 
				{
				SetLastError(iStatus->Int());
				}
			verdict = (GetLastError() == GetExpectedError())?EPass:EFail;
			SetTestStepResult(verdict);
			SetState(EIdle);
			StopScheduler();
			break;
			
		case EIdle:
		default:
			INFO_PRINTF2(_L("Error: EIdle or unknown state %d"), State());
			SetTestStepResult(verdict);
			StopScheduler();
			break;
		}
	return (activate);
	}

CFullySpecWSPStep_7::CFullySpecWSPStep_7() 
	{
	}
CFullySpecWSPStep_7::~CFullySpecWSPStep_7()  
	{
	}
TVerdict CFullySpecWSPStep_7::doTestStepL()
	{
	INFO_PRINTF1(_L("FULLYSPEC-WSP: Connect-UnitResult-TimeOut"));
	if(TestStepResult() == EPass )
		{
		SetTestStepResult(EFail);
		iImp = LoadInterface();
		if(iImp)
			{
			SetLastError(KErrNone);
			TInt bearer, port;
			TInt error = KErrNone;
			if(GetIntFromConfig(ConfigSection(),KWapBearer, bearer))
				{
				iBearer = static_cast<Wap::TBearer>(bearer);
				}
			if(GetIntFromConfig(ConfigSection(),KWapLocalPort, port))
				{
				iLocalPort = static_cast<Wap::TPort>(port);
				}
			if(GetIntFromConfig(ConfigSection(),KWapExError, error))
				{
				SetExpectedError(error);
				}
			TInt headerLength, bufLength; 
			if(GetIntFromConfig(ConfigSection(),KWapWSPRecvHeaderLength, headerLength))
				{
				iHeaderRecvBufLength=static_cast<TUint16>(headerLength);
				}
			if(GetIntFromConfig(ConfigSection(),KWapWSPRecvBodyLength, bufLength))
				{
				iBodyRecvBufLength=static_cast<TUint16>(bufLength);
				}
			iRecvHeaders = HBufC8::NewL(iHeaderRecvBufLength);
			iRecvBody = HBufC8::NewL(iBodyRecvBufLength);
			SetState(EConnect);
			iControl->ReStart();
			ShowConnectionInfo();						

			StartScheduler();
			delete iRecvBody;
			iRecvBody=NULL;
			delete iRecvHeaders;
			iRecvHeaders=NULL;
			UnloadInterface();
			}
		}
	return TestStepResult();
	}

TInt CFullySpecWSPStep_7::CallStateMachine()
	{
	TVerdict verdict = EFail;
	TInt err = KErrNone;
	TInt activate = 0;
	TDes8 headers = iRecvHeaders->Des();
	TDes8 body = iRecvBody->Des();
	
	INFO_PRINTF2(_L("CallStateMachine: state [%d]"), State());
	switch(iState)
		{
		case EConnect:
			INFO_PRINTF1(_L("EConnect"));
			err = iImp->Connect(iRemoteHost, iRemotePort, iBearer, EFalse);
			INFO_PRINTF2(_L("Connection completed with code = %d"), err);
			if( err == KErrNone)
				{
//				WaitForInitializeL();
				TRequestStatus reqStatus;
				iImp->MethodResult(headers, body, iTransactionIdPckg, iWspStatus, reqStatus, iTimeout);
				iImp->MethodResult(headers, body, iTransactionIdPckg, iWspStatus, *iStatus, iTimeout);
				INFO_PRINTF1(_L("MethodResult"));
				activate = 1;
				}
			else
				{
				iControl->ReStart();
				}
			SetLastError(err);
			SetState(EFinish);
			break;
			
		case EFinish:
			INFO_PRINTF1(_L("EFinish"));
			if(iStatus->Int() != KErrNone) 
				{
				SetLastError(iStatus->Int());
				}
			verdict = (GetLastError() == GetExpectedError())?EPass:EFail;
			SetTestStepResult(verdict);
			SetState(EIdle);
			StopScheduler();
			break;
			
		case EIdle:
		default:
			INFO_PRINTF2(_L("Error: EIdle or unknown state %d"), State());
			SetTestStepResult(verdict);
			StopScheduler();
			
			break;
		}
	return (activate);
	}

CFullySpecWSPStep_8::CFullySpecWSPStep_8() 
	{
	}
CFullySpecWSPStep_8::~CFullySpecWSPStep_8()  
	{
	}
TVerdict CFullySpecWSPStep_8::doTestStepL()
/*
*	Using invalid values for API parameters:
*	- Invalid TUid on construction
*	- Invalid method for MethodInvoke
*	- Invalid timeout for MethodResult
*	Note: wrong bearer value has already been implemented as part of test 2
*/
	{
	INFO_PRINTF1(_L("FullySpec-WSP: Wrong parameters testing"));
	if(TestStepResult() == EPass )
		{
		TInt error;
		if(GetIntFromConfig(ConfigSection(),KWapExError2, error))
			{
			iExError2=error;
			}
		TInt trapValue;
		TUid invalidInterfaceUid = {0x00000000}; 	
		INFO_PRINTF2(_L("Calling new NewL(0x%08X) method"), invalidInterfaceUid);
		TRAP(trapValue, iImp = CWapFullySpecCLWSPService::NewL(invalidInterfaceUid));
		TVerdict verdict = (trapValue == iExError2)?EPass:EFail;
		if (verdict==EFail)
			{
			SetTestStepResult(EFail);
			return TestStepResult();
			}
		else
			{
			INFO_PRINTF2(_L("CreateImplementation with wrong Uid- Failed as expected with error %d"),trapValue);
			
			// Now use the correct Uid
			iImp = LoadInterface();
			if(iImp)
				{
				SetLastError(KErrNone);
				TInt error = KErrNone;
				TInt method;
				TPtrC headers;
				TPtrC body;
				TPtrC uri;
				
				if (GetIntFromConfig(ConfigSection(),KWapWSPMethod, method))
					{
					iMethod=method;
					}
				
				if(GetIntFromConfig(ConfigSection(), KWapExError, error))
					{
					SetExpectedError(error);
					}
				
				if(!GetStringFromConfig(ConfigSection(),KWapURI, uri) || \
					!GetStringFromConfig(ConfigSection(),KWapWSPHeaders, headers) || \
					!GetStringFromConfig(ConfigSection(),KWapWSPBody, body))
					{
					User::Leave(KErrNoMemory);
					}
				
				iURI = HBufC16::NewL(uri.Length());
				
				iSendHeaders = HBufC8::NewL(headers.Length());
				
				iSendBody = HBufC8::NewL(body.Length());
				
				TPtr16 tmp16 = iURI->Des();
				tmp16.Copy(uri);
				TPtr8 tmp = iSendHeaders->Des();
				tmp.Copy(headers);
				TPtr8 tmp1 = iSendBody->Des();
				tmp1.Copy(body);
				
				ShowConnectionInfo();						
				error = iImp->Connect(iRemoteHost, iRemotePort, iBearer, EFalse);
				INFO_PRINTF2(_L("Connection completed with code = %d"), error);
				if( error == KErrNone)
					{
					error =iImp->MethodInvoke(iMethod, iURI->Des(), iSendHeaders->Des(), iSendBody->Des(), iTransactionId); 
					}
				TVerdict verdict = (error == GetExpectedError())?EPass:EFail;
				SetTestStepResult(verdict);
				delete iSendBody;
				iSendBody=NULL;
				delete iSendHeaders;
				iSendHeaders=NULL;
				delete iURI;
				iURI=NULL;
				UnloadInterface();
				}
			}
		}
	return TestStepResult();
	}

TInt CFullySpecWSPStep_8::CallStateMachine()
	{
	return (0);
	}

CFullySpecWSPStep_9::CFullySpecWSPStep_9() {}
CFullySpecWSPStep_9::~CFullySpecWSPStep_9()  {}
TVerdict CFullySpecWSPStep_9::doTestStepL()
	{
	return TestStepResult();
	}

TInt CFullySpecWSPStep_9::CallStateMachine()
	{
	return (0);
	}
