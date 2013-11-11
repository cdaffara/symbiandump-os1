// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Wrappers for the RParameterFamilyBundle class for use inside the
// messaging infrastructure.
// CCFParameterFamilyBundle implements reference counting for the owned (by inheritance)
// RParameterFamilyBundle. It also inherits from MCFNodeIdBase so it can be addressed from
// anywhere in the messaging infrastructure.
// RCFParameterFamilyBundle is a handle to CCFParameterFamilyBundle which points to it
// by use of its TCommsId. This potentially allows it to safely access it even if it refers to
// a CCFParameterFamilyBundle residing in a different thread or process.
// It is a facade class to RParameterFamilyBundle so client code using a RCFParameterFamilyBundle
// can look very much like it would if it were using a RParameterFamilyBundle directly.
// Reference to an underlying bundle should be done by passing RCFParameterFamilyBundle by value.
// Anyone interested in keeping the bundle for its own purposes should create a new instance
// of RCFParameterFamilyBundle and Open it, passing the existing handle as an argument.
// 
//
// Note: Every effort should be made to ensure that the classes forming this API be kept consistent with each other
// When making changes to these classes also see the files es_parameterbundle.h, es_parameterfamily.h,
// ss_parameterfamilybundle.h
//

/**
 @file
 @publishedPartner
 @released
*/


#ifndef  __SS_PARAMETERFAMILYBUNDLE_H_
#define  __SS_PARAMETERFAMILYBUNDLE_H_


#include <elements/nm_node.h>
#include <comms-infras/es_parameterfamily.h>

namespace ESock
{

class CCFParameterFamilyBundle;


class RCFParameterFamilyBundleC
/**
@publishedPartner
@released
*/
	{
	friend class RCFParameterFamilyBundle;
public:
	// point to nothing by default.
	RCFParameterFamilyBundleC():
		iBundleId(Messages::TNodeId::NullId())
		{
		}

	TBool operator==(const RCFParameterFamilyBundleC& aRhs) const
		{
		return iBundleId==aRhs.iBundleId;
		}

	TBool operator!=(const RCFParameterFamilyBundleC& aRhs) const
		{
		return !(*this==aRhs);
		}

	// facade to CCFParameterFamilyBundle:
	
	// refcounting
	IMPORT_C void Open();
	IMPORT_C void Open(const RCFParameterFamilyBundleC& aExistingHandle);
	IMPORT_C void Close();

	// creation & deserialising
	IMPORT_C void CreateL();
	IMPORT_C void LoadL(TDesC8& aDes);

	// serialising
	IMPORT_C TUint Length() const;
	IMPORT_C TInt Store(TDes8& aDes) const;

	// get/find families
	IMPORT_C const RParameterFamily GetFamilyAtIndex(TUint aIndex) const;
	IMPORT_C const RParameterFamily FindFamily(TUint32 aFamilyId) const;
	IMPORT_C TUint CountParameterSets() const;

public:
	TBool IsNull() const
		{
		return const_cast<RCFParameterFamilyBundleC&>(*this).TargetBundle()?EFalse:ETrue;
		}
	void CheckOpenL() const
		{
		(void)const_cast<RCFParameterFamilyBundleC&>(*this).TargetBundleL();
		}

protected:
	IMPORT_C CCFParameterFamilyBundle* TargetBundle();

	CCFParameterFamilyBundle& TargetBundleL()
		{
		CCFParameterFamilyBundle* tBundle = TargetBundle();
		if(tBundle==NULL)
			{
			User::Leave(KErrNotReady);
			}
		return *tBundle;
		}

private:
	RCFParameterFamilyBundleC& operator = (const RCFParameterFamilyBundleC& aSrc);

	
private:
	Messages::TNodeId iBundleId;
	};

/**
RCFParameterFamilyBundle_const defined for compatibility, use RCFParameterFamilyBundleC instead
@publishedPartner
@deprecated
*/
typedef RCFParameterFamilyBundleC RCFParameterFamilyBundle_const;


class RCFParameterFamilyBundle : public RCFParameterFamilyBundleC
/**
@internalTechnology
*/
	{
	friend class RCFParameterFamilyBundleC;
public:
	// point to nothing by default.
	RCFParameterFamilyBundle():
		RCFParameterFamilyBundleC()
		{
		}

	// adding/creating families
	IMPORT_C RParameterFamily CreateFamilyL(TUint32 aFamilyId);

	// get/find families
	IMPORT_C RParameterFamily GetFamilyAtIndex(TUint aIndex);
	IMPORT_C RParameterFamily FindFamily(TUint32 aFamilyId);

	// clear all parameters
	IMPORT_C void ClearAllParameters(RParameterFamily::TParameterSetType aType);

private:
	RCFParameterFamilyBundle& operator = (const RCFParameterFamilyBundle& aSrc);
	};


} // namespace ESock

#endif // __SS_PARAMETERFAMILYBUNDLE_H_

