//audiocontext.cpp

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



#include <a3f/maudiostream.h>
#include <a3f/maudiocodec.h>
#include <a3f/maudiogaincontrol.h>
#include <a3f/a3fbase.h>

#include "audiocontext.h"
#include "audioprocessingunit.h"
#include "logicalaudiostream.h"
#include "audiostreammanager.h"


// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CAudioContext::CAudioContext() 
	{
	TRACE_CREATE();
	DP_CONTEXT(CAudioContext::CAudioContext *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iInCommit = EFalse;
	DP_OUT();
	}
	
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor
// ---------------------------------------------------------------------------
void CAudioContext::ConstructL()
	{
	DP_CONTEXT(CAudioContext::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	// Setup Multimedia Resource Control
	User::LeaveIfError(iMMRC.Open(*this));
	User::LeaveIfError(iCriticalSection.CreateLocal());
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Symbian constructor
// ---------------------------------------------------------------------------
EXPORT_C  CAudioContext* CAudioContext::NewL()
	{
	DP_STATIC_CONTEXT(CAudioContext::NewL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	CAudioContext* self = new(ELeave)CAudioContext();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	DP0_RET(self, "0x%x");
	}
	
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAudioContext::~CAudioContext()
	{
	DP_CONTEXT(CAudioContext::~CAudioContext *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iMMRC.Close();	
	iAudioContextObservers.Close();
	iAudioProcessingUnits.Close();
	if (iActualChain)
		{
		iActualChain->Release();
		};
	iCriticalSection.Close();
	DP_OUT();
	}
	
// ---------------------------------------------------------------------------
// From class MAudioContext
// CAudioContext::SetClientSettings
// ---------------------------------------------------------------------------
//
TInt CAudioContext::SetClientSettings(const TClientContextSettings& aSettings)
	{
	DP_CONTEXT(CAudioContext::SetClientSettings *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	iClientSettings = aSettings;
	if( iContextId.Id() == 0 )
		{
		TUint64 id = iMMRC.LogOn(iClientSettings.iProcessId);
		TAudioContextId temp(id);
		iContextId = temp;
		}
	else
		{
		err = KErrAlreadyExists;
		}
	DP0_RET(err, "%d");
	}


// ---------------------------------------------------------------------------
// From class MAudioContext
// CAudioContext::GetContextId
// ---------------------------------------------------------------------------
//
TAudioContextId CAudioContext::ContextId() const 
	{
	DP_CONTEXT(CAudioContext::ContextId *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	return iContextId;
	}

// ---------------------------------------------------------------------------
// From class MAudioContext
// CAudioContext::Commit
// ---------------------------------------------------------------------------
//
TInt CAudioContext::Commit()
	{
	DP_CONTEXT(CAudioContext::Commit *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	iInCommit = ETrue;
	
	if (iClientSettings.iProcessId.Id() == 0)
		{
		DP0_RET(KErrNotReady, "%d");
		}

	err = iMMRC.SendResourceRequest(iActualChain, iDesiredChain, this);
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// From class MAudioContext
// CAudioContext::CreateAudioStream
// ---------------------------------------------------------------------------
//
TInt CAudioContext::CreateAudioStream(MAudioStream *&aStream) 
	{
	DP_CONTEXT(CAudioContext::CreateAudioStream *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);

	CAudioStreamManager* manager(NULL);

	// Create desired logical chain
	TRAP(err, manager = CAudioStreamManager::NewL(KUidAudioStream));
	if(err == KErrNone)
		{
		aStream = static_cast<MAudioStream*>(manager);

		// Use this as desired chain (this is the one that the client modify)
		CLogicalAudioStream* logicalAudioStream = static_cast<CLogicalAudioStream*>(aStream);
		iDesiredChain = static_cast<MLogicalChain*>(logicalAudioStream); 
		logicalAudioStream->SetParentContext(*this);
		if (iActualChain)
			{
			iActualChain->Release();
			iActualChain=NULL;
			}
		TRAP(err, iActualChain = iDesiredChain->CloneL());
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// From class MAudioContext
// CAudioContext::DeleteAudioStream
// ---------------------------------------------------------------------------
//
void CAudioContext::DeleteAudioStream(MAudioStream *&aStream)
	{
	DP_CONTEXT(CAudioContext::DeleteAudioStream *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	CAudioStreamManager* manager = static_cast<CAudioStreamManager*>(aStream);
	delete manager;
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// From class MAudioContext
// CAudioContext::CreateAudioProcessingUnit
// ---------------------------------------------------------------------------
//
TInt CAudioContext::CreateAudioProcessingUnit(TUid aTypeId, MAudioProcessingUnit *&aProcessingUnit)
	{
	DP_CONTEXT(CAudioContext::CreateAudioProcessingUnit *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	TAudioComponentId unitInstance = GetAndSetInstanceID();
	
	CAudioProcessingUnit* pUnit=NULL;
	TComponentParameters cParameters;
	cParameters.iTypeUid = aTypeId;
	cParameters.iInstanceId = unitInstance;
	cParameters.iContextId = iContextId;
	cParameters.iSettingsObserver = this;
	
	TRAP(err, pUnit = CAudioProcessingUnit::NewL(cParameters));
	if (err==KErrNone)
		{
		aProcessingUnit = static_cast<MAudioProcessingUnit*>(pUnit);
		err = iAudioProcessingUnits.Append(aProcessingUnit);
		if(err!=KErrNone)
			{
			delete pUnit;
			aProcessingUnit = NULL;
			}
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// From class MAudioContext
// CAudioContext::DeleteAudioProcessingUnit
// ---------------------------------------------------------------------------
//
void CAudioContext::DeleteAudioProcessingUnit(
	MAudioProcessingUnit *&aProcessingUnit)
	{
	DP_CONTEXT(CAudioContext::DeleteAudioProcessingUnit *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	CAudioProcessingUnit* cUnit = static_cast<CAudioProcessingUnit*>(aProcessingUnit);
	TAudioComponentId param = aProcessingUnit->InstanceId();
	TUint count= iAudioProcessingUnits.Count();
		
	for ( TUint i(0); i < count; i++ )
		{
		// find and delete component
		if( iAudioProcessingUnits[i]->InstanceId() == param)
			{
			iAudioProcessingUnits.Remove(i);
			delete cUnit;
			break;
			}
		}
	}

// ---------------------------------------------------------------------------
// From class MAudioContext
// CAudioContext::RegisterAudioContextObserver
// ---------------------------------------------------------------------------
//
TInt CAudioContext::RegisterAudioContextObserver(
	MAudioContextObserver& aObserver)
	{
	DP_CONTEXT(CAudioContext::RegisterAudioContextObserver *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = iAudioContextObservers.Find(&aObserver);
	if( err != KErrNotFound )
		{
		err = KErrAlreadyExists;
		}
	else
		{
		err = iAudioContextObservers.Append(&aObserver);
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// From class MAudioContext
// CAudioContext::UnregisterAudioContextObserver
// ---------------------------------------------------------------------------
//
void CAudioContext::UnregisterAudioContextObserver(
	MAudioContextObserver& aObserver)
	{
	DP_CONTEXT(CAudioContext::UnregisterAudioContextObserver *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt idxOrErr = iAudioContextObservers.Find(&aObserver);
	if( idxOrErr != KErrNotFound )
		{
		iAudioContextObservers.Remove(idxOrErr);
		}
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// From class MAudioContext
// CAudioContext::Interface
// ---------------------------------------------------------------------------
//
TAny* CAudioContext::Interface(TUid aType)
	{
	DP_CONTEXT(CAudioContext::Interface *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TAny* interface = NULL;
	if(aType == KUIdAudioResourceNotification)
		{
		#ifndef SYMBIAN_DISABLE_ARN
		interface = static_cast<MA3FDevSoundAutoPauseResume*>(this);
		#else
		interface = NULL;
		#endif
		}
	else
		{
		interface = NULL;
		}
	DP_OUT();
	return interface;
	}


// ---------------------------------------------------------------------------
// From class MAudioContext
// CAudioContext::Reset
// ---------------------------------------------------------------------------
//
TInt CAudioContext::Reset()
	{
	DP_CONTEXT(CAudioContext::Reset *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;
	if (iInCommit)
		{
		err = KErrNotReady;
		}
	iDesiredChain->ResetMessage();
	iDesiredChain->CopySettings(*iActualChain);
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// From class MMultimediaResourceControlObserver
// CAudioContext::ReceiveResourceResponse
// ---------------------------------------------------------------------------
//
void CAudioContext::ReceiveResourceResponse(MLogicalChain* aAppliedChain, TInt aError)
	{
	DP_CONTEXT(CAudioContext::ReceiveResourceResponse *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	iCriticalSection.Wait();
	iMsgVersion++;
	iMMRC.ResetMessages();
	iCriticalSection.Signal();
	
	if ( aError == KErrNone || iPreempted )
		{
		if ( iPreempted )
			{
			iPreempted = EFalse;
			}
		// Desired chain was commited succesfully
		TInt err;
		MLogicalChain* lc = NULL;
		TRAP(err, lc = aAppliedChain->CloneL());
		DP1(DLERR,"ECloneLeftWhenReceivingResourceResponse %d", err);
		__ASSERT_DEBUG(err == KErrNone, Panic(ECloneLeftWhenReceivingResourceResponse));
		if ( iActualChain )
			{
			iActualChain->Release();
			}
		iActualChain = lc;
		}
	else
		{
		iDesiredChain->CopySettings(*iActualChain);
		}
	iDesiredChain->ResetMessage();
	ContextEventSignal(KUidA3FContextUpdateComplete, aError);
	iInCommit = EFalse;
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// From class MMultimediaResourceControlObserver
// CAudioContext::ReceiveResourceUpdate
// ---------------------------------------------------------------------------
//
void CAudioContext::ReceiveResourceUpdate(MLogicalChain* /*aMessage*/, TInt aError)
	{
	DP_CONTEXT(CAudioContext::ReceiveResourceUpdate *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	ContextEventSignal(KUidA3FContextCommitUpdate, aError);	
	DP_OUT();
	}


// ---------------------------------------------------------------------------
// From class MMultimediaResourceControlObserver
// CAudioContext::ReceivePreemptionUpdate
// ---------------------------------------------------------------------------
//
void CAudioContext::ReceivePreemptionUpdate(MLogicalChain* /*aMessage*/, TInt aError)
	{
	DP_CONTEXT(CAudioContext::ReceivePreemptionUpdate *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iPreempted = ETrue;
	if(iInCommit)
		{
		// Send only one callback
		ContextEventSignal(KUidA3FContextPreEmptedCommit, aError);
		}
	else
		{
		// Send two callbacks
		ContextEventSignal(KUidA3FContextPreEmption, aError);
		ContextEventSignal(KUidA3FContextPreEmptionUpdate, aError);
		}
	DP_OUT();
	}


// ---------------------------------------------------------------------------
// From class MLogicalSettingObserver
// CAudioContext::ReceiveComponentSettingsChange
// ---------------------------------------------------------------------------
void CAudioContext::ReceiveComponentSettingsChange(TUid /*aId*/, TMMRCMessageType aMessageType)
	{
	iDesiredChain->SetMessageType(aMessageType);
	}

// ---------------------------------------------------------------------------
// Internals
// CAudioContext::GetAndSetInstanceID
// ---------------------------------------------------------------------------
TAudioComponentId CAudioContext::GetAndSetInstanceID()
	{
	DP_CONTEXT(CAudioContext::GetAndSetInstanceID *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TUint id(1); // start ID count from 1
	TAudioComponentId idComponent(1);
	TUint count = iAudioProcessingUnits.Count();
	if ( count < KMaxTUint )
		{
		id = ++count;
		}
	idComponent = id;
	DP_OUT();
	return idComponent;
	}

// ---------------------------------------------------------------------------
// Internals
// CAudioContext::ContextEventSignal
// ---------------------------------------------------------------------------
void CAudioContext::ContextEventSignal(TUid aEvent, TInt aError)
	{
	DP_CONTEXT(CAudioContext::ContextEventSignal *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TUint count = iAudioContextObservers.Count();
	for ( TUint idx(0); idx < count; idx++ )
		{
		iAudioContextObservers[idx]->ContextEvent(aEvent, aError);
		}
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CAudioContext::GetLogicalChain
// ---------------------------------------------------------------------------
EXPORT_C MLogicalChain* CAudioContext::GetLogicalChain(TInt aIndex)
	{
	DP_CONTEXT(CAudioContext::GetLogicalChain *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	MLogicalChain* logChain = NULL;
	if (aIndex == 0)
		{
		logChain = iDesiredChain;
		}
	else
		{
		logChain = iActualChain;
		}
	DP_OUT();
	return logChain;
	}


// ---------------------------------------------------------------------------
// CAudioContext::RegisterAsClient
// ---------------------------------------------------------------------------
TInt CAudioContext::RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData, MA3FDevSoundAutoPauseResumeObserver* aObserverPtr)
	{
	DP_CONTEXT(CAudioContext::RegisterAsClient *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	iObserverPtr = aObserverPtr;
	err = iMMRC.RegisterAsClient(aEventType, aNotificationRegistrationData);
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CAudioContext::CancelRegisterAsClient
// ---------------------------------------------------------------------------
TInt CAudioContext::CancelRegisterAsClient(TUid aEventType)
	{
	DP_CONTEXT(CAudioContext::CancelRegisterAsClient *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	err = iMMRC.CancelRegisterAsClient(aEventType);
	DP0_RET(err, "%d");
	}
	
// ---------------------------------------------------------------------------
// CAudioContext::WillResumePlay
// ---------------------------------------------------------------------------
TInt CAudioContext::WillResumePlay()
	{
	DP_CONTEXT(CAudioContext::WillResumePlay *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	err = iMMRC.WillResumePlay();
	DP0_RET(err, "%d");
	}
	
// ---------------------------------------------------------------------------
// From class MMultimediaResourceControlObserver
// CAudioContext::CanResume
// ---------------------------------------------------------------------------
//
void CAudioContext::CanResume()
	{
	DP_CONTEXT(CAudioContext::CanResume *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	if(iObserverPtr)
		{
		iObserverPtr->NotifyResume();
		}	
	DP_OUT();
	}

void CAudioContext::Panic(TAudioContextPanicCodes aCode)
	{
	User::Panic(KAudioContextPanicCategory, aCode);
	}
	
EXPORT_C TInt CAudioContext::MsgVersion()
	{
	return iMsgVersion;
	}
	
EXPORT_C RCriticalSection& CAudioContext::CriticalSection()
	{
	return iCriticalSection;
	}
// End of file
