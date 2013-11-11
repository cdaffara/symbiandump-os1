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
// PDP config change notifier
// 
//

/**
 @file 
 @internalComponent
*/

#ifndef CPDPCONFIGCHAGENOTIFIER_H
#define CPDPCONFIGCHAGENOTIFIER_H

#include <e32base.h>
#include <etelpckt.h>
#include <networking/umtsnifcontrolif.h>

#include "eteldrivernmspace.h"
#include "ceteldrivernotifier.h"

const TInt KScratchContextConfigSize = 1024;

/** notifier for packet context config */
class CPdpConfigChangeNotifier : public CEtelDriverNotifier
	{
public:
	static CPdpConfigChangeNotifier* NewL(TContextId aId, RPacketContext& aPacketContext, CPdpFsmInterface& aPdpFsmInterface);

	virtual ~CPdpConfigChangeNotifier();
	
	virtual void Start();

protected:
	virtual void Notify(const TRequestStatus& aStatus);
	virtual void DoCancel();
	
private:
	CPdpConfigChangeNotifier(TContextId aId, RPacketContext& aPacketContext, 
							CPdpFsmInterface& aPdpFsmInterface);
	void ConstructL();
	
	    
    template<class CONTEXTTYPE>
    CONTEXTTYPE& GetScratchContextAs() const
    	{
        __ASSERT_COMPILE(KScratchContextConfigSize >= sizeof(CONTEXTTYPE));
        return *const_cast<CONTEXTTYPE*>(reinterpret_cast<const CONTEXTTYPE*>(iScratchContext.Ptr()));
    	}

private:
	/** context id */
	TContextId						iId;
	/** underlying packet context */
	RPacketContext&					iPacketContext;

	/** packet context config */
	mutable RBuf8 iScratchContext;
	};
	
	
class CMbmsPdpConfigChangeNotifier : public CEtelDriverNotifier
	{
public:
	CMbmsPdpConfigChangeNotifier(TContextId aId, RPacketMbmsContext& aPacketContext, 
								CPdpFsmInterface& aPdpFsmInterface);
	virtual ~CMbmsPdpConfigChangeNotifier();
	
	virtual void Start();

protected:
	virtual void Notify(const TRequestStatus& aStatus);
	virtual void DoCancel();

private:
	/** context id */
	TContextId						iId;
	/** underlying packet context */
	RPacketMbmsContext&					iMbmsPacketContext;

	/** packet context config */
	RPacketMbmsContext::TContextConfigMbmsV1	iConfigMbms;
	/** packaged packet context config */
	EtelDriver::TContextConfigMbmsV1Pckg				iConfigMbmsPckg;
	};	
	

#endif // CPDPCONFIGCHAGENOTIFIER_H
