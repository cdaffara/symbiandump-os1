// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BTCOMMUTIL_H__
#define BTCOMMUTIL_H__

_LIT(KBTCommPanicName, "BTComm Panic");

enum TBTCommPanic
	{
	EBTCommBadEventForThisState, // we should have never received this event in this state
	EBTCommPortBufferTerminatorCharNoOtherThanExpected,
	EBTCommPortBufferTerminatedCharsFoundBogus,
	EBTCommPortReaderPotentialLossOfIncomingData,
	EBTCommOpenStateWriteToClientPossibleLossOfData,
	EBTCommCloseWhileWaitingForSockServHandle,
	EBTCommESockSessionAttachNotDone,
	EBTCommPortStateOutOfBounds
	};
	
/**
Panics raised by state machine default behaviour are multiplied by this
factor.  The less signifigant digits are then available to carry a state
index, allowing more information when fixing defects.

For example, a bad event passed to the BTComm state machine may raise panic
'BTComm Panic 1008' if it occurs in the 'Connecting' state.
*/
const TInt KStatePanicMultiplier = 10000;

/**
A delimiter to prevent a panic code of 0 creating ambigious panics.
Without this panic code 0 in state 8 and panic code 8 (not in the state 
machine) would be identical.
*/
const TInt KStatePanicDelimiter = 1000; // '1' is the delimiter

/**
If for any reason the current state cannot be found, this value is placed
in the state field of a panic.
*/
const TInt KUnknownState = KStatePanicDelimiter - 1;

class BTCommUtil
	{
public:
	static void Panic(TBTCommPanic aPanic);
	static void Panic(TBTCommPanic aPanic, TInt aState);
	};

#endif
