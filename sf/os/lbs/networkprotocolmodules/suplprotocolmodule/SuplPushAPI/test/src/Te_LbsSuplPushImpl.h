// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// A unit test wrapper over the CLbsSuplPushImpl class
// 
//

#if (!defined __TE_LBSSUPLPUSHIMPL__)
#define __TE_LBSSUPLPUSHIMPL__

#include "lbssuplpushimpl.h"

/**
A unit test wrapper over the CLbsSuplPushImpl class.

@see CLbsSuplPushImpl
*/
class CTe_LbsSuplPushImpl : public CLbsSuplPushImpl
	{
private:
	/** The flag for enabling/disabling asserts for the CLbsSuplPushImpl class */
	static TBool iEnableAsserts;
	
public:
	static void SetActive(CLbsSuplPushImpl& aPushImpl);

	inline static TBool AssertsEnabled();
	inline static void EnableAsserts();
	inline static void DisableAsserts();
	
private:
	CTe_LbsSuplPushImpl(TLbsSuplPushChannel aChannel, MLbsSuplPushObserver& aObserver) : 
		CLbsSuplPushImpl(aChannel, aObserver){};
	};
	
	
inline TBool CTe_LbsSuplPushImpl::AssertsEnabled()	
	{
	return iEnableAsserts;
	}

inline void CTe_LbsSuplPushImpl::EnableAsserts()
	{
	iEnableAsserts = ETrue;
	}

inline void CTe_LbsSuplPushImpl::DisableAsserts()
	{
	iEnableAsserts = EFalse;
	}

#endif //__TE_LBSSUPLPUSHIMPL__
