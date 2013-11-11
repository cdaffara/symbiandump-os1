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

#ifndef AVCTPOUTBOUNDQ_H
#define AVCTPOUTBOUNDQ_H

#include <e32std.h>

// Forward Declaration
class HAvctpOutgoingSdu;

NONSHARABLE_CLASS(TAvctpOutboundQ)
	{
public:
	inline TAvctpOutboundQ();
	inline HAvctpOutgoingSdu* First();
	inline TDblQueIter<HAvctpOutgoingSdu> Iter();
	inline TUint Count() const;
	inline void Deque(HAvctpOutgoingSdu& aSdu);
	void Insert(HAvctpOutgoingSdu& aSdu);
	
private:
	TDblQue<HAvctpOutgoingSdu> iQue;
	TUint iCount;	
	
	};

#include "avctpOutboundQ.inl"

#endif // AVCTPOUTBOUNDQ_H 

