// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "avctpconstants.h"
#include "avctppacket.h"
#include "avctputils.h"

inline TAvctpOutboundQ::TAvctpOutboundQ()
: iCount(0)
	{
	iQue.SetOffset(_FOFF(HAvctpOutgoingSdu, iQueLink));
	}
	
inline HAvctpOutgoingSdu* TAvctpOutboundQ::First()
	{
	return iQue.First();
	}

inline TDblQueIter<HAvctpOutgoingSdu> TAvctpOutboundQ::Iter()
	{
	TDblQueIter<HAvctpOutgoingSdu> iter(iQue);
	return iter;
	}

inline TUint TAvctpOutboundQ::Count() const
	{
#ifdef _DEBUG
	typedef TDblQue<HAvctpOutgoingSdu>* sduList; // to avoid nested <> nastiness
	TDblQueIter<HAvctpOutgoingSdu> iter(*const_cast<sduList>(&iQue));
	TUint checkCount = 0;
	while(iter++)
		{
		checkCount++;
		}
	if (checkCount != iCount)
		{
		Panic(SymbianAvctp::EOutboundQCountIncorrect);
		}
#endif		

	return iCount;
	}

// Needs to be explicitly called rather than 
inline void TAvctpOutboundQ::Deque(HAvctpOutgoingSdu& aSdu)
	{
	aSdu.iQueLink.Deque();
	iCount--;
	}



