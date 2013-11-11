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



#ifndef AUDIOPROCESSINGUNIT_H
#define AUDIOPROCESSINGUNIT_H

#include <e32base.h>
#include <ecom/ecom.h>
#include <a3f/a3f_trace_utils.h>
#include <a3f/audioprocessingunittypeuids.h>
#include <a3f/maudioprocessingunit.h>
#include <a3f/maudioprocessingunitobserver.h>

#include "componentparameters.h"
#include "mlogicalsettingsobserver.h"


/**
A generic AudioComponent base class implementation.
@see MAudioProcessingUnit
*/
class CAudioProcessingUnit: public CBase, 
							public MAudioProcessingUnit
	{

public:

	static CAudioProcessingUnit* NewL(TComponentParameters aParameters);

	/**
	Destructor.
	Releases all resources and deletes all objects owned by this instance.
	*/
	virtual ~CAudioProcessingUnit();

	// from base class MAudioProcessingUnit
	virtual TAudioComponentId InstanceId() const;
	virtual TAudioContextId ContextId() const;
	virtual TBool IsTypeOf(TUid aTypeId) const;
	virtual TAny* Interface(TUid aType);
	virtual TInt RegisterProcessingUnitObserver(MAudioProcessingUnitObserver& aObserver);
	virtual void UnregisterProcessingUnitObserver(MAudioProcessingUnitObserver& aObserver);

	CAudioProcessingUnit(TAny* aParam);

protected: // data
	// Component type Uid
	TUid iType;

	// Audio context Id
	TAudioContextId iContextId;

	// Component instance Id
	TAudioComponentId iInstanceId;

	// Array of component observers.
	RPointerArray<MAudioProcessingUnitObserver> iAudioProcessingUnitObservers;

	// Settings observer
	MLogicalSettingObserver* iSettingsObserver;

private: // data
	TUid iDtor_ID_Key;
	};

// Interface's (abstract base class's) destructor
inline CAudioProcessingUnit::~CAudioProcessingUnit()
	{
	iAudioProcessingUnitObservers.Close();
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

// Interface's (abstract base class's) static factory method implementation.
// Asks ECOM plugin framework to instantiate appropriate concret plugin
// implementation.
inline CAudioProcessingUnit* CAudioProcessingUnit::NewL(TComponentParameters aParameters)
	{	
	TAny* ptr = REComSession::CreateImplementationL( aParameters.iTypeUid , _FOFF(CAudioProcessingUnit, iDtor_ID_Key), static_cast<TAny*>(&aParameters));
	return static_cast<CAudioProcessingUnit*>(ptr);
	}

inline CAudioProcessingUnit::CAudioProcessingUnit(TAny* aParam)
	{
	TComponentParameters* param = static_cast<TComponentParameters*>(aParam);
	iType = param->iTypeUid;
	iInstanceId = param->iInstanceId;
	iContextId = param->iContextId;
	iSettingsObserver = param->iSettingsObserver;
	}

// ---------------------------------------------------------------------------
// from class MAudioProcessingUnit
// CLogicalAudioCodec::GetContextId
// ---------------------------------------------------------------------------
inline TAudioContextId CAudioProcessingUnit::ContextId() const
	{
	return iContextId;
	}

inline TAudioComponentId CAudioProcessingUnit::InstanceId() const 
	{
	return iInstanceId;
	}

// ---------------------------------------------------------------------------
// from class MAudioProcessingUnit
// CLogicalAudioCodec::RegisterComponentObserver
// ---------------------------------------------------------------------------
inline TInt CAudioProcessingUnit::RegisterProcessingUnitObserver(MAudioProcessingUnitObserver& aObserver)
	{
	TInt err = iAudioProcessingUnitObservers.Find(&aObserver);
	if( err != KErrNotFound )
		{
		err = KErrAlreadyExists;
		}
	else
		{
		err = iAudioProcessingUnitObservers.Append(&aObserver);
		}
	return err;
	}

// ---------------------------------------------------------------------------
// from class MAudioProcessingUnit
// CLogicalAudioCodec::UnregisterComponentObserver
// ---------------------------------------------------------------------------
inline void CAudioProcessingUnit::UnregisterProcessingUnitObserver(MAudioProcessingUnitObserver& aObserver)
	{
	TInt idxOrErr = iAudioProcessingUnitObservers.Find(&aObserver);
	if( idxOrErr != KErrNotFound )
		{
		iAudioProcessingUnitObservers.Remove(idxOrErr);
		}
	}

inline TBool CAudioProcessingUnit::IsTypeOf(TUid) const {return EFalse;}
inline TAny* CAudioProcessingUnit::Interface(TUid) { TAny* any(NULL); return any;}

#endif // AUDIOPROCESSINGUNIT_H
