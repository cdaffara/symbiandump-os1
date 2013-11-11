// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains wrapper helper functions for test code to control the test wrappers 
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include "startupadaptationwrapper.h"

/**
 * Sets the value for the wrapper for if the startup adaptation is loaded.
 * 
 * @internalComponent
 * @test
 */
void CStartupAdaptationWrapper::SetLoaded(TBool aLoaded)
	{
	iIsLoaded = aLoaded;
	}

/**
 * Returns the value for the wrapper for if the startup adaptation is loaded.
 * 
 * @internalComponent
 * @test
 */
TBool CStartupAdaptationWrapper::IsLoaded()
	{
	return iIsLoaded;
	}

/**
 * Allocates and returns a new startup adaptation
 * 
 * Returns NULL if a startup adaptation has already been created
 *  
 * @internalComponent
 * @test
 */
CStartupAdaptation* CStartupAdaptationWrapper::NewStartupAdaptation(TAny* aParams)
	{
	if (iStartupAdaptation != NULL)
		{
		return NULL;
		}
	iStartupAdaptation = new CStartupAdaptationWrapper(static_cast<MStartupAdaptationObserver*>(aParams));
	SetLoaded(ETrue);
	return static_cast<CStartupAdaptation*>(iStartupAdaptation);
	}

/**
 * Deletes the allocated startup adaptation from a previous call to NewStartupAdaptation
 *  
 * @internalComponent
 * @test
 */
void CStartupAdaptationWrapper::DeleteStartupAdaptation()
	{
	SetLoaded(EFalse);
	}

/**
 * Sets the last command id to be returned by the wrapper
 * 
 * @internalComponent
 * @test
 */
void CStartupAdaptationWrapper::SetLastCommandId(StartupAdaptation::TCommand aCommandId)
	{
	iLastCommandId = aCommandId;
	}

/**
 * Returns the last command id to be given to the wrapper
 * 
 * @internalComponent
 * @test
 */
StartupAdaptation::TCommand CStartupAdaptationWrapper::LastCommandId()
	{
	return iLastCommandId;
	}

/**
 * Sets the last command data to be returned
 * 
 * @internalComponent
 * @test
 */
void CStartupAdaptationWrapper::SetLastCommandDataL(TDesC8& aData)
	{
	// Make a copy of the data
	HBufC8* newData = aData.AllocL();

	delete iLastCommandData;
	iLastCommandData = newData;
	}

/**
 * Returns the last command data to be given to the wrapper
 * 
 * Ownership of the HBufC* remains with the CStartupAdaptationWrapper
 * and will be deleted when the next command is issued
 * 
 * @internalComponent
 * @test
 */
HBufC8* CStartupAdaptationWrapper::LastCommandData()
	{
	return iLastCommandData;
	}

/**
 * Frees any data allocated for last command data
 * 
 * @internalComponent
 * @test
 */
void CStartupAdaptationWrapper::DeleteLastCommandData()
	{
	delete iLastCommandData;
	iLastCommandData = NULL;
	}

/**
 * Triggers an event callback on the startup adaptation observer class
 * associated with the wrapper
 * 
 * @internalComponent
 * @test
 */
void CStartupAdaptationWrapper::TriggerObserverEventCallbackL(const StartupAdaptation::TEvent aEventId, TDesC8& aData)
	{
	iStartupAdaptation->iObserver->EventL(aEventId, aData);
	}

/**
 * Triggers a response callback on the startup adaptation observer class
 * associated with the wrapper
 * 
 * @internalComponent
 * @test
 */
void CStartupAdaptationWrapper::TriggerResponseCallbackL(const StartupAdaptation::TCommand aCommandId, TDesC8& aData)
	{
	// Use iResponding to ensure that CommandL is not issued from ResponseL()
	iStartupAdaptation->iResponding = ETrue;
	TRAPD(err, iStartupAdaptation->iObserver->ResponseL(aCommandId, aData));
	iStartupAdaptation->iResponding = EFalse;
	User::LeaveIfError(err);
	}

/**
 * Stores the last command data to be issued to the wrapper
 * 
 * @internalComponent
 * @test
 */
StartupAdaptation::TCommand CStartupAdaptationWrapper::iLastCommandId = static_cast<StartupAdaptation::TCommand>(0);

/**
 * Stores the last command data to be stored by the wrapper
 * 
 * @internalComponent
 * @test
 */
HBufC8* CStartupAdaptationWrapper::iLastCommandData = NULL;

/**
 * Stores the created startup adaptation
 * 
 * @internalComponent
 * @test
 */
CStartupAdaptationWrapper* CStartupAdaptationWrapper::iStartupAdaptation = NULL;

/**
 * Stores the value for the wrapper for if the startup adaptation is loaded.
 * 
 * @internalComponent
 * @test
 */
TBool CStartupAdaptationWrapper::iIsLoaded = EFalse;


CStartupAdaptationWrapper::CStartupAdaptationWrapper(MStartupAdaptationObserver* aObserver)
: iObserver(aObserver)
	{
	
	}

void CStartupAdaptationWrapper::CommandL(const StartupAdaptation::TCommand aCommandId, TDesC8& aData)
	{
	if(iResponding)
		{
		// CommandL has been called from within RespondL, this is not allowed
		User::Panic(_L("StartupAdaptationWrapper"), 20);
		}
	SetLastCommandId(aCommandId);
	SetLastCommandDataL(aData);
	}

void CStartupAdaptationWrapper::CancelCommandL(const StartupAdaptation::TCommand aCommandId)
	{
	_LIT(KWrapperPanic, "SAA Test Wrapper");
	if (aCommandId != LastCommandId())
		{
		// If it's not the last command being cancelled then panic
		User::Panic(KWrapperPanic, 0);
		}
	}

TVersion CStartupAdaptationWrapper::Version() const
	{
	return TVersion(1, 0, 0);
	}
