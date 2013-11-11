// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <barsread.h>
#include <e32uid.h>
#include <e32modes.h>
#include <startup.hrh>
#include <hal.h>
#include <hal_data.h>
#include <startupproperties.h>
#include "resourcefilereader2.h"
#include "SystemStartupStateInfo.h"
#include "StartupCommand.h"
#include "startuputilprovider.h"
#include "BootModeMapping.h"
#include "appstarter2.h"
#include "DllStarter.h"
#include "StartupSplashScreen.h"
#include "initapparcserver2.h"
#include "SystemStartupDllInfo.h"
#include "multiplewait2.h"
#include "amastarter.h"

#include "SysStartDebug.h"
#include "sysstartpanic.h"

const TUint KEntryPointIndex = 1;

/** Read splash screen information from the resource file and return an object
that can start/kill it
*/
MStartupCommand* CResourceFileReader::ReadSplashScreenL(TResourceReader& aReader)
	{
	TBool kill = static_cast<TUint16>(aReader.ReadUint16());
	HBufC* path = aReader.ReadTPtrC().AllocLC();
	CStartupSplashScreen* command = CStartupSplashScreen::NewL(!kill, path);
	CleanupStack::Pop(path);	// "command" takes ownership of path
	return command;
	}
		
/** Read app arc server initialisation information from the resource file
and return an object that can start it
*/
MStartupCommand* CResourceFileReader::ReadAppArcInitL()
	{
	return CInitAppArcServer::NewL(*iStartupUtilProvider);
	}

/** Read in the fields of the MULTIPLE_WAIT command in the SSC. 
Create a CMultipleWait object containing the list of deferred commands encountered
since the last MULTIPLE_WAIT command.   
*/ 
MStartupCommand* CResourceFileReader::ReadMultipleWaitInfoL(TResourceReader& aReader, CommandList& aDeferredList)
	{
	// Read values from the SSC
	TInt timeout = aReader.ReadInt32();
	TInt fail = static_cast<TUint16>(aReader.ReadUint16());
	
	// Create a CMultipleWait object to hold the data to be used during 
	// the command execution.
	CMultipleWait* waitCommand = CMultipleWait::NewL(aDeferredList, *iStartupUtilProvider);
	
	// Populate the command with the values just read from the SSC.
	waitCommand->SetTimeout(timeout);
	
	// Check that the fail_on_error value is valid.	
	if ((fail != EIgnoreCommandFailure) && (fail != EPanicOnCommandFailure))   
		{	
		DEBUGPRINT2(_L("SysStart: Invalid fail_on_error value of %d"), fail);
		PanicNow(KPanicResourceFileReader, EInvalidMultipleWaitFailOnError);
		} 
	waitCommand->SetFailOnError(fail);
	 	
 	return waitCommand; 
	}
	
/**
*/
MStartupCommand* CResourceFileReader::ReadAmaInfoL(TResourceReader& aReader)
	{
	TUid id = TUid::Uid(aReader.ReadUint32());

	CAmaStarter *command = CAmaStarter::NewL(id);
	
	return command;	
	}
	
/** Read DLL information from the resource file and store in an object to be 
used during dll function invocation
*/
MStartupCommand* CResourceFileReader::ReadDllInfoL(TResourceReader& aReader)
{ 
	// Read the main START_DLL_INFO fields from the SSC (static start-up 
	// configuration) file.
	TPtrC dllName = aReader.ReadTPtrC();
	TUint8 ordinal = aReader.ReadUint8();
 	TInt fail = static_cast<TBool>(aReader.ReadInt16());
	TUint8 retries = static_cast<TUint8>(aReader.ReadUint8());

	// Additional validation
	if ((fail != EIgnoreCommandFailure) && (fail != EPanicOnCommandFailure))   
		{	
		DEBUGPRINT2(_L("SysStart: Invalid fail_on_error value of %d"), fail);
		PanicNow(KPanicResourceFileReader, EInvalidDLLFailOnError);
		}  
 
 	if (ordinal == 0)
 		{
 		DEBUGPRINT1(_L("Invalid ordinal value of 0"));
		PanicNow(KPanicResourceFileReader, EInvalidOrdinal0);
 		}
 		
	// Store the information to be used later during function invocation
	CSystemStartupDllInfo* dllInfo = CSystemStartupDllInfo::NewLC();	
	dllInfo->SetDllNameL(dllName); 
	dllInfo->SetOrdinal( ordinal);  
	dllInfo->SetFailOnError(fail);
 	dllInfo->SetNoOfRetries(retries); 
  
  	// Read the DLL custom data link from the START_DLL_INFO struct
 	TUint32 dllDataLink = aReader.ReadUint32();
 
 	// Validate data link value
 	if (dllDataLink == 0)
 		{
 		DEBUGPRINT1(_L("Invalid custom_dll_data_link value of 0"));
		PanicNow(KPanicResourceFileReader, EInvalidDataLink0);
 		}
 		
 	// Store the data found under the dllDataLink structure into a buffer
 	// for custom processing by licensee (See CDllStarter::Execute)
  	dllInfo->SetDllBuffer(iResourceFile.AllocReadLC(dllDataLink));
  		
  	// The above read a resource into a heap descriptor and put a pointer 
  	// to that descriptor onto the cleanup stack. Therefore this must now be 
  	// popped off the stack.
	CleanupStack::Pop();
	
	// Create a command using the DLL info retrieved.
	CDllStarter* command = CDllStarter::NewL(dllInfo);
		
	// Pop dllInfo of the stack
	CleanupStack::Pop(dllInfo);

	return command;
	}
 
//
// Standard Symbian factory functions/destructor
//

CResourceFileReader* CResourceFileReader::NewL(TInt aBootMode, RFs& aFs)
	{
	CResourceFileReader* self = CResourceFileReader::NewLC(aBootMode, aFs);
	CleanupStack::Pop(self);
	return self;
	}

CResourceFileReader* CResourceFileReader::NewLC(TInt aBootMode , RFs& aFs)
	{
	TFileName resourceFile;
	TBootModeMapping mapping;
	
	mapping.GetResourceFileName(aBootMode, resourceFile, aFs);
	
	DEBUGPRINT2(_L("SysStart: using resource file %S"), &resourceFile);

	CResourceFileReader* self = new (ELeave) CResourceFileReader(aBootMode, aFs);
	CleanupStack::PushL(self);
	self->ConstructL(resourceFile);
	return self;
	}

CResourceFileReader::~CResourceFileReader()
	{
	iResourceFile.Close();
	delete iStartupUtilProvider;	
	}

//
// Public functions.
//

/** Gets information about the next start-up state. This involves reading the state information
and constructing the command list for this state.
*/
MStartupStateInfo* CResourceFileReader::GetStateInfoL()
	{
	return ((iState < NULL) ? NULL : ReadStateInformationL());
	}


//
// Private functions
//

CResourceFileReader::CResourceFileReader(TInt aBootMode, RFs& aFs)
	: iFs(aFs), iBootMode(aBootMode)
	{
	iMaxStartupMode = -1;
	}

void CResourceFileReader::ConstructL(const TDesC& aResourceFile)
	{
    iResourceFile.OpenL(iFs, aResourceFile);
    iStartupUtilProvider = CStartupUtilProvider::NewL();    
	}
 
void CResourceFileReader::FindFirstStateEntryL()
	{
	HBufC8* dataBuffer = iResourceFile.AllocReadLC(KEntryPointIndex);
	TResourceReader reader;
	reader.SetBuffer(dataBuffer);
	iState = reader.ReadInt32();
	CleanupStack::PopAndDestroy(dataBuffer);
	}

/** Read state information from the resource file
*/
CSystemStartupStateInfo* CResourceFileReader::ReadStateInformationL()
	{
	if (iState == 0)
		{
		FindFirstStateEntryL();
		}
		
	CSystemStartupStateInfo* stateInfo = CSystemStartupStateInfo::NewLC();

	HBufC8* dataBuffer = iResourceFile.AllocReadLC(iState);
	TResourceReader reader;
	reader.SetBuffer(dataBuffer);

	TUint8 id = reader.ReadUint8();
	
	// Validate state id field. State id can be any int except for the 
	// currently reserved values.
	if ((id == EReservedStartUpState1) || (id == EReservedStartUpState2) ||
	    (id == EReservedStartUpState4) || (id == EReservedStartUpState5)) 
		{
 		DEBUGPRINT2(_L("Invalid state id of %d"), id);
		PanicNow(KPanicResourceFileReader, EInvalidStateId);
		}  	

	// Store the state id value.
	stateInfo->SetStateId(id);

	TPtrC stateName = reader.ReadTPtrC();
	stateInfo->SetName(stateName);

	TUint32 commandListId = reader.ReadUint32();
	
    iState = reader.ReadInt32();	// Link to next state.
 
 	// The retries field specifies how many times to re-attempt 
 	// state transition in the case of failure
	TNoOfRetries noOfRetries = static_cast<TNoOfRetries>(reader.ReadUint16()); 

	// Validate field and store
	if ((noOfRetries != ERetry0) && (noOfRetries != ERetry1))
		{
 		DEBUGPRINT2(_L("Invalid no_of_retries_on_failure value of %d"), noOfRetries);
		PanicNow(KPanicResourceFileReader, EInvalidNoOfRetries);
		}  
	stateInfo->SetNoOfRetries(noOfRetries);
	 
	// The next field specifies what should be done on a state transition 
	// failure - ignore or panic  
	TActionOnStateTransitionFailure actionOnStateTransitionFailure = 
	        static_cast<TActionOnStateTransitionFailure>(reader.ReadUint16());  

	// Validate field and store.
	if ((actionOnStateTransitionFailure != EIgnoreFailure) && (actionOnStateTransitionFailure != EPanicOnFailure))
		{
 		DEBUGPRINT2(_L("Invalid action_on_failure value of %d"), actionOnStateTransitionFailure);
		PanicNow(KPanicResourceFileReader, EInvalidActionOnFailure);
		}  
	stateInfo->SetActionOnStateTransitionFailure(actionOnStateTransitionFailure);
    
    CleanupStack::PopAndDestroy(dataBuffer);

    if (iState == 0)
    	{
    	// Just read the last state info - make sure that we don't try to
    	// read any more.
    	iState = -1;
    	}
     
 	CommandList commandList = ReadCommandListL(commandListId);
	stateInfo->SetCommandList(commandList);
	CleanupStack::Pop(stateInfo);

    return stateInfo;
	}
	
/** Read the command list from the resource file
*/
CommandList CResourceFileReader::ReadCommandListL(TUint32 aCommandListId)
	{
	CommandList commandList; 
	
	// Some of the commands will be deferred apps or processes (i.e. it is not 
	// checked that they have completed until a MULTIPLE_WAIT command is issued). 
	// Create a list of these commands so they can be referred to from a 
	// CMultipleWait object.
	CommandList deferredList;
		
	HBufC8* dataBuffer = iResourceFile.AllocReadLC(aCommandListId);
	TResourceReader reader;
	reader.SetBuffer(dataBuffer);

	TUint16 commandListCount = reader.ReadUint16();

	for(TUint i = 0; i < commandListCount; ++i)
		{
		const TStartupCommandType type = static_cast<TStartupCommandType>(reader.ReadUint16());	
		MStartupCommand* command = NULL;
		switch (type)
			{
		case EStartupApp:					
		case EStartupProcess:
		case EStartupApp2:					
		case EStartupProcess2:			
			{
			reader.Rewind(sizeof(TUint16)); //CStartupProperties need to read the struct from start
			CStartupProperties* appInfo = CStartupProperties::NewLC(reader);
			if (appInfo->RecoveryMethod() == ERestartOSWithMode)
				{
				ValidateRestartMode(appInfo->RestartMode());
				}
			command = CAppStarter::NewL(appInfo, *iStartupUtilProvider);
			CleanupStack::Pop(appInfo); // ownership passed above
			CleanupStack::PushL(command);
			 
			// If the command has a start_method of EDeferredWaitForStart
			// in the SSC then it needs to be added to the current deferred
			// list. This will be passed to a CMultipleWait object once a 
			// MULTIPLE_WAIT command is encountered.
			if (command && (appInfo->StartMethod() == EDeferredWaitForStart))
				{  
				User::LeaveIfError(deferredList.Append(command)); 				
				}
			break;
			}
 		
		case EStartupSplashScreen:
			command = ReadSplashScreenL(reader);
			CleanupStack::PushL(command);
			break;

		case EInitAppArcServer:
			command = ReadAppArcInitL();
			CleanupStack::PushL(command);
			break;

		case EStartupDLL:
			command = ReadDllInfoL(reader);
			CleanupStack::PushL(command);
			break;
			
		case EMultipleWait:
		
			// A MULTIPLE_WAIT command should only be provisioned in the SSC
			// if there are preceeding deferred commands.
			if (deferredList.Count() == 0)
				{		
		 		DEBUGPRINT1(_L("SysStart: Resource File Reader extra multiple wait command in SSC"));
				PanicNow(KPanicResourceFileReader, EExtraMultipleWaitCommand);
				}
			// A list has been built up of any processes/apps with a start_method of
			// EDeferredWaitForStart. Pass this list as parameter so that a 
			// CMultipleWait object can be created which has access to this list.			  
			command = ReadMultipleWaitInfoL(reader, deferredList);
			CleanupStack::PushL(command);
			
			// As processing continues more deferred command may be encountered
			// along with a subsequent MULTIPLE_WAIT struct. Reset the deferred list
			// so that the next set of deferred commands can be stored.
			deferredList.Reset();   
			break;
			
		case EStartupAMAStarter:
			command = ReadAmaInfoL(reader);
			CleanupStack::PushL(command);
			break;
		
		default:
	 		DEBUGPRINT2(_L("SysStart: Resource File Reader unknown command type %d"), type);
			PanicNow(KPanicResourceFileReader, EInvalidCommandType);
			break;
			}
			
		if (command)
			{
			User::LeaveIfError(commandList.Append(command));
			CleanupStack::Pop(command);
			}
		}
	
	// Any deferred commands must be followed by a MULTIPLE_WAIT command within
	// the same state. Therefore at this stage all commands in the state have been 
	// processed so the deferred list should be empty.
	
	if (deferredList.Count() != 0)
		{		
		PanicNow(KPanicResourceFileReader, EMissingMultipleWaitCommand);
		}

	deferredList.Reset();	
    CleanupStack::PopAndDestroy(dataBuffer);
	return commandList;
	}

void CResourceFileReader::ValidateRestartMode(TInt aStartupMode)
	{
#ifndef __WINSCW__ // HAL::Get(EMaximumRestartStartupModes...) is only supported in H4	
	// check that the startup mode is valid, first retrieve max startup mode if not retrieved yet
	if (iMaxStartupMode == -1)
		{
		TInt err = HAL::Get(HALData::EMaximumRestartStartupModes, iMaxStartupMode);
		if (err != KErrNone)
			{
	 		DEBUGPRINT2(_L("SysStart: Failed to get max startup mode err = %d"), err);
			PanicNow(KPanicResourceFileReader, EMissingMaxStartupMode);
			}
		DEBUGPRINT2(_L("SysStart: HAL max startup mode = %d"), iMaxStartupMode);
		}

	if (aStartupMode > iMaxStartupMode)
		{
 		DEBUGPRINT2(_L("SysStart: Invalid failure_recovery_startup_mode %d"), aStartupMode);
		PanicNow(KPanicResourceFileReader, EInvalidStartupMode);
		}
#else
	(void)aStartupMode;
#endif
	}
