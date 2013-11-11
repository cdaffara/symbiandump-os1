// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Mixin observer interface for an data bus
// 
//

/**
 @file
 @InternalComponent
*/

#ifndef __MDATABUSOBSERVER_H__
#define __MDATABUSOBSERVER_H__

#include <e32base.h>
#include <lbspositioninfo.h>

class MDataBusObserver
	{
	public:
		virtual void DataUpdate(TPositionInfo& aPosition, TInt aStatus, TInt aError, TTime aActualTime, TUint aAttributes = 0, TUint aMode = 0) = 0;
		virtual TInt DataError(TInt aError, TInt aStatus) = 0;
	};

#endif // __MDATABUSOBSERVER_H__
