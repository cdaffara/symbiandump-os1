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

#include "T_RConsoleData.h"


/*@{*/
//wrappers
_LIT(KCmdRead,				"Read");
_LIT(KCmdWrite,				"Write");
_LIT(KCmdControl,			"Control");
_LIT(KCmdInit,				"Init");
_LIT(KCmdDestroy,			"Destroy");
_LIT(KCmdCreate,			"Create");
_LIT(KCmdConstructor,		"new");
_LIT(KCmdDestructor,		"~");
_LIT(KCmdClearScreen,		"ClearScreen");

//ini sections
_LIT(KFldText,				"text");
_LIT(KFldConsoleControl,	"consoleControl");
_LIT(KFldConsoleName,		"consoleName");
_LIT(KFldConsoleSize,		"consoleSize");
_LIT(KFldSynchronous,		"synchronous");

//format fields
_LIT(KLogMissingParameter,	"Missing parameter '%S'");
_LIT(KLogError,				"Error=%d");
_LIT(KLogStringNotFound,	"String not found %S");
_LIT(KLogStraySignal,		"Stray signal");
_LIT(KLogAsyncError,		"Async RunL Error %d");
_LIT(KLogDataMissing,		"Data missing for test");
/*@}*/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_RConsoleData* CT_RConsoleData::NewL()
	{
	CT_RConsoleData* console=new (ELeave) CT_RConsoleData();
	CleanupStack::PushL(console);
	console->ConstructL();
	CleanupStack::Pop(console);
	return console;
	}

CT_RConsoleData::CT_RConsoleData()
:	CT_RSessionBaseData()
,	iActiveRConsoleRead(NULL)
,	iConsole(NULL)
	{
	}

void CT_RConsoleData::ConstructL()
/**
 * Second phase construction
 *
 * @internalComponent
 *
 * @return	N/A
 *
 * @pre		None
 * @post	None
 *
 * @leave	system wide error
 */
	{
	iActiveRConsoleRead	= CT_ActiveRConsoleRead::NewL(*this);
	}

CT_RConsoleData::~CT_RConsoleData()
/**
 * Public destructor
 */
	{
	DestroyData();
	delete iActiveRConsoleRead;
	iActiveRConsoleRead= 	NULL;
	}

void CT_RConsoleData::DestroyData()
 /**
 *	RConsole Destruction
 *
 */
	{
 	delete	iConsole;
 	iConsole	=NULL;
	}


 TAny* CT_RConsoleData::GetObject()
/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
	{
	return iConsole;
	}

RSessionBase* CT_RConsoleData::GetSessionBase()
	{
	return iConsole;
	}

RHandleBase* CT_RConsoleData::GetHandleBase()
	{
	return iConsole;
	}

TBool CT_RConsoleData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
/**
 * Process a command read from the ini file
 *
 * @param aCommand                      The command to process
 * @param aSection                      The section in the ini containing data for the command
 * @param aAsyncErrorIndex              Command index for async calls to return errors to
 *
 * @return                              ETrue if the command is processed
 *
 * @leave                               System wide error
 */
	{
	TBool 	ret	=ETrue;
	TInt	error=KErrNone;

	if (aCommand==KCmdConstructor())
		{
		error	=DoCmdConstructor();
		}
	else if ( aCommand==KCmdDestructor )
		{
		DoCmdDestructor();
		}
	else if ( aCommand==KCmdRead())
		{
		error	=DoCmdRead(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdWrite())
		{
		error	=DoCmdWrite(aSection);
		}
	else if ( aCommand==KCmdDestroy())
		{
		error	=DoCmdDestroy();
		}
	else if ( aCommand==KCmdControl())
		{
		error	=DoCmdControl(aSection);
		}
	else if ( aCommand==KCmdInit())
		{
		error	=DoCmdInit(aSection);
		}
	else if ( aCommand==KCmdCreate())
		{
		error	=DoCmdCreate();
		}
	else if ( aCommand==KCmdClearScreen())
		{
		error	=DoCmdClearScreen();
		}
	else
		{
		ret=CT_RSessionBaseData::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}

	if ( error!=KErrNone )
		{
		ERR_PRINTF2(KLogError, error);
		SetError(error);
		}

	return ret;
	}

TInt CT_RConsoleData::DoCmdConstructor()
 /**
 *	Wrapper function for RConsole Construction
 *
 */
	{
	DestroyData();
	TRAPD(err, iConsole=new (ELeave) RConsole());
	return err;
	}

void CT_RConsoleData::DoCmdDestructor()
 /**
 *	Wrapper function for RConsole Destruction
 *
 */
	{
	DestroyData();
	}

TInt CT_RConsoleData::DoCmdRead(const TDesC& aSection , const TInt aAsyncErrorIndex)
 /**
 *	Wrapper function for RConsole::Read(TConsoleKey &aKeystroke,TRequestStatus &aStatus)
 * 	and RConsole::Read(TConsoleKey &aKeystroke)
 *
 *	@param		aSection			The section in the ini containing data for the command
 * 	@param		aAsyncErrorIndex	Command index for async calls to return errors to
 */
 	{
 	TBool	sync=EFalse;
	GETFROMCONFIGOPTIONAL(Bool, aSection, KFldSynchronous(), sync, KLogMissingParameter);

	TInt	error=KErrNone;
	if ( !sync )
		{
		if ( !iActiveRConsoleRead->KickStartL(aSection, aAsyncErrorIndex, *iConsole) )
			{
			ERR_PRINTF1(KLogDataMissing);
			SetBlockResult(EFail);
			}
		else
			{
			IncOutstanding();
			}
		}
	else
		{
		TConsoleKey	key;
		error=iConsole->Read(key);
		}
	return error;
 	}

TInt CT_RConsoleData::DoCmdWrite(const TDesC& aSection )
 /**
 *	Wrapper function for RConsole::Write(const TDesC &aDes)
 *	@param		aSection			The section in the ini containing data for the command
 */
 	{
	TBool dataOk	=EFalse;
	TInt error		=KErrNone;
	TPtrC string;
	GETSTRINGFROMCONFIGMANDATORY(aSection, KFldText(), string, KLogStringNotFound,  dataOk);
	if ( dataOk==KErrNone)
		{
		error	=iConsole->Write(string);
		}
	return error;
 	}

TInt CT_RConsoleData::DoCmdControl(const TDesC& aSection)
 /**
 *	Wrapper function for RConsole::Control(const TDesC &aDes)
 *	@param		aSection			The section in the ini containing data for the command
 */
	{
	TBool dataOk	=EFalse;
	TInt error	=KErrNone;
	TPtrC control;
	GETSTRINGFROMCONFIGMANDATORY(aSection, KFldConsoleControl(), control, KLogStringNotFound, dataOk);
	if (dataOk==KErrNone)
		{
		error	=iConsole->Control(control);
		}
	return error;
	}

TInt CT_RConsoleData::DoCmdInit(const TDesC& aSection)
 /**
 *	Wrapper function for RConsole::Init(const TDesC &aName,const TSize &aSize
 *	@param		aSection			The section in the ini containing data for the command
 */
	{

	TBool	dataOk=ETrue;

	TPtrC	name;
	GETSTRINGFROMCONFIGMANDATORY(aSection, KFldConsoleName(), name, KLogMissingParameter, dataOk);

	TSize	size(KConsFullScreen, KConsFullScreen);
	GetSizeFromConfig(aSection, KFldConsoleSize(), size);
	TInt	error=KErrNone;
	if ( dataOk )
		{
		error=iConsole->Init(name, size);
		}

	return error;
	}

TInt CT_RConsoleData::DoCmdDestroy()
 /**
 *	Wrapper function for RConsole::Destroy()
 */
	{
	return iConsole->Destroy();
	}

TInt CT_RConsoleData::DoCmdCreate()
 /**
 *	Wrapper function for RConsole::Create()
 */
	{
	return iConsole->Create();
	}

TInt CT_RConsoleData::DoCmdClearScreen()
 /**
 *	Wrapper function for RConsole::ClearScreen()
 *
 */
	{
	return iConsole->ClearScreen();
	}

void CT_RConsoleData::RunL(CActive* aActive, TInt aIndex)
 /**
 * Called on completion of an asynchronous command
 * @param               aActive Active Object that RunL has been called on
 * @param               aIndex number of the command.
 * @pre                 N/A
 * @post                N/A
 * @leave               system wide error code
 */
	{
	TBool	activeOk=ETrue;
	TInt	err=KErrNone;
	if ( aActive!=NULL )
		{
		err=aActive->iStatus.Int();
		}

	TBool	moreToDo=EFalse;
	if(	aActive==iActiveRConsoleRead)
		{
		iActiveRConsoleRead->KillTimer();
		if ( err==KErrNone )
			{
			
			moreToDo=iActiveRConsoleRead->VerifyDataAndKick(aIndex, *iConsole);
			if ( !moreToDo )
				{
				INFO_PRINTF1(_L("All Events have been read"));
				}
			}

		}
	else
		{
		activeOk=EFalse;
		ERR_PRINTF1(KLogStraySignal);
		SetBlockResult(EFail);
		}

	if ( activeOk )
		{
		if ( !moreToDo )
			{
			DecOutstanding();
			}
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(KLogAsyncError, err);
			SetAsyncError(aIndex, err);
			}
		}
	}

void CT_RConsoleData::DoCancel(CActive* aActive, TInt aIndex)
 /**
 * Called on cancellation of an asynchronous command
 * @param               aActive Active Object that RunL has been called on
 * @param               aIndex number of the command.
 */
	{
	TBool	activeOk=ETrue;
	TInt	err=KErrNone;
		
	if ( aActive==iActiveRConsoleRead )
		{
		INFO_PRINTF1(_L("ReadCancel Called"));
		iActiveRConsoleRead->KillTimer();
		iConsole->ReadCancel();
		err=aActive->iStatus.Int();
		}
	else
		{
		activeOk=EFalse;
		ERR_PRINTF1(KLogStraySignal);
		SetBlockResult(EFail);
		}

	if ( activeOk )
		{
		DecOutstanding();
	 	if( err != KErrNone )
            {
            ERR_PRINTF2(_L("DoCancel Error %d"), err);
            SetAsyncError( aIndex, err );
            }
		}

	}
