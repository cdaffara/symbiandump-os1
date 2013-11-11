// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __COMMSBUFPOND_INL__
#define __COMMSBUFPOND_INL__

#ifndef __KERNEL_MODE__

inline	RCommsBufPond::RCommsBufPond(MCommsBufPondIntf* aPondImpl)
: iPondImpl(aPondImpl)
{
}

inline TBool RCommsBufPond::IsNull() const
/**
Check whether the pond is opened and is valid
*/
	{
	return (iPondImpl == NULL);
	}

inline TCommsBufAllocator::TCommsBufAllocator(RCommsBufPond aPond)
:iPond(*aPond.iPondImpl)
/**
The constructor. 
*/
	{
		
	}
#endif // __KERNEL_MODE__
#endif
