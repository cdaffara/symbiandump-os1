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
// TRbuf.h
// 
//

/**
 @file
 @internalTechnology
*/

#if (!defined H_STRUCTURES_H)
#define H_STRUCTURES_H

#include <e32base.h>

namespace Elements
{

class TStateChange
/**
@internalTechnology
*/
	{
public:
	inline TStateChange()
	:	iStage(0), iError(0)
		{
		}
	inline TStateChange(TInt aStage, TInt aError)
	:	iStage(aStage), iError(aError)
		{
		}

	inline TBool operator==(const TStateChange& aRHS) const
		{
		return (iStage == aRHS.iStage && iError == aRHS.iError);
		}

	TInt iStage;
	TInt iError;
	};

} //namespace Elements


#endif //H_STRUCTURES_H


