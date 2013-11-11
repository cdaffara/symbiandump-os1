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
// CStepOne derived implementation
// 
//

/**
 @file
 @internalComponent 
*/

#include <e32base.h>
#include <sacls.h>

#include "WapBoundWDPSteps.h"
#include "WapStackSuiteDefs.h"
#include <etel.h>
#include <es_wsms.h>
#include <simtsy.h>
#include <wapmsgerr.h>
#include <ecom/ecom.h>

CBoundWDPStepBase::CBoundWDPStepBase()
/**
* Constructor
*/
	{
	}

CBoundWDPStepBase::~CBoundWDPStepBase()
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
CWapBoundDatagramService* CBoundWDPStepBase::LoadInterface()
	{
	TInt trapValue;
	CWapBoundDatagramService* implementation = NULL;
	// UID {0x101FBB37} - SWS implementation
	TUid KInterfaceUid = {0x101FBB37}; 	
	INFO_PRINTF2(_L("Calling new NewL(0x%08X) method"), KInterfaceUid);
	TRAP(trapValue, implementation = CWapBoundDatagramService::NewL(KInterfaceUid));
	if(!trapValue)
		{
		INFO_PRINTF2(_L("Interface is loaded: implementation.addr = 0x%08x"), implementation);
		return implementation;
		}
	else return NULL;
	}

void CBoundWDPStepBase::UnloadInterface()
	{
	INFO_PRINTF2(_L("Interface at implementation.addr = 0x%08x is deleted"), iImp);
	delete iImp;
	iImp = NULL;
	REComSession::FinalClose();	
	}

CBoundWDPStep_1::CBoundWDPStep_1()
/**
* Constructor
*/
	{
	}

CBoundWDPStep_1::~CBoundWDPStep_1() 
/**
* Destructor
*/
	{
	}

TVerdict CBoundWDPStep_1::doTestStepL()
/** 
* Load and destroy BoundDatagtamService interface implementation
* Two different methods are used NewL() and NewL(TUid InterfaceUid)
* @return - TVerdict code
*/
	{
	// CWapBoundDatagramService
	INFO_PRINTF1(_L("CreateImplementation - CWapBoundDatagramService"));
	INFO_PRINTF1(_L("Calling old NewL() method"));
	
	SetTestStepResult(EFail);

	TInt trapValue(0); 
	TRAP(trapValue, iImp = CWapBoundDatagramService::NewL());
	if(!trapValue)
		{
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

CBoundWDPStep_2::CBoundWDPStep_2()
/**
* Constructor
*/
	{
	}

CBoundWDPStep_2::~CBoundWDPStep_2()
/**
* Destructor
*/
	{
	}

TVerdict CBoundWDPStep_2::doTestStepL()
/**
* Test Connect methoths
* @return - TVerdict code
*
*/
	{
	INFO_PRINTF1(_L("BOUND-WDP: Connect"));
	if(TestStepResult() == EPass )
		{
		SetTestStepResult(EFail);
		iImp = LoadInterface();
		if(iImp)
			{
			TInt bearer, port;
			TInt error = KErrNone;
			TBool reconnect = EFalse;
			TBool delay = EFalse;

			SetLastError(KErrNone);
			if(GetIntFromConfig(ConfigSection(),KWapBearer, bearer)) iBearer = (Wap::TBearer)bearer;
			if(GetIntFromConfig(ConfigSection(),KWapLocalPort, port)) iLocalPort = (Wap::TPort)port;
			if(GetIntFromConfig(ConfigSection(),KWapExError, error)) SetExpectedError(error);
			if(GetBoolFromConfig(ConfigSection(),KWapReconnect, reconnect)) iReconnect = reconnect;
			if(GetBoolFromConfig(ConfigSection(),KWapDelayPortClosure, delay)) iDelayPortClosure = delay;
			
			TInt checkValue = 0;
			if(UseSimTsy())
				{
				GetIntFromConfig(ConfigSection(), KWapSIMTSYState, iSimTsyState);
				User::LeaveIfError(RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, iSimTsyState));
				// double check that base did set it since this is the first place we try setting it in the harness
				RProperty::Get(KUidPSSimTsyCategory, KPSSimTsyTestNumber, checkValue);
				__ASSERT_ALWAYS(checkValue == iSimTsyState,User::Panic(KWapStackSuitePanic, EDataCorrupt));
				} 
			
			User::LeaveIfError(RProperty::Set(KUidSystemCategory, KUidPhonePwr.iUid, ESAPhoneOn));
			// double check
			RProperty::Get(KUidSystemCategory, KUidPhonePwr.iUid, checkValue);
			__ASSERT_ALWAYS(checkValue == ESAPhoneOn,User::Panic(KWapStackSuitePanic, EDataCorrupt));

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

TInt CBoundWDPStep_2::CallStateMachine()
	{
	TInt err = KErrNone;
	INFO_PRINTF2(_L("CallStateMachine: step [%d]"), State());
	switch(iState)
		{
		case EConnect:
			INFO_PRINTF1(_L("EConnect"));
			err = iImp->Connect(iBearer, iLocalPort);
			INFO_PRINTF2(_L("Connection completed with code = %d"), err);
			if( err == KErrNone)
				{
				if(iReconnect)
					{
					INFO_PRINTF1(_L("Second connect"));
					iControl->ReStart();
					break;
					}
				}
			SetState(EFinish);
			SetLastError(err);
			iControl->ReStart();
			break;

		case EFinish:
			// it's necessary to delay port closure, let the other concurrent test step
			// can fail opening the same port with expected error EPortAlreadyBound (-5498)
			if (iDelayPortClosure)
				User::After(5000000);

			INFO_PRINTF1(_L("EFinish"));
			if(GetLastError() == GetExpectedError()) SetTestStepResult(EPass);
			else SetTestStepResult(EFail);
			SetState(EIdle);
			// End of state machine
			StopScheduler();
			break;

		case EIdle:
		default:
			INFO_PRINTF2(_L("Error: bad state EIdle or unknown [%d]"), State());
			SetTestStepResult(EFail);
			// End of state machine
			StopScheduler();
			break;
		}
	return (0);
	}


CBoundWDPStep_3::CBoundWDPStep_3() 
/**
* Constructor
*/
	{
	}

CBoundWDPStep_3::~CBoundWDPStep_3() 
/**
* Destructor
*/
	{
	}

TVerdict CBoundWDPStep_3::doTestStepL()
/*
* Test the CBoundWDP APIs in unconnected state.
*/
	{
	INFO_PRINTF1(_L("BoundWDP. Requests without connection"));
	// check preamble result
	TVerdict verdict = TestStepResult();
	if (verdict==EPass)
		{
		TInt err, ret;
		iImp = LoadInterface();
		if (!iImp)
			{
			ERR_PRINTF1(_L("Unable to load BoundWDP Interface"));
			verdict = EFail;
			}
		else
			{
			GetIntFromConfig(ConfigSection(),KWapExError,(TInt&)iExError);
			ShowConnectionInfo();
			TBuf8<100> buf;
			err = iImp->SendTo(iRemoteHost, iRemotePort, buf, iBearer);
			TRAP(ret, TEST_CHECKL(err, iExError, _L("SendTo Fails not as expected")))
			if (ret) 
				{
				verdict = EFail;
				}
			else
				{
				TPckg<TUint16> aDataSizePckg(0);
				TRequestStatus reqStatus;
				iImp->AwaitRecvDataSize(aDataSizePckg, reqStatus);
				User::WaitForRequest(reqStatus);
				TRAP(ret, TEST_CHECKL(reqStatus.Int(), iExError, _L("AwaitRecvDataSize Fails not as expected")))
				if (ret) 
					{
					verdict = EFail;
					}
				else
					{
					TBuf8<100> buf;
					TBool truncated;
					TUint32 timeout=0;
					iImp->RecvFrom(iRecvRemoteHost, iRecvRemotePort, buf, truncated, reqStatus, timeout);
					User::WaitForRequest(reqStatus);
					TRAP(ret, TEST_CHECKL(reqStatus.Int(), iExError, _L("RecvFrom Fails not as expected")))
					if (ret) 
						{
						verdict = EFail;
						}
					else
						{
						Wap::TPort aPort;
						err=iImp->GetLocalPort(aPort);
						TRAP(ret, TEST_CHECKL(err, iExError, _L("GetLocalPort Fails not as expected")))
						if (ret) 
							{
							verdict = EFail;
							}
						else
							{
							HBufC8* localHost;
							err=iImp->GetLocalAddress(localHost);
							TRAP(ret, TEST_CHECKL(err, iExError, _L("GetLocalAddress Fails not as expected")))
							if (ret) 
								{
								verdict = EFail;
								}
							else
								{
								TUint16 maxSize, nominalSize;
								err=iImp->GetDatagramSizes(maxSize, nominalSize);
								if (err)
									{
									verdict = EFail;
									}
								else
									{
									err=iImp->GetBearer(iBearer);
									TRAP(ret, TEST_CHECKL(err, iExError, _L("GetBearer Fails not as expected")))
									if (ret)
										{
										verdict = EFail;
										}
									}
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
		INFO_PRINTF1(_L("BoundWDP. Requests Fail as expected"));
		}
	SetTestStepResult(verdict);
	return TestStepResult();
	}

TInt CBoundWDPStep_3::CallStateMachine()
	{
	return (0);
	}


CBoundWDPStep_4::CBoundWDPStep_4() 
/**
* Constructor
*/
	{
	}

CBoundWDPStep_4::~CBoundWDPStep_4() 
/**
* Destructor
*/
	{
	}

TVerdict CBoundWDPStep_4::doTestStepL()
/** 
*/
	{
	INFO_PRINTF1(_L("BOUND-WDP: Connect-Send-Await-Receive"));
	if(TestStepResult() == EPass )
		{
		SetTestStepResult(EFail);
		iImp = LoadInterface();
		if(iImp)
			{
			SetLastError(KErrNone);

			TInt bearer, port, bufLen;
			TInt error = KErrNone;
			TPtrC data, remote_host;
			
			if(GetIntFromConfig(ConfigSection(),KWapBearer, bearer))
				{
				iBearer = static_cast<Wap::TBearer>(bearer);
				}
			if(GetIntFromConfig(ConfigSection(),KWapLocalPort, port))
				{
				iLocalPort = static_cast<Wap::TPort>(port);
				}
			if(GetStringFromConfig(ConfigSection(), KWapRemoteHost, remote_host))
				{
				iRemoteHost.Copy(remote_host);
				}
			if(GetIntFromConfig(ConfigSection(),KWapRemotePort, port))
				{
				iRemotePort = static_cast<Wap::TPort>(port);
				}
			if(GetIntFromConfig(ConfigSection(),KWapExError, error))
				{
				SetExpectedError(error);
				}
			GetIntFromConfig(ConfigSection(), KWapExError2, iExError2);
			GetBoolFromConfig(ConfigSection(), KAwaitLengthCancel, iAwaitLengthCancel);
			GetBoolFromConfig(ConfigSection(), KMiddleCancel, iMiddleCancel);
			GetBoolFromConfig(ConfigSection(), KMultiReadCancel, iMultiReadCancel);
			GetBoolFromConfig(ConfigSection(), KAwaitLengthTwice, iAwaitLengthTwice);
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

TInt CBoundWDPStep_4::CallStateMachine()
	{
	TVerdict verdict = EFail;
	TInt err = KErrNone;
	TInt activate = 0;
	
	INFO_PRINTF2(_L("CallStateMachine: state [%d]"), State());
	switch(iState)
		{
		case EConnect:
			INFO_PRINTF1(_L("EConnect"));
			err = iImp->Connect(iBearer, iLocalPort);
			INFO_PRINTF2(_L("Connection completed with code = %d"), err);
			if( err == KErrNone)
				{
					if(iBearer != Wap::EIP)
						{
						TRAP(err, WaitForInitializeL());
						}
					if(iSendBuffPtr->Length())
						{
						INFO_PRINTF1(_L("Data to send"));
						DumpBuf(iSendBuffPtr->Des());
						}
				err = iImp->SendTo(iRemoteHost, iRemotePort, iSendBuffPtr->Des(), iBearer);
				INFO_PRINTF2(_L("SendTo completed with code = %d"), err);
				if(err == KErrNone) 
					{	
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
					iImp->RecvFrom(iRecvRemoteHost, iRecvRemotePort, iRecvBuf, iTruncated, *iStatus, 0);
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
					iImp->RecvFrom(iRecvRemoteHost, iRecvRemotePort, iRecvBuf, iTruncated, *iStatus, 0);
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
			if(iMaxWdpRecvBuffer.Length())
				{
				INFO_PRINTF1(_L("Data received:"));
				DumpBuf(iMaxWdpRecvBuffer);
				}
			verdict = (GetLastError() == GetExpectedError())?EPass:EFail;
			if (verdict==EPass && !iMiddleCancel && !iMultiReadCancel && !iAwaitLengthCancel && !iAwaitLengthTwice)
				{
				iImp->CancelRecv();
				if (verdict==EPass)
					{
					verdict = (iSendBuffPtr->Match(iMaxWdpRecvBuffer)!=KErrNotFound)?EPass:EFail;
					}
				if (verdict==EPass)
					{
					verdict = (iRemoteHost.Match(iRecvRemoteHost)!=KErrNotFound)?EPass:EFail;
					}
				if (verdict==EPass)
					{
					Wap::TPort port;
					iImp->GetLocalPort(port);
					verdict = (port == iLocalPort)?EPass:EFail;
					}
				if (verdict==EPass)
					{
					Wap::TBearer bearer;
					iImp->GetBearer(bearer);
					verdict = (bearer == iBearer)?EPass:EFail;
					}
				if (verdict==EPass)
					{
					//This test can not perform in SIMTSY.
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
			INFO_PRINTF1(_L("EIdle or unknown"));
			INFO_PRINTF2(_L("Error: bad state %d, State = %d"), State());
			SetTestStepResult(verdict);
			StopScheduler();
			
			break;
		}
		return (activate);
	}
	
CBoundWDPStep_5::CBoundWDPStep_5() 
/**
* Constructor
*/
	{
	}
	
CBoundWDPStep_5::~CBoundWDPStep_5() 
/**
* Destructor
*/
	{
	}
	
TVerdict CBoundWDPStep_5::doTestStepL()
/** 
*	Test Cancel
*/
	{
	INFO_PRINTF1(_L("BOUND-WDP: Testing Cancel"));
	
	if(TestStepResult() == EPass )
		{
		SetTestStepResult(EFail);
		iImp = LoadInterface();
		if(iImp)
			{	
			TInt bearer, port;
			TInt error	= KErrNone;
			TBool secure= EFalse;
			TBool reconnect= EFalse;
			SetLastError(KErrNone);
				
			if(GetIntFromConfig(ConfigSection(),KWapBearer, bearer)) iBearer = (Wap::TBearer)bearer;
			if(GetIntFromConfig(ConfigSection(),KWapLocalPort, port)) iLocalPort = (Wap::TPort)port;
			if(GetIntFromConfig(ConfigSection(), KWapExError, error)) SetExpectedError(error);
			if(GetBoolFromConfig(ConfigSection(), KWapSecure, secure)) iSecure = secure;
			if(GetBoolFromConfig(ConfigSection(),_L("Reconnect"), reconnect)) iReconnect = reconnect;
				
			error = iImp->Connect(iBearer, (Wap::TPort)iLocalPort, iSecure);
			INFO_PRINTF2(_L("Connection completed with code = %d"), error);
			if( error == KErrNone)
				{
				TPckg<TUint16> aDataSizePckg(0);
				TRequestStatus reqStatus;
				iImp->AwaitRecvDataSize(aDataSizePckg, reqStatus);
				SetState(EAwaitSize);
				ShowConnectionInfo();

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
	
TInt CBoundWDPStep_5::CallStateMachine()
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
			INFO_PRINTF2(_L("Error: EIdle or unknown state %d"), State());
			SetTestStepResult(EFail);
			StopScheduler();
			break;
		}
	return 0;
	}
	
CBoundWDPStep_6::CBoundWDPStep_6()
	{
	}
	
CBoundWDPStep_6::~CBoundWDPStep_6() 
	{
	}

TVerdict CBoundWDPStep_6::doTestStepL()
//
//Test the CBoundWDP APIs Time out.
//
	{
	//Not in use
	return TestStepResult();
	}
	
TInt CBoundWDPStep_6::CallStateMachine()
	{
	return (0);
	}
	
CBoundWDPStep_7::CBoundWDPStep_7()
	{
	}

CBoundWDPStep_7::~CBoundWDPStep_7()
	{
	}

TVerdict CBoundWDPStep_7::doTestStepL()
//
// Test wrong sequenece
//
	{
	INFO_PRINTF1(_L("BOUND-WDP: Connect-Receive-Await-Await"));
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
			// Start state machine
			StartScheduler();
			UnloadInterface();
			}
		}
	return TestStepResult();
	}
	
TInt CBoundWDPStep_7::CallStateMachine()
	{
	TVerdict verdict = EFail;
	TInt err = KErrNone;
	TInt activate = 0;
		
	INFO_PRINTF2(_L("CallStateMachine: state [%d]"), State());
	switch(iState)
		{
		case EConnect:
			INFO_PRINTF1(_L("EConnect"));
			err = iImp->Connect(iBearer, iLocalPort);
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
					iImp->RecvFrom(iRecvRemoteHost, iRecvRemotePort, iRecvBuf, iTruncated, *iStatus, iTimeout);
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
			INFO_PRINTF1(_L("EIdle or unknown"));
			INFO_PRINTF2(_L("Error: bad state %d, State = %d"), State());
			SetTestStepResult(verdict);
			StopScheduler();
			break;
		}
	return (activate);
	}
	
CBoundWDPStep_8::CBoundWDPStep_8() 
	{
	}

CBoundWDPStep_8::~CBoundWDPStep_8() 
	{
	}

TVerdict CBoundWDPStep_8::doTestStepL()
/*
*	Using invalid values for API parameters:
*	- Invalid TUid on construction
*	- Setting invalid timeout for RecvFrom
*	Note: wrong bearer value has already been implemented as part of test 2
*/
	{
	INFO_PRINTF1(_L("BOUND-WDP: Wrong parameter values"));
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
		TRAP(trapValue, iImp = CWapBoundDatagramService::NewL(invalidInterfaceUid));
		TVerdict verdict = (trapValue == iExError2)?EPass:EFail;	
		if (verdict==EPass)
			{
			INFO_PRINTF2(_L("CreateImplementation with wrong Uid- Failed as expected with error %d"),trapValue);
			}
		SetTestStepResult(verdict);
		}
	return TestStepResult();
	}
	
TInt CBoundWDPStep_8::CallStateMachine()
	{
	return 0;
	}
	
CBoundWDPStep_9::CBoundWDPStep_9() 
	{
	}

CBoundWDPStep_9::~CBoundWDPStep_9() 
	{
	}

TVerdict CBoundWDPStep_9::doTestStepL()
	{
	return TestStepResult();
	}
	
TInt CBoundWDPStep_9::CallStateMachine()
	{
	return 0;
	}
	
CBoundWDPStep_OutOfMemory::CBoundWDPStep_OutOfMemory() 
	{
	}

CBoundWDPStep_OutOfMemory::~CBoundWDPStep_OutOfMemory() 
	{
	}

TVerdict CBoundWDPStep_OutOfMemory::doTestStepL()
	{
	// to test fix for DEF117706: Memory leak in CWapBoundDatagramService::Connect under OOM
	__UHEAP_MARK;

	for(TInt i=1;; i++)
		{
		__UHEAP_FAILNEXT(i);

		CWapBoundDatagramService* datServ = NULL;
		TRAP_IGNORE(datServ = CWapBoundDatagramService::NewL());
		if(datServ)
			{
			datServ->Connect(Wap::EAll, 7576);
			delete datServ;
			}		 
		REComSession::FinalClose();

		TInt* marker = new TInt(0);
		if (!marker)
			{
			break;		
			}
		delete marker;
		}

	__UHEAP_MARKEND;	
	return TestStepResult();
	}
	
TInt CBoundWDPStep_OutOfMemory::CallStateMachine()
	{
	return 0;
	}
