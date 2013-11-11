/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file
*/

#ifndef LBS_LOGGER_H
#define LBS_LOGGER_H

#include <lbs/lbslogevent.h>
#include <e32base.h>

class CLbsLogWriterEngine;

class RLbsLogger
/**
 Class the provides the APIs for adding and updating LBS events into the LogEng.
 In order to add (or update) any event, a handle must have been opened for this class 
 (so OpenL() must have been called, without calling Close() meanwhile on the class).
 
 Internally, the engine, queues the add and update requests until they can be processed. The 
 queue has a maximum size, so if too many items will get on the queue the AddEventL() method 
 can return an error.

 When the client finishes sending all the add and update requests, Close() should be 
 called on the class. This will ensure that the internal engine logs all the events it 
 queued into the LogEng. When calling Close() it will wait for LogEng to finish 
 logging all the events.
 
 @internalTechnology
 @released
*/
	{
public:
	IMPORT_C RLbsLogger();
	IMPORT_C ~RLbsLogger();
	
	IMPORT_C TInt Open(TUint aMaxBufferSize);
	IMPORT_C TInt AddEvent(CLbsLogEvent& aLogEvent);
	IMPORT_C TInt UpdateEvent(CLbsLogEvent& aLogEvent);
	IMPORT_C TInt Close();
	// for BC consideration
	virtual void ExtensionInterfaceL(TUid aInterface, void*& aObject);

private:
	void CheckLoggingAdminSettings();

private:
	/** Pointer to the active object which does the logging */
	CLbsLogWriterEngine* iLogWriterEngine;
	
	/** Flag to indicate if logging is currently On or Off, determined from the admin db setting. */
	TBool iLoggingOn;
	}; // class RLbsLogger

#endif // LBS_LOGGER_H
