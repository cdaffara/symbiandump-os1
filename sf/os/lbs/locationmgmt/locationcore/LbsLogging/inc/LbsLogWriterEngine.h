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
 @internalTechnology
 @released
*/

#ifndef LBS_LOG_WRITER_ENGINE_H
#define LBS_LOG_WRITER_ENGINE_H

#include <e32base.h>
#include <lbs/lbslogevent.h>

enum TLbsLogEventType
/**
 enum for type of LbsLogging event handled by Lbs Logging Engine	
*/
	{
	ELbsLogNoEvent = 0,
	ELbsLogAddEvent,
	ELbsLogUpdateEvent
	};
	
	
typedef TUint32 TLbsLogWriterEventTypeGroup;

enum TLbsLogWriterEventTypes
	{
	ELbsNoEventType = 				0x00000000,
	ELbsSelfLocateEventType = 		0x00000001,
	ELbsExternalLocateEventType = 	0x00000002,
	ELbsTransmitLocationEventType = 0x00000004,
	ELbsNetworkLocateEventType = 	0x00000008,
	ELbsAssistanceDataEventType = 	0x00000010
	};
	
class CLbsLogWriterEngine : public CActive
/**
 Active class which does the logging of LBS events. When it receives a new event, 
 if it isn't processing anything it starts logging that event, otherwise it elaves it 
 on a waiting queue.
 
*/
	{
public:
	static CLbsLogWriterEngine* NewL(TUint aMaxBufferSize);
	~CLbsLogWriterEngine();
	
	TInt AddEventL(CLbsLogEvent& aLogEvent);
	TInt UpdateEventL(CLbsLogEvent& aLogEvent);
	// Start to log the event request asyn
	TInt StartAddingEventType(CLogEventType& aType);
	
private:
	CLbsLogWriterEngine(TUint aMaxBufferSize);
	void ConstructL();

	// Start to log the event request asyn
	void StartLoggingEventL(CLbsLogEvent& aLogEvent);
	void InitialiseLogEventTypeL(CLbsLogEvent& aLogEvent);
	void AddLogEventToQueueL(CLbsLogEvent& aLogEvent);
	
private: // from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	/** The maximum numer of events that can be queued */
	TUint 							iMaxBufferSize;
	/** Flag indicating if there is an ID available of the last logged event */
	TBool 							iLastLogIdAvailable;
	/** The ID of the last logged event */
	TLogId 							iLastLogId;
	/** The array with the queued events */
	RPointerArray<CLbsLogEvent> 	iLogEvents;
	/** The array with the queued event types */
	RArray<TLbsLogEventType>		iLogEventTypes;
	/** Current processing event type */
	TLbsLogEventType				iLastLogEventType;
	/** Bitmask listing which event types have already been added */
	TLbsLogWriterEventTypeGroup		iInitLogEventTypes;
	/** The event type currently being logged */
	CLogEventType* 					iEventTypeBeingLogged; 
	/** Flag indicating if an event type is currently being added */
	TBool 							iLogEventTypeInit;
	/** The event that is being logged now */
	CLogEvent* 						iEventBeingLogged;

	/***/
	TBool							iLoggingType;
	/** Handle to the db file file sesstion */
	RFs 							iFs;
	/** CLogClient handler */
	CLogClient* 					iClient;	
	

	
__DECLARE_TEST; // Declaration for the invariant
	}; // class CLbsLogWriterEngine

#endif // LBS_LOG_WRITER_ENGINE_H
