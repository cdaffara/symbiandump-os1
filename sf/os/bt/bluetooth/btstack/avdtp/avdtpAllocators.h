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
// Defines the avdtp allocators
// These classes help track values that are selected from a pool
// The values are released as R-classes to reflect the "resource ownership"
// that is conferred upon the owner.
// 
//

/**
 @file
 @internalComponent
*/


#ifndef _AVDTPALLOCATORS_H
#define _AVDTPALLOCATORS_H

#include <bluetoothav.h>
#include "gavdpinterface.h"

NONSHARABLE_CLASS(TBitFieldAllocator)
/*
abstract base for types that need to use a bit field to handle their values
*/
	{
friend class RTrackedValue;
protected:
	inline TBitFieldAllocator() : iBitField(0) {};
	TInt Get(TInt& aResult, TInt aMax, TInt aMin=0);
	void Free(TInt aVal);
protected:
	TUint64 iBitField;
	TInt iMin;
	};


NONSHARABLE_CLASS(RTrackedValue)
	{
public:
	inline void Close();
	inline void Acquire(RTrackedValue& aTrackedValue); // this takes ownership
protected:
	inline RTrackedValue();
	inline void Set(TBitFieldAllocator& aAllocator, TUint32 aValue);
protected:
	TBitFieldAllocator* iAllocator; //non-owned
	TUint32 iValue;
private:
	RTrackedValue(const RTrackedValue&); // forbid copying
	RTrackedValue& operator=(const RTrackedValue&);
	};
	
inline RTrackedValue::RTrackedValue()
: iAllocator(NULL), iValue(0)
	{
	}
	
inline void RTrackedValue::Set(TBitFieldAllocator& aAllocator, TUint32 aValue)
	{
	iAllocator=&aAllocator;
	iValue=aValue;
	}
	
inline void RTrackedValue::Close()
	{
	if (iAllocator)
		{
		iAllocator->Free(iValue);
		}
	}
	
inline void RTrackedValue::Acquire(RTrackedValue& aValue)
	{
	Set(*aValue.iAllocator, aValue.iValue);
	// to transfer ownership:
	new (&aValue) RTrackedValue;
	}
	
NONSHARABLE_CLASS(RAvdtpTransactionLabel) : public RTrackedValue
	{
friend class TTransactionLabelManager;
public:	
	inline TAvdtpTransactionLabel TransactionLabel() const;
	};
	
NONSHARABLE_CLASS(RTSID) : public RTrackedValue
	{
friend class TTSIDManager;
public:	
	inline TTSID TSID() const;
	};	

NONSHARABLE_CLASS(RTCID) : public RTrackedValue
	{
friend class TTCIDManager;
public:	
	inline TTCID TCID() const;
	};	
	
NONSHARABLE_CLASS(RSEID) : public RTrackedValue
	{
friend class TSEIDManager;
public:	
	inline TSEID SEID() const;
	};

inline TAvdtpTransactionLabel RAvdtpTransactionLabel::TransactionLabel() const
	{
	return static_cast<TAvdtpTransactionLabel>(iValue);
	}

inline TTCID RTCID::TCID() const
	{
	return static_cast<TTCID>(iValue);
	}

inline TTSID RTSID::TSID() const
	{
	return static_cast<TTSID>(iValue);
	}

inline TSEID RSEID::SEID() const
	{
	return static_cast<TSEID>(iValue);
	}

NONSHARABLE_CLASS(TTransactionLabelManager) : public TBitFieldAllocator
	{
public:
	inline TInt	GetLabel(RAvdtpTransactionLabel& aLabel);
	};
	
	
NONSHARABLE_CLASS(TTCIDManager) : public TBitFieldAllocator
	{
public:
	inline TInt GetTCID(RTCID& aTCID);
	inline TInt GrabTCID(RTCID& aTCID, TTCID aAssignedTCID);
	};
	
NONSHARABLE_CLASS(TTSIDManager) : public TBitFieldAllocator
	{
public:
	inline TInt GetTSID(RTSID& aTSID);
	};
	
NONSHARABLE_CLASS(TSEIDManager) : public TBitFieldAllocator
	{
public:
	inline TInt GetSEID(RSEID& aSEID);
	};
	
inline TInt TTransactionLabelManager::GetLabel(RAvdtpTransactionLabel& aLabel)
	{
	TInt val, res;
	res = Get(val, KMaxTUint5);
	if (res==KErrNone)
		{
		aLabel.Set(*this, val);
		}
	return res;
	}

inline TInt TTCIDManager::GetTCID(RTCID& aTCID)
	{
	TInt val, res;
	res = Get(val, KMaxTUint5-1, KMinimumRealTCID); 	// cannot use top bit as RFD in spec
	if (res==KErrNone)
		{
		aTCID.Set(*this, val);
		}
	return res;
	}

inline TInt TTCIDManager::GrabTCID(RTCID& aTCID, TTCID aAssignedTCID)
	{
	// during muxing the remote can "assign" a TCID from our pool
	// if things are going ok with remote it should be a free TCID
	// but the return allows for an error just in case
	// constrain the range to reuse case-class code :o)
	TInt val, res;
	res = Get(val, aAssignedTCID, aAssignedTCID);
	if (res==KErrNone)
		{
		aTCID.Set(*this, val);
		}
	return res;
	}
	
inline TInt TTSIDManager::GetTSID(RTSID& aTSID)
	{
	TInt val, res;
	res = Get(val, KMaxTUint5-1, KMinimumRealTSID); 	// cannot use top bit as RFD in spec
	if (res==KErrNone)
		{
		aTSID.Set(*this, val);
		}
	return res;
	}

inline TInt TSEIDManager::GetSEID(RSEID& aSEID)
	{
	TInt val, res;
	// to make agnostic to implementation of local/remote seids
	// remembering this only allocates semantically local seids...
	TSEID min(KMinimumRealSEID, ETrue);
	TSEID max(KMaxTUint6-1, ETrue); // 0x3f is RFD, see 8.18
	__ASSERT_COMPILE(KMinimumRealSEID<KMaxTUint6);
	res = Get(val, max.Value(), min.Value()); 
	if (res==KErrNone)
		{
		aSEID.Set(*this, val);
		}
	return res;
	}

#endif //_AVDTPALLOCATORS_H
