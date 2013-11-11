// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TBNEPBTDEVADDR_H
#define __TBNEPBTDEVADDR_H 

#include <e32def.h>

class RMBufChain;
class TBTDevAddr;

NONSHARABLE_CLASS(TBnepBTDevAddr) 
    {

public:
    inline TBnepBTDevAddr();
    inline TBnepBTDevAddr(const TBnepBTDevAddr& aRight);
	TBnepBTDevAddr(const TBTDevAddr& aAddr);
	
	TInt SetAddress(RMBufChain& aChain, TInt aOffset);
	inline TInt64 Address() const;
		 
    inline TBnepBTDevAddr& operator=(const TBnepBTDevAddr &right);

    inline TBool operator==(const TBnepBTDevAddr &right) const;

    inline TBool operator!=(const TBnepBTDevAddr &right) const;

    inline TBool operator<(const TBnepBTDevAddr &right) const;

    inline TBool operator>(const TBnepBTDevAddr &right) const;

    inline TBool operator<=(const TBnepBTDevAddr &right) const;

    inline TBool operator>=(const TBnepBTDevAddr &right) const;

private: 
    TInt64 iAddress;
    };

inline TBnepBTDevAddr::TBnepBTDevAddr()
	{
	iAddress = 0;
	}

inline TBnepBTDevAddr::TBnepBTDevAddr(const TBnepBTDevAddr &right)
    {
    iAddress = right.iAddress;
    }

inline TInt64 TBnepBTDevAddr::Address() const
	{
	return iAddress;
	}

inline TBnepBTDevAddr& TBnepBTDevAddr::operator=(const TBnepBTDevAddr &right)
    {
    iAddress = right.iAddress;
    return (*this);
    }

inline TBool TBnepBTDevAddr::operator==(const TBnepBTDevAddr &right) const
    {
    return (iAddress == right.iAddress);
    }

inline TBool TBnepBTDevAddr::operator!=(const TBnepBTDevAddr &right) const
    {
    return (iAddress != right.iAddress);
    }

inline TBool TBnepBTDevAddr::operator<(const TBnepBTDevAddr &right) const
    {
    return (iAddress < right.iAddress);
    }

inline TBool TBnepBTDevAddr::operator>(const TBnepBTDevAddr &right) const
    {
    return (iAddress > right.iAddress);
    }

inline TBool TBnepBTDevAddr::operator<=(const TBnepBTDevAddr &right) const
    {
    return (iAddress <= right.iAddress);
    }

inline TBool TBnepBTDevAddr::operator>=(const TBnepBTDevAddr &right) const
    {
    return (iAddress >= right.iAddress);
    }


#endif
