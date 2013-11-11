// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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


inline TBool DChannelComm::IsReading() const
// Return TRUE if a read request is pending.
	{ return(iIsReading); }

inline TBool DChannelComm::IsWriting() const
// Return TRUE if a write request is pending.
	{ return(iIsWriting); }

inline TBool DChannelComm::AreAnyPending() const
// Return TRUE if any requests are pending.
#ifdef _EARLY_WRITE_COMPLETE
	{ return(IsReading() || (!(iConfig.iHandshake&KConfigWriteBufferedComplete)?IsWriting():iIsClientWriting)); }
#else
	{ return(IsReading() || IsWriting()); }
#endif

inline TBool DChannelComm::IsPowerGood()
	{ return(iPowerHandler->PowerGood()); }

inline void DChannelComm::SetStatus(TState aStatus)
	{iStatus=aStatus;}

inline void DChannelComm::CompleteAllReqs(TInt aReason)
	{DLogicalChannel::CompleteAll(aReason);}

inline TCommSeqNum& TCommSeqNum::operator++(TInt)
	{ TCommSeqNum seq = *this; iNum += 1;  return *this; }
inline TCommSeqNum& TCommSeqNum::operator++()
	{ iNum += 1;  return *this; }
inline TCommSeqNum& TCommSeqNum::operator--(TInt)
	{ TCommSeqNum seq = *this; iNum -= 1;  return *this; }
inline TCommSeqNum& TCommSeqNum::operator--()
	{ iNum -= 1;  return *this; }
inline TCommSeqNum& TCommSeqNum::operator=(TUint aVal)
	{ iNum = aVal;  return *this; }
inline TCommSeqNum& TCommSeqNum::operator=(const TCommSeqNum& aVal)
	{ iNum = aVal.iNum;  return *this; }
inline TCommSeqNum& TCommSeqNum::operator+=(TInt aVal)
	{ iNum += aVal;  return *this; }
inline TCommSeqNum& TCommSeqNum::operator-=(TInt aVal)
	{ iNum += aVal;  return *this; }
inline TBool TCommSeqNum::operator==(const TCommSeqNum& aVal) const
	{ return iNum==aVal.iNum; }
inline TBool TCommSeqNum::operator!=(const TCommSeqNum& aVal) const
	{ return iNum==aVal.iNum; }
inline TBool TCommSeqNum::operator<(const TCommSeqNum& aVal) const
	{ return ((TInt)(iNum-aVal.iNum))<0; }
inline TBool TCommSeqNum::operator<=(const TCommSeqNum& aVal) const
	{ return ((TInt)(iNum-aVal.iNum))<=0; }
inline TBool TCommSeqNum::operator>(const TCommSeqNum& aVal) const
	{ return ((TInt)(iNum-aVal.iNum))>0; }
inline TBool TCommSeqNum::operator>=(const TCommSeqNum& aVal) const
	{ return ((TInt)(iNum-aVal.iNum))>=0; }
inline TCommSeqNum TCommSeqNum::operator+(TInt aVal)
	{ TCommSeqNum tmp; tmp.iNum = iNum+aVal; return *this; }
inline TCommSeqNum TCommSeqNum::operator-(TInt aVal)
	{ TCommSeqNum tmp; tmp.iNum = iNum-aVal; return *this; }
inline TInt TCommSeqNum::operator-(const TCommSeqNum& aVal)
	{ return iNum-aVal.iNum; }
inline TUint TCommSeqNum::Int() const
	{ return iNum; }
