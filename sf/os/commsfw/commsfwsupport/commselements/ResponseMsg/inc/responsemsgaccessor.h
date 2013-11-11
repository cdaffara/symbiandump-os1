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
 @internalTechnology
*/


#if !defined(RESPONSEMSGACCESSOR_H)
#define RESPONSEMSGACCESSOR_H

#include <elements/responsemsg.h>

namespace Elements
{

/**
	The RResponseMsg class provides a controlled wrapper around an IPC
	client-provided RMessage2.

	@internalTechnology
	@released
*/
class TResponseMsgAccessor
	{
public:
	explicit TResponseMsgAccessor(RResponseMsg& aResponseMsg)
	:	iResponseMsg(aResponseMsg)
		{
		}

	RMessage2& Message()
		{
		return static_cast<RMessage2&>(iResponseMsg);
		}

private:
	RResponseMsg& iResponseMsg;
	};

} // namespace Elements

#endif	// RESPONSEMSGACCESSOR_H


