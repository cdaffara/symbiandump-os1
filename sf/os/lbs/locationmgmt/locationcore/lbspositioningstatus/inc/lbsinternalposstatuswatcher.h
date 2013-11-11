/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @file cmopositioningstatuswatcher.h
 @internalTechnology
*/
#ifndef MO_POSITIONING_STATUS_LISTENER_H_
#define MO_POSITIONING_STATUS_LISTENER_H_

#include <e32base.h>
#include <e32property.h>

/** A type for the internal positioning status updates */
typedef TUint32 TLbsPositioningStatusType;

/** The response to a privacy request.

Note that the _TLbsPositioningStatusType enum may be extended in the future 
by adding more enumerated values. To maintain compatibility any unrecognized 
values must be handled as ELbsPositioningStatusUnknown. 
*/
enum _TLbsPositioningStatusType
    {
        /** Not used/unknown */
    ELbsPositioningStatusUnknown = 0,
        /** Used to indicate Mobile Originated positioning status. */
    ELbsPositioningStatusMO      = 1,
        /** Used to indicate Network Initiated positioning status. */
    ELbsPositioningStatusNI      = 2
    };


class MLbsInternalPosStatusObserver
	{
public:

	/** Send Positioning Status to the Observer.
		Called whenever a change to the devices positioning status occurs.
	@param aType   The positioning status type being reported
	@param aStatus The current positioning status
	*/
	virtual void OnInternalStatusUpdate(const TLbsPositioningStatusType& aType, const TInt& aStatus) = 0;
	};


class CLbsInternalPosStatusWatcher : public CActive
	{
public:
	static CLbsInternalPosStatusWatcher* NewL(const TLbsPositioningStatusType& aType, MLbsInternalPosStatusObserver& aObserver);
	~CLbsInternalPosStatusWatcher();

private:
	// Constructors
	void ConstructL();
	CLbsInternalPosStatusWatcher(const TLbsPositioningStatusType& aType, MLbsInternalPosStatusObserver& aObserver);
	
	// CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	// Observer
	MLbsInternalPosStatusObserver& iObserver;

	// the positioning status type watched.
	TLbsPositioningStatusType iType;
	
	// handle to the P&S property being watched
	RProperty iProperty;
	};


#endif // MO_POSITIONING_STATUS_LISTENER_H_ 
