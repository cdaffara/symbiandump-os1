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


#include "T_DataRSocketServ.h"


//Constant Literals used.

_LIT(KNamedProtocol,							"name");
_LIT(KMessageSlots,								"messageslots");
_LIT(KSockType,									"socktype");
_LIT(KProtocol,									"protocol");
_LIT(KAddrFamily,								"addrfamily");
_LIT(KIndex,									"index");
_LIT(KDLLName,									"dllname");
_LIT(KArgs,										"args");
_LIT(KExpectedValue,							"expectedvalue");
_LIT(KExpectedValueName,						"expectedvalue_name");
_LIT(KExpectedValueAddrFamily,					"expectedvalue_addrfamily");
_LIT(KExpectedValueProtocol,					"expectedvalue_protocol");
_LIT(KExpectedVersionName,						"expected_version_name");
_LIT(KExpectedVersionBuild,						"expected_version_build");
_LIT(KExpectedVersionMajor,						"expected_version_major");
_LIT(KExpectedVersionMinor,						"expected_version_minor");

_LIT(KCmdClearExclusiveMode,					"ClearExclusiveMode");
_LIT(KCmdConnect,								"Connect");
_LIT(KCmdFindProtocol,							"FindProtocol");
_LIT(KCmdGetProtocolInfo,						"GetProtocolInfo");
_LIT(KCmdInstallExtension,						"InstallExtension");
_LIT(KCmdNumProtocols,							"NumProtocols");
_LIT(KCmdSetExclusiveMode,						"SetExclusiveMode");
_LIT(KCmdStartProtocol,							"StartProtocol");
_LIT(KCmdStopProtocol,							"StopProtocol");
_LIT(KCmdVersion,								"Version");
_LIT(KCmdClose,									"Close");
_LIT(KConstructor,								"Constructor");
_LIT(KDestructor,								"~");


/*@}*/

CT_DataRSocketServ* CT_DataRSocketServ::NewL()
	{
	CT_DataRSocketServ*	ret=new (ELeave) CT_DataRSocketServ();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_DataRSocketServ::CT_DataRSocketServ()
:	CT_DataCommsBase()
,	iSocketServ(NULL)
,	iActiveStartProtocol(NULL)
,	iActiveStopProtocol(NULL)
,	iActiveSetExclusiveMode(NULL)
,	iNumOfProtocols(0)
	{
	}
	
void CT_DataRSocketServ::ConstructL()
	{
	iActiveStartProtocol = CActiveCallback::NewL(*this);
	iActiveStopProtocol = CActiveCallback::NewL(*this);
	iActiveSetExclusiveMode = CActiveCallback::NewL(*this);
	iProtocolDesc.iAddrFamily = 0;
	iProtocolDesc.iSockType = 0;
	iProtocolDesc.iProtocol = 0;
	}


CT_DataRSocketServ::~CT_DataRSocketServ()
	{
	delete iActiveStartProtocol;
	iActiveStartProtocol = NULL;

	delete iActiveStopProtocol;
	iActiveStopProtocol = NULL;

	delete iActiveSetExclusiveMode;
	iActiveSetExclusiveMode = NULL;
	
	delete iSocketServ;
	iSocketServ = NULL;
	
	}


TBool CT_DataRSocketServ::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
/**
Delegates the commands to function

@internalComponent
@param aCommand Current command
@param aSection Current ini file command entry
@param aSynchronous Updated if an asynchronous call is used
@return TBool Returns whether the command was executed or not
*/
    TBool	ret=ETrue;
	
	if( aCommand==KCmdClearExclusiveMode )
		{
		DoCmdClearExclusiveMode();
		}
	else if( aCommand==KCmdConnect )
		{
		DoCmdConnect(aSection);
		}
	else if( aCommand==KCmdFindProtocol )
		{
		DoCmdFindProtocol(aSection);
		}
	else if( aCommand==KCmdGetProtocolInfo )
		{
		DoCmdGetProtocolInfo(aSection);
		}
	else if( aCommand==KCmdInstallExtension )
		{
		DoCmdInstallExtension(aSection);
		}
	else if( aCommand==KCmdNumProtocols )
		{
		DoCmdNumProtocols(aSection);
		}
	else if( aCommand==KCmdSetExclusiveMode )
		{
		DoCmdSetExclusiveMode(aAsyncErrorIndex);
		}
	else if( aCommand==KCmdStartProtocol )
		{
		DoCmdStartProtocolL(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KCmdStopProtocol )
		{
		DoCmdStopProtocolL(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KCmdVersion )
		{
		DoCmdVersion(aSection);
		}
	else if( aCommand==KCmdClose )
		{
		INFO_PRINTF1(_L("Calling RSocketServ::Close()"));			
		iSocketServ->Close();
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
		ret=EFalse;
		}

	return ret;
	}
	
TAny* CT_DataRSocketServ::GetObject()
	{
	return iSocketServ;
	}
	
void CT_DataRSocketServ::DoCmdConstructorL()
	{
	delete iSocketServ;
	iSocketServ = NULL;	

	INFO_PRINTF1(_L("Calling RSocketServ Constructor"));
	TRAPD(err, iSocketServ = new (ELeave) RSocketServ());
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("RSocketServ Constructor failed with error %d"), err);
		SetError(err);
		}
	}
	
void CT_DataRSocketServ::DoCmdDestructor()
	{
	if( iSocketServ )
		{
		INFO_PRINTF1(_L("Calling RSocketServ Destructor"));
		delete iSocketServ;
		iSocketServ = NULL;
		}
	else
		{
		ERR_PRINTF1(_L("RSocketServ object invalid, can't call destructor"));
		SetBlockResult(EFail);
		}
	}

void CT_DataRSocketServ::DoCmdClearExclusiveMode()
	{	
	INFO_PRINTF1(_L("Calling RSocketServ::ClearExclusiveMode()"));			
	iSocketServ->ClearExclusiveMode();
	}
	
void CT_DataRSocketServ::DoCmdConnect(const TDesC& aSection)
	{	
	TInt messageSlots=KESockDefaultMessageSlots;
	GetIntFromConfig(aSection, KMessageSlots(), messageSlots);

	TUint	protocol = 0;
	TPtrC	protocolStr;
	TBool	hasProtocol = GetStringFromConfig(aSection, KProtocol(), protocolStr);
	if( hasProtocol )
		{
		if( GetProtocol( protocolStr, protocol ) )
			{
			INFO_PRINTF3(_L("RSocketServ::Connect() - Using protocol %S (%d)"), &protocolStr, protocol);
			}
		else
			{
			TInt	protocolInt;
			if( GetIntFromConfig(aSection, KProtocol(), protocolInt) )
				{
				protocol = protocolInt;
				INFO_PRINTF2(_L("RSocketServ::Connect() - Using protocol %d"), protocol);
				}
			else
				{
				ERR_PRINTF2(_L("RSocketServ::Connect() - Given protocol (%S) is invalid"), &protocolStr);
				SetBlockResult(EFail);
				}
			}
		}
					
	TUint	addrFamily = 0;
	TPtrC	addrFamilyStr;
	TBool	hasAddrFamily = GetStringFromConfig(aSection, KAddrFamily(), addrFamilyStr);	
	if( hasAddrFamily )
		{
		if( GetAddrFamily( addrFamilyStr, addrFamily ) )
			{
			INFO_PRINTF3(_L("RSocketServ::Connect() - Using addrFamily %S (%d)"), &addrFamilyStr, addrFamily);
			}
		else
			{
			TInt	addrFamilyInt;
			if( GetIntFromConfig(aSection, KAddrFamily(), addrFamilyInt) )
				{
				addrFamily = addrFamilyInt;
				INFO_PRINTF2(_L("RSocketServ::Connect() - Using addrFamily %d"), addrFamily);
				}
			else
				{
				ERR_PRINTF2(_L("RSocketServ::Connect() - Given addrFamily (%S) is invalid"), &addrFamilyStr);
				SetBlockResult(EFail);
				}
			}
		}

	TInt err;

	if( hasProtocol || hasAddrFamily )
		{
		TSessionPref preference;
		preference.iProtocol = protocol;
		preference.iAddrFamily = addrFamily;
		INFO_PRINTF4(_L("Calling RSocketServ::Connect({%d, %d}, %d)"), preference.iProtocol, preference.iAddrFamily, messageSlots);			
		err = iSocketServ->Connect(preference, messageSlots);		
		}
	else
		{
		INFO_PRINTF2(_L("Calling RSocketServ::Connect(%d)"), messageSlots);			
		err = iSocketServ->Connect(messageSlots);			
		}

	if( err!=KErrNone )
		{
		ERR_PRINTF2(_L("RSocketServ::Connect() failed with error %d"), err);
		SetError(err);	
		}
	}

void CT_DataRSocketServ::DoCmdFindProtocol(const TDesC& aSection)
	{	
	TInt			err = KErrNone;
	TPtrC			protocolName;
		
	if( GetStringFromConfig(aSection, KNamedProtocol(), protocolName) )
		{
		// We have specified a protocol name
		INFO_PRINTF2(_L("Calling RSocketServ::FindProtocol(%S)"), &protocolName);			
		err = iSocketServ->FindProtocol(protocolName, iProtocolDesc);
		if( err!=KErrNone )
			{
			ERR_PRINTF3(_L("RSocketServ::FindProtocol(%S) failed with error %d"), &protocolName, err);
			SetError(err);	
			}
		else
			{
			PrintProtocol(_L("RSocketServ::FindProtocol()"), iProtocolDesc, ETrue);
					
			// Get the expected results from the ini file
			TPtrC	expectedName;
			if( GetStringFromConfig(aSection, KExpectedValueName(), expectedName) )
				{
				if( expectedName != iProtocolDesc.iName )
					{
					ERR_PRINTF4(_L("RSocketServ::FindProtocol(%S) expected name(%S) != actual name(%S)"), &protocolName, &expectedName, &iProtocolDesc.iName);
					SetBlockResult(EFail);	
					}
				}

			TUint	expectedAddrFamily;
			TPtrC	expectedAddrFamilyStr;
			if( GetStringFromConfig(aSection, KExpectedValueAddrFamily(), expectedAddrFamilyStr) )
				{
				if( GetAddrFamily( expectedAddrFamilyStr, expectedAddrFamily ) )
					{
					INFO_PRINTF3(_L("RSocketServ::FindProtocol() - Using expected addrFamily %S (%d)"), &expectedAddrFamilyStr, expectedAddrFamily);
					if( expectedAddrFamily != iProtocolDesc.iAddrFamily )
						{
						ERR_PRINTF4(_L("RSocketServ::FindProtocol(%S) expected Addr Family(%d) != actual Addr Family(%d)"), &protocolName, expectedAddrFamily, iProtocolDesc.iAddrFamily);
						SetBlockResult(EFail);	
						}
					}
				else
					{
					ERR_PRINTF2(_L("RSocketServ::FindProtocol() - Given addrFamily (%S) is invalid"), &expectedAddrFamilyStr);
					SetBlockResult(EFail);
					}
				}

			TUint	expectedProtocol;
			TPtrC	expectedProtocolStr;
			if( GetStringFromConfig(aSection, KExpectedValueProtocol(), expectedProtocolStr) )
				{
				if( GetProtocol( expectedProtocolStr, expectedProtocol ) )
					{
					INFO_PRINTF3(_L("RSocketServ::FindProtocol() - Using expected protocol %S (%d)"), &expectedProtocolStr, expectedProtocol);
					if( expectedProtocol != iProtocolDesc.iProtocol )
						{
						ERR_PRINTF4(_L("RSocketServ::FindProtocol(%S) expected protocol(%d) != actual protocol(%d)"), &protocolName, expectedProtocol, iProtocolDesc.iProtocol);
						SetBlockResult(EFail);	
						}
					}
				else
					{
					ERR_PRINTF2(_L("RSocketServ::FindProtocol() - Given protocol (%S) is invalid"), &expectedProtocolStr);
					SetBlockResult(EFail);
					}
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("RSocketServ::FindProtocol() error - No %S given"), &KNamedProtocol());
		SetBlockResult(EFail);	
		}
	}
	
void CT_DataRSocketServ::DoCmdGetProtocolInfo(const TDesC& aSection)
	{	
	TInt	start = 1;
	TInt	end = iNumOfProtocols;
	
	TInt	index;
	if( GetIntFromConfig(aSection, KIndex(), index) )
		{
		start = index;
		end = index;
		}

	for( TInt i=start; i<=end; ++i )
		{
		INFO_PRINTF2(_L("Calling RSocketServ::GetProtocolInfo(%d)"), i);   
		TInt	err = iSocketServ->GetProtocolInfo(i, iProtocolDesc);
		if( err!=KErrNone )
			{
			ERR_PRINTF3(_L("RSocketServ::GetProtocolInfo(%d) failed with error %d"), i, err);
			SetError(err); 
			}
		else
			{
			PrintProtocol(_L("RSocketServ::GetProtocolInfo()"), iProtocolDesc);
			}
		}
	}

void CT_DataRSocketServ::DoCmdInstallExtension(const TDesC& aSection)
	{	
	TPtrC	dllName;
	if( GetStringFromConfig(aSection, KDLLName(), dllName) )
		{
		TPtrC	args = TPtrC();
		GetStringFromConfig(aSection, KArgs(), args);
		
		INFO_PRINTF3(_L("Calling RSocketServ::InstallExtension(%S, %S)"), &dllName, &args);			

		TInt	err = iSocketServ->InstallExtension(dllName, args);
		if( err!=KErrNone )
			{
			ERR_PRINTF2(_L("RSocketServ::InstallExtension() failed with error %d"), err);
			SetError(err);	
			}
		}
	else
		{
		ERR_PRINTF1(_L("No DLL Name"));
		SetBlockResult(EFail);	
		}
	}

void CT_DataRSocketServ::DoCmdNumProtocols(const TDesC& aSection)
	{	
	iNumOfProtocols = 0;
	INFO_PRINTF1(_L("Calling RSocketServ::NumProtocols()"));			
	TInt	err = iSocketServ->NumProtocols(iNumOfProtocols);
	
	if( err!=KErrNone )
		{
		ERR_PRINTF2(_L("RSocketServ::NumProtocols() failed with error %d"), err);
		SetError(err);	
		}
	else
		{
		INFO_PRINTF2(_L("RSocketServ::NumProtocols() = %d"), iNumOfProtocols);			

		TInt	expectedValue;
		if( GetIntFromConfig(aSection, KExpectedValue(), expectedValue) )
			{
			if( (TInt)iNumOfProtocols != expectedValue )
				{
				ERR_PRINTF3(_L("Expected NumProtocols (%d) != Actual NumProtocols (%d)"), expectedValue, iNumOfProtocols);
				SetBlockResult(EFail);	
				}
			}
		}			
	}

void CT_DataRSocketServ::DoCmdSetExclusiveMode( const TInt aAsyncErrorIndex )
	{
	INFO_PRINTF1(_L("Calling RSocketServ::SetExclusiveMode()"));
	iSocketServ->SetExclusiveMode(iActiveSetExclusiveMode->iStatus);
	iActiveSetExclusiveMode->Activate(aAsyncErrorIndex);
	IncOutstanding();
	}

void CT_DataRSocketServ::DoCmdStartProtocolL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
				
	TUint	addrFamily = iProtocolDesc.iAddrFamily;
	TPtrC	addrFamilyStr;
	if( GetStringFromConfig(aSection, KAddrFamily(), addrFamilyStr) )
		{
		if( GetAddrFamily( addrFamilyStr, addrFamily ) )
			{
			INFO_PRINTF3(_L("RSocketServ::StartProtocol() - Using addrFamily %S (%d)"), &addrFamilyStr, addrFamily);
			}
		else
			{
			TInt	addrFamilyInt;
			if( GetIntFromConfig(aSection, KAddrFamily(), addrFamilyInt) )
				{
				addrFamily = addrFamilyInt;
				INFO_PRINTF2(_L("RSocketServ::StartProtocol() - Using addrFamily %d"), addrFamily);
				}
			else
				{
				ERR_PRINTF2(_L("RSocketServ::StartProtocol() - Given addrFamily (%S) is invalid"), &addrFamilyStr);
				SetBlockResult(EFail);
				}
			}
		}

	TUint	sockType = iProtocolDesc.iSockType;
	TPtrC	sockTypeStr;
	if( GetStringFromConfig(aSection, KSockType(), sockTypeStr) )
		{
		if( GetSockType( sockTypeStr, sockType ) )
			{
			INFO_PRINTF3(_L("RSocketServ::StartProtocol() - Using sockType %S (%d)"), &sockTypeStr, sockType);
			}
		else
			{
			TInt	sockTypeInt;
			if( GetIntFromConfig(aSection, KSockType(), sockTypeInt) )
				{
				sockType = sockTypeInt;
				INFO_PRINTF2(_L("RSocketServ::StartProtocol() - Using sockType %d"), sockType);
				}
			else
				{
				ERR_PRINTF2(_L("RSocketServ::StartProtocol() - Given sockType (%S) is invalid"), &sockTypeStr);
				SetBlockResult(EFail);
				}
			}
		}

	TUint	protocol = iProtocolDesc.iProtocol;
	TPtrC	protocolStr;
	if( GetStringFromConfig(aSection, KProtocol(), protocolStr) )
		{
		if( GetProtocol( protocolStr, protocol ) )
			{
			INFO_PRINTF3(_L("RSocketServ::StartProtocol() - Using protocol %S (%d)"), &protocolStr, protocol);
			}
		else
			{
			TInt	protocolInt;
			if( GetIntFromConfig(aSection, KProtocol(), protocolInt) )
				{
				protocol = protocolInt;
				INFO_PRINTF2(_L("RSocketServ::StartProtocol() - Using protocol %d"), protocol);
				}
			else
				{
				ERR_PRINTF2(_L("RSocketServ::StartProtocol() - Given protocol (%S) is invalid"), &protocolStr);
				SetBlockResult(EFail);
				}
			}
		}
						
	INFO_PRINTF4(_L("Calling RSocketServ::StartProtocol(%d, %d, %d)"), addrFamily, sockType, protocol);
	iSocketServ->StartProtocol(addrFamily, sockType, protocol, iActiveStartProtocol->iStatus);
	iActiveStartProtocol->Activate(aAsyncErrorIndex);
	IncOutstanding();
	}

void CT_DataRSocketServ::DoCmdStopProtocolL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
				
	TUint	addrFamily = iProtocolDesc.iAddrFamily;
	TPtrC	addrFamilyStr;
	if( GetStringFromConfig(aSection, KAddrFamily(), addrFamilyStr) )
		{
		if( GetAddrFamily( addrFamilyStr, addrFamily ) )
			{
			INFO_PRINTF3(_L("RSocketServ::StopProtocol() - Using addrFamily %S (%d)"), &addrFamilyStr, addrFamily);
			}
		else
			{
			TInt	addrFamilyInt;
			if( GetIntFromConfig(aSection, KAddrFamily(), addrFamilyInt) )
				{
				addrFamily = addrFamilyInt;
				INFO_PRINTF2(_L("RSocketServ::StopProtocol() - Using addrFamily %d"), addrFamily);
				}
			else
				{
				ERR_PRINTF2(_L("RSocketServ::StopProtocol() - Given addrFamily (%S) is invalid"), &addrFamilyStr);
				SetBlockResult(EFail);
				}
			}
		}

	TUint	sockType = iProtocolDesc.iSockType;
	TPtrC	sockTypeStr;
	if( GetStringFromConfig(aSection, KSockType(), sockTypeStr) )
		{
		if( GetSockType( sockTypeStr, sockType ) )
			{
			INFO_PRINTF3(_L("RSocketServ::StopProtocol() - Using sockType %S (%d)"), &sockTypeStr, sockType);
			}
		else
			{
			TInt	sockTypeInt;
			if( GetIntFromConfig(aSection, KSockType(), sockTypeInt) )
				{
				sockType = sockTypeInt;
				INFO_PRINTF2(_L("RSocketServ::StopProtocol() - Using sockType %d"), sockType);
				}
			else
				{
				ERR_PRINTF2(_L("RSocketServ::StopProtocol() - Given sockType (%S) is invalid"), &sockTypeStr);
				SetBlockResult(EFail);
				}
			}
		}

	TUint	protocol = iProtocolDesc.iProtocol;
	TPtrC	protocolStr;
	if( GetStringFromConfig(aSection, KProtocol(), protocolStr) )
		{
		if( GetProtocol( protocolStr, protocol ) )
			{
			INFO_PRINTF3(_L("RSocketServ::StopProtocol() - Using protocol %S (%d)"), &protocolStr, protocol);
			}
		else
			{
			TInt	protocolInt;
			if( GetIntFromConfig(aSection, KProtocol(), protocolInt) )
				{
				protocol = protocolInt;
				INFO_PRINTF2(_L("RSocketServ::StopProtocol() - Using protocol %d"), protocol);
				}
			else
				{
				ERR_PRINTF2(_L("RSocketServ::StopProtocol() - Given protocol (%S) is invalid"), &protocolStr);
				SetBlockResult(EFail);
				}
			}
		}

	INFO_PRINTF4(_L("Calling RSocketServ::StopProtocol(%d, %d, %d)"), addrFamily, sockType, protocol);
	iSocketServ->StopProtocol(addrFamily, sockType, protocol, iActiveStopProtocol->iStatus);
	iActiveStopProtocol->Activate(aAsyncErrorIndex);
	IncOutstanding();
	}

void CT_DataRSocketServ::DoCmdVersion(const TDesC& aSection)
	{	

	INFO_PRINTF1(_L("Calling RSocketServ::Version()"));			
	TVersion				version = iSocketServ->Version();
	TBuf<KMaxVersionName>	versionName(version.Name());

	INFO_PRINTF2(_L("Version name  : %S"), &versionName);
	INFO_PRINTF2(_L("Version build : %d"), (TInt)version.iBuild);
	INFO_PRINTF2(_L("Version major : %d"), (TInt)version.iMajor);
	INFO_PRINTF2(_L("Version minor : %d"), (TInt)version.iMinor);			 

	TPtrC	expectedVersionName;
	if( GetStringFromConfig(aSection, KExpectedVersionName(), expectedVersionName) )
		{
		if( version.Name() != expectedVersionName )
			{
			ERR_PRINTF3(_L("Expected Version Name (%S) != Actual Version Name (%S)"), &expectedVersionName, &versionName);
			SetBlockResult(EFail);	
			}
		}
		
	TInt	intTemp;
	if( GetIntFromConfig(aSection, KExpectedVersionBuild(), intTemp) )
		{
		if( version.iBuild != intTemp )
			{
			ERR_PRINTF3(_L("Expected Version Build (%d) != Actual Version Build (%d)"), &intTemp, version.iBuild);
			SetBlockResult(EFail);	
			}
		}
		
	if( GetIntFromConfig(aSection, KExpectedVersionMajor(), intTemp) )
		{
		if( version.iMajor != intTemp )
			{
			ERR_PRINTF3(_L("Expected Version Major (%d) != Actual Version Major (%d)"), &intTemp, version.iMajor);
			SetBlockResult(EFail);	
			}
		}
	
	if( GetIntFromConfig(aSection, KExpectedVersionMinor(), intTemp) )
		{
		if( version.iMinor != intTemp )
			{
			ERR_PRINTF3(_L("Expected Version Minor (%d) != Actual Version Minor (%d)"), &intTemp, version.iMinor);
			SetBlockResult(EFail);	
			}
		}

	if (version.Name() == _L("") && version.iBuild == 0 && version.iMajor == 0 && version.iMinor == 0)
		{
		ERR_PRINTF1(_L("Some version fields are not set!"));
		SetBlockResult(EFail);
		}
	}

	
/**
 Virtual RunL - Called on completion of an asynchronous command
 @internalComponent
 @see MT_MMActiveCallback
 @param aActive Active Object that RunL has been called on
 @pre N/A
 @post N/A
 @leave system wide error code
*/
void CT_DataRSocketServ::RunL(CActive* aActive, TInt aAsyncErrorIndex)
 	{
	TBool	straySignal = EFalse;

	if( aActive==iActiveStartProtocol )
		{
		INFO_PRINTF1(_L("RunL StartProtocol called"));
		}
 	else if( aActive==iActiveStopProtocol )
 		{
		INFO_PRINTF1(_L("RunL StopProtocol called"));
 		}
 	else if( aActive==iActiveSetExclusiveMode )
 		{
		INFO_PRINTF1(_L("RunL SetExclusiveMode called"));
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
		if ( err != KErrNone )
			{
			ERR_PRINTF2(_L("RunL Error %d"), err);
			SetAsyncError(aAsyncErrorIndex, err);
			}
		DecOutstanding();
 		}
 	}

/**
  Virtual DoCancel - Request to cancel the asynchronous command
  @internalComponent
  @see - MTPActiveCallback
  @param aActive Active Object that DoCancel has been called on
  @pre - N/A
  @post - N/A
  @leave system wide error code
 */
 void CT_DataRSocketServ::DoCancel(CActive* aActive, TInt aAsyncErrorIndex)
 	{
	TBool	straySignal = EFalse;

	if( aActive==iActiveStartProtocol )
		{
		INFO_PRINTF1(_L("DoCancel StartProtocol called"));
		}
 	else if( aActive==iActiveStopProtocol )
 		{
		INFO_PRINTF1(_L("DoCancel StopProtocol called"));
 		}
 	else if( aActive==iActiveSetExclusiveMode )
 		{
		INFO_PRINTF1(_L("DoCancel SetExclusiveMode called"));
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
		if ( err != KErrNone )
			{
			ERR_PRINTF2(_L("DoCancel Error %d"), err);
			SetAsyncError(aAsyncErrorIndex, err);
			}
		DecOutstanding();
 		}
 	}
