// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "LbsLogger.h"
#include "LbsLogWriterEngine.h"
#include <lbs/lbsadmin.h>

EXPORT_C RLbsLogger::RLbsLogger() : iLogWriterEngine(NULL)
/**
 C++ constructor
*/
	{
	}

EXPORT_C RLbsLogger::~RLbsLogger()
/**
 C++ destructor
 
 @panic If the handle hasn't been closed.
*/
	{
	if (iLoggingOn)
		{
		__ASSERT_DEBUG(iLogWriterEngine == NULL, User::Panic(KLbsLoggerPanic, KErrInUse));
		}
	}

EXPORT_C void RLbsLogger::ExtensionInterfaceL(TUid /*aInterface*/, void*& aObject)
	{
	aObject = NULL;
	} 

EXPORT_C TInt RLbsLogger::Open(TUint aMaxBufferSize)
/**
 Opens the handle to the internal engine. Only one handle can be open, so if one 
 is already open it does nothing.
 
 @param aMaxBufferSize The maximum number of items to queue. If this is not specified 
 			a default value will be used.
*/
	{
	TInt err = KErrNone;

	// Determine if logging is actually required.
	CheckLoggingAdminSettings();

	if (iLoggingOn)
		{
		if(aMaxBufferSize == 0)
			aMaxBufferSize = 10;
		
		
		if(!iLogWriterEngine)
			{
			TRAP(err, iLogWriterEngine = CLbsLogWriterEngine::NewL(aMaxBufferSize);)
			}

		}
	return err;
	}

EXPORT_C TInt RLbsLogger::AddEvent(CLbsLogEvent& aLogEvent)
/**
 Handles receiving a new event to log. If there is nothing else being logged then it 
 will start logging it straight away, otherwise it will add the event to the queue.
 
 @param aLogEvent The new event to be logged
 
 @return KErrNone if everything fine
 @return KErrNotSupported if the LogEng is not installed
 @return KErrOverflow if the queue already contains the maximum number of items
 
 @panic If the handle is not open.
*/
	{
	TInt err = KErrNone;
	
	if (iLoggingOn)
		{
		__ASSERT_DEBUG(iLogWriterEngine, User::Panic(KLbsLoggerPanic, KErrBadHandle));
		TRAP(err, iLogWriterEngine->AddEventL(aLogEvent);)
		}	
	return err;
	}

EXPORT_C TInt RLbsLogger::UpdateEvent(CLbsLogEvent& aLogEvent)
/**
 Handles receiving an update for the last event. If there is nothing else being logged 
 then it will start logging it straight away, otherwise it will add the event to 
 the queue.
 
 Warning - This may give unexpected results if multiple users share one Logger.
 
 @param aLogEvent The information for updating

 @return KErrNone if everything is fine
 @return KErrNotSupported if the LogEng is not installed
 @return KErrNotFound if there is nothing to update
 
 @pre At least one event should have been added in this session before calling update.
 
 @panic If the handle is not open.
 @panic If no event was added in this session before calling update.
*/
	{
	TInt err = KErrNone;

	if (iLoggingOn)
		{
		__ASSERT_DEBUG(iLogWriterEngine, User::Panic(KLbsLoggerPanic, KErrBadHandle));
		TRAP(err, iLogWriterEngine->UpdateEventL(aLogEvent);)
		}
	return err;
	}

EXPORT_C TInt RLbsLogger::Close()
/**
 Waits for logging all the queue and then closes the handle to the engine.
*/
	{
	// disable lbs logging event types
	TInt err = KErrNone;
	
	if (iLogWriterEngine)
		{
		iLogWriterEngine->Cancel();
		delete iLogWriterEngine;
		iLogWriterEngine = NULL;
		}
	
	return err;
	}

/* Determine if logging service is required.
*/
void RLbsLogger::CheckLoggingAdminSettings()
	{
	iLoggingOn = EFalse;

	// Open admin db.
	CLbsAdmin* adminApi = NULL;

	TRAPD(err, adminApi = CLbsAdmin::NewL();)
	if (err == KErrNone)
		{
		// Read setting.
		CLbsAdmin::TLogger serviceStatus(CLbsAdmin::ELoggerOff);
	
		err = adminApi->Get(KLbsSettingLogger, serviceStatus);
		if (err == KErrNone)
			{
			// Determine the status.
			if (serviceStatus == CLbsAdmin::ELoggerOn)
				{
				iLoggingOn = ETrue;
				}
			}

		// Done with the admin db.
		delete adminApi;
		}
	}
