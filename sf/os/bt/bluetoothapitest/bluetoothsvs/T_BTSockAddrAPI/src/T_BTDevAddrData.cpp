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


#include "T_BTDevAddrData.h"
#include "T_CBluetoothSocketData.h"


//Constant Literals used.

//Parameters
_LIT(KDevAddress,										"btdevaddress");
_LIT(KBTAddr,											"btaddr");
_LIT(KHexBTAddrL,										"hex_btaddr_l");
_LIT(KHexBTAddrR,										"hex_btaddr_r");
_LIT(KDeviceName,										"devicename");
_LIT(KBTSocket,											"btsocket");
_LIT(KUseLocalName,										"uselocalname");

//TBTDevAddr
_LIT(KCmdConstruct, 									"Construct");
_LIT(KCmdSetAddress, 									"SetBTAddr");

_LIT(KCmdOpenHostResolver,								"OpenHostResolver");
_LIT(KCmdSetHostNameL,									"SetHostNameL");
_LIT(KCmdSetDiscoverable,								"SetDiscoverable");
_LIT(KCmdGetDeviceAddress,								"GetDeviceAddress");
_LIT(KCmdDistructor,									"~");

/*@}*/

CT_BTDevAddrData* CT_BTDevAddrData::NewL()
	{
	CT_BTDevAddrData*	ret=new (ELeave) CT_BTDevAddrData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_BTDevAddrData::CT_BTDevAddrData()
: iBTDevAddr(NULL),	iActive(NULL), iHostResolverOpen(EFalse)
	{
	}

void CT_BTDevAddrData::ConstructL()
	{
	iActive = CActiveCallback::NewL(*this);
	}

CT_BTDevAddrData::~CT_BTDevAddrData()
	{
	if( iHostResolverOpen )
		{
		iHostResolver.Close();
		}

	Cleanup();

	delete iActive;
	iActive = NULL;
	}

void CT_BTDevAddrData::Cleanup()
	{
	delete iBTDevAddr;
	iBTDevAddr=NULL;
	}

inline TCleanupOperation CT_BTDevAddrData::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_BTDevAddrData::CleanupOperation(TAny* aAny)
	{
	TBTDevAddr* arg=static_cast<TBTDevAddr*>(aAny);
	delete arg;
	}

TBool CT_BTDevAddrData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool								retVal=ETrue;

	if ( aCommand==KCmdConstruct )
		{
		TBTDAConstruct(aSection);
		}
	else if ( aCommand==KCmdSetAddress )
		{
		TBTDASetAddress(aSection);
		}
	else if ( aCommand==KCmdSetHostNameL )
		{
		SetHostNameL(aSection);
		}
	else if ( aCommand==KCmdOpenHostResolver )
		{
		OpenHostResolver(aSection);
		}
	else if ( aCommand==KCmdSetDiscoverable )
		{
		SetDiscoverable(aSection);
		}
	else if ( aCommand==KCmdGetDeviceAddress )
		{
		GetDeviceAddress(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdDistructor )
		{				
		Cleanup();
		}	
	else
		{
		retVal=EFalse;
		}
	return retVal;
	}

void CT_BTDevAddrData::TBTDAConstruct(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TBTDevAddr Constructor Call"));
	TPtrC myDevAddrName;
	TBTDevAddr*	myDevAddr=NULL;
	if ( !GetStringFromConfig(aSection, KDevAddress(), myDevAddrName) )
		{
		//Use default constructor
		Cleanup();
		iBTDevAddr = new (ELeave) TBTDevAddr();
		}
	else
		{
		TRAPD( err, myDevAddr=static_cast<TBTDevAddr*>(GetDataObjectL(myDevAddrName)) );

		if ( (!err) && (myDevAddr!=NULL) )
			{
			Cleanup();
			TRAPD( err, iBTDevAddr = new (ELeave) TBTDevAddr(*myDevAddr) );
			SetError(err);
			}
		else
			{
			ERR_PRINTF2(_L("TBTDAConstructFromDevAddr TSockAddr is NULL and GetDataObjectL returned err = %d"), err);
			SetBlockResult(EFail);
			}
		}
	}


void CT_BTDevAddrData::TBTDASetAddress(const TDesC& aSection)
	{
	TInt	devId;
	if( GetIntFromConfig(aSection, KBTAddr(), devId))
		{
		INFO_PRINTF2(_L("Set TBTDevAddr (%d)"), devId);
		*iBTDevAddr = devId;
		}
	else
		{
		TPtrC		bluetoothSocketName;
		if( GetStringFromConfig(aSection, KBTSocket(), bluetoothSocketName))
			{
			CT_BluetoothSocketNotifier*	bluetoothSocketNotifier=static_cast<CT_BluetoothSocketNotifier*>(GetDataObjectL(bluetoothSocketName));
			if ( bluetoothSocketNotifier!=NULL )
				{
				TBool				useLocalName = ETrue;
				GetBoolFromConfig(aSection, KUseLocalName(), useLocalName);

				T_TBTSockAddrChild	addr;
				if( useLocalName )
					{
					bluetoothSocketNotifier->iBluetoothSocket->LocalName(addr);
					}
				else
					{
					bluetoothSocketNotifier->iBluetoothSocket->RemoteName(addr);
					}

				*iBTDevAddr = addr.BTAddr();
				TBuf<KMaxSockAddrSize>	tmpBuf;
				iBTDevAddr->GetReadable(tmpBuf);
				INFO_PRINTF2(_L("Set TBTDevAddr (%S)"), &tmpBuf);
				}
			else
				{
				ERR_PRINTF1(_L("CT_BluetoothSocketNotifier is NULL"));
				SetBlockResult(EFail);
				}
			}
		else
			{
			TInt	lhs;
			if ( !GetHexFromConfig(aSection, KHexBTAddrL(), lhs ))
				{
				ERR_PRINTF1(_L("TBTDevAddr GetHexFromConfig failed"));
				SetBlockResult(EFail);
				}

			TInt	rhs;
			if ( !GetHexFromConfig(aSection, KHexBTAddrR(), rhs ))
				{
				ERR_PRINTF1(_L("TBTDevAddr GetHexFromConfig failed"));
				SetBlockResult(EFail);
				}
			*iBTDevAddr = (MAKE_TINT64(lhs, rhs));
			}
		}
	}


void CT_BTDevAddrData::OpenHostResolver(const TDesC& aSection)
	{
	RSocketServ	sockServ;
	TPtrC		bluetoothSocketName;
	if( GetStringFromConfig(aSection, KBTSocket(), bluetoothSocketName))
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
        SetError(err);
		}
	if ( protocolDesc.iAddrFamily != KBTAddrFamily )
		{
		ERR_PRINTF1(_L("Wrong iAddrFamily"));
        SetBlockResult(EFail);
		}

	//	Open host resolver
	err=iHostResolver.Open(sockServ, protocolDesc.iAddrFamily, protocolDesc.iProtocol);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("iHostResolver.Open(iSockServ, protocolDesc.iAddrFamily, protocolDesc.iProtocol) = %d"), err);
        SetError(err);
		}
	else
		{
		iHostResolverOpen = ETrue;
		}
	}

void CT_BTDevAddrData::SetHostNameL(const TDesC& aSection)
	{
	TPtrC deviceName;
	if( GetStringFromConfig(aSection, KDeviceName(), deviceName) )
		{
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

void CT_BTDevAddrData::SetDiscoverable(const TDesC& /*aSection*/)
	{
	TInt err = RProperty::Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetLimitedDiscoverableStatus, EFalse);
	INFO_PRINTF1(_L("RProperty::Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetLimitedDiscoverableStatus, EFalse) called"));
	if( err!=KErrNone )
		{
		ERR_PRINTF2(_L("RProperty::Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetLimitedDiscoverableStatus, EFalse) = %d"), err);
		SetError(err);
		}

	err = RProperty::Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetScanningStatus, EInquiryAndPageScan);
	INFO_PRINTF1(_L("RProperty::Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetScanningStatus, EInquiryAndPageScan) called"));
	if( err!=KErrNone )
		{
		ERR_PRINTF2(_L("RProperty::Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetScanningStatus, EInquiryAndPageScan) = %d"), err);
		SetError(err);
		}
	}

void CT_BTDevAddrData::GetDeviceAddress(const TDesC& aSection, const TInt aIndex)
	{
	if( GetStringFromConfig(aSection, KDeviceName(), iDeviceName) )
		{
		// Set up the type of Bluetooth search we want to do
		iAddr.SetIAC(KGIAC);
		iAddr.SetAction(KHostResInquiry | KHostResName);

		// Make the asynchronous call to look for all devices
		iHostResolver.GetByAddress(iAddr, iBtNameEntry, iActive->iStatus);
		INFO_PRINTF1(_L("GetByAddress() called"));

		iActive->Activate(aIndex);
		IncOutstanding();
		}
	else
		{
		ERR_PRINTF1(_L("No device name given"));
		SetBlockResult(EFail);
		}
	}

const TBTDevAddr* CT_BTDevAddrData::GetAddress()
	{
	return iBTDevAddr;
	}

TAny* CT_BTDevAddrData::GetObject()
	{
	return iBTDevAddr;
	}

void CT_BTDevAddrData::SetObjectL(TAny* aObject)
	{
	delete iBTDevAddr;
	iBTDevAddr = static_cast<TBTDevAddr*>(aObject);
	}

void CT_BTDevAddrData::DisownObjectL()
	{
	iBTDevAddr=NULL;
	}

void CT_BTDevAddrData::CopyObjectL(TAny* aObjectPtr)
	{
	Cleanup();
	TBTDevAddr* tmp = static_cast<TBTDevAddr*>(aObjectPtr);
	iBTDevAddr =new (ELeave) TBTDevAddr(*tmp);
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
void CT_BTDevAddrData::RunL(CActive* aActive, const TInt aIndex)
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
 void CT_BTDevAddrData::DoCancel(CActive* aActive, const TInt /*aIndex*/)
 	{
 	if ( aActive!=iActive )
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
 void CT_BTDevAddrData::RunGetDeviceAddress(const TInt aIndex)
	{
	TInt	err = iActive->iStatus.Int();
	if ( err == KErrNone )
		{
		THostName	deviceName = iBtNameEntry().iName;
		INFO_PRINTF2(_L("Device %S Found"), &deviceName);

		if( deviceName == iDeviceName )
			{
			INFO_PRINTF2(_L("Found required device %S"), &deviceName);

			TBTSockAddr	sockAddr(iBtNameEntry().iAddr);
			*iBTDevAddr = sockAddr.BTAddr();
			TBuf<KMaxSockAddrSize> tmpBuf;
			iBTDevAddr->GetReadable(tmpBuf);
			INFO_PRINTF2(_L("Set TBTDevAddr (%S)"), &tmpBuf);

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
