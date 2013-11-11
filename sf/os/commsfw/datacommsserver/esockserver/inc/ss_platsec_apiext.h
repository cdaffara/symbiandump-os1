// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Platsec extension API used for checking client side capabilities
// 
//

/**
 @file
 @internalComponent
*/


#ifndef SS_PLATSECEXTAPI_H
#define SS_PLATSECEXTAPI_H

#define SYMBIAN_NETWORKING_UPS

#include <e32cmn.h>

#include <comms-infras/api_ext_list.h>
#include <comms-infras/ss_api_ext.h>
#include <comms-infras/ss_nodeinterfaces.h>
#include <comms-infras/ss_dispatchers.h>


namespace ESock
{

/**
API Extension for performing platsec checks
@internalComponent
*/
class MPlatsecApiExt
    {
public:
	static const TInt KInterfaceId = 0x102864DA;


    /**
    Obtains the TSecureId of the client process
    @param aResult If the call is successful, the TSecureId.
    @return KErrNone if the call succeeds.
    Otherwise one of the other system-wide error codes.
    */
    virtual TInt SecureId(TSecureId& aResult) const = 0;

    /**
    Obtains the TVendorId of the client process
    @param aResult If the call is successful, the TVendorId.
    @return KErrNone if the call succeeds.
    Otherwise one of the other system-wide error codes.
    */
    virtual TInt VendorId(TVendorId& aResult) const = 0;

    /**
    Checks the client process for the capability aCapability
    @param aCapability The capability to be checked for
    @return KErrNone if the call succeeds.
    KErrPermissionDenied if the client process does not have the capability.
    Otherwise one of the other system-wide error codes.
    */
    virtual TBool HasCapability(const TCapability aCapability) const = 0;

    /**
    Checks the client process for a match against the policy aPolicy
    @param aPolicy A security policy that must be matched
    @return KErrNone if the call succeeds.
    KErrPermissionDenied if the client process fails the policy check.
    Otherwise one of the other system-wide error codes.
    */
    virtual TInt CheckPolicy(const TSecurityPolicy& aPolicy) const = 0;
    
#ifdef SYMBIAN_NETWORKING_UPS
    virtual TInt GetProcessAndThreadId(TProcessId& /*aProcessId*/, TThreadId& /*aThreadId*/) const
        {
        return KErrNotSupported;
        }
#endif        
    };

/**
	Implementation for the MPlatsecExtensionApi interface

	@internalComponent
*/
class ASubSessionPlatsecApiExt : private AIPCClientPlatsecInfo, public MPlatsecApiExt
    {
public:
    IMPORT_C ASubSessionPlatsecApiExt(const TSubSessionUniqueId aSubSessionUniqueId);

    // From MPlatsecApiExt
    IMPORT_C virtual TInt SecureId(TSecureId& aResult) const;
    IMPORT_C virtual TInt VendorId(TVendorId& aResult) const;
    IMPORT_C virtual TBool HasCapability(const TCapability aCapability) const;
    IMPORT_C virtual TInt CheckPolicy(const TSecurityPolicy& aPolicy) const;
#ifdef SYMBIAN_NETWORKING_UPS
    IMPORT_C virtual TInt GetProcessAndThreadId(TProcessId& aProcessId, TThreadId& aThreadId) const; 
#endif //SYMBIAN_NETWORKING_UPS
private:
    const TSubSessionUniqueId iSubSessionUniqueId;
    };

#ifdef SYMBIAN_NETWORKING_UPS

NONSHARABLE_CLASS(ASockSubSessionPlatsecApiExt) : public MPlatsecApiExt
/**
Support for UPS.

Aggregate class used with CSockSubSession derived classes to allow them
to provide an interface for retrieving thread/process id dynamically
according to the last message.

@internalTechnology
*/
    {
public:
    static TInt GetProcessAndThreadIdFromRMessage(const RMessage2& aMessage, TProcessId& aProcessId, TThreadId& aThreadId);
	virtual ~ASockSubSessionPlatsecApiExt();

protected:
	ASockSubSessionPlatsecApiExt();

private:
	// from MPlatsecApiExt
    TInt SecureId(TSecureId& aResult) const;
    TInt VendorId(TVendorId& aResult) const;
    TBool HasCapability(const TCapability aCapability) const;
    TInt CheckPolicy(const TSecurityPolicy& aPolicy) const;
    };

#endif //SYMBIAN_NETWORKING_UPS

class TCFSigRMessage2PlatsecExt : public TCFSigRMessage2Ext, protected MPlatsecApiExt
	{
public:
	inline TCFSigRMessage2PlatsecExt() {};
	TCFSigRMessage2PlatsecExt(const RMessage2& aMessage)
	:	TCFSigRMessage2Ext(aMessage)
        {
        }

protected:
    // From MPlatsecExtensionApi
    IMPORT_C virtual TInt SecureId(TSecureId& aResult) const;
    IMPORT_C virtual TInt VendorId(TVendorId& aResult) const;
    IMPORT_C virtual TBool HasCapability(const TCapability aCapability) const;
    IMPORT_C virtual TInt CheckPolicy(const TSecurityPolicy& aPolicy) const;
    };



} // namespace ESock


#endif
// SS_PLATSECEXTAPI_H

