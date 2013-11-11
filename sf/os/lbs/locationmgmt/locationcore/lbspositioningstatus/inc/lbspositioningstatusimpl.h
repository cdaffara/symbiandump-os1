// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalTechnology
*/

#ifndef POSITIONING_STATUS_IMPL_H
#define POSITIONING_STATUS_IMPL_H

#include <e32base.h>
#include "lbspositioningstatus.h"
#include "lbsinternalposstatuswatcher.h"

/**
The class provides an internal implementation of the functionality exposed 
by the CLbsPositioningStatus interface.

@see CLbsPositioningStatus
@see MLbsInternalPosStatusObserver

@internalComponent
@prototype
*/
NONSHARABLE_CLASS(CLbsPositioningStatusImpl) : public CBase, 
                                               public MLbsInternalPosStatusObserver
	{
public:
	IMPORT_C static CLbsPositioningStatusImpl* NewL(MLbsPositioningStatusObserver& aObserver);
	virtual ~CLbsPositioningStatusImpl();
	
public:
	/** Returns the current positioning status */
	IMPORT_C CLbsPositioningStatus::TLbsPositioningStatus CurrentPositioningStatus();

private:
	CLbsPositioningStatusImpl(MLbsPositioningStatusObserver& aObserver);
	void ConstructL();

    /** from MLbsInternalPosStatusObserver */
    void OnInternalStatusUpdate(const TLbsPositioningStatusType& aType, const TInt& aCount);
	    
    /** Returns the combined MO/NI positioning status */
    CLbsPositioningStatus::TLbsPositioningStatus CombinedStatus();
	
private:
	/** Prohibit copy constructor */
	CLbsPositioningStatusImpl(const CLbsPositioningStatusImpl&);
	/** Prohibit assigment operator */
	CLbsPositioningStatusImpl& operator= (const CLbsPositioningStatusImpl&);

private:
	/** Consumer for positioning status updates, SysApp */
	MLbsPositioningStatusObserver& iObserver;
	
	TUint iLastKnownMoStatus;
	TUint iLastKnownNiStatus;

	/** P&S API Watchers */
    CLbsInternalPosStatusWatcher* iMoWatcher;
    CLbsInternalPosStatusWatcher* iNiWatcher;
	};

#endif // POSITIONING_STATUS_IMPL_H
