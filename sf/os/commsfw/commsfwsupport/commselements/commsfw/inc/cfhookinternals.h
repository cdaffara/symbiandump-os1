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
 @internalComponent
*/

#ifndef __CFHOOKINTERNALS_H__
#define __CFHOOKINTERNALS_H__


#ifdef HOOKS_IMPLEMENTED

#include <cftransporthook.h>

namespace CommsFW 
{

class TCookieReg
	{
public:
	TCookieReg():
    	iObjPtr(NULL),
    	iSalt(0),
    	iCount(1)
	    {
	    }
	TCookieReg(const TCookie& aRHS):
    	iObjPtr(TCookieAccessor(aRHS).Ptr()),
    	iSalt(TCookieAccessor(aRHS).PtrSalt()),
    	iCount(1)
	    {
	    }
	    
	TBool operator==(const TCookie& aRHS) const
	    {
	    TCookieAccessor acc(aRHS);
	    return acc.Variety() == TCookie::EDispatchItfPtrListener && acc.Ptr() == iObjPtr &&
	        acc.PtrSalt() == iSalt;
	    }

    void AddRef(){++iCount;};
    TInt ReleaseRef(){return --iCount;}

private:
	TAny* iObjPtr;
	TUint8 iSalt;			// Reuse protection for pointers
	TUint8 iCount; //number of registered hooks
	};

class TCFTransportHooks
    {
    friend class CCommsTransportImpl;
public:

    //"remote" registration (e.g. in the thread where the monitored node resides)
    void RegisterHookL( const TCFRegisterHookSignal& aReq );
    void WalkHooks( const TCFSignalBase& aMessage, const TCookie& aRecipient );
    void DeregisterHook( const TCFDeregisterHookSignal& aReq );

    //"local" registration (e.g. in the thread where the monitor node resides)
    TInt RegisterSpy( const TCookie& aSpy );
    void DeregisterSpy( const TCookie& aSpy );
    TInt FindSpy( const TCookie& aSpy )
        {
        return iSpies.Find(TCookieReg(aSpy));
        }

protected:
    explicit TCFTransportHooks() :
    iHookEntries(NULL)
        {
        }
    ~TCFTransportHooks();

private:
	RPointerArray<CCFHookEntry>* iHookEntries;
	RArray<TCookieReg> iSpies;
    };
    
} // namespace CommsFW

#endif //HOOKS_IMPLEMENTED

#endif // __CFTRANSPORTHOOK_H__

