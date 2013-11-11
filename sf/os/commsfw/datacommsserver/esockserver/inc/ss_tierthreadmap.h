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
 @internalTechnology
*/

#if !defined(__SS_TIERTHREADMAP_H__)
#define __SS_TIERTHREADMAP_H__

#include <elements/cftransport.h>

namespace  ESock
	{
	struct TTierMapEntry
		{
		TTierMapEntry(TInt aUid, CommsFW::TWorkerId aWorker)
			: iUid(aUid),
			  iWorker(aWorker)
			{ }
		TInt iUid;
		CommsFW::TWorkerId iWorker;
		};

	class RTierThreadMap : public RArray<TTierMapEntry>
		{
		};
	}

#endif

