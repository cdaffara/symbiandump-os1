// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The LL-level frames that are suitable for Q-ing
// 
//

#ifndef LINKQITEM_H__
#define LINKQITEM_H__

#include <e32std.h>
#include <e32base.h>
#include <bttypes.h>

#include <bluetooth/hci/hciframe.h>

class TDataPlaneElementHandle;
class TSockAddr;

NONSHARABLE_CLASS(CACLDataItem) : public CBase
	{
friend class CAclDataQ;
public:
	CACLDataItem();
	CHctlAclDataFrame*& Frame();
	static CACLDataItem* NewL();
	~CACLDataItem();

	inline void SetElementHandle(TSockAddr* aAddr);
	inline TDataPlaneElementHandle* ElementHandle() const;
private:
	TSglQueLink iLink;
	CHctlAclDataFrame* iFrame;

	TDataPlaneElementHandle* iElementHandle;  // Non-owned pointer.
	};

inline void CACLDataItem::SetElementHandle(TSockAddr* aAddr)
	{
	iElementHandle = reinterpret_cast<TDataPlaneElementHandle*>(aAddr);
	}

inline TDataPlaneElementHandle* CACLDataItem::ElementHandle() const
	{
	return iElementHandle;
	}

#endif
