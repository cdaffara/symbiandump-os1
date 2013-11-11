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
//

#ifndef L2CAPSDU_H_
#define L2CAPSDU_H_

#include <e32def.h>
#include <e32base.h>
#include <es_mbuf.h>

#include "L2CapPDU.h"

class CL2CapSDU;
class ML2CapSDUHandler
	{
public:
	virtual void ProcessFlushTimerExpiry(CL2CapSDU& aL2CapSDU) = 0;
	};



NONSHARABLE_CLASS(CL2CapSDU) : public CBase
	{
public:		
	// MBuf based descriptor creation.
	static CL2CapSDU* NewLC(RMBufChain& aSDUData,
                            ML2CapSDUHandler& aParent,
			                TUint16 aPDUSize,
                            TBool aIsBasicDataVersion = ETrue,
			                TUint16 aTimerDuration = KInfiniteFlush);
	static CL2CapSDU* NewL(RMBufChain& aSDUData,
		                   ML2CapSDUHandler& aParent,
			               TUint16 aPDUSize,
		                   TBool aIsBasicDataVersion = ETrue,
			               TUint16 aTimerDuration = KInfiniteFlush);

	static TInt FlushTimerExpired(TAny* aL2CapSDU);
	
	
	virtual ~CL2CapSDU();

	static TInt GetSDUOverhead(TBool aBasicMode);

	TBool GetPDU(HL2CapPDU*& aReturnedPDU);
	
	TBool IsSDUEmpty() const;
	TBool CurrentPDUIsFirstPDU();

	void StartFlushTimer(TUint16 aTimerDuration);
	
	TInt ChangeSDUSegmentation(TBool aIsBasicDataVersion, TUint16 aPDUSize);
#ifdef _DEBUG
	TInt DebugManualFlush();
#endif

		
private:
	CL2CapSDU(ML2CapSDUHandler& aParent, RMBufChain& aDataFrame, TUint16 aReassembledSDULength);
	CL2CapSDU(ML2CapSDUHandler& aParent);

	void ConstructL(RMBufChain& aSDUData, TBool aIsBasicDataVersion, TUint16 aTimerDuration, TUint16 aPDUSize); 
	TInt SegmentSDUIntoPDUs(RMBufChain& aSDUData, TBool aIsBasicDataVersion, TUint16 aPDUSize);
	void HandleFlushTimerExpired();

public:
	TDblQueLink iLink;
	
private:
	ML2CapSDUHandler& iParent;

	TDblQue<HL2CapPDU> iPDUs;
	TDblQueIter<HL2CapPDU> iCurrentPDU;

	TDeltaTimerEntry iFlushTimerEntry;
	TBool iFlushTimerRunning;
	
	RMBufChain iIncomingSDU;
	};


#endif
