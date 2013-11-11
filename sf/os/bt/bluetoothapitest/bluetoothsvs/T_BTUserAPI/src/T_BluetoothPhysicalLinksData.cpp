/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "T_BluetoothPhysicalLinksData.h"
#include "T_BTDevAddrData.h"

/*@{*/
//Parameters
_LIT(KRSocketServ,												"socketserv");
_LIT(KTBTDevAddr,												"tbtdevaddr");
_LIT(KIntDevAddr,												"integerdevaddr");
_LIT(KData,														"data");
_LIT(KMaxNumber,												"maxnumber");
_LIT(KExpectedSize, 											"expectedsize");
_LIT(KExpectedDevAddr,											"expecteddevaddr");
_LIT(KExpectedIntDevAddr,										"expectedintdevaddr");
_LIT(KUid,														"uid");
_LIT(KExpectNull,												"expectnull");
_LIT(KErrForCreateConnection,									"errforcreateconnection");
_LIT(KErrForDisconnect,											"errfordisconnect");
_LIT(KErrForDisconnectAll,										"errfordisconnectall");
_LIT(KDelayAfterCreateConnection,								"delayaftercreateconnection");

//Commands
_LIT(KCmdNewL,													"NewL");
_LIT(KCmdNewLC,													"NewLC");
_LIT(KCmdCreateConnection,										"CreateConnection");
_LIT(KCmdCancelCreateConnection,								"CancelCreateConnection");
_LIT(KCmdDisconnect,											"Disconnect");
_LIT(KCmdDisconnectAll,											"DisconnectAll");
_LIT(KCmdBroadcast,												"Broadcast");
_LIT(KCmdReadRaw,												"ReadRaw");
_LIT(KCmdEnumerate,												"Enumerate");
_LIT(KCmdDestructor,											"~");
_LIT(KCmdMBPLN_ExtensionInterfaceL,								"MBPLN_ExtensionInterfaceL");

//	Logging
_LIT(KLogMissingParameters,			"Missing parameters '%S' or '%S'");
_LIT(KLogMissingParameter,			"Missing parameter '%S'");

//constants
const TInt	KDataSize = 128;
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_BluetoothPhysicalLinksData* CT_BluetoothPhysicalLinksData::NewL()
	{
	CT_BluetoothPhysicalLinksData*	ret=new (ELeave) CT_BluetoothPhysicalLinksData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

/**
 * First phase construction
 */
CT_BluetoothPhysicalLinksData::CT_BluetoothPhysicalLinksData()
:	iData(NULL)
,	iErrForCreateConnection(0)
,	iErrForDisconnect(0)
,	iErrForDisconnectAll(0)
,	iDelayAfterCreateConnection(0)
,	iCreateIndex(0)
,	iDisconnectIndex(0)
,	iDisconnectAllIndex(0)
	{
	}

/**
 * Second phase construction
 */
void CT_BluetoothPhysicalLinksData::ConstructL()
	{
	}

CT_BluetoothPhysicalLinksData::~CT_BluetoothPhysicalLinksData()
	{
	DestroyData();
	}

//	Service methods
TAny* CT_BluetoothPhysicalLinksData::GetObject()
	{
	return iData;
	}

void CT_BluetoothPhysicalLinksData::SetObjectL(TAny* aObject)
	{
	//called when loading up persistent data
	DestroyData();
	iData = static_cast<CBluetoothPhysicalLinks*>(aObject);
	}

void CT_BluetoothPhysicalLinksData::DisownObjectL()
	{
	iData = NULL;
	}

void CT_BluetoothPhysicalLinksData::DestroyData()
	{
	delete iData;
	iData=NULL;
	}

inline TCleanupOperation CT_BluetoothPhysicalLinksData::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_BluetoothPhysicalLinksData::CleanupOperation(TAny* aAny)
	{
	CBluetoothPhysicalLinks* arg=static_cast<CBluetoothPhysicalLinks*>(aAny);
	delete arg;
	}

void CT_BluetoothPhysicalLinksData::HandleCreateConnectionCompleteL(TInt aError)
	{
	INFO_PRINTF2(_L("MBluetoothPhysicalLinksNotifier::HandleCreateConnectionCompleteL Status (%d)"), aError);
	
	DecOutstanding();
	if ( aError!=iErrForCreateConnection )
		{
		ERR_PRINTF2(_L("HandleCreateConnectionCompleteL Call failed: aError = %d"),aError);
		SetAsyncError(iCreateIndex, aError);
		}
	else
		{
		INFO_PRINTF1(_L("HandleCreateConnectionCompleteL successful" ));
		}
	TTimeIntervalMicroSeconds32	interval = TTimeIntervalMicroSeconds32(iDelayAfterCreateConnection);
	INFO_PRINTF2(_L("Waiting for %d micro secs"),iDelayAfterCreateConnection);
	User::After(interval);
	}

void CT_BluetoothPhysicalLinksData::HandleDisconnectCompleteL(TInt aError)
	{
	INFO_PRINTF2(_L("MBluetoothPhysicalLinksNotifier::HandleDisconnectCompleteL Status (%d)"), aError);
	
	DecOutstanding();
	if ( aError!=iErrForDisconnect )
		{
		ERR_PRINTF2(_L("HandleDisconnectCompleteL Call failed: aError = %d"),aError);
		SetAsyncError(iDisconnectIndex, aError);
		}
	else
		{
		INFO_PRINTF1(_L("HandleDisconnectCompleteL successful" ));
		}
	}

void CT_BluetoothPhysicalLinksData::HandleDisconnectAllCompleteL(TInt aError)
	{
	INFO_PRINTF2(_L("MBluetoothPhysicalLinksNotifier::HandleDisconnectAllCompleteL Status (%d)"), aError);
	
	DecOutstanding();
	if ( aError!=iErrForDisconnectAll )
		{
		ERR_PRINTF2(_L("HandleDisconnectAllCompleteL Call failed: aError = %d"),aError);
		SetAsyncError(iDisconnectAllIndex, aError);
		}
	else
		{
		INFO_PRINTF1(_L("HandleDisconnectAllCompleteL successful" ));
		}
	}

TBool CT_BluetoothPhysicalLinksData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret = ETrue;

	if ( aCommand==KCmdNewL )
		{
		DoCmdNewL(aSection);
		}
	else if ( aCommand==KCmdNewLC )
		{
		DoCmdNewLC(aSection);
		}
	else if ( aCommand==KCmdCreateConnection )
		{
		DoCmdCreateConnection(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdCancelCreateConnection )
		{
		DoCmdCancelCreateConnection();
		}
	else if ( aCommand==KCmdDisconnect )
		{
		DoCmdDisconnect(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdDisconnectAll )
		{
		DoCmdDisconnectAll(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdBroadcast )
		{
		DoCmdBroadcast(aSection);
		}
	else if ( aCommand==KCmdReadRaw )
		{
		DoCmdReadRaw(aSection);
		}
	else if ( aCommand==KCmdEnumerate )
		{
		DoCmdEnumerate(aSection);
		}
	else if ( aCommand==KCmdDestructor )
		{
		DoCmdDestructor();
		}
	else if ( aCommand==KCmdMBPLN_ExtensionInterfaceL )
		{
		DoCmdMBPLN_ExtensionInterfaceL(aSection);
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}


/**
Test NewL()
*/
void CT_BluetoothPhysicalLinksData::DoCmdNewL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CBluetoothPhysicalLinks NewL Call"));
	DestroyData();
	
	TPtrC	socketServName;
	if(GetStringFromConfig(aSection, KRSocketServ(), socketServName))
		{
		RSocketServ* socketServ=static_cast<RSocketServ*>(GetDataObjectL(socketServName));
		TRAPD(err, iData = CBluetoothPhysicalLinks::NewL(*this, *socketServ));
		if( err!=KErrNone )
			{
			INFO_PRINTF2(_L("CBluetoothPhysicalLinks Standard Constructor NewL err=%d" ),err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KRSocketServ());
		SetBlockResult(EFail);
		}
	}


/**
Test NewLC()
*/
void CT_BluetoothPhysicalLinksData::DoCmdNewLC(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CBluetoothPhysicalLinks NewLC Call"));
	DestroyData();
	
	TPtrC	socketServName;
	if( GetStringFromConfig(aSection, KRSocketServ(), socketServName))
		{
		RSocketServ* socketServ=static_cast<RSocketServ*>(GetDataObjectL(socketServName));
		TRAPD(err, iData = CBluetoothPhysicalLinks::NewLC(*this, *socketServ); CleanupStack::Pop(););
		if( err!=KErrNone )
			{
			INFO_PRINTF2(_L("CBluetoothPhysicalLinks Standard Constructor NewLC err=%d" ),err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KRSocketServ());
		SetBlockResult(EFail);
		}
	}


/**
Test CreateConnection()
*/
void CT_BluetoothPhysicalLinksData::DoCmdCreateConnection(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("CBluetoothPhysicalLinks CreateConnection Call"));
	
	TPtrC	tBTDevAddrName;
	TBool	foundParameter=EFalse;
	TInt err = 0;
	if( GetStringFromConfig(aSection, KTBTDevAddr(), tBTDevAddrName))
		{
		foundParameter=ETrue;
		CT_BTDevAddrData* tBTDevAddrData=static_cast<CT_BTDevAddrData*>(GetDataWrapperL(tBTDevAddrName));
		const TBTDevAddr* btDevAddr = tBTDevAddrData->GetAddress();
		err = iData->CreateConnection(*btDevAddr);
		}
		
	TInt	intBtDevAddr=0;	
	if( GetIntFromConfig(aSection, KIntDevAddr(), intBtDevAddr))
		{
		foundParameter=ETrue;
		const TBTDevAddr btDevAddr(intBtDevAddr);
		err = iData->CreateConnection(btDevAddr);
		}
	
	if( !foundParameter )
		{
		ERR_PRINTF3(KLogMissingParameters, &KTBTDevAddr(), &KIntDevAddr());
		SetBlockResult(EFail);
		}
	
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("CBluetoothPhysicalLinks CreateConnection failed with error %d"), err);
		SetError(err);
		}
	else
		{
		IncOutstanding();
		iCreateIndex = aAsyncErrorIndex;
		}
	
	if( !GetIntFromConfig(aSection, KErrForCreateConnection(), iErrForCreateConnection))
		{
		ERR_PRINTF2(KLogMissingParameter, &KErrForCreateConnection());
		SetBlockResult(EFail);
		}
	
	if( !GetIntFromConfig(aSection, KDelayAfterCreateConnection(), iDelayAfterCreateConnection))
		{
		ERR_PRINTF2(KLogMissingParameter, &KDelayAfterCreateConnection());
		SetBlockResult(EFail);
		}
	}


/**
Test CancelCreateConnection()
*/
void CT_BluetoothPhysicalLinksData::DoCmdCancelCreateConnection()
	{
	INFO_PRINTF1(_L("CBluetoothPhysicalLinks CancelCreateConnection Call"));
	iData->CancelCreateConnection();
	DecOutstanding();
	}


/**
Test Disconnect()
*/
void CT_BluetoothPhysicalLinksData::DoCmdDisconnect(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("CBluetoothPhysicalLinks Disconnect Call"));
	
	TPtrC	tBTDevAddrName;
	TBool	foundParameter=EFalse;	
	TInt err = 0;
	if( GetStringFromConfig(aSection, KTBTDevAddr(), tBTDevAddrName))
		{
		foundParameter=ETrue;
		CT_BTDevAddrData *tBTDeveAddr = static_cast<CT_BTDevAddrData*>(GetDataWrapperL(tBTDevAddrName));
		const TBTDevAddr* btDevAddr = tBTDeveAddr->GetAddress();
		err = iData->Disconnect(*btDevAddr);
		}
		
	TInt	intBtDevAddr=0;
	if( GetIntFromConfig(aSection, KIntDevAddr(), intBtDevAddr))
		{
		foundParameter=ETrue;
		TBTDevAddr btDevAddr(intBtDevAddr);
		iBTDeveAddr = btDevAddr;
		err = iData->Disconnect(iBTDeveAddr);
		}
	
	if( !foundParameter )
		{
		ERR_PRINTF3(KLogMissingParameters, &KTBTDevAddr(), &KIntDevAddr());
		SetBlockResult(EFail);
		}
	
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("CBluetoothPhysicalLinks Disconnect failed with error %d"), err);
		SetError(err);
		}
	else
		{
		IncOutstanding();
		iDisconnectIndex = aAsyncErrorIndex;
		}
	
	if( !GetIntFromConfig(aSection, KErrForDisconnect(), iErrForDisconnect))
		{
		ERR_PRINTF2(KLogMissingParameter, &KErrForDisconnect());
		SetBlockResult(EFail);
		}
	}


/**
Test DisconnectAll()
*/
void CT_BluetoothPhysicalLinksData::DoCmdDisconnectAll(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("CBluetoothPhysicalLinks DisconnectAll Call"));
	
	TInt err = iData->DisconnectAll();
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("CBluetoothPhysicalLinks DisconnectAll failed with error %d"), err);
		SetError(err);
		}
	else
		{
		IncOutstanding();
		iDisconnectAllIndex = aAsyncErrorIndex;
		}
	
	if( !GetIntFromConfig(aSection, KErrForDisconnectAll(), iErrForDisconnectAll))
		{
		ERR_PRINTF2(KLogMissingParameter, &KErrForDisconnectAll());
		SetBlockResult(EFail);
		}
	}


/**
Test Broadcast()
*/
void CT_BluetoothPhysicalLinksData::DoCmdBroadcast(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CBluetoothPhysicalLinks Broadcast Call"));
	
	TPtrC	data;
	if( GetStringFromConfig(aSection, KData(), data))
		{
		HBufC8*	data8 = HBufC8::NewLC(data.Length());
		data8->Des().Copy(data);
		TPtrC8 dataPtr = data8->Des();
		
		TInt err = iData->Broadcast(dataPtr);
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("CBluetoothPhysicalLinks Broadcast failed with error %d"), err);
			SetError(err);
			}
		CleanupStack::PopAndDestroy(data8);
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KData());
		SetBlockResult(EFail);
		}
	}


/**
Test ReadRaw()
*/
void CT_BluetoothPhysicalLinksData::DoCmdReadRaw(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CBluetoothPhysicalLinks ReadRaw Call"));
	
	TPtrC	data;
	if( GetStringFromConfig(aSection, KData(), data))
		{
		TBuf8<KDataSize> dataPtr;
		dataPtr.Copy(data);
		
		TInt err = iData->ReadRaw(dataPtr);
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("CBluetoothPhysicalLinks ReadRaw failed with error %d"), err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KData());
		SetBlockResult(EFail);
		}
	}


/**
Test Enumerate()
*/
void CT_BluetoothPhysicalLinksData::DoCmdEnumerate(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CBluetoothPhysicalLinks Enumerate Call"));
	TBool	foundParameter=EFalse;
	TInt	maxNumber = 0;
	RBTDevAddrArray	devAddrArry;
	if( GetIntFromConfig(aSection, KMaxNumber(), maxNumber))
		{
		foundParameter = ETrue;
		TInt err = iData->Enumerate(devAddrArry, maxNumber);
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("CBluetoothPhysicalLinks Enumerate failed with error %d"), err);
			SetError(err);
			}
			
		INFO_PRINTF2(_L("CBluetoothPhysicalLinks Enumerate RBTDevAddrArray size = %d"), devAddrArry.Count());
		TInt expectedSize = 0;
		TBool result = EFalse;
		if( GetIntFromConfig(aSection, KExpectedSize(), expectedSize) )
			{
			foundParameter = ETrue;
			if( expectedSize != devAddrArry.Count() )
				{
				ERR_PRINTF1(_L("RBTDevAddrArray size not as expected!"));
				SetBlockResult(EFail);
				}
			}
		TBuf<KMaxSockAddrSize> tmpBuf;
		for( int i=0; i<=devAddrArry.Count()-1; i++ )
			{
			TPtrC	tBTDevAddrName;
			if( GetStringFromConfig(aSection, KExpectedDevAddr(), tBTDevAddrName))
				{
				foundParameter=ETrue;
				CT_BTDevAddrData* tBTDevAddrData=static_cast<CT_BTDevAddrData*>(GetDataWrapperL(tBTDevAddrName));
				TBTDevAddr expectedDevAddr = *tBTDevAddrData->GetAddress();	
				expectedDevAddr.GetReadable(tmpBuf);
				result = devAddrArry[i]==(expectedDevAddr);
				if ( !result )
					{
					ERR_PRINTF2(_L("Actual BT Address is not as expected, expected address:(%S)"), &tmpBuf);
					SetBlockResult(EFail);
					}
				}
				
			TInt	intBtDevAddr=0;	
			if( GetIntFromConfig(aSection, KExpectedIntDevAddr(), intBtDevAddr))
				{
				foundParameter=ETrue;
				const TBTDevAddr expectedIntDevAddr(intBtDevAddr);
				result = devAddrArry[i]==(expectedIntDevAddr);
				if ( !result )
					{
			    	ERR_PRINTF2(_L("Actual BT Address is not as expected, expected address:(%D)"), intBtDevAddr);
					SetBlockResult(EFail);
					}
				}
			}
		devAddrArry.Reset();
		INFO_PRINTF1(_L("devAddrArry.Reset()"));
		}
	if (!foundParameter)
		{
		ERR_PRINTF3(KLogMissingParameters, &KExpectedDevAddr(), &KExpectedIntDevAddr());
		SetBlockResult(EFail);
		}
	}


/**
Test Destructor
*/
void CT_BluetoothPhysicalLinksData::DoCmdDestructor()
	{
	DestroyData();
	}


/**
Test MBPLN_ExtensionInterfaceL()
*/
void CT_BluetoothPhysicalLinksData::DoCmdMBPLN_ExtensionInterfaceL(const TDesC& aSection)
	{
	TInt tmp = 0;
	void* theExtensionObjectPtr;

	if( GetIntFromConfig(aSection, KUid(), tmp))
		{
		TUid uid=TUid::Uid(tmp);
		
		TBool expectNull = EFalse;
		if( GetBoolFromConfig(aSection, KExpectNull(), expectNull))
			{
			TRAPD(err, this->MBPLN_ExtensionInterfaceL(uid, theExtensionObjectPtr));
			if( err!=KErrNone )
				{
				ERR_PRINTF2(_L("MBPLN_ExtensionInterfaceL err=%d"),err);
				SetError(err);
				}
			
			if (expectNull)
				{
				//Fail if not NULL
				if (theExtensionObjectPtr!=NULL)
					{
					ERR_PRINTF1(_L("Expected Null pointer but object found"));
					SetBlockResult(EFail);
					}
				}
			else
				{
				//Fail if NULL
				if (theExtensionObjectPtr==NULL)
					{
					ERR_PRINTF1(_L("Expected object but found NULL"));
					SetBlockResult(EFail);
					}
				}
			}
		else
			{
			ERR_PRINTF2(KLogMissingParameter, &KExpectNull());
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KUid());
		SetBlockResult(EFail);
		}
	}
	
	

