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
// A unit test wrapper over the CLbsSuplPushRecChannel class
// 
//

#if (!defined __TE_LBSSUPLPUSHRECCHANNEL__)
#define __TE_LBSSUPLPUSHRECCHANNEL__

#include "lbssuplpushreceiverchannel.h"


/**
A unit test wrapper over the CLbsSuplPushRecChannel class.

@see CLbsSuplPushRecChannel
*/
class CTe_LbsSuplPushRecChannel : public CLbsSuplPushRecChannel
	{
private:
	/** The flag for enabling/disabling asserts for the CLbsSuplPushRecChannel class*/
	static TBool iEnableAsserts;
	
public:
	static void RunL(CLbsSuplPushRecChannel& aChannel, TInt aStatus);
	
	inline static TBool AssertsEnabled();
	inline static void EnableAsserts();
	inline static void DisableAsserts();
	
private:
	//This constructor is only to avoid gcce error
	CTe_LbsSuplPushRecChannel(TLbsSuplPushChannel aChannel, MLbsSuplPushRecObserver& aObserver, 
				TUid aPropOwnerSecureId) : CLbsSuplPushRecChannel(aChannel, aObserver, aPropOwnerSecureId){};
	
	};
	
inline TBool CTe_LbsSuplPushRecChannel::AssertsEnabled()	
	{
	return iEnableAsserts;
	}

inline void CTe_LbsSuplPushRecChannel::EnableAsserts()
	{
	iEnableAsserts = ETrue;
	}

inline void CTe_LbsSuplPushRecChannel::DisableAsserts()
	{
	iEnableAsserts = EFalse;
	}

#endif //__TE_LBSSUPLPUSHRECCHANNEL__
