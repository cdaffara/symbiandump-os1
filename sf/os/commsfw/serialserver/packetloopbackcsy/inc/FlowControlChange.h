// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// FlowControlChange.cpp
// Class that listens for requests to change flow control in packet loopback
// 
//

/**
 @file
*/

#ifndef __FLOW_CONTROL_CHANGE__
#define __FLOW_CONTROL_CHANGE__

#include <e32base.h>
#include <e32property.h>

class MFlowControlChangeCallBack
	{
	public:
	/** This method is called when flow control is started */
	virtual void StartFlowControl() = 0;
	/** This method is called when flow control is stopped */
	virtual void StopFlowControl() = 0;
	};
	
NONSHARABLE_CLASS(CFlowControlChange) : public CActive
	{
public:
	static CFlowControlChange* NewL(TUint aUnit, MFlowControlChangeCallBack* aCallback);
	~CFlowControlChange();

	TBool FlowControlIsOn() const;

private:
	CFlowControlChange(TUint aUnit, MFlowControlChangeCallBack* aCallback);
	
	void RunL();
	void DoCancel();

    /** Class with implementation of MFlowControlChangeCallBack::StartFlowControl that is called when the flow control is changed */
	MFlowControlChangeCallBack *iCallback;
	/** The property to listen on for changes to flow control */
	RProperty iProperty;
	/** The port to listen on for changes to flow control */
	TUint iUnit;
	/** True if flow control is on */
	TBool iFlowControlIsOn;
	};

#endif

