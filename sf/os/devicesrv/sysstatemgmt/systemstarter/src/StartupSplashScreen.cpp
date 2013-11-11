// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Start/kill the splash screen. Note that this only works with the techview
// splash screen (or any other that responds to the global chunk created
// here). This class is provided as a example of a startup command other than
// the standard app starter.
// 
//

#include "StartupSplashScreen.h"

#include "SysStartDebug.h"
#include <e32property.h>
#include <sysstartdefs.h>


//
// Standard Symbian factory functions/destructor
//

CStartupSplashScreen* CStartupSplashScreen::NewL(TBool aStart, HBufC* aPath)
	{
	CStartupSplashScreen* self = NewLC(aStart, aPath);
	CleanupStack::Pop(self);
	return self;
	}

CStartupSplashScreen* CStartupSplashScreen::NewLC(TBool aStart, HBufC* aPath)
	{
	CStartupSplashScreen* self = new (ELeave) CStartupSplashScreen(aStart, aPath);
	CleanupStack::PushL(self);
	return self;
	}

CStartupSplashScreen::~CStartupSplashScreen()
	{
	delete iSplashPath;
	}

//
// Public member functions
//

/** Implementation of MStartupCommand interface.

@see MStartupCommand.
*/
void CStartupSplashScreen::Execute(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;

	TInt err = KErrNone;
	if (iStartSplash)
		{
		err = iSplashPath ? StartSplashScreen() : KErrArgument;
		}
	else
		{
		err = KillSplashScreen();
		}

	TRequestStatus* statusValue = &aStatus;
	User::RequestComplete(statusValue, err);
	}

//
// Private functions
//

CStartupSplashScreen::CStartupSplashScreen(TBool aStart, HBufC* aPath) :
	iStartSplash(aStart),
	iSplashPath(aPath)
	{
	}

TInt CStartupSplashScreen::StartSplashScreen()
	{
	DEBUGPRINT2(_L("SysStart: Start splash screen %S"), iSplashPath); 
	            
	_LIT_SECURITY_POLICY_PASS(KSplashReadPolicy);
	_LIT_SECURITY_POLICY_S0(KSplashWritePolicy, RProcess().SecureId());   
   
  	// Define a splash property to control display and removal of
  	// the splash screen on system start up.    
 	// Ignoring the error value.
	TInt err = RProperty::Define(KSplashPropertyKey, RProperty::EInt, 
 							KSplashReadPolicy, KSplashWritePolicy);
	DEBUGPRINT2(_L("Defining the KSplashPropertyKey completed with %d"), err);

 	// Create and set off the splash screen process.
	RProcess splash;	 
	err = splash.Create(*iSplashPath, KNullDesC);
	if (err == KErrNone)
		{
		splash.Resume();
		splash.Close();
		}
	
	return err;
	}

TInt CStartupSplashScreen::KillSplashScreen()
	{
	DEBUGPRINT1(_L("SysStart: Kill splash screen"));
  
  	// Set the splash property so that it indicates that the splash screen
  	// can now be removed. 
 	const TInt KOkayToRemoveSplash = 1;  
 	TInt err = RProperty::Set(KSysStartPropertyCat, KSplashPropertyKey, KOkayToRemoveSplash);
	return err;  
	}

void CStartupSplashScreen::Release() 
	{
	delete this;
	}
