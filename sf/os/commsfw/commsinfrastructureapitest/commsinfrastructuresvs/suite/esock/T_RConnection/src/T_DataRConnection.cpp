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


#include "T_DataRConnection.h"
#include "T_DataRSocketServ.h"

//	Epoc Includes
#include <es_enum.h>
#include <commdbconnpref.h>

/*@{*/
_LIT(KOpen,									"Open");
_LIT(KClose,								"Close");
_LIT(KStart,								"Start");
_LIT(KStop,									"Stop");
_LIT(KProgressNotification,					"ProgressNotification");
_LIT(KCancelProgressNotification,			"CancelProgressNotification");
_LIT(KProgress,								"Progress");
_LIT(KLastProgressError,					"LastProgressError");
_LIT(KServiceChangeNotification,			"ServiceChangeNotification");
_LIT(KCancelServiceChangeNotification,		"CancelServiceChangeNotification");
_LIT(KGetIntSetting,						"GetIntSetting");
_LIT(KGetBoolSetting,						"GetBoolSetting");
_LIT(KGetDesSetting,						"GetDesSetting");
_LIT(KGetLongDesSetting,					"GetLongDesSetting");
_LIT(KName,									"Name");
_LIT(KEnumerateConnections,					"EnumerateConnections");
_LIT(KGetConnectionInfo,					"GetConnectionInfo");
_LIT(KAllInterfaceNotification,				"AllInterfaceNotification");
_LIT(KCancelAllInterfaceNotification,		"CancelAllInterfaceNotification");
_LIT(KAttach,								"Attach");
_LIT(KConstructor,							"Constructor");
_LIT(KDestructor,							"~");

_LIT(KRSocketServ,							"socketserv");
_LIT(KRConnection,							"connection");
_LIT(KAsync,								"async");
_LIT(KExpected,								"expected");
_LIT(KExpectedStage,						"expectedstage");
_LIT(KExpectedError,						"expectederror");
_LIT(KExpectedIapId,						"expectediapid");
_LIT(KExpectedNetworkTable,					"expectednetworktable");
_LIT(KIndex,								"index");
_LIT(KSettingName,							"settingname");
_LIT(KExpectedNewISPId,						"expectednewispid");
_LIT(KExpectedNewServiceType,				"expectednewservicetype");
_LIT(KIapId,								"iap");
_LIT(KStopType,								"stoptype");
_LIT(KAttachType,							"attachtype");
_LIT(KUniCode,								"unicode");
_LIT(KIsFile,								"isfile");


const	TInt	KMaxSettingLen	= 256;

CT_DataRConnection* CT_DataRConnection::NewL()
	{
	CT_DataRConnection*	ret=new (ELeave) CT_DataRConnection();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_DataRConnection::CT_DataRConnection()
:	CT_DataCommsBase()
,	iActive(NULL)
,	iActiveProgress(NULL)
,	iActiveServiceChange(NULL)
,	iActiveAllInterface(NULL)
,	iConnection(NULL)
,	iActualNewISPId(0)
,	iExpectedNewISPId(0)
,	iExpectedStage(0)
	{
	}

void CT_DataRConnection::ConstructL()
	{
	iActive = CActiveCallback::NewL(*this);
	iActiveProgress = CActiveCallback::NewL(*this);
	iActiveServiceChange = CActiveCallback::NewL(*this);
	iActiveAllInterface = CActiveCallback::NewL(*this);
	}

CT_DataRConnection::~CT_DataRConnection()
	{
	delete iActive;
	iActive=NULL;
	delete iActiveProgress;
	iActiveProgress=NULL;
	delete iActiveServiceChange;
	iActiveServiceChange=NULL;
	delete iActiveAllInterface;
	iActiveAllInterface=NULL;
	delete iConnection;
	iConnection = NULL;
	}

TBool CT_DataRConnection::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
/**
Delegates the commands to a function

@internalComponent
@param aCommand Current command
@param aSection Current ini file command section
@param aAsyncErrorIndex index of command for SetAsyncError()
@return TBool Returns whether the command was executed or not
*/
	{
	TBool	ret = ETrue;
	TInt 	res	= KErrNone;

	if(KOpen() == aCommand)
		{
		res = DoCmdOpenL(aSection);
		}
	else if(KClose() == aCommand)
		{
		res = DoCmdClose();
		}
	else if(KStart() == aCommand)
		{
		res = DoCmdStart(aSection, aAsyncErrorIndex);
		}
	else if(KStop() == aCommand)
		{
		res = DoCmdStop(aSection);
		}
	else if(KProgressNotification() == aCommand)
		{
		res = DoCmdProgressNotification(aSection, aAsyncErrorIndex);
		}
	else if(KCancelProgressNotification() == aCommand)
		{
		res = DoCmdCancelProgressNotification();
		}
	else if(KProgress() == aCommand)
		{
		res = DoCmdProgress();
		}
	else if(KLastProgressError() == aCommand)
		{
		res = DoCmdLastProgressError(aSection);
		}
	else if(KServiceChangeNotification() == aCommand)
		{
		res = DoCmdServiceChangeNotification(aSection, aAsyncErrorIndex);
		}
	else if(KCancelServiceChangeNotification() == aCommand)
		{
		res = DoCmdCancelServiceChangeNotification();
		}
	else if(KGetIntSetting() == aCommand)
		{
		res = DoCmdGetIntSetting(aSection);
		}
	else if(KGetBoolSetting() == aCommand)
		{
		res = DoCmdGetBoolSetting(aSection);
		}
	else if(KGetDesSetting() == aCommand)
		{
		res = DoCmdGetDesSettingL(aSection);
		}
	else if(KGetLongDesSetting() == aCommand)
		{
		res = DoCmdGetLongDesSettingL(aSection);
		}
	else if(KName() == aCommand)
		{
		res = DoCmdName();
		}
	else if(KEnumerateConnections() == aCommand)
		{
		res = DoCmdEnumerateConnections(aSection);
		}
	else if(KGetConnectionInfo() == aCommand)
		{
		res = DoCmdGetConnectionInfo(aSection);
		}
	else if(KAllInterfaceNotification() == aCommand)
		{
		res = DoCmdAllInterfaceNotification( aAsyncErrorIndex );
		}
	else if(KCancelAllInterfaceNotification() == aCommand)
		{
		res = DoCmdCancelAllInterfaceNotification();
		}
	else if(KAttach() == aCommand)
		{
		res = DoCmdAttachL(aSection);
		}
	else if( aCommand==KConstructor )
		{
		res = DoCmdConstructorL();
		}
	else if( aCommand==KDestructor )
		{
		res = DoCmdDestructor();
		}
	else
		{
		ret = EFalse;
		}

	if( KErrNone != res )
		{
		ERR_PRINTF2(_L("Error = %d"), res);
		SetError( res );
		}

	return ret;
	}

TAny* CT_DataRConnection::GetObject()
	{
	return iConnection;
	}


TInt CT_DataRConnection::DoCmdConstructorL()
/**
Test wrapper for RConnection Constructor

Ini file options:
	none

@internalComponent
@return TInt Error code
*/
	{
	if( iConnection )
		{
		delete iConnection;
		iConnection = NULL;
		}

	INFO_PRINTF1(_L("Calling RConnection Constructor"));
	TRAPD(err, iConnection = new (ELeave) RConnection());
	return err;
	}

TInt CT_DataRConnection::DoCmdDestructor()
/**
Test wrapper for RConnection Destructor

Ini file options:
	none

@internalComponent
@return TInt Error code
*/
	{
	TInt	res = KErrNone;

	if( iConnection )
		{
		INFO_PRINTF1(_L("Calling RConnection Destructor"));
		delete iConnection;
		iConnection = NULL;
		}
	else
		{
		ERR_PRINTF1(_L("RConnection object invalid, can't call destructor"));
		SetBlockResult(EFail);
		}

	return res;
	}


TInt CT_DataRConnection::DoCmdOpenL( const TDesC& aSection )
/**
Test wrapper for RConnection::Open

Ini file options:
	socketserv - an RSocketServ dictionary entry

@internalComponent
@param aEntry Current ini file command entry
@return TInt Error code
*/
	{
	TInt		res = KErrNone;
	TPtrC		socketServName;

	if( !GetStringFromConfig( aSection, KRSocketServ(), socketServName) )
		{
		MissingParamError( KRSocketServ() );
		}
	else
		{
		RSocketServ*	socketServ = static_cast<RSocketServ*>(GetDataObjectL(socketServName));
		TPtrC			connName;

		if( GetStringFromConfig( aSection, KRConnection(), connName) )
			{
			RConnection*	conn = static_cast<RConnection*>(GetDataObjectL(connName));
			_LIT_SECURITY_POLICY_C2(KProcPolicy, ECapabilityNetworkServices, ECapabilityNetworkControl);

			TSecurityPolicyBuf secPolicyBuf;
			secPolicyBuf().Set(KProcPolicy().Package());

			conn->Control(KCOLConnection, KCoEnableCloneOpen, secPolicyBuf);

			TName name;
			res = conn->Name( name );
			if( KErrNone == res )
				{
				res = iConnection->Open( *socketServ, name );
				}
			}
		else
			{
			res = iConnection->Open( *socketServ );
			}
		}

	return res;
	}

TInt CT_DataRConnection::DoCmdClose()
/**
Test wrapper for RConnection::Close

@internalComponent
@return TInt Error code
*/
	{
	iConnection->Close();
	return KErrNone;
	}

TInt CT_DataRConnection::DoCmdStart(const TDesC& aSection, const TInt aAsyncErrorIndex)
/**
Test wrapper for RConnection::Start

Ini file options:
	async - determines if an asynchronous Start call is to be made
	iap - The IAP to use

@internalComponent
@param aEntry Current ini file command entry
@return TInt Error code
*/
	{
	TInt		res = KErrNone;
	TBool		async = EFalse;
	TInt 		iapId = 0;

	if ( !GetBoolFromConfig( aSection, KAsync(), async) )
		{
		MissingParamError( KAsync() );
		}
	else
		{
		if ( GetIntFromConfig( aSection, KIapId(), iapId) )
			{
			TCommDbConnPref	prefs;
			prefs.SetIapId( iapId );
			prefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
			if( async )
				{
				iConnection->Start(prefs, iActive->iStatus);
				iActive->Activate(aAsyncErrorIndex);
				IncOutstanding();
				}
			else
				{
				res = iConnection->Start(prefs);
				}
			}
		else
			{
			if( async )
				{
				iConnection->Start(iActive->iStatus);
				iActive->Activate(aAsyncErrorIndex);
				IncOutstanding();
				}
			else
				{
				res = iConnection->Start();
				}
			}
		}
	return res;
	}


TInt CT_DataRConnection::DoCmdStop( const TDesC& aSection )
/**
Test wrapper for RConnection::Stop

Ini file options:
	subconnection - an RSubConnection dictionary entry
	stoptype - the stop type to be used

@internalComponent
@param aEntry Current ini file command entry
@return TInt Error code
*/
	{
	TInt		res = KErrNone;
	TPtrC		stopType;

	if( GetStringFromConfig(aSection, KStopType(), stopType) )
		{
		res = iConnection->Stop( ConnectionStopType(stopType) );
		}
	else
		{
		res = iConnection->Stop();
		}

	return res;
	}

TInt CT_DataRConnection::DoCmdProgressNotification( const TDesC& aSection, const TInt aAsyncErrorIndex )
/**
Test wrapper for RConnection::ProgressNotification

Ini file options:
	expectederror - expected error
	expectedstage - expected stage
	subconnection - an RSubConnection dictionary entry

@internalComponent
@param aEntry Current ini file command entry
@return TInt Error code
*/
	{
	TPtrC		stage;

	iExpectedStage=0;
	if( GetStringFromConfig( aSection, KExpectedStage(), stage) )
		{
		iExpectedStage=ProgressNotificationExpectedStage(stage);
		}

	if( 0 == iExpectedStage )
		{
		iConnection->ProgressNotification( iActualProgressBuf, iActiveProgress->iStatus );
		}
	else
		{
		iConnection->ProgressNotification( iActualProgressBuf, iActiveProgress->iStatus, iExpectedStage );
		}

	iActiveProgress->Activate(aAsyncErrorIndex);
	IncOutstanding();

	return KErrNone;
	}

TInt CT_DataRConnection::DoCmdCancelProgressNotification()
/**
Test wrapper for RConnection::CancelProgressNotification

@internalComponent
@param aEntry Current ini file command entry
@return TInt Error code
*/
	{
	TInt		res = KErrNone;
	
	iConnection->CancelProgressNotification();

	return res;
	}

TInt CT_DataRConnection::DoCmdProgress()
/**
Test wrapper for RConnection::Progress

Ini file options:
	expectederror - expected error
	expectedstage - expected stage

@internalComponent
@param aEntry Current ini file command entry
@return TInt Error code
*/
	{
	TInt			res = KErrNone;
	TNifProgress	progress;

	res = iConnection->Progress( progress );

	if( KErrNone == res )
		{
		INFO_PRINTF3(_L("Progress response, error = %d & stage = %d")
								,progress.iError
								,progress.iStage);
		}

	return res;
	}

TInt CT_DataRConnection::DoCmdLastProgressError( const TDesC& aSection )
/**
Test wrapper for RConnection::LastProgressError

Ini file options:
	expectederror - expected error
	expectedstage - expected stage

@internalComponent
@param aEntry Current ini file command entry
@return TInt Error code
*/
	{
	TInt		res = KErrNone;
	TInt		expectedStage;
	TInt		expectedError;
	TBool		dataOk=ETrue;

	if ( !GetIntFromConfig(aSection, KExpectedStage(), expectedStage) )
		{
		MissingParamError( KExpectedStage() );
		dataOk=EFalse;
		}

	if ( !GetIntFromConfig(aSection, KExpectedError(), expectedError) )
		{
		MissingParamError( KExpectedError() );
		dataOk=EFalse;
		}

	if ( dataOk )
		{
		TNifProgress	progress;
		res = iConnection->LastProgressError(progress);

		if( KErrNone == res )
			{
			if( progress.iStage != expectedStage )
				{
				ERR_PRINTF3(_L("Expected stage = %d, actual stage = %d"), expectedStage, progress.iStage);
				SetBlockResult(EFail);
				}
			if( progress.iError != expectedError )
				{
				ERR_PRINTF3(_L("Expected error = %d, actual error = %d"), expectedError, progress.iError);
				SetBlockResult(EFail);
				}
			}
		}

	return res;
	}

TInt CT_DataRConnection::DoCmdServiceChangeNotification(const TDesC& aSection, const TInt aAsyncErrorIndex)
/**
Test wrapper for RConnection::ServiceChangeNotification

Ini file options:
	expectednewispid - expected new ISP
	expectednewservicetype - expected new service type

@internalComponent
@param aEntry Current ini file command entry
@return TInt Error code
*/
	{
	TInt		res = KErrNone;
	TBool		dataOk=ETrue;

	if ( !GetIntFromConfig( aSection, KExpectedNewISPId(), iExpectedNewISPId) )
		{
		MissingParamError( KExpectedNewISPId() );
		dataOk=EFalse;
		}

	if ( !GetStringFromConfig( aSection, KExpectedNewServiceType(), iExpectedNewServiceType ) )
		{
		MissingParamError( KExpectedNewServiceType() );
		dataOk=EFalse;
		}

	if ( dataOk )
		{
		iConnection->ServiceChangeNotification( iActualNewISPId, iActualNewServiceType, iActiveServiceChange->iStatus );
		iActiveServiceChange->Activate(aAsyncErrorIndex);
		IncOutstanding();
		}

	return res;
	}

TInt CT_DataRConnection::DoCmdCancelServiceChangeNotification()
/**
Test wrapper for RConnection::CancelServiceChangeNotification

@internalComponent
*/
	{
	iConnection->CancelServiceChangeNotification();
	return KErrNone;
	}

TInt CT_DataRConnection::DoCmdGetIntSetting( const TDesC& aSection )
/**
Test wrapper for RConnection::GetIntSetting

Ini file options:
	settingname - the name of the setting
	expected - expected setting value

@internalComponent
@param aEntry Current ini file command entry
@return TInt Error code
*/
	{
	TInt		res = KErrNone;
	TPtrC		settingName;
	TUint32 	actual = 0;
	TInt		expected;
	TBool		dataOk=ETrue;

	if ( !GetStringFromConfig(aSection, KSettingName(), settingName) )
		{
		MissingParamError( KSettingName() );
		dataOk=EFalse;
		}

	if ( !GetIntFromConfig(aSection, KExpected(), expected) )
		{
		MissingParamError( KExpected() );
		dataOk=EFalse;
		}

	if ( dataOk )
		{
		res = iConnection->GetIntSetting( settingName, actual );
		if( res==KErrNone )
			{
			if ( (TInt)actual!=expected )
				{
				ERR_PRINTF3(_L("Expected = %d, actual = %d"), expected, actual);
				SetBlockResult(EFail);
				}
			}
		}
	return res;
	}

TInt CT_DataRConnection::DoCmdGetBoolSetting(const TDesC& aSection)
/**
Test wrapper for RConnection::GetBoolSetting

Ini file options:
	settingname - the name of the setting
	expected - expected setting value

@internalComponent
@param aEntry Current ini file command entry
@return TInt Error code
*/
	{
	TInt		res = KErrNone;
	TPtrC		settingName;
	TBool	 	actual = EFalse;
	TBool	 	expected = EFalse;
	TBool		dataOk=ETrue;

	if ( !GetStringFromConfig(aSection, KSettingName(), settingName) )
		{
		MissingParamError( KSettingName() );
		dataOk=EFalse;
		}

	if ( !GetBoolFromConfig(aSection, KExpected(), expected) )
		{
		MissingParamError( KExpected() );
		dataOk=EFalse;
		}

	if ( dataOk )
		{
		res = iConnection->GetBoolSetting( settingName, actual );
		if( res==KErrNone )
			{
			if ( actual!=expected )
				{
				ERR_PRINTF3(_L("Expected = %d, actual = %d"), expected, actual);
				SetBlockResult(EFail);
				}
			}
		}

	return res;
	}

TInt CT_DataRConnection::DoCmdGetDesSettingL( const TDesC& aSection )
/**
Test wrapper for RConnection::GetDesSetting

Ini file options:
	settingname - the name of the setting
	expected - expected setting value

@internalComponent
@param aEntry Current ini file command entry
@return TInt Error code
*/
	{
	TInt		res=KErrNone;
	TBool		dataOk=ETrue;

	TPtrC		settingName;
	if ( !GetStringFromConfig(aSection, KSettingName(), settingName) )
		{
		MissingParamError( KSettingName() );
		dataOk=EFalse;
		}

	TPtrC	 	expected;
	if ( !GetStringFromConfig(aSection, KExpected(), expected) )
		{
		MissingParamError( KExpected() );
		dataOk=EFalse;
		}

	TBool		unicode = EFalse;
	if ( !GetBoolFromConfig(aSection, KUniCode(), unicode) )
		{
		MissingParamError( KUniCode() );
		dataOk=EFalse;
		}

	if ( dataOk )
		{
		TBuf<KMaxSettingLen>	actual;
		if( unicode )
			{
			res = iConnection->GetDesSetting(settingName, actual);
			}
		else
			{
			TBuf8<KMaxSettingLen>	actual8;
			res = iConnection->GetDesSetting(settingName, actual8);

			if ( res==KErrNone )
				{
				actual.Copy(actual8);
				}
			}
		if ( res==KErrNone )
			{
			if ( actual.Compare(expected)!=0 )
				{
				ERR_PRINTF3(_L("Length Expected = %d, actual = %d"), expected.Length(), actual.Length());
				ERR_PRINTF3(_L("Expected = <%S>, actual = <%S>"), &expected, &actual);
				SetBlockResult(EFail);
				}
			}
		}

	return res;
	}

TInt CT_DataRConnection::DoCmdGetLongDesSettingL( const TDesC& aSection )
/**
Test wrapper for RConnection::GetLongDesSetting

Ini file options:
	settingname - the name of the setting
	expected - expected setting value

@internalComponent
@param aEntry Current ini file command entry
@return TInt Error code
*/
	{
	TInt					res = KErrNone;
	TPtrC					settingName;
	TPtrC	 				expected;
	TInt					lengthOfData = 0;
	TBuf<KMaxSettingLen>	actual;
	TBool					dataOk=ETrue;

	if ( !GetStringFromConfig(aSection, KSettingName(), settingName) )
		{
		MissingParamError( KSettingName() );
		dataOk=EFalse;
		}

	if ( !GetStringFromConfig(aSection, KExpected(), expected) )
		{
		MissingParamError( KExpected() );
		dataOk=EFalse;
		}

	if ( dataOk )
		{
		TBool	isFile=EFalse;
		GetBoolFromConfig(aSection, KIsFile(), isFile);
		if(isFile)
			{
			//if read string is a file name
			//Read the data from file in to Heap to match it with the actual data stored in heap.
			RFile	file;
			RFs     fileSession;
			User::LeaveIfError(fileSession.Connect());
			CleanupClosePushL(fileSession);
			TInt err = file.Open(fileSession, expected, EFileShareAny);
			CleanupClosePushL(file);

			if( KErrNone == err )
				{
				User::LeaveIfError(file.Size(lengthOfData));
				HBufC8*	expectedBuffer = HBufC8::NewLC(lengthOfData);
				TPtr8	expectedPtr = expectedBuffer->Des();
				err = file.Read( expectedPtr );

				if(KErrNone != err )
					{
					ERR_PRINTF1(_L("Failed to open the data file"));
					SetBlockResult(EFail);
					}
				else
					{
					HBufC16*	actualBuffer = HBufC16::NewLC( KMaxSettingLen );
					TPtr16		actualPtr = actualBuffer->Des();
					res = iConnection->GetLongDesSetting( settingName, actualPtr );

					if( KErrNone == res )
						{
						HBufC16*	expectedBuffer16 = HBufC16::NewLC( expectedPtr.Length() );
						TPtr16		expectedPtr16 = expectedBuffer16->Des();
						expectedPtr16.Copy( expectedPtr );

						if( actualPtr != expectedPtr16 )
							{
							ERR_PRINTF3(_L("Expected = %S, actual = %S"), &expectedPtr16, &actualPtr);
							SetBlockResult(EFail);
							}

						CleanupStack::PopAndDestroy(expectedBuffer16);
						}

					CleanupStack::PopAndDestroy(actualBuffer);
					}

				CleanupStack::PopAndDestroy(expectedBuffer);
				}

			file.Close();
			fileSession.Close();

			CleanupStack::PopAndDestroy(&file);
			CleanupStack::PopAndDestroy(&fileSession);
			}
		else
			{
			//if read string is a data to be read in
			res = iConnection->GetLongDesSetting( settingName, actual );
			if( KErrNone == res && actual != expected )
				{
				ERR_PRINTF3(_L("Expected = <%S>, actual = <%S>"), &expected, &actual);
				SetBlockResult(EFail);
				}

			}
		}

	return res;
	}

TInt CT_DataRConnection::DoCmdName()
/**
Test wrapper for RConnection::Name

@internalComponent
@return TInt Error code
*/
	{
	TName	name;
	TInt res = iConnection->Name( name );
	if( KErrNone == res )
		{
		INFO_PRINTF2(_L("Name = %S"), &name);
		}
	return res;
	}

TInt CT_DataRConnection::DoCmdEnumerateConnections( const TDesC& aSection )
/**
Test wrapper for RConnection::EnumerateConnections

Ini file options:
	expected - expected number of connections

@internalComponent
@param aEntry Current ini file command entry
@return TInt Error code
*/
	{
	TInt		res = KErrNone;
	TInt 		expected = 0;
	TUint 		actual = 0;

	res = iConnection->EnumerateConnections( actual );
	if( KErrNone != res  )
		{
		ERR_PRINTF2(_L("EnumerateConnections failed with error %d"), res);
		SetError(res);
		}
	else
		{
		INFO_PRINTF2(_L("Number of currently active interfaces = %d"), actual);
		if ( GetIntFromConfig(aSection, KExpected(), expected) )
			{
			if( ((TInt)actual) != expected )
				{
				ERR_PRINTF3(_L("Expected active interface = %d, actual = %d"), expected, actual);
				SetBlockResult(EFail);
				}
			}
		}

	return res;
	}

TInt CT_DataRConnection::DoCmdGetConnectionInfo( const TDesC& aSection )
/**
Test wrapper for RConnection::GetConnectionInfo

Ini file options:
	index - the index of the connection to retrieve info from
	expectediapid - expected IAP id
	expectednetid - expected Net id

@internalComponent
@param aEntry Current ini file command entry
@return TInt Error code
*/
	{
	TInt		res = KErrNone;
	TInt		index = 0;
	TPtrC		iapSection;
	TInt 		expectedIapId;
	TInt 		expectedNetId = 0;
	TBool		dataOk=ETrue;

	if( !GetIntFromConfig(aSection, KIndex(), index) )
		{
		MissingParamError( KIndex() );
		dataOk=EFalse;
		}

	if ( !GetIntFromConfig(aSection, KExpectedIapId(), expectedIapId) )
		{
		MissingParamError( KIndex() );
		dataOk=EFalse;
		}

	if ( !GetIntFromConfig(aSection, KExpectedNetworkTable(), expectedNetId) )
		{
		MissingParamError( KIndex() );
		dataOk=EFalse;
		}

	if ( dataOk )
		{
		TPckgBuf<TConnectionInfo>	connInfoBuf;
		res = iConnection->GetConnectionInfo(index, connInfoBuf);
		if( KErrNone == res )
			{
			INFO_PRINTF2(_L("Number Active Connection = %d"), connInfoBuf().iIapId);
			if( ((TInt)connInfoBuf().iNetId) != expectedNetId )
				{
				ERR_PRINTF3(_L("Expected NetId = %d, actual NetId = %d"), expectedNetId, connInfoBuf().iNetId);
				SetBlockResult(EFail);
				}
			}
		}

	return res;
	}

TInt CT_DataRConnection::DoCmdAllInterfaceNotification( const TInt aAsyncErrorIndex )
/**
Test wrapper for RConnection::AllInterfaceNotification

Ini file options:
	expectedstate - the expected state

@internalComponent
@param aEntry Current ini file command entry
@return TInt Error code
*/
	{
	iConnection->AllInterfaceNotification( iActualIfNotification, iActiveAllInterface->iStatus );
	iActiveAllInterface->Activate( aAsyncErrorIndex );
	IncOutstanding();

	return KErrNone;
	}

TInt CT_DataRConnection::DoCmdCancelAllInterfaceNotification()
/**
Test wrapper for RConnection::CancelAllInterfaceNotification

@internalComponent
@return TInt Error code
*/
	{
	iConnection->CancelAllInterfaceNotification();
	return KErrNone;
	}

TInt CT_DataRConnection::DoCmdAttachL( const TDesC& aSection )
/**
Test wrapper for RConnection::Attach

Ini file options:
	attachtype - the attach type
	connection - an RConnection dictionary entry
	index - the index of the connection to retrieve info from
			(this is required for the Attach call)

@internalComponent
@param aEntry Current ini file command entry
@return TInt Error code
*/
	{
	TInt		res = KErrNone;
	TPtrC		connName;
	TPtrC		attachType;
	TInt		index;
	TBool		dataOk=ETrue;

	if( !GetStringFromConfig(aSection, KAttachType(), attachType) )
		{
		MissingParamError( KAttachType() );
		dataOk=EFalse;
		}

	if( !GetStringFromConfig(aSection, KRConnection(), connName) )
		{
		MissingParamError( KRConnection() );
		dataOk=EFalse;
		}

	if( !GetIntFromConfig(aSection, KIndex(), index) )
		{
		MissingParamError( KIndex() );
		dataOk=EFalse;
		}

	if ( dataOk )
		{
		TPckgBuf<TConnectionInfo>	connInfoBuf;
		RConnection*				conn = static_cast<RConnection*>(GetDataObjectL(connName));
		if( KErrNone != conn->GetConnectionInfo(index, connInfoBuf) )
			{
			ERR_PRINTF1(_L("GetConnectionInfo() failed on the second RConnection object"));
			SetBlockResult(EFail);
			}
		else
			{
			res = iConnection->Attach(connInfoBuf, ConnectionAttachType(attachType));
			}
		}
	return res;
	}

void CT_DataRConnection::RunL(CActive* aActive, TInt aIndex)
/**
 Virtual RunL - Called on completion of an asynchronous command
 @internalComponent
 @see MActiveCallback
 @leave system wide error code
*/
	{
	INFO_PRINTF1(_L("CT_DataRConnection::RunL"));
	if( aActive == iActive )
		{
		TInt err=iActive->iStatus.Int();
		if( KErrNone != err )
			{
			ERR_PRINTF2(_L("RunL Error = %d"), err);
			SetAsyncError( aIndex, err );
			}
		// Reset the outstanding request state
		DecOutstanding();
		}
	else if( aActive == iActiveProgress )
		{
		CompleteProgressNotification(aActive, aIndex);
		}
	else if( aActive == iActiveServiceChange )
		{
		CompleteServiceChangeNotification( aIndex );
		}
	else if( aActive == iActiveAllInterface )
		{
		CompleteAllInterfaceNotification( aIndex );
		}
	}

void CT_DataRConnection::DoCancel(CActive* /*aActive*/, TInt aIndex)
/**
 Virtual DoCancel - Request to cancel the asynchronous command
 @internalComponent
 @see - MActiveCallback
 @leave system wide error code
*/
	{
	ERR_PRINTF1(_L("Asynchronous call cancelled"));
	SetAsyncError( aIndex, KErrCancel );
	DecOutstanding();
	}

void CT_DataRConnection::CompleteProgressNotification(CActive* aActive, TInt aIndex)
/**
Handle ProgressNotification completion

@internalComponent
*/
	{
	TInt err=aActive->iStatus.Int();
	if( KErrNone != err )
		{
		ERR_PRINTF2(_L("ProgressNotification Error = %d"), err);
		SetAsyncError(aIndex ,err);
		}
	else if( 0 != iExpectedStage )
		{
		if( iActualProgressBuf().iStage!=(TInt)iExpectedStage )
			{
			ERR_PRINTF3(_L("ProgressNotification incorrect, expected stage = %d, actual stage = %d")
								,iExpectedStage
								,iActualProgressBuf().iStage );
			SetBlockResult( EFail );
			}
		else
			{
			INFO_PRINTF3(_L("ProgressNotification response, error = %d & stage = %d")
									,iActualProgressBuf().iError
									,iActualProgressBuf().iStage);
			INFO_PRINTF1(_L("ProgressNotification completed successfully"));
			}
		}
	else
		{
		INFO_PRINTF3(_L("ProgressNotification response, error = %d & stage = %d")
								,iActualProgressBuf().iError
								,iActualProgressBuf().iStage);
		INFO_PRINTF1(_L("ProgressNotification completed successfully"));
		}
	DecOutstanding();
	}

void CT_DataRConnection::CompleteServiceChangeNotification(TInt aIndex)
/**
Handle ServiceChangeNotification completion

@internalComponent
*/
	{
	TInt err=iActiveServiceChange->iStatus.Int();
	if( KErrNone != err )
		{
		ERR_PRINTF2(_L("ServiceChange Error = %d"), err);
		SetAsyncError( aIndex, err );
		}
	else if( (iExpectedNewISPId != (TInt)iActualNewISPId)  ||
			 (iExpectedNewServiceType != iActualNewServiceType) )
		{
		ERR_PRINTF5(_L("ServiceChange notification response incorrect, expected ISP ID = %d & service type = %S, actual ISP ID = %d & service type = %S")
								,iExpectedNewISPId
								,&iExpectedNewServiceType
								,iActualNewISPId
								,&iActualNewServiceType);
		SetBlockResult( EFail );
		}
	else
		{
		INFO_PRINTF1(_L("ServiceChangeNotification completed successfully"));
		}
	DecOutstanding();
	}

void CT_DataRConnection::CompleteAllInterfaceNotification( TInt aIndex)
/**
Handle AllInterfaceNotification completion

@internalComponent
*/
	{
	TInt err=iActiveAllInterface->iStatus.Int();
	if( KErrNone != err )
		{
		ERR_PRINTF2(_L("AllInterfaceNotification Error = %d"), err);
		SetAsyncError( aIndex, err );
		}
	else
		{
		INFO_PRINTF2(_L("AllInterfaceNotification response, state = %d")
								,iActualIfNotification().iState);
		}
	DecOutstanding();
	}
