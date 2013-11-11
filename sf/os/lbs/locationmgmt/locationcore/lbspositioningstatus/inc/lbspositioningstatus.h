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
 @publishedPartner
 */

#ifndef POSITIONING_STATUS_H
#define POSITIONING_STATUS_H

#include <e32base.h>

class CLbsPositioningStatusImpl;
class MLbsPositioningStatusObserver;

/**
Positioning Status Library's API.

This class provides an API by which an observer can register for notifications
regarding the current status of LBS, i.e. if one or more positioning sessions
are currently in progress or otherwise. 

@see MLbsPositioningStatusObserver
@capability Location
*/
NONSHARABLE_CLASS(CLbsPositioningStatus) : public CBase
	{
public:
    /** Positioning Status define */
    typedef TUint32 TLbsPositioningStatus;
    
    /** Positioning Status enumeration 
     *  Note that this enumeration may be extended by future updates */
    enum _TLbsPositioningStatus
        {
        ELbsPositioningStatusNotActive    = 0,
        ELbsPositioningStatusActive       = 1
        };

public:
	IMPORT_C static CLbsPositioningStatus* NewL(MLbsPositioningStatusObserver& aObserver);
	virtual ~CLbsPositioningStatus();
	
public:
	/** Returns the current positioning status */
	IMPORT_C TLbsPositioningStatus CurrentPositioningStatus();

private:
	CLbsPositioningStatus();
	void ConstructL(MLbsPositioningStatusObserver& aObserver);

private:
    /** Handle to the internal implementation */
	CLbsPositioningStatusImpl* iImpl;
	};


/** Positioning Status observer definition.
This interface is implemented by observers to handle calls from
the Positioning Status manager.

@see CLbsPositioningStatus
*/
class MLbsPositioningStatusObserver
    {
public:
    IMPORT_C virtual TVersion Version() const;

    /** Send Positioning Status to the Observer.
        Called whenever a change to the devices positioning status occurs.
    @param aPositioningStatus The current positioning status
    */
    virtual void OnPositioningStatusUpdate(const CLbsPositioningStatus::TLbsPositioningStatus& aPositioningStatus) = 0;
    };


#endif // POSITIONING_STATUS_H
