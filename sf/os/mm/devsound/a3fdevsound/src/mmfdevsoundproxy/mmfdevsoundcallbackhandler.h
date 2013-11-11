/*
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



#ifndef MMFDEVSOUNDCALLBACKHANDLER_H
#define MMFDEVSOUNDCALLBACKHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <e32msgqueue.h>
#include "mmfaudioclientserver.h"

// CLASS DECLARATION
class RMMFDevSoundProxy;
class MDevSoundObserver;

/**
*  Utility class to handle Play Error callback from DevSound server.
*
*  @lib MMFDevSound.dll
*  @since 
*/
class CMsgQueueHandler : public CActive
	{
public:  // Constructors and destructor

	/**
	* Constructs, and returns a pointer to, a new
	* CMMFPlayErrorHandler object.
	* Leaves on failure..
	* @param RMMFDevSoundProxy* aDevSoundProxy A handle to DevSoundProxy to
	*        subcribe for event notification from server.
	* @param MDevSoundObserver& aDevSoundObserver A reference to
	*        MDevSoundObserver to which the events will be forwarded to.
	* @param aDevSoundCIObserver Observer which will receive Custom Interface events
	* @return CMMFPlayErrorHandler* A pointer to newly created object.
	*/
	static CMsgQueueHandler* NewL(RMMFDevSoundProxy* aDevSoundProxy,
								MDevSoundObserver& aDevSoundObserver,
								RMsgQueue<TMMFDevSoundQueueItem>* aMsgQueue,
								MMMFDevSoundCustomInterfaceObserver& aDevSoundCIObserver);
	/**
	* Destructor.
	*/
	~CMsgQueueHandler();

	/**
	* Subscribes for Play Error event from DevSound server.
	* @since 
	* @return void
	*/
	void ReceiveEvents();

	/**
	* Called by DevSound server when Play Error event occurs.
	* Leaves on failure.
	* @since 
	* @return void
	*/
	void RunL();

	/**
	* Called by CActive object framework if RunL leaves.
	* @since 
	* @param TInt aError Reason for leave
	* @return TInt KErrNone if error is handled succesfully, else
	* corresponding error codel
	*/
	TInt RunError(TInt aError);

	/**
	* Called by CActive object framework when client cancels active object.
	* @since 
	* @return void
	*/
	void DoCancel();
	/**
	* Called as part of Stop() process to recover any buffers etc
	*/
	void Finish();

private:
	/**
	* C++ default constructor.
	*/
	CMsgQueueHandler(RMMFDevSoundProxy* aDevSoundProxy,
					MDevSoundObserver& aDevSoundObserver,
					RMsgQueue<TMMFDevSoundQueueItem>* aMsgQueue,
					MMMFDevSoundCustomInterfaceObserver& aDevSoundCIObserver);
	/**
	* By default Symbian 2nd phase constructor is private.
	*/
	void ConstructL();

	/*
	* Handles initialization completion event.
	*/
	void DoInitComplete();

	/*
	* Handles play completion or cancel event.
	*/
	void DoPlayErrorComplete();

	/*
	* Handles record completion or cancel event.
	*/
	void DoRecordErrorComplete();

	/*
	* Handles CMMFDevSound object's data request event to supply
	* CMMFDevSound with the buffer that it needs to play.
	*/
	void DoBTBFCompleteL();

	/*
	* Handles CMMFDevSound object's data request event to supply
	* CMMFDevSound with the buffer that it needs to record.
	*/
	void DoBTBECompleteL();

	/*
	* Handles tone play completion event.
	*/
	void DoToneFinishedComplete();

	/*
	* Sends DevSound server event completion notification to the client.
	*/
	void DoSendEventToClientComplete();
	
	/* Handles CMMFDevSound object's data request event to supply 
	* CMMFDevSound with the last buffer that it needs to record.
	*/
	void DoPausedRecordComplete();
	
	/*
	* Allocates local data buffer and links it with the global chunk.
	* @param aHandle Handle to a global chunk or 0 if chunk already exists.
	*/
	void AssignDataBufferToChunkL(TInt aHandle);
	/*
	* Update existing data buffer with latest buffer info from server
	* but using existing chunk
	*/
	void UpdateDataBufferL();
	
	static TInt AsyncQueueFinishCallback(TAny* aPtr);
	void DoAsyncQueueFinishCallback();

private:
	
	RMMFDevSoundProxy* iDevSoundProxy;
	MDevSoundObserver& iDevSoundObserver;
	TMMFDevSoundProxyHwBufPckg iSetPckg;

	TMMFEventPckg iEventPckg;

	RMsgQueue<TMMFDevSoundQueueItem>* iMsgQueue;
	TMMFDevSoundQueueItem   iCurrentItem;
	RChunk iChunk;
	TPtr8 iChunkDataPtr;
	CMMFPtrBuffer* iDataBuffer;
	CMMFDataBuffer* iEmptyBuffer;
	MMMFDevSoundCustomInterfaceObserver& iDevSoundCIObserver;
	CAsyncCallBack*				iAsyncQueueFinish;
	};

#endif      // MMFDEVSOUNDCALLBACKHANDLER_H

// End of File
