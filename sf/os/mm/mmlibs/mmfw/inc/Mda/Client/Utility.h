// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Mda\Client\Utility.h
// maintained for backwards compatibility
// 
//


#ifndef __MDA_CLIENT_UTILITY_H__
#define __MDA_CLIENT_UTILITY_H__

// Standard EPOC32 includes
#include <e32base.h>
#include <mda/common/base.h>

/**
@publishedAll
@removed
*/
enum {
	KDummyMdaServerNotImplemented
	};

// Maintained for backwards compatibility
// Some code such as CONE uses constucts such as CMdaServer* iServer = CMdaServer::NewL();

/**
@publishedAll
@deprecated

This class has been deprecated in 7.0s.
*/
class MMdaObjectEventListener
	{
public:

	/**
    Handles an event that occurred in the media server. Called by the CMdaServer object that the listener has been added to.

	@param  aEvent
	        A single event that occurred in the media server.
	
	@see CMdaServer::AddListenerL()
	*/
	virtual void MoelEvent(const TMdaEvent& aEvent) =0;

	/**
    This function, rather than MoelEvent() is called if the event's ID is EMdaEventMissed. Events are stored when they are 
	received and a missed event occurs when there are no spare slots.
	*/
	virtual void MoelEventMissed() =0;
	friend class CMdaServer;
	};

/**
@publishedAll
@deprecated

This class has been deprecated in 7.0s.
*/
class CMdaServer : public CBase
	{
public:
	IMPORT_C static CMdaServer* NewL();
	IMPORT_C ~CMdaServer();
	IMPORT_C void AddListenerL(MMdaObjectEventListener& aListener);
	IMPORT_C void RemoveListener(MMdaObjectEventListener& aListener);
	};

/**
@publishedAll
@released

An interface class for handling the change of state of an audio data sample object.

The class is a mixin and is intended to be inherited by the client class which is observing the audio recording or the 
audio conversion operation. The class defines a single function which must be implemented by the inheriting client class.

This class is maintained for binary compatibility with versions prior to 7.0s.

Defined in MMdaObjectStateChangeObserver:
MoscoStateChangeEvent()
*/
class MMdaObjectStateChangeObserver
	{
public:
	/**
	This method is used to receive the updated state of an audio data sample object.  This method must be implemented 
	by the inheriting client class.

	@param aObject
		   The audio data sample object whose state has changed.

	@param aPreviousState
		   This is the previous state of the audio data sample object before it went into the current state.  
		   It is expected to be one of the enum values defined in CMdaAudioClipUtility::TState, which defines 
		   the possible states of an audio data sample.

	@param aCurrentState
		   This is the current state of the audio data sample object.  It is expected to be one of the enum values defined 
		   in CMdaAudioClipUtility::TState, which defines the possible states of an audio data sample.
			
	@param aErrorCode
		   A system wide error code, KErrNone for sucessful completion.  KErrInUse, KErrDied or KErrAccessDenied 
		   may be returned to indicate that the sound device is in use  by another higher priority client.
	 */
	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)=0;
	};

#endif
