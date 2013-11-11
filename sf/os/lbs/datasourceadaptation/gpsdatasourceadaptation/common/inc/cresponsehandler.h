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
// responsehandler.h
// 
//

/**
 @file
 @InternalComponent
*/

#include <e32base.h>
#include "mdatabusobserver.h"

class CResponseHandler : public CBase, 
						 public MDataBusObserver
	{
    public:
        virtual ~CResponseHandler();

	public:
		virtual TBool GetLastPosition(TPositionInfoBase& aPos, TTime aMaxAge, TBool aAllowPartial) = 0;

		// from MDataBusObserver
		virtual TInt DataError(TInt aError, TInt aStatus);	
	
	protected:
        CResponseHandler();
        
	protected:
		TPositionInfoBase* iPos;
	};
