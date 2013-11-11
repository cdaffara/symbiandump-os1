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

/**
 @file
 @internalComponent
*/

#ifndef HCTLBCSPFRAMEQUEUE_H
#define HCTLBCSPFRAMEQUEUE_H

#include <e32base.h>

// To Turn on window debuggng #define _DEBUG_WINDOW
#undef _DEBUG_WINDOW

class CHCTLBcspSequencer;
class CHCTLBcsp;
class CTxHctlBcspFrame;

NONSHARABLE_CLASS(CHCTLBcspWindow) : public CBase
/**
Window and associated functions to contain a queue of frames
handles acknowledgement of a particular received frame and moves the window onwards
within the scope of total number of sequence IDs

Is only used by CHCTLBcspFrameQueue
*/
	{
public:
	static CHCTLBcspWindow *NewL(TInt aWindowSize, TInt aFrameSize, TInt aNumberOfSeqIds);
	~CHCTLBcspWindow();

	TInt FreeFrames();

    CTxHctlBcspFrame* WriteFrame(); // Gets the write frame and moves the WritePosn
	CTxHctlBcspFrame* ReadFrame();  // Gets the read frame and moves the ReadPosn

	TBool Acknowledged(TInt AckNo);
	TInt StartOfWindow() const;
 	void Reset();

private:
	void ConstructL(TInt aFrameSize);
	CHCTLBcspWindow(TInt aWindowSize, TInt aNumberOfSeqIds);

	// Two simple inline private methods to reduce complexity of code whilst maintaining
	// efficiency
	TInt ToSeqId(TInt aIndex) const;
	TInt ToWindowIndex(TInt aSeqId) const; 
	void IncrementSeqId(TInt &aSeqId) const;
	CTxHctlBcspFrame *GetFrame(TInt aIndex);

	// Internal consistency checking for the class
    void __DbgTestInvariant() const;
	
#ifdef _DEBUG_WINDOW
	void TraceQueue();
	void TraceWindowVars() const;
#endif


private:
	TInt iStartOfWindow;
	TInt iReadPosition;
	TInt iHighReadPosition;
	TInt iWritePosition;
	TInt iEndOfWindow;
	TInt iWindowSize;
	TInt iNumberOfSeqIds;
	TSglQue<CTxHctlBcspFrame> iFrameQue;

	// This class stores most of its indices as SequneceIds, the only exception being
	// iWindowSize which is used in the conversion between SequenceIds and WindowIndex.
	//
	//   SequenceIds          Window Index
	//                     |
	//   iStartOfWindow    +   .
	//                     |   .
	//   iReadIndex        |   .
	//                     |   .
	//   iHighReadIndex    |   .
	//                     |   .
	//   iWriteIndex       |   .
	//                     |   .
	//   iEndOfWindow      +   iWindowSize
	//                     |
	//                     |
	//                     |
	//   iNumberOfSeqIds   |
	};

NONSHARABLE_CLASS(CHCTLBcspFrameQueue) : public CBase
/**
This is used by CHCTLBcsp to queue frames for transmission
the frames queued are stored in CHCTLBcspWindow
there are two windows used 
	one for reliable frames
	one for unreliable frames
*/
	{
public:
	static CHCTLBcspFrameQueue* NewL(CHCTLBcsp &aBcsp);
	~CHCTLBcspFrameQueue();

	void SetSequencer(CHCTLBcspSequencer& aSequencer);

	// Methods called by sequencer
	// Gets the next frame for sending 
	// (aFrame is the slip encoded frame
	//  returned by this method)
	//
	// returns: -
	//
	// KErrNone          - all is fine
	// KErrNothingToSend - nothing 
	//                     available to
	//                     send
	// KErrMaxRetries    - exceeded max 
	//                     retries for a
	//                     frame
	TInt GetNextFrame(TDesC8* &aFrame, TBool& aIsReliable);

	TBool AckReceived(TInt aAckValue); // Ack has been Received
	void AckTimeout();                 // Rx Ack Timer has expired

	// Methods called by CHCTLBcsp
	TInt AddReliableFrame  (const TDesC8 &aData, TUint8 aProtocolId);
	void AddUnreliableFrame(const TDesC8 &aData, TUint8 aProtocolId, 
												 TUint8 aAck,
												 TBool aCRCEnabled);

	void AddUnreliableFrame(TUint8 aProtocolId, 
							TUint8 aAck,
							TBool aCRCEnabled);
	void Reset();

private:
	void CanSend(); 
	void ConstructL();
	CHCTLBcspFrameQueue(CHCTLBcsp& aBcsp);

private:
	// Not owned by this class
	CHCTLBcspSequencer* iSequencer; // For calls to Wakeup()
	CHCTLBcsp& iBcsp; // For calls to CanSend()

	// Owned by this class
	CHCTLBcspWindow* iReliableQueue;
	CHCTLBcspWindow* iUnreliableQueue;

	HBufC8* iSlipEncodedFrame;
	};

#endif // HCTLBCSPFRAMEQUEUE_H
