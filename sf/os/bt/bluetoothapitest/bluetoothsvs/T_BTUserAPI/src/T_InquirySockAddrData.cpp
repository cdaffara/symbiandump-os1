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

#include "T_InquirySockAddrData.h"
#include "T_SockAddrData.h"
#include "T_BTDevAddrData.h"
#include "T_CBluetoothSocketData.h"
#include "T_BTUtil.h"

/*@{*/
//Parameters
_LIT(KExpected, 					"expected");
_LIT(KTSockAddr,					"sockaddr");
_LIT(KTBTDevAddr,					"tbtdevaddr");
_LIT(KClass,						"class");
_LIT(KIAC,							"iac");
_LIT(KFlag,							"flag");
_LIT(KIntDevAddr,					"integerdevaddr");
_LIT(KVersionName,					"name");
_LIT(KVersionBuild,					"build");
_LIT(KVersionMajor,					"major");
_LIT(KVersionMinor,					"minor");
_LIT(KBTSocket,						"btsocket");
_LIT(KDeviceName,					"devicename");




//Commands
_LIT(KCmdTInquirySockAddr,			"TInquirySockAddr");
_LIT(KCmdBTAddr,					"BTAddr");
_LIT(KCmdSetBTAddr,					"SetBTAddr");
_LIT(KCmdCast,						"Cast");
_LIT(KCmdMajorServiceClass,			"MajorServiceClass");
_LIT(KCmdSetMajorServiceClass,		"SetMajorServiceClass");
_LIT(KCmdMajorClassOfDevice,		"MajorClassOfDevice");
_LIT(KCmdSetMajorClassOfDevice,		"SetMajorClassOfDevice");
_LIT(KCmdMinorClassOfDevice,		"MinorClassOfDevice");
_LIT(KCmdSetMinorClassOfDevice,		"SetMinorClassOfDevice");
_LIT(KCmdIAC,						"IAC");
_LIT(KCmdSetIAC,					"SetIAC");
_LIT(KCmdAction,					"Action");
_LIT(KCmdSetAction,					"SetAction");
_LIT(KCmdVersion,					"Version");
//Use of Protected method
_LIT(KCmdFormatTypeField,			"PROTECTED-FormatTypeField");

//Commands
_LIT(KCmdOpenHostResolver,			"OpenHostResolver");
_LIT(KCmdGetByAddress,				"GetByAddress");
_LIT(KCmdSetHostName,				"SetHostName");
_LIT(KCmdSetDiscoverable,			"SetDiscoverable");

//	Logging
_LIT(KLogMissingParameters,			"Missing parameters '%S' or '%S'");
_LIT(KLogMissingParameter,			"Missing parameter '%S'");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_InquirySockAddrData* CT_InquirySockAddrData::NewL()
	{
	CT_InquirySockAddrData*	ret=new (ELeave) CT_InquirySockAddrData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_InquirySockAddrData::CT_InquirySockAddrData()
:	iData(NULL)
,	iTInquirySockAddrOwned(EFalse)
,	iActive(NULL)
,	iHostResolverOpen(EFalse)
	{
	}

void CT_InquirySockAddrData::ConstructL()
	{
	iActive = CActiveCallback::NewL(*this);
	}


CT_InquirySockAddrData::~CT_InquirySockAddrData()
	{
	DestroyData();
	if ( iHostResolverOpen )
		{
		iHostResolver.Close();
		}
	delete iActive;
	iActive = NULL;
	}

TAny* CT_InquirySockAddrData::GetObject()
	{
	return iData;
	}

void CT_InquirySockAddrData::DestroyData()
	{
	if (iTInquirySockAddrOwned)
		{
		delete iData;
		}
	iData=NULL;	
	}

void CT_InquirySockAddrData::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iData = static_cast<T_InquirySockAddrChild*> (aAny);
	}

void CT_InquirySockAddrData::DisownObjectL()
	{
	iData = NULL;
	}

inline TCleanupOperation CT_InquirySockAddrData::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_InquirySockAddrData::CleanupOperation(TAny* aAny)
	{
	//may be unnecessary
	T_InquirySockAddrChild* arg=static_cast<T_InquirySockAddrChild*>(aAny);
	delete arg;
	}


TBool CT_InquirySockAddrData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret = ETrue;

	if ( aCommand==KCmdTInquirySockAddr )
		{
		DoCmdTInquirySockAddr(aSection);
		}
	else if ( aCommand==KCmdBTAddr )
		{
		DoCmdBTAddr(aSection);
		}
	else if ( aCommand==KCmdSetBTAddr )
		{
		DoCmdSetBTAddr(aSection);
		}
	else if ( aCommand==KCmdCast )
		{
		DoCmdCast(aSection);
		}
	else if ( aCommand==KCmdMajorServiceClass )
		{
		DoCmdMajorServiceClass(aSection);
		}
	else if ( aCommand==KCmdSetMajorServiceClass )
		{
		DoCmdSetMajorServiceClass(aSection);
		}
	else if ( aCommand==KCmdMajorClassOfDevice )
		{
		DoCmdMajorClassOfDevice(aSection);
		}
	else if ( aCommand==KCmdSetMajorClassOfDevice )
		{
		DoCmdSetMajorClassOfDevice(aSection);
		}
	else if ( aCommand==KCmdMinorClassOfDevice )
		{
		DoCmdMinorClassOfDevice(aSection);
		}
	else if ( aCommand==KCmdSetMinorClassOfDevice )
		{
		DoCmdSetMinorClassOfDevice(aSection);
		}
	else if ( aCommand==KCmdIAC )
		{
		DoCmdIAC(aSection);
		}
	else if ( aCommand==KCmdSetIAC )
		{
		DoCmdSetIAC(aSection);
		}
	else if ( aCommand==KCmdAction )
		{
		DoCmdAction(aSection);
		}
	else if ( aCommand==KCmdSetAction )
		{
		DoCmdSetAction(aSection);
		}
	else if ( aCommand==KCmdVersion )
		{
		DoCmdVersion(aSection);
		}
	else if ( aCommand==KCmdFormatTypeField )
		{
		DoCmdPROTECTEDFormatTypeField(aSection);
		}
	else if ( aCommand==KCmdOpenHostResolver )
		{
		DoCmdOpenHostResolver(aSection);
		}
	else if ( aCommand==KCmdGetByAddress )
		{
		DoCmdGetByAddress(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdSetHostName )
		{
		DoCmdSetHostName(aSection);
		}
	else if ( aCommand==KCmdSetDiscoverable )
		{
		SetDiscoverable(aSection);
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}


/**
Test TInquirySockAddr()
*/
void CT_InquirySockAddrData::DoCmdTInquirySockAddr(const TDesC& aSection)
	{
	TPtrC myTSockAddrName;
	TSockAddr* myTSockAddr = NULL;
	if ( !GetStringFromConfig(aSection, KTSockAddr(), myTSockAddrName) )
		{
		DestroyData();
		//Use default constructor	
		INFO_PRINTF1(_L("TInquirySockAddr Constructor Call"));	
		iData = new (ELeave) T_InquirySockAddrChild();
		iTInquirySockAddrOwned=ETrue;
		}
	else
		{
		myTSockAddr = static_cast<TSockAddr*>(GetDataObjectL(myTSockAddrName));
		
		if ( myTSockAddr!=NULL )
			{
			DestroyData();
			INFO_PRINTF1(_L("TInquirySockAddr Constructor Call"));	
			iData = new (ELeave) T_InquirySockAddrChild(*myTSockAddr);
			iTInquirySockAddrOwned=ETrue;
			}
		else
			{
			ERR_PRINTF1(_L("TInquirySockAddr Construct From TSockAddr. TSockAddr is NULL"));
			SetBlockResult(EFail);	
			}	
		}
	if ( iData == NULL ) 
		{
		ERR_PRINTF1(_L("TInquirySockAddr is NULL"));
		SetBlockResult(EFail);
		}
	}
	
	
/**
Test BTAddr()
*/
void CT_InquirySockAddrData::DoCmdBTAddr(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TInquirySockAddr BTAddr Call"));	
	TBTDevAddr btDevAddr = iData->BTAddr();
	
	TBuf<KMaxSockAddrSize> tmpbtDevAddr;
	btDevAddr.GetReadable(tmpbtDevAddr);
	INFO_PRINTF2(_L("Returned BT Addres = (%S)"), &tmpbtDevAddr);		
    TBuf<KMaxSockAddrSize> tmptBTDevAddrData; 

	TPtrC	tBTDevAddrName;
	TBool	foundParameter=EFalse;	
	TBool result = EFalse;
	
	if(GetStringFromConfig(aSection, KTBTDevAddr(), tBTDevAddrName))
		{
		foundParameter=ETrue;			
		TBTDevAddr* tBTDevAddrData=static_cast<TBTDevAddr*>(GetDataObjectL(tBTDevAddrName));

		tBTDevAddrData->GetReadable(tmptBTDevAddrData);
		INFO_PRINTF2(_L("Expected BT address = (%S) from ini file"), &tmptBTDevAddrData);

		result = btDevAddr==(*tBTDevAddrData);
		
		if ( !result )
			{
			ERR_PRINTF3(_L("BT Address (%S) is not as expected (%S)"),&tmpbtDevAddr, &tmptBTDevAddrData);
			SetBlockResult(EFail);
			}
		}
	
	TInt	intBtDevAddr=0;	
	if(GetIntFromConfig(aSection, KIntDevAddr(), intBtDevAddr))
		{
		foundParameter=ETrue;			
		const TBTDevAddr expectedBTAddr(intBtDevAddr);
		
		result = btDevAddr==(expectedBTAddr);
		btDevAddr.GetReadable(tmpbtDevAddr);
		if ( !result )
			{
			ERR_PRINTF3(_L("BT Address (%S) is not as expected (%d)"), &tmpbtDevAddr, intBtDevAddr);
			SetBlockResult(EFail);
			}
		}
		
	if( !foundParameter )
		{
		ERR_PRINTF3(KLogMissingParameters, &KTBTDevAddr(), &KIntDevAddr());
		SetBlockResult(EFail);
		}
	}


/**
Test SetBTAddr()
*/
void CT_InquirySockAddrData::DoCmdSetBTAddr(const TDesC& aSection)
	{	
	TPtrC	tBTDevAddrName;
	TBool	foundParameter=EFalse;	
	
	if(GetStringFromConfig(aSection, KTBTDevAddr(), tBTDevAddrName))
		{
		foundParameter=ETrue;			
		CT_BTDevAddrData* tBTDevAddrData=static_cast<CT_BTDevAddrData*>(GetDataWrapperL(tBTDevAddrName));
		const TBTDevAddr* btDevAddr = tBTDevAddrData->GetAddress();
		
		INFO_PRINTF1(_L("TInquirySockAddr SetBTAddr Call"));
		iData->SetBTAddr(*btDevAddr);
		}
	
	TInt	intBtDevAddr=0;	
	if(GetIntFromConfig(aSection, KIntDevAddr(), intBtDevAddr))
		{
		foundParameter=ETrue;			
		const TBTDevAddr btDevAddr(intBtDevAddr);
		
		INFO_PRINTF1(_L("TInquirySockAddr SetBTAddr Call"));	
		iData->SetBTAddr(btDevAddr);
		}
		
	if( !foundParameter )
		{
		ERR_PRINTF3(KLogMissingParameters, &KTBTDevAddr(), &KIntDevAddr());
		SetBlockResult(EFail);
		}
	}
	

/**
Test Cast()
*/
void CT_InquirySockAddrData::DoCmdCast(const TDesC& aSection)
	{			
	TPtrC myTSockAddrName;
	TSockAddr* myTSockAddr = NULL;
	
	if (GetStringFromConfig(aSection, KTSockAddr(), myTSockAddrName) )
		{
		myTSockAddr = static_cast<TSockAddr*>(GetDataObjectL(myTSockAddrName));
		if ( myTSockAddr!=NULL )
			{
			DestroyData();
			
			INFO_PRINTF1(_L("TInquirySockAddr Cast Call"));
			iData = &T_InquirySockAddrChild::Cast(*myTSockAddr);
			iTInquirySockAddrOwned=EFalse;			
			}		
		else
			{
			ERR_PRINTF1(_L("TSockAddr is NULL"));
			SetBlockResult(EFail);	
			}	
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KTSockAddr());
		SetBlockResult(EFail);
		}
	}


/**
Test MajorServiceClass()
*/
void CT_InquirySockAddrData::DoCmdMajorServiceClass(const TDesC& aSection)
	{	
	INFO_PRINTF1(_L("TInquirySockAddr MajorServiceClass Call"));	
	TUint16 result = iData->MajorServiceClass();

	TInt expected = 0;
	if(GetIntFromConfig(aSection, KExpected(), expected) )
		{
		if ( result != (TUint16)expected )
			{
			ERR_PRINTF3(_L("Return value (%d) is not as expected (%d)"), result, expected);
			SetBlockResult(EFail);
			}	
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KExpected());
		SetBlockResult(EFail);
		}
	}



/**
Test SetMajorServiceClass()
*/
void CT_InquirySockAddrData::DoCmdSetMajorServiceClass(const TDesC& aSection)
	{
	TInt classValue = 0;
	
	if (GetIntFromConfig(aSection, KClass(), classValue) )
		{
		INFO_PRINTF1(_L("TInquirySockAddr SetMajorServiceClass Call"));
		iData->SetMajorServiceClass((TUint16)classValue);
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KClass());
		SetBlockResult(EFail);
		}	
	}


/**
Test MajorClassOfDevice()
*/
void CT_InquirySockAddrData::DoCmdMajorClassOfDevice(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TInquirySockAddr MajorClassOfDevice Call"));	
	TUint8 result = iData->MajorClassOfDevice();

	TInt expected = 0;
	if(GetIntFromConfig(aSection, KExpected(), expected) )
		{
		if ( result != (TUint8)expected )
			{
			ERR_PRINTF3(_L("Return value (%d) is not as expected (%d)"), result, expected);
			SetBlockResult(EFail);
			}	
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KExpected());
		SetBlockResult(EFail);
		}
	}
	


/**
Test SetMajorClassOfDevice()
*/
void CT_InquirySockAddrData::DoCmdSetMajorClassOfDevice(const TDesC& aSection)
	{
	TInt classValue = 0;
	
	if (GetIntFromConfig(aSection, KClass(), classValue) )
		{
		INFO_PRINTF1(_L("TInquirySockAddr SetMajorClassOfDevice Call"));
		iData->SetMajorClassOfDevice((TUint8)classValue);
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KClass());
		SetBlockResult(EFail);
		}	
	}


/**
Test MinorClassOfDevice()
*/
void CT_InquirySockAddrData::DoCmdMinorClassOfDevice(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TInquirySockAddr MinorClassOfDevice Call"));
	TUint8 result = iData->MinorClassOfDevice();
	
	TInt expected = 0;
	if(GetIntFromConfig(aSection, KExpected(), expected) )
		{
		if ( result != (TUint8)expected )
			{
			ERR_PRINTF3(_L("Return value (%d) is not as expected (%d)"), result, expected);
			SetBlockResult(EFail);
			}	
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KExpected());
		SetBlockResult(EFail);
		}
	}
	


/**
Test SetMinorClassOfDevice()
*/
void CT_InquirySockAddrData::DoCmdSetMinorClassOfDevice(const TDesC& aSection)
	{
	TInt classValue = 0;
	
	if (GetIntFromConfig(aSection, KClass(), classValue) )
		{
		INFO_PRINTF1(_L("TInquirySockAddr SetMinorClassOfDevice Call"));
		iData->SetMinorClassOfDevice((TUint8)classValue);
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KClass());
		SetBlockResult(EFail);
		}	
	}


/**
Test IAC()
*/
void CT_InquirySockAddrData::DoCmdIAC(const TDesC& aSection)
	{	
	INFO_PRINTF1(_L("TInquirySockAddr IAC Call"));	
	TUint  result = iData->IAC();

	TInt expected = 0;
	if(GetIntFromConfig(aSection, KExpected(), expected) )
		{
		if ( result != (TUint)expected )
			{
			ERR_PRINTF3(_L("Return value (%d) is not as expected (%d)"), result, expected);
			SetBlockResult(EFail);
			}	
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KExpected());
		SetBlockResult(EFail);
		}	
	}	
	
	

/**
Test SetIAC()
*/
void CT_InquirySockAddrData::DoCmdSetIAC(const TDesC& aSection)
	{
	TInt theIAC = 0;
	
	if (GetIntFromConfig(aSection, KIAC(), theIAC) )
		{
		INFO_PRINTF1(_L("TInquirySockAddr SetIAC Call"));
		iData->SetIAC((TUint)theIAC);
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KIAC());
		SetBlockResult(EFail);
		}	
	}
	
	
/**
Test Action()
*/
void CT_InquirySockAddrData::DoCmdAction(const TDesC& aSection)
	{	
	INFO_PRINTF1(_L("TInquirySockAddr Action Call"));	
	TUint8 result = iData->Action();

	TInt expected = 0;
	if(GetIntFromConfig(aSection, KExpected(), expected) )
		{
		if ( result != (TUint8)expected )
			{
			ERR_PRINTF3(_L("Return value (%d) is not as expected (%d)"), result, expected);
			SetBlockResult(EFail);
			}	
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KExpected());
		SetBlockResult(EFail);
		}	
	}	


/**
Test SetAction()
*/
void CT_InquirySockAddrData::DoCmdSetAction(const TDesC& aSection)
	{
	TBool	foundParameter=EFalse;
	TPtrC	flagName;
	TInt	flagValue=0;
	if (GetStringFromConfig(aSection, KFlag(), flagName) )
		{
		foundParameter=ETrue;
		if (CT_BTUtil::GetIntValue(flagName, flagValue))
			{
			INFO_PRINTF3(_L("The flag to set: %S , its value: %d" ), &flagName, flagValue);
			
			INFO_PRINTF1(_L("TInquirySockAddr SetAction Call"));
			iData->SetAction(KHostResInquiry | KHostResName);
			}
		}
	
	if ( !foundParameter )
		{
		ERR_PRINTF2(KLogMissingParameter, &KFlag());
		SetBlockResult(EFail);		
		}
	}


/**
Test Version()
*/
void CT_InquirySockAddrData::DoCmdVersion(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TInquirySockAddr Version Call"));
	TVersion		version=iData->Version();
	TVersionName	versionName	= version.Name();

	INFO_PRINTF1(_L("Version name  :"));
	INFO_PRINTF1(versionName);
	INFO_PRINTF2(_L("Version build : %d"), (TInt)version.iBuild);
	INFO_PRINTF2(_L("Version major : %d"), (TInt)version.iMajor);
	INFO_PRINTF2(_L("Version minor : %d"), (TInt)version.iMinor);

	TPtrC	name;
	if (GetStringFromConfig(aSection, KVersionName(), name) )
		{
		if ( name != versionName )
			{
			ERR_PRINTF1(_L("Name does not match expected name"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KVersionName());
		SetBlockResult(EFail);
		}
	
	/*
	The following expected values are optional since the version name already includes these info.
	*/
	TInt	intTemp = 0;
	if (GetIntFromConfig(aSection, KVersionBuild(), intTemp) )
		{
		if ( intTemp != version.iBuild )
			{
			ERR_PRINTF1(_L("Build does not match expected build"));
			SetBlockResult(EFail);
			}
		}
	
	if (GetIntFromConfig(aSection, KVersionMajor(), intTemp) )
		{
		if ( intTemp != version.iMajor )
			{
			ERR_PRINTF1(_L("Major does not match expected major"));
			SetBlockResult(EFail);
			}
		}

	if (GetIntFromConfig(aSection, KVersionMinor(), intTemp) )
		{
		if ( intTemp != version.iMinor )
			{
			ERR_PRINTF1(_L("Minor does not match expected minor"));
			SetBlockResult(EFail);
			}
		}
	
	if (versionName == _L("") && version.iBuild == 0)
		{
		ERR_PRINTF1(_L("Some version fields are not set!"));
		SetBlockResult(EFail);
		}
		
	if (version.iMajor == 0 && version.iMinor == 0)
		{
		ERR_PRINTF1(_L("Some version fields are not set!"));
		SetBlockResult(EFail);
		}
	}


/**
Test FormatTypeField() - protected method
*/
void CT_InquirySockAddrData::DoCmdPROTECTEDFormatTypeField(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TInquirySockAddr FormatTypeField Call"));	
	TUint8	result = iData->T_FormatTypeField();

	TInt expected = 0;
	if(GetIntFromConfig(aSection, KExpected(), expected) )
		{
		if ( result > (TUint8)expected )
			{
			ERR_PRINTF3(_L("Return value (%d) is not as expected (%d)"), result, expected);
			SetBlockResult(EFail);
			}	
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KExpected());
		SetBlockResult(EFail);
		}
	}

/**
RHostResolver::Open()
*/
void CT_InquirySockAddrData::DoCmdOpenHostResolver(const TDesC& aSection)
	{
	RSocketServ	sockServ;
	TPtrC		bluetoothSocketName;
	if(GetStringFromConfig(aSection, KBTSocket(), bluetoothSocketName))
		{
		CT_BluetoothSocketNotifier*	bluetoothSocketData=static_cast<CT_BluetoothSocketNotifier*>(GetDataObjectL(bluetoothSocketName));
		if ( bluetoothSocketData!=NULL )
			{
			sockServ = bluetoothSocketData->iSocketServer;
			}
		else
			{
			ERR_PRINTF1(_L("CT_CBluetoothSocketDataPersistentObject is NULL"));
			SetBlockResult(EFail);
			}
		}
	
	
	//	Find protocol from socket
	TProtocolDesc	protocolDesc;
	TInt			err=sockServ.FindProtocol(_L("BTLinkManager"), protocolDesc);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("iSockServ.FindProtocol(aProtocolName, protocolDesc) = %d"), err);
        SetBlockResult(EFail);
		}
	if ( protocolDesc.iAddrFamily != KBTAddrFamily )
		{
		ERR_PRINTF1(_L("Wrong iAddrFamily"));
        SetBlockResult(EFail);
		}
		
	INFO_PRINTF1(_L("Close a RHostResolver first..."));
	iHostResolver.Close();
	//	Open host resolver
	INFO_PRINTF1(_L("Create and initialise an RHostResolver"));
	err=iHostResolver.Open(sockServ, protocolDesc.iAddrFamily, protocolDesc.iProtocol);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("iHostResolver.Open(iSockServ, protocolDesc.iAddrFamily, protocolDesc.iProtocol) = %d"), err);
        SetError(err);
		}
	else
		{
		iHostResolverOpen = ETrue;
		INFO_PRINTF1(_L("Create and initialise an RHostResolver Completed..."));
		}
	}


/**
RHostResolver::SetHostName()
*/
void CT_InquirySockAddrData::DoCmdSetHostName(const TDesC& aSection)
	{
	TPtrC deviceName;
	if(GetStringFromConfig(aSection, KDeviceName(), deviceName) )
		{
		INFO_PRINTF1(_L("Call RHostResolver::SetHostName()"));
		TInt err=iHostResolver.SetHostName(deviceName);
		INFO_PRINTF2(_L("iHostResolver.SetHostName(%S)"), &deviceName);

		if( err!=KErrNone )
			{
			ERR_PRINTF2(_L("iHostResolver.SetHostName() = %d"), err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF1(_L("No HostName given"));
		SetBlockResult(EFail);
		}
	}

/**
 SetDiscoverable
*/
void CT_InquirySockAddrData::SetDiscoverable(const TDesC& /*aSection*/)
	{
	TInt err = RProperty::Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetLimitedDiscoverableStatus, EFalse);
	INFO_PRINTF1(_L("RProperty::Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetLimitedDiscoverableStatus, EFalse) called"));
	if( err!=KErrNone )
		{
		ERR_PRINTF2(_L("RProperty::Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetLimitedDiscoverableStatus, EFalse) = %d"), err);
		SetBlockResult(EFail);
		}

	err = RProperty::Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetScanningStatus, EInquiryAndPageScan);
	INFO_PRINTF1(_L("RProperty::Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetScanningStatus, EInquiryAndPageScan) called"));
	if( err!=KErrNone )
		{
		ERR_PRINTF2(_L("RProperty::Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetScanningStatus, EInquiryAndPageScan) = %d"), err);
		SetBlockResult(EFail);
		}
	}


/**
RHostResolver::GetByAddress()
*/
void CT_InquirySockAddrData::DoCmdGetByAddress(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	if(GetStringFromConfig(aSection, KDeviceName(), iDeviceName) )
		{
		iSockAddr = *iData;
		
		INFO_PRINTF1(_L("Call RHostResolver::GetByAddress()"));
		iHostResolver.GetByAddress(iSockAddr, iBtNameEntry, iActive->iStatus);
		INFO_PRINTF1(_L("RHostResolver::GetByAddress() completed"));
	
		iActive->Activate(aAsyncErrorIndex);
		IncOutstanding();
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KDeviceName());
		SetBlockResult(EFail);
		}
	}
	
/**
 Virtual RunL - Called on completion of an a Synchronous command
 @publishedInternal
 @see MT_MMActiveCallback
 @param aActive Active Object that RunL has been called on
 @pre N/A
 @post N/A
 @leave system wide error code
*/
void CT_InquirySockAddrData::RunL(CActive* aActive, TInt aIndex)
 	{
 	if ( aActive==iActive )
 		{
 		RunGetDeviceAddress(aIndex);
 		}
 	else
 		{
 		ERR_PRINTF1(_L("Stray signal"));
 		SetBlockResult(EFail);
 		}
 	}

/**
  Virtual DoCancel - Request to cancel the asynchronous command
  @publishedInternal
  @see - MTPActiveCallback
  @param aActive Active Object that DoCancel has been called on
  @pre - N/A
  @post - N/A
  @leave system wide error code
 */
 void CT_InquirySockAddrData::DoCancel(CActive* aActive, TInt /*aIndex*/)
 	{
 	if ( aActive==iActive )
 		{
 		DoCancelGetDeviceAddress();
 		}
 	else
 		{
 		ERR_PRINTF1(_L("Stray signal"));
 		SetBlockResult(EFail);
 		}
 	}

/**
  RunCustomCommand - Process RunL for iActive
  @publishedInternal
  @pre N/A
  @post N/A
  @leave system wide error code
 */
 void CT_InquirySockAddrData::RunGetDeviceAddress(TInt aIndex)
	{
	TInt	err = iActive->iStatus.Int();
	if ( err == KErrNone )
		{
		THostName	deviceName = iBtNameEntry().iName;
		INFO_PRINTF2(_L("Device %S Found"), &deviceName);

		if( deviceName == iDeviceName )
			{
			INFO_PRINTF2(_L("Found required device %S"), &deviceName);

			INFO_PRINTF1(_L("Cast TSockAddr to the iData..."));
			*iData = iSockAddr.Cast(iBtNameEntry().iAddr);
			INFO_PRINTF1(_L("Cast completed"));
			
			iBTDevAddr = new (ELeave)TBTDevAddr();
			*iBTDevAddr = iData->BTAddr();
			TBuf<KMaxSockAddrSize> tmpBuf;
			iBTDevAddr->GetReadable(tmpBuf);
			INFO_PRINTF2(_L("Required Device BT Address = %S"), &tmpBuf);
			DecOutstanding();
			}
		else
			{
			INFO_PRINTF1(_L("Still Searching..."));
			//Grab the details of the next device
			iHostResolver.Next(iBtNameEntry, iActive->iStatus);
			iActive->Activate(aIndex);
			}
		}
	else
		{
		ERR_PRINTF2(_L("RunL Error %d"), err);
		SetAsyncError(aIndex, err);
     	DecOutstanding();
		}
	}

 
/**
 DoCancelActive - Process DoCancel for iActive
 @publishedInternal
 @pre N/A
 @post N/A
 @leave system wide error code
*/
void CT_InquirySockAddrData::DoCancelGetDeviceAddress()
	{
	}

