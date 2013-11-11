/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef AUDIOSTREAMMANAGER_H
#define AUDIOSTREAMMANAGER_H

#include <e32base.h>
#include <ecom/ecom.h>
#include <a3f/a3f_trace_utils.h>
#include <a3f/a3fbase.h>
#include <a3f/maudiostream.h>

/**
	A generic AudioComponent base class implementation.
	@see MAudioProcessingUnit
*/
NONSHARABLE_CLASS(CAudioStreamManager) :	public CBase,
											public MAudioStream
	{
public:
	static CAudioStreamManager* NewL(TUid aImplementationUid);
	/**
		Destructor.
		Releases all resources and deletes all objects owned by this instance.
	*/
	virtual ~CAudioStreamManager();

	//From MAudioStream
	virtual TInt RegisterAudioStreamObserver(MAudioStreamObserver& aObserver);
	virtual void UnregisterAudioStreamObserver(MAudioStreamObserver& aObserver);
	virtual TInt SetAudioType(const TAudioTypeSettings& aAudioTypeSettings);
	virtual void GetAudioType(TAudioTypeSettings& aAudioTypeSettings) const;
	virtual TAudioStreamId InstanceId() const;
	CAudioStreamManager();

protected: // data
	TAudioStreamId iAudioStreamId;
	TAudioTypeSettings  iAudioTypeSettings;
	RPointerArray<MAudioStreamObserver> iAudioStreamObserver;

private:
	TUid iDtor_ID_Key;

	};


// Interface's (abstract base class's) destructor
inline CAudioStreamManager::~CAudioStreamManager()
	{
	iAudioStreamObserver.Close();
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

// Interface's (abstract base class's) static factory method implementation.
// Asks ECOM plugin framework to instantiate appropriate concret plugin
// implementation.
inline CAudioStreamManager* CAudioStreamManager::NewL(TUid aImplementationUid)
	{
	TAny* ptr = REComSession::CreateImplementationL( aImplementationUid , _FOFF(CAudioStreamManager, iDtor_ID_Key));
	return static_cast<CAudioStreamManager*>(ptr);
	}

inline CAudioStreamManager::CAudioStreamManager()
	{
	//iAudioStreamId
	}

// ---------------------------------------------------------------------------
// CAudioStreamManager::RegisterAudioStreamObserver
// ---------------------------------------------------------------------------
inline TInt CAudioStreamManager::RegisterAudioStreamObserver(MAudioStreamObserver& aObserver)
	{
	TInt err = iAudioStreamObserver.Find(&aObserver);
	if(err == KErrNotFound)
		{
		err = iAudioStreamObserver.Append(&aObserver);
		}
	else
		{
		err = KErrAlreadyExists;
		}
	return err;
	}

// ---------------------------------------------------------------------------
// CAudioStreamManager::UnregisterAudioStreamObserver
// ---------------------------------------------------------------------------
inline void CAudioStreamManager::UnregisterAudioStreamObserver(MAudioStreamObserver& aObserver)
	{
	TInt idxOrErr = iAudioStreamObserver.Find(&aObserver);
	if( idxOrErr != KErrNotFound )
		{
		iAudioStreamObserver.Remove(idxOrErr);
		}
	}

// ---------------------------------------------------------------------------
// CAudioStreamManager::InstanceId
// ---------------------------------------------------------------------------
inline TAudioStreamId CAudioStreamManager::InstanceId() const
	{
	return iAudioStreamId;
	}

// ---------------------------------------------------------------------------
// CAudioStreamManager::SetAudioType
// ---------------------------------------------------------------------------
inline TInt CAudioStreamManager::SetAudioType(const TAudioTypeSettings& aAudioTypeSettings)
	{
	TInt err(KErrNone);
	iAudioTypeSettings = aAudioTypeSettings;
	return err;
	}

// ---------------------------------------------------------------------------
// CAudioStreamManager::GetAudioType
// ---------------------------------------------------------------------------
inline void CAudioStreamManager::GetAudioType(TAudioTypeSettings& aAudioTypeSettings) const
	{
	aAudioTypeSettings = iAudioTypeSettings;
	}
#endif // CAUDIOSTREAMFACTORY_H
