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

#include "T_BTPhysicalLinkAdapterData.h"
#include "T_BTDevAddrData.h"
#include "T_BTUtil.h"

/*@{*/
//Parameters
_LIT(KRSocketServ,												"socketserv");
_LIT(KRSocket,													"rsocket");
_LIT(KTBTDevAddr,												"tbtdevaddr");
_LIT(KIntDevAddr,												"integerdevaddr");
_LIT(KLowPowerMode,												"lowpowermode");
_LIT(KPacketType,												"packettype");
_LIT(KPhysicalLinkState,										"physicallinkstate");
_LIT(KExpected, 												"expected");

//Commands
_LIT(KCmdRBTPhysicalLinkAdapter,								"RBTPhysicalLinkAdapter");
_LIT(KCmdOpen,													"Open");
_LIT(KCmdIsOpen,												"IsOpen");
_LIT(KCmdClose,													"Close");
_LIT(KCmdPhysicalLinkState,										"PhysicalLinkState");
_LIT(KCmdPreventRoleSwitch,										"PreventRoleSwitch");
_LIT(KCmdAllowRoleSwitch,										"AllowRoleSwitch");
_LIT(KCmdRequestMasterRole,										"RequestMasterRole");
_LIT(KCmdRequestSlaveRole,										"RequestSlaveRole");
_LIT(KCmdPreventLowPowerModes,									"PreventLowPowerModes");
_LIT(KCmdAllowLowPowerModes,									"AllowLowPowerModes");
_LIT(KCmdActivateSniffRequester,								"ActivateSniffRequester");
_LIT(KCmdActivateParkRequester,									"ActivateParkRequester");
_LIT(KCmdActivateActiveRequester,								"ActivateActiveRequester");
_LIT(KCmdCancelLowPowerModeRequester,							"CancelLowPowerModeRequester");
_LIT(KCmdRequestChangeSupportedPacketTypes,						"RequestChangeSupportedPacketTypes");
_LIT(KCmdNotifyNextBasebandChangeEvent,							"NotifyNextBasebandChangeEvent");
_LIT(KCmdCancelNextBasebandChangeEventNotifier,					"CancelNextBasebandChangeEventNotifier");
_LIT(KCmdAuthenticate,											"Authenticate");
_LIT(KCmdDestructor,											"~");

//	Logging
_LIT(KLogMissingParameters,			"Missing parameters '%S' or '%S'");
_LIT(KLogMissingParameter,			"Missing parameter '%S'");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_BTPhysicalLinkAdapterData* CT_BTPhysicalLinkAdapterData::NewL()
	{
	CT_BTPhysicalLinkAdapterData*	ret=new (ELeave) CT_BTPhysicalLinkAdapterData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_BTPhysicalLinkAdapterData::CT_BTPhysicalLinkAdapterData()
:	iData(NULL)
,	iActive(NULL)
,	iEventType(0)
,	iExpectedEventType(0)
,	iEventNotification(NULL)
	{
	}

void CT_BTPhysicalLinkAdapterData::ConstructL()
	{
	iActive = CActiveCallback::NewL(*this);
	}

CT_BTPhysicalLinkAdapterData::~CT_BTPhysicalLinkAdapterData()
	{
	DestroyData();
	delete iActive;
	iActive = NULL;
	}

//	Service methods
TAny* CT_BTPhysicalLinkAdapterData::GetObject()
	{
	return iData;
	}

void CT_BTPhysicalLinkAdapterData::SetObjectL(TAny* aAny)
	{
	//called when loading up persistent data
	DestroyData();
	iData = static_cast<RBTPhysicalLinkAdapter*> (aAny);
	}

void CT_BTPhysicalLinkAdapterData::DisownObjectL()
	{
	iData = NULL;
	}

void CT_BTPhysicalLinkAdapterData::DestroyData()
	{
	delete iData;
	iData=NULL;
	}

inline TCleanupOperation CT_BTPhysicalLinkAdapterData::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_BTPhysicalLinkAdapterData::CleanupOperation(TAny* aAny)
	{
	RBTPhysicalLinkAdapter* arg=static_cast<RBTPhysicalLinkAdapter*>(aAny);
	delete arg;
	}

TBool CT_BTPhysicalLinkAdapterData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret = ETrue;

	if ( aCommand==KCmdRBTPhysicalLinkAdapter )
		{
		DoCmdRBTPhysicalLinkAdapter();
		}
	else if ( aCommand==KCmdOpen )
		{
		DoCmdOpen(aSection);
		}
	else if ( aCommand==KCmdIsOpen )
		{
		DoCmdIsOpen(aSection);
		}
	else if ( aCommand==KCmdClose )
		{
		DoCmdClose();
		}
	else if ( aCommand==KCmdPhysicalLinkState )
		{
		DoCmdPhysicalLinkState(aSection);
		}
	else if ( aCommand==KCmdPreventRoleSwitch )
		{
		DoCmdPreventRoleSwitch();
		}
	else if ( aCommand==KCmdAllowRoleSwitch )
		{
		DoCmdAllowRoleSwitch();
		}
	else if ( aCommand==KCmdRequestMasterRole )
		{
		DoCmdRequestMasterRole();
		}
	else if ( aCommand==KCmdRequestSlaveRole )
		{
		DoCmdRequestSlaveRole();
		}
	else if ( aCommand==KCmdPreventLowPowerModes )
		{
		DoCmdPreventLowPowerModes(aSection);
		}
	else if ( aCommand==KCmdAllowLowPowerModes )
		{
		DoCmdAllowLowPowerModes(aSection);
		}
	else if ( aCommand==KCmdActivateSniffRequester )
		{
		DoCmdActivateSniffRequester();
		}
	else if ( aCommand==KCmdActivateParkRequester )
		{
		DoCmdActivateParkRequester();
		}
	else if ( aCommand==KCmdActivateActiveRequester )
		{
		DoCmdActivateActiveRequester();
		}
	else if ( aCommand==KCmdCancelLowPowerModeRequester )
		{
		DoCmdCancelLowPowerModeRequester();
		}
	else if ( aCommand==KCmdRequestChangeSupportedPacketTypes )
		{
		DoCmdRequestChangeSupportedPacketTypes(aSection);
		}
	else if ( aCommand==KCmdNotifyNextBasebandChangeEvent )
		{
		DoCmdNotifyNextBasebandChangeEvent(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdCancelNextBasebandChangeEventNotifier )
		{
		DoCmdCancelNextBasebandChangeEventNotifier();
		}
	else if ( aCommand==KCmdAuthenticate )
		{
		DoCmdAuthenticate();
		}
	else if ( aCommand==KCmdDestructor )
		{
		DoCmdDestructor();
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}


/**
Test RBTPhysicalLinkAdapter()
*/
void CT_BTPhysicalLinkAdapterData::DoCmdRBTPhysicalLinkAdapter()
	{
	INFO_PRINTF1(_L("RBTPhysicalLinkAdapter Constructor Call"));
	DestroyData();
	iData = new (ELeave) RBTPhysicalLinkAdapter();
	}

/**
Test Open()
*/
void CT_BTPhysicalLinkAdapterData::DoCmdOpen(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("RBTPhysicalLinkAdapter Open Call"));
	
	TPtrC	socketServName;
	if(GetStringFromConfig(aSection, KRSocketServ(), socketServName))
		{
		RSocketServ* socketServ=static_cast<RSocketServ*>(GetDataObjectL(socketServName));
		
		TPtrC	socketName;
		if(GetStringFromConfig(aSection, KRSocket(), socketName))
			{
			RSocket* socket = static_cast<RSocket*>(GetDataObjectL(socketName));
			TInt err = iData->Open(*socketServ, *socket);
			if( err!=KErrNone )
				{
				INFO_PRINTF2(_L("RBTPhysicalLinkAdapter Open err=%d" ),err);
				SetError(err);
				}
			}
		else
			{
			TPtrC	tBTDevAddrName;
			TBool	foundParameter=EFalse;	
			if(GetStringFromConfig(aSection, KTBTDevAddr(), tBTDevAddrName))
				{
				foundParameter=ETrue;
				CT_BTDevAddrData* tBTDevAddrData=static_cast<CT_BTDevAddrData*>(GetDataWrapperL(tBTDevAddrName));
				TBTDevAddr btDevAddr = *tBTDevAddrData->GetAddress();
				TInt err = iData->Open(*socketServ, btDevAddr);
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("RBTPhysicalLinkAdapter Open failed with error %d"), err);
					SetError(err);
					}
				}
			
			TInt	intBtDevAddr=0;	
			if(GetIntFromConfig(aSection, KIntDevAddr(), intBtDevAddr))
				{
				foundParameter=ETrue;			
				TBTDevAddr btDevAddr(intBtDevAddr);
				TInt err = iData->Open(*socketServ, btDevAddr);
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("RBTPhysicalLinkAdapter Open failed with error %d"), err);
					SetError(err);
					}
				}
				
			if( !foundParameter )
				{
				ERR_PRINTF3(KLogMissingParameters, &KTBTDevAddr(), &KIntDevAddr());
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KRSocket());
		SetBlockResult(EFail);
		}
	}


/**
Test IsOpen()
*/
void CT_BTPhysicalLinkAdapterData::DoCmdIsOpen(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("RBTPhysicalLinkAdapter IsOpen Call"));
	
	TBool isOpen = iData->IsOpen();
	INFO_PRINTF2(_L("RBTPhysicalLinkAdapter IsOpen result: actual = %d"), isOpen);
	
	TBool	expected = EFalse;
	if(GetBoolFromConfig(aSection, KExpected(), expected))
		{
		if( expected != isOpen )
			{
			ERR_PRINTF1(_L("IsOpen not as expected!"));
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
Test Close()
*/
void CT_BTPhysicalLinkAdapterData::DoCmdClose()
	{
	INFO_PRINTF1(_L("RBTPhysicalLinkAdapter Close Call"));
	iData->Close();
	}


/**
Test PhysicalLinkState()
*/
void CT_BTPhysicalLinkAdapterData::DoCmdPhysicalLinkState(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("RBTPhysicalLinkAdapter PhysicalLinkState Call"));
	
	TUint32	state = 0;
	TInt err = iData->PhysicalLinkState(state);
	INFO_PRINTF2(_L("RBTPhysicalLinkAdapter PhysicalLinkState result: actual = %d"), state);
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("RBTPhysicalLinkAdapter PhysicalLinkState failed with error %d"), err);
		SetError(err);
		}
	
	TInt expected = 0;
	if(GetIntFromConfig(aSection, KExpected(), expected) )
		{
		if( expected != state )
			{
			ERR_PRINTF1(_L("PhysicalLinkState not as expected!"));
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
Test PreventRoleSwitch()
*/
void CT_BTPhysicalLinkAdapterData::DoCmdPreventRoleSwitch()
	{
	INFO_PRINTF1(_L("RBTPhysicalLinkAdapter PreventRoleSwitch Call"));
	TInt err = iData->PreventRoleSwitch();
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("RBTPhysicalLinkAdapter PreventRoleSwitch failed with error %d"), err);
		SetError(err);
		}
	}


/**
Test AllowRoleSwitch()
*/
void CT_BTPhysicalLinkAdapterData::DoCmdAllowRoleSwitch()
	{
	INFO_PRINTF1(_L("RBTPhysicalLinkAdapter AllowRoleSwitch Call"));
	TInt err = iData->AllowRoleSwitch();
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("RBTPhysicalLinkAdapter AllowRoleSwitch failed with error %d"), err);
		SetError(err);
		}
	}


/**
Test RequestMasterRole()
*/
void CT_BTPhysicalLinkAdapterData::DoCmdRequestMasterRole()
	{
	INFO_PRINTF1(_L("RBTPhysicalLinkAdapter RequestMasterRole Call"));
	TInt err = iData->RequestMasterRole();
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("RBTPhysicalLinkAdapter RequestMasterRole failed with error %d"), err);
		SetError(err);
		}
	else
		{
		TTimeIntervalMicroSeconds32	interval = TTimeIntervalMicroSeconds32(3000000); // 3 secs
		User::After(interval);
		}
	}


/**
Test RequestSlaveRole()
*/
void CT_BTPhysicalLinkAdapterData::DoCmdRequestSlaveRole()
	{
	INFO_PRINTF1(_L("RBTPhysicalLinkAdapter RequestSlaveRole Call"));
	TInt err = iData->RequestSlaveRole();
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("RBTPhysicalLinkAdapter RequestSlaveRole failed with error %d"), err);
		SetError(err);
		}
	else
		{
		TTimeIntervalMicroSeconds32	interval = TTimeIntervalMicroSeconds32(3000000); // 3 secs
		User::After(interval);
		}
	}


/**
Test PreventLowPowerModes()
*/
void CT_BTPhysicalLinkAdapterData::DoCmdPreventLowPowerModes(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("RBTPhysicalLinkAdapter PreventLowPowerModes Call"));
	TPtrC	lowPowerMode;
	TInt	lowPowerModeValue = 0;
	if(GetStringFromConfig(aSection, KLowPowerMode(), lowPowerMode))
		{
		if( CT_BTUtil::GetLowPowerModeIntValue(lowPowerMode, lowPowerModeValue) )
			{
			INFO_PRINTF2(_L("RBTPhysicalLinkAdapter PreventLowPowerModes: lowPowerModeValue = %d"), lowPowerModeValue);
			TInt err = iData->PreventLowPowerModes(lowPowerModeValue);
			if(err != KErrNone)
				{
				ERR_PRINTF2(_L("RBTPhysicalLinkAdapter PreventLowPowerModes failed with error %d"), err);
				SetError(err);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Unknown input for TBTLinkMode"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KLowPowerMode());
		SetBlockResult(EFail);
		}
	}


/**
Test AllowLowPowerModes()
*/
void CT_BTPhysicalLinkAdapterData::DoCmdAllowLowPowerModes(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("RBTPhysicalLinkAdapter AllowLowPowerModes Call"));
	
	TPtrC	lowPowerMode;
	TInt	lowPowerModeValue = 0;
	if(GetStringFromConfig(aSection, KLowPowerMode(), lowPowerMode))
		{
		if(CT_BTUtil::GetLowPowerModeIntValue(lowPowerMode, lowPowerModeValue) )
			{
			INFO_PRINTF2(_L("RBTPhysicalLinkAdapter AllowLowPowerModes: lowPowerModeValue = %d"), lowPowerModeValue);
			TInt err = iData->AllowLowPowerModes(lowPowerModeValue);
			if(err != KErrNone)
				{
				ERR_PRINTF2(_L("RBTPhysicalLinkAdapter AllowLowPowerModes failed with error %d"), err);
				SetError(err);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Unknow input for TBTLinkMode"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KLowPowerMode());
		SetBlockResult(EFail);
		}
	}


/**
Test ActivateSniffRequester()
*/
void CT_BTPhysicalLinkAdapterData::DoCmdActivateSniffRequester()
	{
	INFO_PRINTF1(_L("RBTPhysicalLinkAdapter ActivateSniffRequester Call"));
	
	TInt err = iData->ActivateSniffRequester();
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("RBTPhysicalLinkAdapter ActivateSniffRequester failed with error %d"), err);
		SetError(err);
		}
	else
		{
		TTimeIntervalMicroSeconds32	interval = TTimeIntervalMicroSeconds32(3000000); // 3 secs
		User::After(interval);
		}
	}


/**
Test ActivateParkRequester()
*/
void CT_BTPhysicalLinkAdapterData::DoCmdActivateParkRequester()
	{
	INFO_PRINTF1(_L("RBTPhysicalLinkAdapter ActivateParkRequester Call"));
	
	TInt err = iData->ActivateParkRequester();
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("RBTPhysicalLinkAdapter ActivateParkRequester failed with error %d"), err);
		SetError(err);
		}
	else
		{
		TTimeIntervalMicroSeconds32	interval = TTimeIntervalMicroSeconds32(3000000); // 3 secs
		User::After(interval);
		}
	}


/**
Test ActivateActiveRequester()
*/
void CT_BTPhysicalLinkAdapterData::DoCmdActivateActiveRequester()
	{
	INFO_PRINTF1(_L("RBTPhysicalLinkAdapter ActivateActiveRequester Call"));
	
	TInt err = iData->ActivateActiveRequester();
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("RBTPhysicalLinkAdapter ActivateActiveRequester failed with error %d"), err);
		SetError(err);
		}
	else
		{
		TTimeIntervalMicroSeconds32	interval = TTimeIntervalMicroSeconds32(3000000); // 3 secs
		User::After(interval);
		}
	}


/**
Test CancelLowPowerModeRequester()
*/
void CT_BTPhysicalLinkAdapterData::DoCmdCancelLowPowerModeRequester()
	{
	INFO_PRINTF1(_L("RBTPhysicalLinkAdapter CancelLowPowerModeRequester Call"));
	
	TInt err = iData->CancelLowPowerModeRequester();
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("RBTPhysicalLinkAdapter CancelLowPowerModeRequester failed with error %d"), err);
		SetError(err);
		}
	}


/**
Test RequestChangeSupportedPacketTypes()
*/
void CT_BTPhysicalLinkAdapterData::DoCmdRequestChangeSupportedPacketTypes(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("RBTPhysicalLinkAdapter RequestChangeSupportedPacketTypes Call"));
	
	TPtrC	packetTypeName;
	TInt	packetTypeValue;
	if(GetStringFromConfig(aSection, KPacketType(), packetTypeName))
		{
		if ( CT_BTUtil::GetPacketTypeIntValue(packetTypeName, packetTypeValue) )
			{
			INFO_PRINTF2(_L("RBTPhysicalLinkAdapter RequestChangeSupportedPacketTypes: packetTypeValue = %d"), packetTypeValue);
			
			TInt err = iData->RequestChangeSupportedPacketTypes(packetTypeValue);
			if(err != KErrNone)
				{
				ERR_PRINTF2(_L("RBTPhysicalLinkAdapter RequestChangeSupportedPacketTypes failed with error %d"), err);
				SetError(err);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Unknow input for PacketType"));
			SetBlockResult(EFail);	
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KPacketType());
		SetBlockResult(EFail);
		}
	}


/**
Test NotifyNextBasebandChangeEvent()
*/
void CT_BTPhysicalLinkAdapterData::DoCmdNotifyNextBasebandChangeEvent(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	TPtrC	physicalLinkSatateName;
	TInt	physicalLinkSatateValue;
	TBool foundParameter = EFalse;
	if(GetStringFromConfig(aSection, KPhysicalLinkState(), physicalLinkSatateName))
		{
		foundParameter = ETrue;
		if ( CT_BTUtil::GetPhysicalLinkStateIntValue(physicalLinkSatateName, physicalLinkSatateValue) )
			{
			INFO_PRINTF2(_L("Verify the existing physicalLinkSatateValue = %d"), physicalLinkSatateValue);
			
			INFO_PRINTF1(_L("Call NotifyNextBasebandChangeEvent() method"));
			iData->NotifyNextBasebandChangeEvent(iEventNotification, iActive->iStatus, physicalLinkSatateValue);
			
			if(GetIntFromConfig(aSection, KExpected(), iExpectedEventType) )
				{
				foundParameter = ETrue;
				}
			iActive->Activate(aAsyncErrorIndex);
			IncOutstanding();
			}
		else
			{
			ERR_PRINTF1(_L("Unknow input for PhysicalLinkState"));
			SetBlockResult(EFail);	
			}
		}
	if (!foundParameter)
		{
		ERR_PRINTF3(KLogMissingParameters, &KPhysicalLinkState(), &KExpected());
		SetBlockResult(EFail);
		}
	}


/**
Test CancelNextBasebandChangeEventNotifier()
*/
void CT_BTPhysicalLinkAdapterData::DoCmdCancelNextBasebandChangeEventNotifier()
	{
	INFO_PRINTF1(_L("RBTPhysicalLinkAdapter CancelNextBasebandChangeEventNotifier Call"));
	
	iData->CancelNextBasebandChangeEventNotifier();
	}


/**
Test Authenticate()
*/
void CT_BTPhysicalLinkAdapterData::DoCmdAuthenticate()
	{
	INFO_PRINTF1(_L("RBTPhysicalLinkAdapter Authenticate Call"));
	
	TInt err = iData->Authenticate();
	INFO_PRINTF2(_L("Returned value (%d) of Authenticate()"), err);
	
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("RBTPhysicalLinkAdapter Authenticate failed with error %d"), err);
		SetError(err);
		}
	}

/**
Test Destructor
*/
void CT_BTPhysicalLinkAdapterData::DoCmdDestructor()
	{
	DestroyData();
	}


/**
 Virtual RunL - Called on completion of an asynchronous command
 @see MTPActiveCallback
 @param aActive Active Object that RunL has been called on
 @pre N/A
 @post N/A
 @leave system wide error code
*/
void CT_BTPhysicalLinkAdapterData::RunL(CActive* aActive, TInt aIndex)
	{
	INFO_PRINTF1(_L("Execute Async RunL() Call"));
	if( aActive==iActive )
		{
		TInt err = aActive->iStatus.Int();
		INFO_PRINTF2(_L("Returned result (%d) of aActive->istatus.Int()"), err);
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("RunL failed with error %d"), err);
			SetAsyncError(aIndex, err);
			}
		iEventType = iEventNotification().EventType();
		INFO_PRINTF2(_L("Returned TBTBasebandEvent::EventType(%d)"), iEventType);
		if( iEventType != iExpectedEventType )
			{
			ERR_PRINTF1(_L("NotifyNextBasebandChangeEvent is not as expected!"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF1(_L("Stray signal"));
		SetBlockResult(EFail);
		}
	DecOutstanding();
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
void CT_BTPhysicalLinkAdapterData::DoCancel(CActive* /* aActive */, TInt /*aIndex*/)
	{
	}

