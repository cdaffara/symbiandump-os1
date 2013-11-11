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

#ifndef MAUDIOSTREAMADAPTATIONOBSERVER_H
#define MAUDIOSTREAMADAPTATIONOBSERVER_H


enum TPhysicalEvent
	{
	EOperationComplete,
	EOperationIncomplete, 
	};

// TO DO Use real Uid
const TUid KUidAudioStreamAdaptationObserver = {0x102834D2};

class MAudioStreamAdaptationObserver
	{
public:
	virtual void PhysicalAdaptationEvent(TPhysicalEvent aEvent, TInt aError) = 0;
	virtual void StateEvent(TInt aReason, TAudioState aNewState) = 0;
	virtual void AddProcessingUnitComplete(TUid aType, TInt aError) = 0;
	virtual void RemoveProcessingUnitComplete(TUid aType, TInt aError) = 0;
	virtual void ProcessingFinished() = 0;
	virtual void FlushComplete(TInt aError) = 0;
	};

#endif  // MAUDIOSTREAMADAPTATIONOBSERVER_H