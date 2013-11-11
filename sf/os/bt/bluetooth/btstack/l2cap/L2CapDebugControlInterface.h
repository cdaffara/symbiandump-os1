// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef L2CAP_DEBUG_CONTROL_INTERFACE_H_
#define L2CAP_DEBUG_CONTROL_INTERFACE_H_

#include <e32std.h>
#include <e32property.h>
#include <bluetooth/l2capdebuginfo.h>

using namespace L2capDebugInfo;

#ifdef _DEBUG
	#include "debug.h"
	#define L2CAP_DEBUG(x) (static_cast<SBtTls*>(Dll::Tls()))->iDebugCtrlIF->x
	//PDU tracking allocates memory on each PDU event an only free them on stack unload.
	//Therefore it is by default macroed out to prevent udeb performance degradation.
	#ifdef TRACK_L2CAP_PDU
		#define L2CAP_DEBUG_PDU(x) L2CAP_DEBUG(x)
	#else
		#define L2CAP_DEBUG_PDU(x)
	#endif
#else
	#define L2CAP_DEBUG(x)
	#define L2CAP_DEBUG_PDU(x)
#endif



NONSHARABLE_CLASS(TPDUTimerRecord)
	{
public:
	enum TPDUTimerType
		{
		EPDUBFrameTimer,
		EPDUIFrameTimer,
		EPDUUnsegmentedFrameTimer,
		};
		
	TPDUTimerRecord(TAny* aUID, TUint16 aGroupID, TPDUTimerType aTimerType);

	// Inlines
	inline TAny* GetUID() const;
	inline TUint16 GetGroupID() const;

	inline const TTimeIntervalMicroSeconds& GetPDUTI() const;
	inline const TTimeIntervalMicroSeconds& SentPDUTI() const;
		
	void MarkGetPDUCalled();
	void MarkPDUSent();
	
private:
	TTime iTime;
	TAny* iUID;
	TUint16 iGroupID;
	TPDUTimerType iTimerType;
	
	TTimeIntervalMicroSeconds iGetPDUTI;
	TTimeIntervalMicroSeconds iSentPDUTI;
	};

inline TAny* TPDUTimerRecord::GetUID() const	
	{
	return iUID;
	}
	
inline TUint16 TPDUTimerRecord::GetGroupID() const	
	{
	return iGroupID;
	}

inline const TTimeIntervalMicroSeconds& TPDUTimerRecord::GetPDUTI() const
	{
	return iGetPDUTI;	
	}
	
inline const TTimeIntervalMicroSeconds& TPDUTimerRecord::SentPDUTI() const
	{
	return iSentPDUTI;	
	}


NONSHARABLE_CLASS(TPDUGroupTimerRecord)
	{
public:
	TPDUGroupTimerRecord(TUint16 aGroupID, RProperty& aProperty);

	void UpdateGroup(const TTimeIntervalMicroSeconds& aGetPDUTime,
					 const TTimeIntervalMicroSeconds& aSentPDUTime);
	
	inline TUint16 GetGroupID() const;				 	

private:
	TInt iMinGetPDU;
	TInt iMinSentPDU;
	TInt iMaxGetPDU;
	TInt iMaxSentPDU;
	TInt iAveGetPDU;
	TInt iAveSentPDU;
	TUint16 iSampleSize;
	
	TUint16 iGroupID;
	RProperty& iProperty;
	};
	
inline TUint16 TPDUGroupTimerRecord::GetGroupID() const	
	{
	return iGroupID;
	}
	


	
	
NONSHARABLE_CLASS(CDebugControlInterface)
	{
public:
	static CDebugControlInterface* NewL();
	virtual ~CDebugControlInterface();

	// Debug Interface.
	void ObjectAllocation(TObjectType aObjectType, TAllocationType aAllocType);
	void PDUTimer(TPDUTimerEvent aEvent, TAny* aUID, TUint16 aGrpID = 0);
	void UpdateFlushCounters(TFlushCountersEvent aEvent);


	static TBool UIDLinkMatch(const TPDUTimerRecord& aA, 
                              const TPDUTimerRecord& aB);

	static TBool GroupLinkMatch(const TPDUGroupTimerRecord& aA, 
                                const TPDUGroupTimerRecord& aB);
	 
private:
	CDebugControlInterface();
	void DefinePublications();
	void DeletePublications();
	
private:
	TUint16 iAllocations[EMaxType][EMaxAllocationType];

	RPointerArray<TPDUTimerRecord> iPDUTimerRecords;
	RPointerArray<TPDUGroupTimerRecord> iPDUGroupTimerRecords;

	TUint16 iSDUFlushQueuedSDUFlushed;
	TUint16 iSDUFlushPartialSentSDUFlushed;
	TUint16 iSDUFlushSentSDUFlushed;
	TUint16 iFlushedPDUMuxerResend;
	TUint16 iFlushedPDUDataFlowResend;
	TUint16 iFlushedPDUIFrame;
	TUint16 iIncompleteErroredSDUReceived;
	
	RProperty iProperty;
	};
	
#endif
