/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef AUDIOCONTEXT_H
#define AUDIOCONTEXT_H

#include <e32base.h>
#include <a3f/a3fbase.h>
#include <a3f/a3f_trace_utils.h>
#include <a3f/a3fdevsoundaudiopauseresume.h>

#include "mmrcclient.h"
#include "multimediaresourcecontrolobserver.h"

#include <a3f/maudiocontext.h>
#include <a3f/maudiocontextobserver.h>

#include "mlogicalsettingsobserver.h"


class CAudioProcessingUnit;
class CLogicalAudioStream;


/**
Panic category and codes
*/
_LIT(KAudioContextPanicCategory, "AudioContext");

enum TAudioContextPanicCodes
	{
	ECloneLeftWhenReceivingResourceResponse = 1,
	};

////
 // Implementation of the audio context.
 //
 // @see MAudioContext
 // @see MMultimediaResourceControlObserver
 // @see MLogicalSettingObserver
 //
 // @lib AudioComponentFramework.lib
 //
NONSHARABLE_CLASS(CAudioContext) : 	public CBase,
									public MAudioContext,
									public MMultimediaResourceControlObserver,
									public MLogicalSettingObserver,
									public MA3FDevSoundAutoPauseResume
	{

public:

	IMPORT_C static CAudioContext* NewL();

	IMPORT_C virtual ~CAudioContext();

	// 
	IMPORT_C MLogicalChain* GetLogicalChain(TInt aIndex);
	IMPORT_C TInt MsgVersion();
	IMPORT_C RCriticalSection& CriticalSection();
	// from base class MAudioContext (A3F API)
	virtual TInt Commit  ( ); 
	virtual TInt CreateAudioProcessingUnit  (TUid  aTypeId, MAudioProcessingUnit *& aProcessingUnit );
	virtual TInt CreateAudioStream  ( MAudioStream *& aStream  );
	virtual void DeleteAudioProcessingUnit  ( MAudioProcessingUnit *& aProcessingUnit );
	virtual void DeleteAudioStream  ( MAudioStream *& aStream );
	virtual TAudioContextId ContextId  ( ) const;
	virtual TAny* Interface(TUid aType);
	virtual TInt RegisterAudioContextObserver  ( MAudioContextObserver & aObserver );
	virtual TInt Reset  ( );
	virtual TInt SetClientSettings(const TClientContextSettings &aSettings);
	virtual void UnregisterAudioContextObserver ( MAudioContextObserver & aObserver  ) ;

	//from base class MMultimediaResourceControlObserver
	virtual void ReceiveResourceResponse(MLogicalChain *aMessage, TInt aError);
	virtual void ReceiveResourceUpdate(MLogicalChain *aMessage, TInt aError);
	virtual void ReceivePreemptionUpdate(MLogicalChain *aMessage, TInt aError);
	virtual void CanResume();

	//from MLogicalSettingObserver
	virtual void ReceiveComponentSettingsChange(TUid aId, TMMRCMessageType aMessageType);

	//Internals.
	//Resolve and get component context-unique component instance id.
	TAudioComponentId GetAndSetInstanceID();
	//Send a the notification about context event to all observers.
	void ContextEventSignal(TUid aEvent, TInt aError);
	
	//from MA3FDevSoundAutoPauseResume
	virtual TInt RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData, MA3FDevSoundAutoPauseResumeObserver* aObserverPtr);
	virtual TInt CancelRegisterAsClient(TUid aEventType);
	virtual TInt WillResumePlay();

private:

	CAudioContext();

	void ConstructL();

	void Panic(TAudioContextPanicCodes aCode);

private:	
	//Context identifier.
	
	TAudioContextId iContextId;

	//Array of components within this context (stream not included) (array members not own).
	
	RPointerArray<MAudioProcessingUnit> iAudioProcessingUnits;

	
	// Array of context observers.
	// Members not own.
	
	RPointerArray<MAudioContextObserver> iAudioContextObservers;
	

	//Multimedia Resource Control API.
	
	RMMRCClient iMMRC;

	
	//Client context identifiers.
	
	TClientContextSettings iClientSettings;

	//Audio type settings.
	
	TAudioTypeSettings iAudioTypeSettings;

	// Not owned
	MLogicalChain* iDesiredChain;

	// Owned
	MLogicalChain* iActualChain;

	TBool iInCommit;
	
	TBool iPreempted;
	
	MA3FDevSoundAutoPauseResumeObserver* iObserverPtr;
	
	TInt iMsgVersion;
	
	RCriticalSection iCriticalSection;
	};

#endif // AUDIOCONTEXT_H
