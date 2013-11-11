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
//

/**
 @file
 @internalComponent
*/

#define SYMBIAN_NETWORKING_UPS

#include <comms-infras/ss_platsec_apiext.h>
#include <ss_glob.h>
#include <comms-infras/ss_roles.h>
#include "SS_conn.H"

using namespace ESock;


// ======== MPlatsecExtensionApi Implementation ========

EXPORT_C ASubSessionPlatsecApiExt::ASubSessionPlatsecApiExt(const TSubSessionUniqueId aSubSessionUniqueId)
    : iSubSessionUniqueId(aSubSessionUniqueId)
    {
    }

EXPORT_C TInt ASubSessionPlatsecApiExt::SecureId(TSecureId& aResult) const
    {
	return AIPCClientPlatsecInfo::SecureId(iSubSessionUniqueId, aResult);
    }


EXPORT_C TInt ASubSessionPlatsecApiExt::VendorId(TVendorId& aResult) const
    {
	return AIPCClientPlatsecInfo::VendorId(iSubSessionUniqueId, aResult);
    }

EXPORT_C TBool ASubSessionPlatsecApiExt::HasCapability(const TCapability aCapability) const
   {
   return AIPCClientPlatsecInfo::HasCapability(iSubSessionUniqueId, aCapability);
   }


EXPORT_C TInt ASubSessionPlatsecApiExt::CheckPolicy(const TSecurityPolicy& aPolicy) const
    {
    return AIPCClientPlatsecInfo::CheckPolicy(iSubSessionUniqueId, aPolicy);
    }

#ifdef SYMBIAN_NETWORKING_UPS

//
// UPS support
//

EXPORT_C TInt ASubSessionPlatsecApiExt::GetProcessAndThreadId(TProcessId& aProcessId, TThreadId& aThreadId) const
    {
    return AIPCClientPlatsecInfo::GetProcessAndThreadId(iSubSessionUniqueId, aProcessId, aThreadId);
    }
    
ASockSubSessionPlatsecApiExt::ASockSubSessionPlatsecApiExt()
  	{
  	}

ASockSubSessionPlatsecApiExt::~ASockSubSessionPlatsecApiExt()
  	{
  	}

TInt ASockSubSessionPlatsecApiExt::SecureId(TSecureId& /*aResult*/) const
	{
	Fault(ENotImplementedYet);
	return KErrNotSupported;
	}

TInt ASockSubSessionPlatsecApiExt::VendorId(TVendorId& /*aResult*/) const
	{
	Fault(ENotImplementedYet);
	return KErrNotSupported;
	}

TBool ASockSubSessionPlatsecApiExt::HasCapability(const TCapability /*aCapability*/) const
	{
	Fault(ENotImplementedYet);
	return EFalse;
	}

TInt ASockSubSessionPlatsecApiExt::CheckPolicy(const TSecurityPolicy& /*aPolicy*/) const
	{
	Fault(ENotImplementedYet);
	return KErrNotSupported;
	}

TInt ASockSubSessionPlatsecApiExt::GetProcessAndThreadIdFromRMessage(const RMessage2& aMessage, TProcessId& aProcessId, TThreadId& aThreadId)
/**
Retrieve the process and thread id associated with the current IPC.

@param aMessage RMessage2 associated with current IPC
@return KErrNone or a system wide error code .
*/
	{
	// Retrieve thread and process id, if message is not null.
	TInt err = KErrNone;
	if (!aMessage.IsNull())
		{
		RThread thread;
		err = aMessage.Client(thread);
		if (err == KErrNone)
			{
			RProcess process;
			err = thread.Process(process);
			if (err == KErrNone)
				{
				aProcessId = process.Id();
				aThreadId = thread.Id();
				process.Close();
				}
			thread.Close();
			}
		}
	else
		{
		err = KErrArgument;
		}
	return err;
	}

#endif //SYMBIAN_NETWORKING_UPS

EXPORT_C TInt TCFSigRMessage2PlatsecExt::SecureId(TSecureId& aResult) const
    {
    aResult = iMessage.SecureId();

    return KErrNone;
    }


EXPORT_C TInt TCFSigRMessage2PlatsecExt::VendorId(TVendorId& aResult) const
    {
    aResult = iMessage.VendorId();

    return KErrNone;
    }

EXPORT_C TBool TCFSigRMessage2PlatsecExt::HasCapability(const TCapability aCapability) const
    {
    return iMessage.HasCapability(aCapability);
    }


EXPORT_C TInt TCFSigRMessage2PlatsecExt::CheckPolicy(const TSecurityPolicy& aPolicy) const
    {
    TBool platsecResult;
    platsecResult = aPolicy.CheckPolicy(iMessage);
    if (!platsecResult)
        {
        return KErrPermissionDenied;
        }
    return KErrNone;
    }

