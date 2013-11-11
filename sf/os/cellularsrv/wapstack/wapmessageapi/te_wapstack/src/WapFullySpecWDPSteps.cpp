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

#include "WapFullySpecWDPSteps.h"
#include "WapStackSuiteDefs.h"
#include <wapmsgerr.h>
#include <simtsy.h>
#include <ecom/ecom.h>

CFullySpecWDPStepBase::CFullySpecWDPStepBase()
/*
* Constructor
*/
	{
	}
CFullySpecWDPStepBase::~CFullySpecWDPStepBase()
/*
* Destructor
*/
	{
	if(iImp) 
		{
		UnloadInterface();
		}
	}

CWapFullySpecDatagramService* CFullySpecWDPStepBase::LoadInterface()
/*
* Load CWapFullySpecifiedDatagramService Interface with Interface ID
*/
	{
	CWapFullySpecDatagramService* implementation = NULL;
	TUid KInterfaceUid = {0x101FBB38};
	INFO_PRINTF2(_L("Calling new NewL(%08X) method"), KInterfaceUid);
	TRAPD(trapValue, implementation = CWapFullySpecDatagramService::NewL(KInterfaceUid));
	if(!trapValue)	
		{
		INFO_PRINTF2(_L("Interface is loaded: implementation.addr = 0x%08x"), implementation);
		return implementation;
		}
	else 
		return NULL;
	}

void CFullySpecWDPStepBase::UnloadInterface()
	{
	INFO_PRINTF2(_L("Interface at implementation.addr = 0x%08x is deleted"), iImp);
	delete iImp;
	iImp = NULL;
	REComSession::FinalClose();	
	}

CFullySpecWDPStep_1::CFullySpecWDPStep_1()
/**
* Constructor
*/
	{
	}

CFullySpecWDPStep_1::~CFullySpecWDPStep_1()
/**
* Destructor
*/
	{
	}
TVerdict CFullySpecWDPStep_1::doTestStepL()
/** 
* Load and destroy FullySpecifiedDatagtamService interface implementation
* Two different methods are used NewL() and NewL(TUid InterfaceUid)
* @return - TVerdict code
*/
	{
	// CWapFullySpecDatagramService
	INFO_PRINTF1(_L("CreateImplementation - CWapFullySpecDatagramService"));
	INFO_PRINTF1(_L("Calling old NewL() method"));
	
	TInt trapValue; 
	
	SetTestStepResult(EFail);
	TRAP(trapValue, iImp = CWapFullySpecDatagramService::NewL());
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

CFullySpecWDPStep_2::CFullySpecWDPStep_2()
/**
* Constructor
*/
	{
	}

CFullySpecWDPStep_2::~CFullySpecWDPStep_2() 
/**
* Destructor
*/
	{
	}

TVerdict CFullySpecWDPStep_2::doTestStepL()
/** 
* Test Connect methoths 
* @return - TVerdict code
* 
*/
	{
	INFO_PRINTF1(_L("FULLYSPEC-WDP: Testing Connect"));
	if(TestStepResult() == EPass )
		{
		SetTestStepResult(EFail);
		iImp = LoadInterface();
		if(iImp)
			{
			TInt bearer	= 0;
			TInt error	= KErrNone;
			TBool secure= EFalse;
			TBool reconnect = EFalse;
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
			StartScheduler();
			UnloadInterface();
			}
		}
	return TestStepResult();
	}

TInt CFullySpecWDPStep_2::CallStateMachine()
	{
	TInt err = KErrNone;
	INFO_PRINTF2(_L("CallStateMachine: state [%d]"), State());
	switch(iState)
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
			if(GetLastError() == GetExpectedError()) SetTestStepResult(EPass);
			else SetTestStepResult(EFail);
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

CFullySpecWDPStep_3::CFullySpecWDPStep_3()
/**
* Constructor
*/
	{
	SetTestStepName(KWapWDPStackSuiteLogFile);
	}
CFullySpecWDPStep_3::~CFullySpecWDPStep_3()
/**
* Destructor
*/
	{
	}
TVerdict CFullySpecWDPStep_3::doTestStepL()
/*
* Test the FullySpecWDP APIs in unconnected state.
*/
	{
	INFO_PRINTF1(_L("FULLYSPECIFIEDWDP. Requests without connection"));
	// check preamble result
	TVerdict verdict = TestStepResult();
	if (verdict==EPass)
		{
		TInt err, ret;
		iImp = LoadInterface();
		if (!iImp)
			verdict = EFail;
		else
			{
			GetIntFromConfig(ConfigSection(),KWapExError,(TInt&)iExError);
			ShowConnectionInfo();
			TBuf8<100> buf;
			err = iImp->Send(buf);
			TRAP(ret, TEST_CHECKL(err, iExError, _L("Connect Fails as expected")))
				if (ret) 
					verdict = EFail;
				else
					{
					TPckg<TUint16> aDataSizePckg(0);
					TRequestStatus reqStatus;
					iImp->AwaitRecvDataSize(aDataSizePckg, reqStatus);
					User::WaitForRequest(reqStatus);
					TRAP(ret, TEST_CHECKL(reqStatus.Int(), iExError, _L("Connect Fails as expected")))
						if (ret) 
							verdict = EFail;
						else
							{
							TBuf8<100> buf;
							TBool truncated;
							TUint32 timeout=0;
							iImp->Recv(buf, truncated,reqStatus,timeout);
							User::WaitForRequest(reqStatus);
							TRAP(ret, TEST_CHECKL(reqStatus.Int(), iExError, _L("Connect Fails as expected")))
								if (ret) 
									verdict = EFail;
								else
									{
									Wap::TPort aPort;
									err=iImp->GetLocalPort(aPort);
									TRAP(ret, TEST_CHECKL(err, iExError, _L("Connect Fails as expected")))
										if (ret) 
											verdict = EFail;
										else
											{
											HBufC8* localHost;
											err=iImp->GetLocalAddress(localHost);
											TRAP(ret, TEST_CHECKL(err, iExError, _L("Connect Fails as expected")))
												if (ret) 
													verdict = EFail;
												else
													{
													TUint16 maxSize, nominalSize;
													err=iImp->GetDatagramSizes(maxSize, nominalSize);
													if (err)
														verdict = EFail;
													}
											}
									}
							}
					}
				UnloadInterface();
			}
		}
	if (verdict==EPass)
		{
		INFO_PRINTF1(_L("FullySpecWDP. Requests Fail as expected"));
		}
	SetTestStepResult(verdict);
	return TestStepResult();
	}

TInt CFullySpecWDPStep_3::CallStateMachine()
	{
	return KErrNone;
	}

CFullySpecWDPStep_4::~CFullySpecWDPStep_4()
/**
* Destructor
*/
	{
	}

CFullySpecWDPStep_4::CFullySpecWDPStep_4()
/**
* Constructor
*/
	{
	}

TVerdict CFullySpecWDPStep_4::doTestStepL()
	{
	INFO_PRINTF1(_L("FULLYSPEC-WDP: Connect-Send-Await-Receive"));
	if(TestStepResult() == EPass )
		{
		SetTestStepResult(EFail);
		iImp = LoadInterface();
		if(iImp)
			{
			SetLastError(KErrNone);
			
			TInt bearer, port, bufLen;
			TInt error = KErrNone;
			TPtrC data;
			TPtrC16 remote_host;
			
			if(GetIntFromConfig(ConfigSection(),KWapBearer, bearer))
				{
				iBearer = static_cast<Wap::TBearer>(bearer);
				}
			if(GetStringFromConfig(ConfigSection(), KWapRemoteHost, remote_host))
				{
				iRemoteHost.Copy(remote_host);
				}
			if(GetIntFromConfig(ConfigSection(),KWapRemotePort, port))
				{
				iRemotePort = static_cast<Wap::TPort>(port);
				}
			if(GetIntFromConfig(ConfigSection(),KWapLocalPort, port))
				{
				iLocalPort = static_cast<Wap::TPort>(port);
				}
			if(GetIntFromConfig(ConfigSection(),KWapExError, error))
				{
				SetExpectedError(error);
				}
			GetIntFromConfig(ConfigSection(), KWapExError2, iExError2);
			GetBoolFromConfig(ConfigSection(), KAwaitLengthTwice, iAwaitLengthTwice);
			GetBoolFromConfig(ConfigSection(), KAwaitLengthCancel, iAwaitLengthCancel);
			GetBoolFromConfig(ConfigSection(), KMiddleCancel, iMiddleCancel);
			GetBoolFromConfig(ConfigSection(), KMultiReadCancel, iMultiReadCancel);
			if(GetIntFromConfig(ConfigSection(),KWapWdpBufLength, bufLen))
				{
				iWdpRecvBufLength=static_cast<TUint16>(bufLen);
				iRecvBuffPtr = HBufC8::NewL(iWdpRecvBufLength);
				}
			error = GetStringFromConfig(ConfigSection(),KWapData, data);
			if(!error)
				{
				User::Leave(error);
				}
			iSendBuffPtr = HBufC8::NewL(data.Length());
			TPtr8 tmp = iSendBuffPtr->Des();
			tmp.Copy(data);
			
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

TInt CFullySpecWDPStep_4::CallStateMachine()
	{
	TVerdict verdict = EFail;
	TInt err = KErrNone;
	TInt activate = 0;
	TInt leaveError = KErrNone;
	
	INFO_PRINTF2(_L("CallStateMachine: state  [%d]"), State());
	switch(iState)
		{
		case EConnect:
			INFO_PRINTF1(_L("EConnect"));
			err = iImp->Connect(iRemoteHost, iRemotePort, iBearer);
			INFO_PRINTF2(_L("Connection completed with code = %d"), err);
			if( err == KErrNone)
				{
				TRAP(leaveError, WaitForInitializeL());
				err = iImp->Send(iSendBuffPtr->Des());
				if(err == KErrNone && leaveError == KErrNone) 
					{	
					INFO_PRINTF2(_L("Send completetd with code = %d"), err);
					SetState(EAwaitSize);
					}
				else 
					{	
					SetState(EFinish);
					}
				}
			else 
				{
				SetState(EFinish);
				}
			SetLastError(err);
			iControl->ReStart();
			break;
			
		case EAwaitSize:
			INFO_PRINTF1(_L("EAwaitSize"));
			iImp->AwaitRecvDataSize(iLength, *iStatus);
			activate = 1;
			if (!iAwaitLengthCancel)
				{
				SetState(EReceive);
				}
			else
				{
				INFO_PRINTF1(_L("Cancel Receive in AwaitSize state"));
				iImp->CancelRecv();
				SetState(EFinish);
				}
			
			break;
			
		case EReceive:
			INFO_PRINTF1(_L("EReceive"));
			if(iStatus->Int()!=KErrNone) 
				// AwaitSize failed
				{
				INFO_PRINTF1(_L("AwaitSize failed"));
				SetLastError(iStatus->Int());
				iControl->ReStart();
				SetState(EFinish);
				}
			else
				{	
				INFO_PRINTF2(_L("AwaitSize returned - %d"),iLength());
				if (!iAwaitLengthTwice)
					{
					if (iRecvBuffPtr==NULL)
						{
						TInt len=iLength();
						TRAP(err, iRecvBuffPtr = HBufC8::NewL(len))
							if (err!=KErrNone)
								{
								SetLastError(err);
								iControl->ReStart();
								SetState(EFinish);
								break;
								}
						}
					iRecvBuf.Set(iRecvBuffPtr->Des());
					iImp->Recv(iRecvBuf, iTruncated, *iStatus, 0);
					activate = 1;
					if (!iMiddleCancel)
						{
						SetState(EMutipleReceive);
						}
					else
						{
						INFO_PRINTF1(_L("Cancel Receive in Receive state"));
						iImp->CancelRecv();
						SetState(EFinish);
						}
					}
				else
					{
					iImp->AwaitRecvDataSize(iLength, *iStatus);
					activate = 1;
					SetState(EFinish);
					}
				}
			break;
			
		case EMutipleReceive:
			INFO_PRINTF1(_L("EMultipleReceive"));
			if (iStatus->Int()!=KErrNone && iStatus->Int()!=Wap::EMoreData)
				{
				INFO_PRINTF1(_L("Receive failed"));
				SetLastError(iStatus->Int());
				iControl->ReStart();
				SetState(EFinish);
				}
			else
				{
				iMaxWdpRecvBuffer.Append(iRecvBuf);
				if (iTruncated)
					{
					iImp->Recv(iRecvBuf, iTruncated, *iStatus, 0);
					activate = 1;
					if (!iMultiReadCancel)
						{
						SetState(EMutipleReceive);
						}
					else
						{
						INFO_PRINTF1(_L("Cancel Receive in EMutipleReceive state"));
						iImp->CancelRecv();
						SetState(EFinish);
						}					
					}
				else
					{
					SetLastError(iStatus->Int());
					iControl->ReStart();
					SetState(EFinish);
					}
				}	
			break;
			
		case EFinish:
			INFO_PRINTF1(_L("EFinish"));
			if(iStatus->Int() != KErrNone) 
				{
				SetLastError(iStatus->Int());
				}
			verdict = (GetLastError() == GetExpectedError())?EPass:EFail;
			if (verdict==EPass && !iAwaitLengthTwice && !iMiddleCancel && !iMultiReadCancel && !iAwaitLengthCancel)
				{
				iImp->CancelRecv();
				INFO_PRINTF1(_L("Data received"));
				DumpBuf(iMaxWdpRecvBuffer);
				if (verdict==EPass)
					{
					verdict = (iSendBuffPtr->Match(iMaxWdpRecvBuffer)!=KErrNotFound)?EPass:EFail;
					}
				if (verdict==EPass)
					{
					Wap::TPort port;
					iImp->GetLocalPort(port);
					verdict = (port == iLocalPort)?EPass:EFail;
					}
				if (verdict==EPass)
					{
					//This test can not perform to get own telnumber
					HBufC8* buf;
					iImp->GetLocalAddress(buf);
					delete buf;
					}
				}
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
	
	CFullySpecWDPStep_5::~CFullySpecWDPStep_5()
	/**
	* Destructor
	*/
		{
		}
	
	CFullySpecWDPStep_5::CFullySpecWDPStep_5()
	/**
	* Constructor
	*/
		{
		}
	
	TVerdict CFullySpecWDPStep_5::doTestStepL()
	/** 
	*	Test Cancel
	*/
		{
		INFO_PRINTF1(_L("Fully Specified-WDP: Testing Cancel"));
		
		if(TestStepResult() == EPass )
			{
			SetTestStepResult(EFail);
			iImp = LoadInterface();
			if(iImp)
				{	
				TInt bearer	= 0;
				TInt error	= KErrNone;
				TBool secure= EFalse;
				TBool reconnect = EFalse;
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
					TPckg<TUint16> aDataSizePckg(0);
					TRequestStatus reqStatus;
					iImp->AwaitRecvDataSize(aDataSizePckg, reqStatus);
					SetState(EAwaitSize);
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

TInt CFullySpecWDPStep_5::CallStateMachine()
	{
	INFO_PRINTF2(_L("CallStateMachine: step [%d]"), State());
	switch(iState)
		{
		case EAwaitSize:
			INFO_PRINTF1(_L("EAwaitSize"));
			iImp->CancelRecv();
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
			INFO_PRINTF2(_L("Error: EIdle or unknow state %d"), State());
			SetTestStepResult(EFail);
			StopScheduler();
			break;
		}
	return 0;
	}
	
CFullySpecWDPStep_6::~CFullySpecWDPStep_6() 
	{
	}

CFullySpecWDPStep_6::CFullySpecWDPStep_6()
	{
	}

TVerdict CFullySpecWDPStep_6::doTestStepL()
	{
	//Test timeout (Not in use)
	return TestStepResult();
	}

TInt CFullySpecWDPStep_6::CallStateMachine()
	{
	return KErrNone;
	}

CFullySpecWDPStep_7::~CFullySpecWDPStep_7()
	{
	}

CFullySpecWDPStep_7::CFullySpecWDPStep_7()
	{
	}

TVerdict CFullySpecWDPStep_7::doTestStepL()
	//
	//Test wrong sequenece
	//
	{
	INFO_PRINTF1(_L("FULLYSPEC-WDP: Connect-Receive-Await-Await"));
	if(TestStepResult() == EPass )
		{
		SetTestStepResult(EFail);
		iImp = LoadInterface();
		if(iImp)
			{
			SetLastError(KErrNone);
			
			TInt bearer, port, bufLen;
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
			if(GetIntFromConfig(ConfigSection(),KWapWdpBufLength, bufLen))
				{
				iWdpRecvBufLength=static_cast<TUint16>(bufLen);
				}
			iRecvBuffPtr = HBufC8::NewL(iWdpRecvBufLength);
			SetState(EConnect);
			iControl->ReStart();
			ShowConnectionInfo();

			StartScheduler();
			UnloadInterface();
			}
		}
	return TestStepResult();
	}

TInt CFullySpecWDPStep_7::CallStateMachine()
	{
	TVerdict verdict = EFail;
	TInt err = KErrNone;
	TInt activate = 0;
	
	INFO_PRINTF2(_L("CallStateMachine: state  [%d]"), State());
	switch(iState)
		{
		case EConnect:
			INFO_PRINTF1(_L("EConnect"));
			err = iImp->Connect(iRemoteHost, iRemotePort, iBearer);
			INFO_PRINTF2(_L("Connection completed with code = %d"), err);
			if( err == KErrNone)
				{
//				WaitForInitializeL();
				if(iRecvBuffPtr == NULL)
					{
					err=KErrNoMemory;
					}
				else
					{
					iRecvBuf.Set(iRecvBuffPtr->Des());
					iImp->Recv(iRecvBuf, iTruncated, *iStatus, iTimeout);
					SetState(EReceive);
					activate = 1;
					}
				}
			SetLastError(err);
			if (err!=KErrNone)
				{
				SetState(EFinish);
				iControl->ReStart();
				}
			break;
			
		case EReceive:
			INFO_PRINTF1(_L("EReceive"));
			SetLastError(iStatus->Int());
			if(iStatus->Int() != KErrNone) 
				// Receiving fails as expect
				{
				verdict = (GetLastError() == GetExpectedError())?EPass:EFail;
				if (verdict==EPass)
					{
					INFO_PRINTF1(_L("Receiving failed as expect"));
					TRequestStatus reqStatus;
					iImp->AwaitRecvDataSize(iLength, reqStatus);
					iImp->AwaitRecvDataSize(iLength, *iStatus);
					activate = 1;
					SetState(EAwaitSize);
					}
				else
					{
					INFO_PRINTF1(_L("Receiving failed not as expect"));
					iControl->ReStart();
					SetState(EFinish);
					}
				}
			else
				{	
				INFO_PRINTF1(_L("Receiving succeed not as expect"));
				iControl->ReStart();
				SetState(EFinish);
				}
			break;
			
		case EAwaitSize:
			INFO_PRINTF1(_L("EAwaitSize"));
			SetLastError(iStatus->Int());
			iImp->CancelRecv();
			SetState(EFinish);
			iControl->ReStart();
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

CFullySpecWDPStep_8::~CFullySpecWDPStep_8()
	{
	}
CFullySpecWDPStep_8::CFullySpecWDPStep_8() 
	{
	}

TVerdict CFullySpecWDPStep_8::doTestStepL()
/*
*	Using invalid values for API parameters:
*	- Invalid TUid on construction
*	- Setting invalid timeout for Recv
*	Note: wrong bearer value has already been implemented as part of test 2
*/
	{
	INFO_PRINTF1(_L("FullySpec-WDP: Wrong parameter values"));
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
		TRAP(trapValue, iImp = CWapFullySpecDatagramService::NewL(invalidInterfaceUid));
		TVerdict verdict = (trapValue == iExError2)?EPass:EFail;
		if (verdict==EPass)
			{
			INFO_PRINTF2(_L("CreateImplementation with wrong Uid- Failed as expected with error %d"),trapValue);
			}
		SetTestStepResult(verdict);
		}
	return TestStepResult();
	}

TInt CFullySpecWDPStep_8::CallStateMachine()
	{
	return KErrNone;
	}

CFullySpecWDPStep_9::~CFullySpecWDPStep_9() {}
CFullySpecWDPStep_9::CFullySpecWDPStep_9() {}
TVerdict CFullySpecWDPStep_9::doTestStepL()
	{
	return TestStepResult();
	}
TInt CFullySpecWDPStep_9::CallStateMachine()
	{
	return KErrNone;
	}

