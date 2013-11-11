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

#include "T_TCommCapsV02Data.h"

/*@{*/
_LIT(KStrNull,								" ");

_LIT(KStrKCapsFlowControlStatusSupported,	"KCapsFlowControlStatusSupported");
const CDataWrapperBase::TEnumEntryTable	CT_TCommCapsV02Data::iEnumTableFlowControlCaps[] =
	{
//	Enum as a descriptor					Enum
	KStrKCapsFlowControlStatusSupported,	KCapsFlowControlStatusSupported,
	KStrNull,								-1
	};

_LIT(KStrKNotifySignalsChangeSupported,		"KNotifySignalsChangeSupported");
_LIT(KStrKNotifyRateChangeSupported,		"KNotifyRateChangeSupported");
_LIT(KStrKNotifyDataFormatChangeSupported,	"KNotifyDataFormatChangeSupported");
_LIT(KStrKNotifyHandshakeChangeSupported,	"KNotifyHandshakeChangeSupported");
_LIT(KStrKNotifyBreakSupported,				"KNotifyBreakSupported");
_LIT(KStrKNotifyFlowControlChangeSupported,	"KNotifyFlowControlChangeSupported");
_LIT(KStrKNotifyDataAvailableSupported,		"KNotifyDataAvailableSupported");
_LIT(KStrKNotifyOutputEmptySupported,		"KNotifyOutputEmptySupported");
const CDataWrapperBase::TEnumEntryTable	CT_TCommCapsV02Data::iEnumTableNotificationCaps[] =
	{
//	Enum as a descriptor					Enum
	KStrKNotifySignalsChangeSupported,		KNotifySignalsChangeSupported,
	KStrKNotifyRateChangeSupported,			KNotifyRateChangeSupported,
	KStrKNotifyDataFormatChangeSupported,	KNotifyDataFormatChangeSupported,
	KStrKNotifyHandshakeChangeSupported,	KNotifyHandshakeChangeSupported,
	KStrKNotifyBreakSupported,				KNotifyBreakSupported,
	KStrKNotifyFlowControlChangeSupported,	KNotifyFlowControlChangeSupported,
	KStrKNotifyDataAvailableSupported,		KNotifyDataAvailableSupported,
	KStrKNotifyOutputEmptySupported,		KNotifyOutputEmptySupported,
	KStrNull,								-1
	};

_LIT(KStrKCapsRoleSwitchSupported,			"KCapsRoleSwitchSupported");
const CDataWrapperBase::TEnumEntryTable	CT_TCommCapsV02Data::iEnumTableRoleCaps[] =
	{
//	Enum as a descriptor					Enum
	KStrKCapsRoleSwitchSupported,			KCapsRoleSwitchSupported,
	KStrNull,								-1
	};

_LIT(KCmdConstructor,						"new");
_LIT(KCmdDestructor,						"~");
_LIT(KCmdiFlowControlCaps,					"iFlowControlCaps");
_LIT(KCmdiNotificationCaps,					"iNotificationCaps");
_LIT(KCmdiRoleCaps,							"iRoleCaps");

_LIT(KFldExpected,							"expected");

_LIT(KLogError,								"Error=%d");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_TCommCapsV02Data::CT_TCommCapsV02Data()
:	CT_TCommCapsV01Data()
,	iCommCaps2(NULL)
	{
	}

CT_TCommCapsV02Data::~CT_TCommCapsV02Data()
/**
 * Public destructor
 */
	{
	DestroyData();
	}

void CT_TCommCapsV02Data::DestroyData()
	{
	delete iCommCaps2;
	iCommCaps2=NULL;
	}

TDes8* CT_TCommCapsV02Data::Descriptor()
/**
 * Return a pointer to the buffer
 *
 * @return	pointer to the buffer
 */
	{
	return iCommCaps2;
	}

TAny* CT_TCommCapsV02Data::GetObject()
/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
	{
	return iCommCaps2;
	}

TBool CT_TCommCapsV02Data::ReadFlowControlCaps(const TDesC& aSection, const TDesC& aKeyName, TUint& aFlowControlCaps)
	{
	return GetOrFromConfig(aSection, aKeyName, iEnumTableFlowControlCaps, aFlowControlCaps);
	}

TBool CT_TCommCapsV02Data::ReadNotificationCaps(const TDesC& aSection, const TDesC& aKeyName, TUint& aNotificationCaps)
	{
	return GetOrFromConfig(aSection, aKeyName, iEnumTableNotificationCaps, aNotificationCaps);
	}

TBool CT_TCommCapsV02Data::ReadRoleCaps(const TDesC& aSection, const TDesC& aKeyName, TUint& aRoleCaps)
	{
	return GetOrFromConfig(aSection, aKeyName, iEnumTableRoleCaps, aRoleCaps);
	}

TBool CT_TCommCapsV02Data::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
/**
 * Process a command read from the ini file
 *
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return					ETrue if the command is processed
 *
 * @leave					System wide error
 */
	{
	TBool	ret=ETrue;
	TInt	err=KErrNone;

	if ( aCommand==KCmdConstructor )
		{
		err=DoCmdConstructor();
		}
	else if ( aCommand==KCmdDestructor )
		{
		DoCmdDestructor();
		}
	else if ( aCommand==KCmdiFlowControlCaps )
		{
		DoCmdiFlowControlCaps(aSection);
		}
	else if ( aCommand==KCmdiNotificationCaps )
		{
		DoCmdiNotificationCaps(aSection);
		}
	else if ( aCommand==KCmdiRoleCaps )
		{
		DoCmdiRoleCaps(aSection);
		}
	else
		{
		ret=CT_TCommCapsV01Data::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}

	if ( err!=KErrNone )
		{
		ERR_PRINTF2(KLogError, err);
		SetError(err);
		}

	return ret;
	}

TCommCapsV01& CT_TCommCapsV02Data::GetCommCapsV01()
	{
	return (*iCommCaps2)();
	}

TCommCapsV02& CT_TCommCapsV02Data::GetCommCapsV02()
	{
	return (*iCommCaps2)();
	}

TInt CT_TCommCapsV02Data::DoCmdConstructor()
	{
	DestroyData();
	TRAPD(err, iCommCaps2=new (ELeave) TCommCaps2());
	iCommCaps2->FillZ();
	return err;
	}

void CT_TCommCapsV02Data::DoCmdDestructor()
	{
	DestroyData();
	}

void CT_TCommCapsV02Data::DoCmdiFlowControlCaps(const TDesC& aSection)
	{
	TUint	actual=GetCommCapsV02().iFlowControlCaps;
	INFO_PRINTF2(_L("iFlowControlCaps : 0x%x"), actual);

	TUint	expected;
	if ( ReadFlowControlCaps(aSection, KFldExpected(), expected) )
		{
		if ( expected!=actual )
			{
			ERR_PRINTF1(_L("Expected Value does not match actual"));
			SetBlockResult(EFail);
			}
		}
	}

void CT_TCommCapsV02Data::DoCmdiNotificationCaps(const TDesC& aSection)
	{
	TUint	actual=GetCommCapsV02().iNotificationCaps;
	INFO_PRINTF2(_L("iNotificationCaps : 0x%x"), actual);

	TUint	expected;
	if ( ReadNotificationCaps(aSection, KFldExpected(), expected) )
		{
		if ( expected!=actual )
			{
			ERR_PRINTF1(_L("Expected Value does not match actual"));
			SetBlockResult(EFail);
			}
		}
	}

void CT_TCommCapsV02Data::DoCmdiRoleCaps(const TDesC& aSection)
	{
	TUint	actual=GetCommCapsV02().iRoleCaps;
	INFO_PRINTF2(_L("iRoleCaps : 0x%x"), actual);

	TUint	expected;
	if ( ReadRoleCaps(aSection, KFldExpected(), expected) )
		{
		if ( expected!=actual )
			{
			ERR_PRINTF1(_L("Expected Value does not match actual"));
			SetBlockResult(EFail);
			}
		}
	}
