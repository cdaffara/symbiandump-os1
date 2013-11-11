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
// Implementation for CCFParameterFamilyBundle and RCFParameterFamilyBundle
// 
//

/**
 @file
 @internalTechnology
*/

#include "ss_parameterfamilybundle.h"
#include <comms-infras/es_parameterfamily.h> 


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSocksprmtr, "ESockSSocksprmtr");
#endif


#ifdef _DEBUG
_LIT (KBundlePanic,"BundlePanic");
#endif

namespace BundlePanics
{
enum
	{
	KPanicNotReallyANodeYet = 1
	};
}

using namespace ESock;

namespace ESock
	{
	
	
class CCFParameterFamilyBundle :	public CBase,
									public RParameterFamilyBundle,
									protected Messages::ASimpleNodeIdBase
	{
	friend class RCFParameterFamilyBundle;
	friend class RCFParameterFamilyBundleC;
public:
	static CCFParameterFamilyBundle* NewL()
		{
		CCFParameterFamilyBundle* newObj = new(ELeave)CCFParameterFamilyBundle;
		CleanupStack::PushL(newObj);
		static_cast<RParameterFamilyBundle&>(*newObj).CreateL();
		CleanupStack::Pop();
		return newObj;
		}

	static CCFParameterFamilyBundle* LoadL(TDesC8& aDes)
		{
		CCFParameterFamilyBundle* newObj = new(ELeave)CCFParameterFamilyBundle;
		CleanupStack::PushL(newObj);
		static_cast<RParameterFamilyBundle&>(*newObj).LoadL(aDes);
		CleanupStack::Pop();
		return newObj;		
		}

	void Open() // increase refcount.. returns new refcount
		{
		User::LockedInc(iRefCount);
		}

	void Close() // decrease refcount or destroy
		{
		 __ASSERT_DEBUG(iRefCount >= 0, User::Panic(KSpecAssert_ESockSSocksprmtr, 1));
		if(User::LockedDec(iRefCount) <= 1)
			{
			static_cast<RParameterFamilyBundle&>(*this).Destroy();
			delete this;
			}
		}
	
protected:
	CCFParameterFamilyBundle():
		iRefCount(0) 
		{
		}

    virtual ~CCFParameterFamilyBundle()
    	{
    	}

   	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage)
   		{
		__ASSERT_DEBUG(0,User::Panic(KBundlePanic,BundlePanics::KPanicNotReallyANodeYet));
		(void)aSender;
		(void)aRecipient;
		(void)aMessage;
   		}


	
private:
	TInt iRefCount;
	};

} // namespace ESock
	
EXPORT_C void RCFParameterFamilyBundleC::Open()
/** Increases the refcount of the target family bundle. Ensure it
 *   stays in existence til at least after we call Close() on this instance
 *   of the handle.

@param none
@return none
*/
	{
	CCFParameterFamilyBundle* tBundle=TargetBundle();
	if(tBundle)
		{
		tBundle->Open();
		}
	}


EXPORT_C void RCFParameterFamilyBundleC::Open(const RCFParameterFamilyBundleC& aExistingHandle)
/** Initialises this handle from an existing handle, whilst increasing the
 *   refcount. This ensures that the target
 *   stays in existence til at least after we call Close() on this instance
 *   of the handle.

@param none
@return none
*/
	{
	CCFParameterFamilyBundle* tBundle=TargetBundle();
	if (tBundle)
		{
		tBundle->Close();
		}
	iBundleId = aExistingHandle.iBundleId;
	tBundle=TargetBundle();
	if (tBundle)
		{
		tBundle->Open();
		}
	}

EXPORT_C void RCFParameterFamilyBundleC::Close()
/** Opens a new RConnection instance.

@param aSocketServer Socket Server session.
@param aConnectionType Reserved.
@return KErrNone if successful, otherwise another of the system wide error
codes. */
	{
	CCFParameterFamilyBundle* tBundle=TargetBundle();
	if(tBundle)
		{
		tBundle->Close();
		}
	iBundleId = Messages::TNodeId::NullId();
	}

EXPORT_C void RCFParameterFamilyBundleC::CreateL()
	{
	CCFParameterFamilyBundle* tBundle = CCFParameterFamilyBundle::NewL();
	iBundleId = tBundle->NodeId();
	}

EXPORT_C void RCFParameterFamilyBundleC::LoadL(TDesC8& aDes)
	{
	CCFParameterFamilyBundle* tBundle = CCFParameterFamilyBundle::LoadL(aDes);
	iBundleId = tBundle->NodeId();
	}

EXPORT_C TUint RCFParameterFamilyBundleC::Length() const
	{
	CCFParameterFamilyBundle* tBundle=const_cast<RCFParameterFamilyBundleC&>(*this).TargetBundle();
	return tBundle?tBundle->Length():0;
	}

EXPORT_C TUint RCFParameterFamilyBundleC::CountParameterSets() const
	{
	CCFParameterFamilyBundle* tBundle=const_cast<RCFParameterFamilyBundleC&>(*this).TargetBundle();
	return tBundle?tBundle->CountParameterFamilies():0;
	}
EXPORT_C TInt RCFParameterFamilyBundleC::Store(TDes8& aDes) const
	{
	CCFParameterFamilyBundle* tBundle=const_cast<RCFParameterFamilyBundleC&>(*this).TargetBundle();
	return tBundle?tBundle->Store(aDes):KErrNotReady;
	}

EXPORT_C const RParameterFamily RCFParameterFamilyBundleC::GetFamilyAtIndex(TUint aIndex) const
	{
	CCFParameterFamilyBundle* tBundle=const_cast<RCFParameterFamilyBundleC&>(*this).TargetBundle();
	return tBundle?tBundle->GetFamilyAtIndex(aIndex):RParameterFamily();
	}

EXPORT_C const RParameterFamily RCFParameterFamilyBundleC::FindFamily(TUint32 aFamilyId) const
	{
	CCFParameterFamilyBundle* tBundle=const_cast<RCFParameterFamilyBundleC&>(*this).TargetBundle();
	return tBundle?tBundle->FindFamily(aFamilyId):RParameterFamily();
	}


EXPORT_C CCFParameterFamilyBundle* RCFParameterFamilyBundleC::TargetBundle()
	{
	return (iBundleId != Messages::TNodeId::NullId() ?
		static_cast<CCFParameterFamilyBundle*>(static_cast<Messages::ASimpleNodeIdBase*>(iBundleId.Ptr())) :
		NULL);
	}

EXPORT_C RParameterFamily RCFParameterFamilyBundle::CreateFamilyL(TUint32 aFamilyId)
	{
	return TargetBundleL().CreateFamilyL(aFamilyId);
	}

EXPORT_C RParameterFamily RCFParameterFamilyBundle::GetFamilyAtIndex(TUint aIndex)
	{
	CCFParameterFamilyBundle* tBundle=TargetBundle();
	return tBundle?tBundle->GetFamilyAtIndex(aIndex):RParameterFamily();
	}

EXPORT_C RParameterFamily RCFParameterFamilyBundle::FindFamily(TUint32 aFamilyId)
	{
	CCFParameterFamilyBundle* tBundle=TargetBundle();
	return tBundle?tBundle->FindFamily(aFamilyId):RParameterFamily();
	}

EXPORT_C void RCFParameterFamilyBundle::ClearAllParameters(RParameterFamily::TParameterSetType aType)
	{
	CCFParameterFamilyBundle* tBundle=TargetBundle();
	if(tBundle)
		{
		tBundle->ClearAllParameters(aType);
		}
	}


