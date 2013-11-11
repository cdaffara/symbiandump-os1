/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implementation of the base class for effects.
*
*/



// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif
#include <AudioEffectBase.h>
#include <MAudioEffectObserver.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAudioEffect::CAudioEffect
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEffect::CAudioEffect()
    : 	iEnabled(EFalse),
    	iEnforced(EFalse),
    	iHaveUpdateRights(ETrue)
    {
    }


// Destructor
EXPORT_C CAudioEffect::~CAudioEffect()
    {
	iObservers.Close();
    }

// -----------------------------------------------------------------------------
// CAudioEffect::IsEnable
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CAudioEffect::IsEnabled() const
    {
	return iEnabled;
    }

// -----------------------------------------------------------------------------
// CAudioEffect::Enable
// -----------------------------------------------------------------------------
//
EXPORT_C void CAudioEffect::EnableL()
    {
	iEnabled = ETrue;
	ApplyL();
    }

// -----------------------------------------------------------------------------
// CAudioEffect::Disable
// -----------------------------------------------------------------------------
//
EXPORT_C void CAudioEffect::DisableL()
    {
	iEnabled = EFalse;
	ApplyL();
    }

// -----------------------------------------------------------------------------
// CAudioEffect::Enforce
// -----------------------------------------------------------------------------
//
EXPORT_C void CAudioEffect::EnforceL(
	TBool aEnforced )
    {
	iEnforced = aEnforced;
	ApplyL();
    }

// -----------------------------------------------------------------------------
// CAudioEffect::HaveUpdateRights
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CAudioEffect::HaveUpdateRights() const
    {
	return iHaveUpdateRights;
    }

// -----------------------------------------------------------------------------
// CAudioEffect::IsEnforced
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CAudioEffect::IsEnforced() const
    {
	return iEnforced;
    }

// -----------------------------------------------------------------------------
// CVolume::RegisterObserverL
// -----------------------------------------------------------------------------
//
EXPORT_C void CAudioEffect::RegisterObserverL(
	MAudioEffectObserver& aObserver )
	{
	User::LeaveIfError(iObservers.Append(&aObserver));
	}

// -----------------------------------------------------------------------------
// CVolume::UnRegisterObserver
// -----------------------------------------------------------------------------
//
EXPORT_C void CAudioEffect::UnRegisterObserver(
	MAudioEffectObserver& aObserver )
	{
	TInt index = iObservers.Find(&aObserver);
	if( index != KErrNotFound )
		{
		iObservers.Remove(index);
		}
	}

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
