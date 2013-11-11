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

#include "T_DataRSubConnection.h"

const TInt	KDefaultOptionBufferSize	=128;

/*@{*/
///	Constant Literals used.
_LIT(KCmdOpen,								"Open");
_LIT(KCmdClose,								"Close");
_LIT(KCmdAdd,								"Add");
_LIT(KCmdRemove,							"Remove");
_LIT(KCmdEventNotification,					"EventNotification");
_LIT(KCmdCancelEventNotification,			"CancelEventNotification");
_LIT(KCmdGetParameters,						"GetParameters");
_LIT(KCmdSetParameters,						"SetParameters");
_LIT(KCmdControl,							"Control");
_LIT(KConstructor,							"Constructor");
_LIT(KDestructor,							"~");

_LIT(KRSocketServ,							"socketserv");
_LIT(KRConnection,							"connection");
_LIT(KSubConnType,							"subconntype");
_LIT(KRSocket,								"socket");
_LIT(KGenericEventsOnly,					"genericeventsonly");
_LIT(KEventGroupUid,						"eventgroupuid%d");
_LIT(KEventMask,							"eventmask%d");
_LIT(KSubConParameterBundleName,			"subconparameterbundle");
_LIT(KOptionLevel,							"optionlevel");
_LIT(KOptionName,							"optionname");
_LIT(KOptionBufferSize,						"buffersize");
_LIT(KOption,								"option");
/*@}*/

CT_DataRSubConnection* CT_DataRSubConnection::NewL()
	{
	CT_DataRSubConnection*	ret=new (ELeave) CT_DataRSubConnection();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}


CT_DataRSubConnection::CT_DataRSubConnection()
:	CT_DataCommsBase()
,	iActiveAdd(NULL)
,	iActiveRemove(NULL)
,	iActiveEventNotification(NULL)
,	iSubConnection(NULL)
,	iEventFilterArray()
	{
	}


void CT_DataRSubConnection::ConstructL()
	{
	iActiveAdd = CActiveCallback::NewL(*this);
	iActiveRemove = CActiveCallback::NewL(*this);
	iActiveEventNotification = CActiveCallback::NewL(*this);
	}


CT_DataRSubConnection::~CT_DataRSubConnection()
	{
	iEventFilterArray.Reset();
	iEventFilterArray.Close();
	delete iActiveAdd;
	iActiveAdd=NULL;

	delete iActiveRemove;
	iActiveRemove=NULL;

	delete iActiveEventNotification;
	iActiveEventNotification=NULL;

	delete iSubConnection;
	iSubConnection = NULL;
	}


TBool CT_DataRSubConnection::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
/**
Delegates the commands to a function

@internalComponent
@param aCommand Current command
@param aSection Current ini file command section
@param aAsyncErrorIndex index of command for SetAsyncError()
@return TBool Returns whether the command was executed or not
*/
	{
	TBool			ret = ETrue;
	if(KCmdOpen() == aCommand)
		{
		DoCmdOpenL(aSection);
		}
	else if(KCmdClose() == aCommand)
		{
		DoCmdClose();
		}
	else if(KCmdAdd() == aCommand)
		{
		DoCmdAddL(aSection, aAsyncErrorIndex);
		}
	else if (KCmdRemove() == aCommand)
		{
		DoCmdRemoveL(aSection, aAsyncErrorIndex );
		}
	else if (KCmdEventNotification() == aCommand)
		{
		DoCmdEventNotificationL(aSection, aAsyncErrorIndex);
		}
	else if(KCmdCancelEventNotification() == aCommand)
		{
		DoCmdCancelEventNotification();
		}
	else if(KCmdGetParameters() == aCommand)
		{
		DoCmdGetParametersL(aSection);
		}
	else if(KCmdSetParameters() == aCommand)
		{
		DoCmdSetParametersL(aSection);
		}
	else if(KCmdControl() == aCommand)
		{
		DoCmdControlL(aSection);
		}
	else if( aCommand==KConstructor )
		{
		DoCmdConstructorL();
		}
	else if( aCommand==KDestructor )
		{
		DoCmdDestructor();
		}
	else
		{
		ret = EFalse;
		}

	return ret;
	}

TAny* CT_DataRSubConnection::GetObject()
	{
	return iSubConnection;
	}


void CT_DataRSubConnection::DoCmdConstructorL()
/**
Test code for RSubConnection Constructor

Ini file options:
	none

@internalComponent
*/
	{
	if( iSubConnection )
		{
		delete iSubConnection;
		iSubConnection = NULL;
		}

	INFO_PRINTF1(_L("Calling RSubConnection Constructor"));
	TRAPD(err, iSubConnection = new (ELeave) RSubConnection());
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("RSubConnection Constructor failed with error %d"), err);
		SetError(err);
		}
	}

void CT_DataRSubConnection::DoCmdDestructor()
/**
Test code for RSubConnection Destructor

Ini file options:
	none

@internalComponent
*/
	{
	if( iSubConnection )
		{
		INFO_PRINTF1(_L("Calling RSubConnection Destructor"));
		delete iSubConnection;
		iSubConnection = NULL;
		}
	else
		{
		ERR_PRINTF1(_L("RSubConnection object invalid, can't call destructor"));
		SetBlockResult(EFail);
		}
	}


void CT_DataRSubConnection::DoCmdOpenL( const TDesC& aSection )
/**
Test code for RSubConnection::Open

Ini file options:
	socketserv 	- an RSocketServ dictionary section
	connection 	- an RConnection dictionary section
	subconntype - subconnection type to be used

@internalComponent
@param aSection Current ini file command section
*/
	{
	TPtrC		socketServName;

	if( !GetStringFromConfig(aSection, KRSocketServ(), socketServName) )
		{
		MissingParamError( KRSocketServ() );
		}
	else
		{
		RSocketServ*	socketServ = static_cast<RSocketServ*>(GetDataObjectL(socketServName));

		if (socketServ == NULL)
			{
			ERR_PRINTF1(_L("RSubConnection::Open() - RSocketServ is NULL"));
			SetBlockResult(EFail);
			}

		TPtrC connName;

		if( !GetStringFromConfig( aSection, KRConnection(), connName) )
			{
			MissingParamError( KRConnection() );
			}
		else
			{
			RConnection*	conn = static_cast<RConnection*>(GetDataObjectL(connName));

			if (conn == NULL)
				{
				ERR_PRINTF1(_L("RSubConnection::Open() - RConnection is NULL"));
				SetBlockResult(EFail);
				}

			TPtrC 							subConnTypeStr;
			RSubConnection::TSubConnType	subConnType = RSubConnection::EAttachToDefault;

			if (GetStringFromConfig(aSection, KSubConnType(), subConnTypeStr))
				{
				if(SubConnectionType(subConnTypeStr, subConnType))
					{
					INFO_PRINTF3(_L("RSubConnection::Open() - Using subconntype %S (%d)"), &subConnTypeStr, subConnType);
					}
				else
					{
					TInt	subConnTypeInt;
					if (GetIntFromConfig( aSection, KSubConnType(), subConnTypeInt))
						{
						subConnType = (RSubConnection::TSubConnType)subConnTypeInt;
						INFO_PRINTF2(_L("RSubConnection::Open() - Using subconntype %d"), subConnType);
						}
					else
						{
						ERR_PRINTF2(_L("RSubConnection::Open() - Given subconntype (%S) is invalid"), &subConnTypeStr);
						SetBlockResult(EFail);
						}
					}
				}

			INFO_PRINTF4(_L("Calling RSubConnection::Open( %S, %d, %S)"), &socketServName, subConnType, &connName);
			TInt err = iSubConnection->Open(*socketServ, subConnType, *conn);

			if( err!=KErrNone )
				{
				ERR_PRINTF2(_L("RSubConnection::Open() failed with error %d"), err);
				SetError(err);
				}
			}
		}
	}


void CT_DataRSubConnection::DoCmdClose()
/**
Test code for RSubConnection::Close

Ini file options:
	none

@internalComponent
*/
	{
	INFO_PRINTF1(_L("Calling RSubConnection::Close()"));
	iSubConnection->Close();
	}


void CT_DataRSubConnection::DoCmdAddL( const TDesC& aSection, const TInt aAsyncErrorIndex )
/**
Test code for RSubConnection::Add

Ini file options:
	socket - an RSocket dictionary section

@internalComponent
@param aSection Current ini file command section
*/
	{
	TPtrC		socketName;

	if( !GetStringFromConfig( aSection, KRSocket(), socketName) )
		{
		MissingParamError( KRSocket() );
		}
	else
		{
		RSocket*	socket = static_cast<RSocket*>(GetDataObjectL(socketName));

		if (socket == NULL)
			{
			ERR_PRINTF1(_L("RSubConnection::Add() - RSocket is NULL"));
			SetBlockResult(EFail);
			}

		INFO_PRINTF2(_L("Calling RSubConnection::Add(%S)"), &socketName);
		iSubConnection->Add( *socket, iActiveAdd->iStatus );
		iActiveAdd->Activate( aAsyncErrorIndex );
		IncOutstanding();
		}
	}


void CT_DataRSubConnection::DoCmdRemoveL( const TDesC& aSection, const TInt aAsyncErrorIndex )
/**
Test code for RSubConnection::Remove

Ini file options:
	socket - an RSocket dictionary section

@internalComponent
@param aSection Current ini file command section
*/
	{
	TPtrC		socketName;

	if( !GetStringFromConfig( aSection, KRSocket(), socketName) )
		{
		MissingParamError( KRSocket() );
		}
	else
		{
		RSocket*	socket = static_cast<RSocket*>(GetDataObjectL(socketName));

		if (socket == NULL)
			{
			ERR_PRINTF1(_L("RSubConnection::Remove() - RSocket is NULL"));
			SetBlockResult(EFail);
			}

		INFO_PRINTF2(_L("Calling RSubConnection::Remove(%S)"), &socketName);
		iSubConnection->Remove( *socket, iActiveRemove->iStatus );
		iActiveRemove->Activate( aAsyncErrorIndex );
		IncOutstanding();
		}
	}


void CT_DataRSubConnection::DoCmdEventNotificationL( const TDesC& aSection, const TInt aAsyncErrorIndex )
/**
Test code for RSubConnection::EventNotification

Ini file options:
	genericeventsonly - Generic events only?
	eventmask%1		  - event mask to be used
	eventlistlength	  - event list length

@internalComponent
@param aSection Current ini file command section
*/
	{
	TBool	genericEventsOnly;
	if ( GetBoolFromConfig( aSection, KGenericEventsOnly(), genericEventsOnly) )
		{
		INFO_PRINTF2(_L("Calling RSubConnection::EventNotification(%d). Generic"), genericEventsOnly);
		iSubConnection->EventNotification(iActualEventBuf, genericEventsOnly, iActiveEventNotification->iStatus);
		iActiveEventNotification->Activate( aAsyncErrorIndex );
		IncOutstanding();
		}
	else
		{
		iEventFilterArray.Reset();
		TInt	eventListLength = 0;
		TBool	moreData=ETrue;
		while ( moreData )
			{
			TInt	index=eventListLength+1;
			TUint32	eventMask = 0;

			TPtrC	eventMaskName;
			TTempBuf	tempStore;
			tempStore.Format(KEventMask(), index);
			TBool	hasMask=GetStringFromConfig(aSection, tempStore, eventMaskName);
			if ( hasMask )
				{
				if ( !GetEventMaskBits(eventMaskName, eventMask) )
					{
					ERR_PRINTF2(_L("RSubConnection::EventNotification() - Given eventmask (%S) is invalid"), &eventMaskName);
					SetBlockResult(EFail);
					}
				}

			TPtrC	eventGroupUidStr;
			TInt32	eventGroupUid=KSubConnGenericEventsImplUid;
			tempStore.Format(KEventGroupUid(), index);
			TBool	hasUid=GetStringFromConfig(aSection, tempStore, eventGroupUidStr);
			if ( hasUid )
				{
				if( !GetEventGroupUid(eventGroupUidStr, eventGroupUid) )
					{
					TInt	eventGroupUidInt;
					if ( GetIntFromConfig(aSection, tempStore, eventGroupUidInt) )
						{
						eventGroupUid = (TInt32)eventGroupUidInt;
						}
					else
						{
						ERR_PRINTF2(_L("RSubConnection::EventNotification() - Given eventgroupuid (%S) is invalid"), &eventGroupUidStr);
						SetBlockResult(EFail);
						}
					}
				}

			moreData=hasMask;
			if ( moreData )
				{
				RSubConnection::TEventFilter	filter(eventGroupUid, eventMask);

				iEventFilterArray.AppendL(filter);
				++eventListLength;
				}
			}

		INFO_PRINTF2(_L("Calling RSubConnection::EventNotification(%d), EventList."), eventListLength);
		iSubConnection->EventNotification(iActualEventBuf, &iEventFilterArray[0], eventListLength, iActiveEventNotification->iStatus);
		iActiveEventNotification->Activate( aAsyncErrorIndex );
		IncOutstanding();
		}
	}


void CT_DataRSubConnection::DoCmdCancelEventNotification()
/**
Test code for RSubConnection::CancelEventNotification

Ini file options:
	none

@internalComponent
*/
	{
	INFO_PRINTF1(_L("Calling RSubConnection::CancelEventNotification()"));
	iSubConnection->CancelEventNotification();
	}


void CT_DataRSubConnection::DoCmdGetParametersL( const TDesC& aSection )
/**
Test code for RSubConnection::GetParameters

Ini file options:
	subconparameterbundle		- name of the parameter bundle to be used

@internalComponent
@param aSection Current ini file command section
*/
	{
	TPtrC 			subConParameterBundleName;

	if( !GetStringFromConfig( aSection, KSubConParameterBundleName(), subConParameterBundleName) )
		{
		MissingParamError( KSubConParameterBundleName() );
		}
	else
		{
		RSubConParameterBundle*	subConParameterBundle = static_cast<RSubConParameterBundle*>(GetDataObjectL(subConParameterBundleName));

		if (subConParameterBundle == NULL)
			{
			ERR_PRINTF1(_L("RSubConnection::GetParameters() - RSubConParameterBundle is NULL"));
			SetBlockResult(EFail);
			}

		INFO_PRINTF1(_L("Calling RSubConnection::GetParameters()"));
		TInt err = iSubConnection->GetParameters(*subConParameterBundle);

		if( err!=KErrNone )
			{
			ERR_PRINTF2(_L("RSubConnection::GetParameters() failed with error %d"), err);
			SetError(err);
			}
		}
	}


void CT_DataRSubConnection::DoCmdSetParametersL( const TDesC& aSection )
/**
Test code for RSubConnection::SetParameters

Ini file options:
	subconparameterbundle		- name of the parameter bundle to be used

@internalComponent
@param aSection Current ini file command section
*/
	{
	TPtrC 			subConParameterBundleName;

	if( !GetStringFromConfig( aSection, KSubConParameterBundleName(), subConParameterBundleName) )
		{
		MissingParamError( KSubConParameterBundleName() );
		}
	else
		{
		RSubConParameterBundle*	subConParameterBundle = static_cast<RSubConParameterBundle*>(GetDataObjectL(subConParameterBundleName));

		if (subConParameterBundle == NULL)
			{
			ERR_PRINTF1(_L("RSubConnection::SetParameters() - RSubConParameterBundle is NULL"));
			SetBlockResult(EFail);
			}

		TInt err = iSubConnection->SetParameters(*subConParameterBundle);

		if( err!=KErrNone )
			{
			ERR_PRINTF2(_L("RSubConnection::SetParameters() failed with error %d"), err);
			SetError(err);
			}
		}
	}


void CT_DataRSubConnection::DoCmdControlL( const TDesC& aSection )
/**
Test code for RSubConnection::Control

Ini file options:
	optionlevel 				- option level to be used
	optionname					- option name to be used
	option						- on return, the option value

@internalComponent
@param aSection Current ini file command section
@return TInt Error code
*/
	{

	TInt	optionLevel = 0;
	GetIntFromConfig( aSection, KOptionLevel(), optionLevel);

	TInt	optionName = 0;
	GetIntFromConfig( aSection, KOptionName(), optionName);

	TInt	bufferSize=KDefaultOptionBufferSize;
	GetIntFromConfig( aSection, KOptionBufferSize(), bufferSize);
	HBufC8*	optionBuffer = HBufC8::NewLC(bufferSize);
	TPtr8	optionPtr = optionBuffer->Des();

	INFO_PRINTF3(_L("Calling RSubConnection::Control(%d, %d)"), optionLevel, optionName);
	TInt err = iSubConnection->Control(optionLevel, optionName, optionPtr);

	if( err!=KErrNone )
		{
		ERR_PRINTF2(_L("RSubConnection::Control() failed with error %d"), err);
		SetError(err);
		}
	else
		{
		TPtrC	expectedOption;
		if(GetStringFromConfig( aSection, KOption(), expectedOption))
			{
			HBufC*	optionBuffer16 = HBufC::NewLC(bufferSize);
			TPtr	optionPtr16 = optionBuffer16->Des();
			optionPtr16.Copy(optionPtr);
			if( optionPtr16 != expectedOption)
				{
				ERR_PRINTF3(_L("Received [%S], expected [%S]"), &optionPtr16, &expectedOption);
				SetBlockResult(EFail);
				}
			}
		}

	CleanupStack::PopAndDestroy(optionBuffer);
	}


void CT_DataRSubConnection::RunL(CActive* aActive, TInt aIndex)
/**
 Virtual RunL - Called on completion of an asynchronous command
 @internalComponent
 @see MTPActiveCallback
 @leave system wide error code
*/
	{

	TBool	straySignal = EFalse;

	if( aActive==iActiveAdd )
 		{
		INFO_PRINTF1(_L("RunL Add called"));
 		}
 	else if( aActive==iActiveRemove )
 		{
		INFO_PRINTF1(_L("RunL Remove called"));
 		}
 	else if( aActive==iActiveEventNotification )
 		{
		INFO_PRINTF1(_L("RunL EventNotification called"));
 		}
 	else
 		{
 		ERR_PRINTF1(_L("Stray RunL signal"));
 		SetBlockResult(EFail);
 		straySignal = ETrue;
 		}

 	if( !straySignal )
 		{
		TInt	err = aActive->iStatus.Int();
		if( err != KErrNone )
			{
			ERR_PRINTF2(_L("RunL Error %d"), err);
			SetAsyncError( aIndex, err );
			}
		else
			{
			INFO_PRINTF1(_L("RunL call completed successfully"));
			}

		DecOutstanding();
 		}
	}


void CT_DataRSubConnection::DoCancel( CActive* aActive, TInt aIndex )
/**
 Virtual DoCancel - Request to cancel the asynchronous command
 @internalComponent
 @see - MActiveCallback
 @leave system wide error code
*/
	{

	TBool	straySignal = EFalse;

	if( aActive==iActiveAdd )
 		{
		INFO_PRINTF1(_L("DoCancel Add called"));
 		}
 	else if( aActive==iActiveRemove )
 		{
		INFO_PRINTF1(_L("DoCancel Remove called"));
 		}
 	else if( aActive==iActiveEventNotification )
 		{
		INFO_PRINTF1(_L("DoCancel EventNotification called"));
 		}
	else
 		{
 		ERR_PRINTF1(_L("Stray DoCancel signal"));
 		SetBlockResult(EFail);
 		straySignal = ETrue;
 		}

 	if( !straySignal )
 		{
		TInt	err = aActive->iStatus.Int();
		if( err != KErrNone )
			{
			ERR_PRINTF2(_L("DoCancel Error %d"), err);
			SetAsyncError( aIndex, err );
			}
		DecOutstanding();
 		}
	}
