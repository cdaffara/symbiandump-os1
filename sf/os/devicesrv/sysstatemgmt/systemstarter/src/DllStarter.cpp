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

#include "DllStarter.h"
#include <f32file.h>
#include "SystemStartupDllInfo.h"
#include "SysStartDebug.h"
#include "SysStartDefs.h"

#include "sysstartpanic.h"
#include "restartsys.h"


//
// Standard Symbian factory functions/destructor
//
 
CDllStarter* CDllStarter::NewL(const CSystemStartupDllInfo* aDllInfo )
	{
	CDllStarter* self = CDllStarter::NewLC(aDllInfo );
	CleanupStack::Pop(self);
	return self;
	}

CDllStarter* CDllStarter::NewLC(const CSystemStartupDllInfo* aDllInfo )
	{
	CDllStarter* self = new (ELeave) CDllStarter(aDllInfo );
	CleanupStack::PushL(self);
	return self;
	}
	
CDllStarter::~CDllStarter()
	{
	delete iDllInfo;
	}
 	
CDllStarter::CDllStarter(const CSystemStartupDllInfo* aDllInfo ):
	iDllInfo(aDllInfo)  
	{
	}
 	
	
//
// Public member functions
//

/** Implementation of MStartupCommand interface.
This function retrieves the DLL name and ordinal number from its 
CSystemStartupDllInfo data member. It attempts to load the DLL 
and call the specified function. 

@see MStartupCommand.
*/
void CDllStarter::Execute(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending; 
	DEBUGPRINT1(_L("SysStart: CDllStarter::Execute"));
	
	// Find the name of the DLL to be loaded from the information 
	// already retrieved from the SSC file.
	TFileName dllName =iDllInfo->DllName();
	_LIT(KExtension, ".dll");
	dllName.Append(KExtension);
    
    // Initialise variables
    _LIT(KSysBin, "z:\\sys\\bin\\");
	RLibrary lib;
	TInt err = KErrGeneral; 
	TBool dllLoaded = EFalse; 
	
	TUint8 ordinal = iDllInfo->Ordinal();
 
	// The DLL may not be found or the DLL function may return an error 
	// code. The number of times to re-attempt is specified in the SSC file.
	TInt attemptsRequired = iDllInfo->NoOfRetries() + 1; 
  
 	while ((err != KErrNone)   && (attemptsRequired-- >0))
	 	{
		// Search for and load the DLL. 	 
       	if ((err = lib.Load(dllName, KSysBin)) == KErrNone)
 			{ 
 			DEBUGPRINT2(_L("SysStart: dll loaded - %S"), &dllName);
 			dllLoaded = ETrue;		 
 			}
 		else
			{
			DEBUGPRINT3(_L("SysStart: Unable to load DLL - %S, error - %d"), &dllName, err);
			}
			
  		// Call the function in the Dll
  		if (dllLoaded)
  			{	
  			// Identify the function to be called according to the ordinal 
 			// number specified in the SSC file.
 			TLibraryFunction ordinalfunction = lib.Lookup(ordinal);
 				
 			if (ordinalfunction != NULL)
 				{	
 				Dllfunctiontype getDllFunction = reinterpret_cast<Dllfunctiontype>(ordinalfunction);
  		 		DEBUGPRINT2(_L("SysStart: Calling function at ordinal %d."), ordinal);
  		 		err = (*getDllFunction)(iDllInfo->DllBuffer());
 				}
 			else
 				{
 				DEBUGPRINT2(_L("Sysstart: No function at specified ordinal %d"), ordinal);
 				err = KErrNotFound;
 				// No function exists at the ordinal specified so 
 				// there is no point in re-attempting execution.
 				attemptsRequired = 0;
 				}
  			}
	 	}
 	
				 	
	// Take action on error condition
	if (err != KErrNone) 
		{	
 		// Check required behaviour on failure specified in SSC
 		if (iDllInfo->FailOnError())
 			{ 
			err = RestartSys::RestartSystem();
			if (KErrNone != err)
				{
				DEBUGPRINT2(_L("Sysstart: Restart System Call failed with error %d"), err);
 				PanicNow(KPanicDllStarter, ERestartSystemCallFailed);
				}
			User::After(5000000); // required by RestartSys API, see comments in RestartSys::RestartSystem()
 			}			 
		}
 
 	lib.Close();
  
	TRequestStatus* requestStatus = &aStatus;
	User::RequestComplete(requestStatus, err);
	}

void CDllStarter::Release() 
	{
	delete this;
	}
