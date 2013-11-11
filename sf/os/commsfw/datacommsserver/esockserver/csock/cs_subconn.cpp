// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file cs_subconn.cpp
*/

#include <comms-infras/nifprvar.h>
#include <ecom/ecom.h>
#include <es_sock.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_sock_partner.h>
#include <es_sock_internal.h>
#endif


#include <comms-infras/sockmes.h>
#include <connpref.h>
#include <comms-infras/ss_log.h>
#include <comms-infras/es_parameterbundle.h>
#include <comms-infras/es_parameterfamily.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockCSockcssbcn, "ESockCSockcssbcn");
#endif

using Meta::STypeId;

EXPORT_C CSubConParameterSet* CSubConParameterSet::NewL(const STypeId& aTypeId)
/** Creates a new sub-connection parameter set using ECOM to load the DLL plugin
    that implements the set.

@param aTypeId Id of the class (Composed of Uid of the implementation and an integer sub-type
@return a generic pointer to a specific parameter set if successful, otherwise leaves with system error code.
*/
	{
	return static_cast<CSubConParameterSet*>(Meta::SMetaDataECom::NewInstanceL(aTypeId));
	}

CSubConParameterSet::~CSubConParameterSet()
/** Empty (virtual) sub-connection parameter set destructor
*/
	{
	}

CSubConParameterSet::CSubConParameterSet()
/** Empty sub-connection parameter set constructor
*/
	{
	}

EXPORT_C CSubConGenericParameterSet::~CSubConGenericParameterSet()
/** Empty (virtual) sub-connection generic parameter set destructor
*/
	{
	LOG( ESockLog::Printf(_L8("CSubConGenericParameterSet::~CSubConGenericParameterSet() %08x"), this););
	}

EXPORT_C CSubConGenericParameterSet::CSubConGenericParameterSet()
/** Empty sub-connection generic parameter set constructor
*/
	{
	LOG( ESockLog::Printf(_L8("CSubConGenericParameterSet::CSubConGenericParameterSet() %08x"), this););
	}

EXPORT_C CSubConExtensionParameterSet::~CSubConExtensionParameterSet()
/** Empty (virtual) sub-connection extension parameter set destructor
*/
	{
	}
	
EXPORT_C CSubConExtensionParameterSet::CSubConExtensionParameterSet()
/** Empty sub-connection extension parameter set constructor
*/
	{
	}

EXPORT_C CSubConParameterFamily* CSubConParameterFamily::NewL(RSubConParameterBundle& aBundle, TUint32 aFamilyId)
/** Creates a new sub-connection parameter set family. This class is used as a container for a number of parameter
    sets (generic and extension) that make up a family. It is a specific instantiatable class and therefore creates
	an instance of CSubConParameterFamily without using ECOM. 

Note:
The aBundle parameter that is passed into this method will take ownership of the newly created 
CSubConParameterFamily object.  When the bundle is destroyed, this family object will also be 
destroyed  (along with any parameter sets that are owned by the family).

@param aBundle Family container (bundle) that this family is to be added to
@param aFamilyId identifier for the specific family type, currently only 4, QoS (KSubConQoSFamily),Authorisation (KSubConAuthorisationFamily),
CallDescrParams (KSubConnCallDescrParamsFamily) and ContextDescrParams (KSubConnContextDescrParamsFamily), are defined.
@return a pointer to a sub-connection parameter family if successful, otherwise leaves with system error code.
*/
	{
	CSubConParameterFamily* family = new (ELeave) CSubConParameterFamily(aFamilyId);
	CleanupStack::PushL(family);
	family->ConstructL(aBundle);
	CleanupStack::Pop(family);
	return family;
	}

EXPORT_C CSubConParameterFamily* CSubConParameterFamily::NewL(CSubConParameterBundle& aBundle, TUint32 aFamilyId)
/** Creates a new sub-connection parameter set family. This class is used as a container for a number of parameter
    sets (generic and extension) that make up a family. It is a specific instantiatable class and therefore creates
	an instance of CSubConParameterFamily without using ECOM. 

Note:
The aBundle parameter that is passed into this method will take ownership of the newly created 
CSubConParameterFamily object.  When the bundle is destroyed, this family object will also be 
destroyed  (along with any parameter sets that are owned by the family).

@param aBundle Family container (bundle) that this family is to be added to.
@param aFamilyId identifier for the specific family type, currently only 4, QoS (KSubConQoSFamily),Authorisation (KSubConAuthorisationFamily),
CallDescrParams (KSubConnCallDescrParamsFamily) and ContextDescrParams (KSubConnContextDescrParamsFamily), are defined.
@return a pointer to a sub-connection parameter family if successful, otherwise leaves with system error code.
*/
	{
	CSubConParameterFamily* family = new (ELeave) CSubConParameterFamily(aFamilyId);
	CleanupStack::PushL(family);
	family->ConstructL(aBundle);
	CleanupStack::Pop(family);
	return family;
	}

EXPORT_C TInt32 CSubConParameterFamily::ExtractFamilyAndCreateBufferL(TPtrC8& aBuffer, TPtrC8& aContainerBuffer)
/** Used internally to extract the family id for the descriptor storing the serialised version of this object (and others).
    It also sets up a new buffer that is the section of the original buffer that represents this object only.

@param aBuffer Source buffer containing mulitple serialised objects (family at the start)
@param aContainerBuffer Set up by this method and represents only the serialised data of the family object.
@return the family Id, otherwise leaves with system error code.
*/
	{
	const TInt KDataHdrLen = 2 * static_cast<TInt>(sizeof(TUint32));

	// Guard to ensure length of data, pointed to by the descriptor, is not too small,
	// I.e. we are pointing to something!
	if (aBuffer.Length() < KDataHdrLen)
		{
		User::Leave(KErrArgument);
		}
 
 	// Obtain the 'length' value of the data pointed to by the descriptor
 	// Has to be at least size of header!
    TInt32 dataLength;
    Mem::Copy(&dataLength, aBuffer.Ptr(),sizeof(TInt32));
    
    
	// Guard to ensure 'length' value of data contains enough data to read.
	// i.e. aBuffer.Length() could point to some data whose 'length' is zero.
	//
	// Guard to ensure length of data, pointed to by the descriptor and specified by that
	// descriptor, is not smaller that the length stated in the data itself.
	// i.e. could be an array of objects.
	if ((dataLength < KDataHdrLen) || (aBuffer.Length() < dataLength))
		{
		User::Leave(KErrArgument);
		}

 	// Obtain the 'familyId' value of the data pointed to by the descriptor
	TInt32 familyId;
	Mem::Copy(&familyId, aBuffer.Ptr() + sizeof(TInt32),sizeof(TInt32));
	
	// Set up the container buffer
	// Point it past the 'length' & 'familyId' fields (header).
	// The data pointed to may be an array of user defined datatypes, and each datatype
	// may have arbitrary data following the header fields.
	// Point aContainerBuffer to that arbitrary data.
	aContainerBuffer.Set(aBuffer.Ptr() + KDataHdrLen, dataLength - KDataHdrLen);
	
	// Move source buffer ptr on to the next family, plus any following data
	aBuffer.Set(aBuffer.Ptr() + dataLength, aBuffer.Length() - dataLength);

	return familyId;
	}

EXPORT_C CSubConParameterFamily* CSubConParameterFamily::LoadL(RSubConParameterBundle& aBundle, TPtrC8& aBuffer)
/** Creates a new sub-connection parameter set family from a buffer containing the serialised object.

Note:
The aBundle parameter that is passed into this method will take ownership of the newly created 
CSubConParameterFamily object.  When the bundle is destroyed, this family object will also be 
destroyed  (along with any parameter sets that are owned by the family).

@param aBundle Family container (bundle) that this family is to be added to
@param aBuffer Buffer containing the serialised object information
@return a pointer to a sub-connection parameter family if successful, otherwise leaves with system error code.
*/
	{
	TInt32 familyId = 0;
	TPtrC8 containerBuffer(aBuffer.Ptr(), aBuffer.Length());
	familyId = ExtractFamilyAndCreateBufferL(aBuffer, containerBuffer);
	CSubConParameterFamily* family = new (ELeave) CSubConParameterFamily(familyId);
	CleanupStack::PushL(family);
	family->ConstructL(aBundle);
	User::LeaveIfError(family->Load(containerBuffer));
	CleanupStack::Pop(family);
	return family;
	}

EXPORT_C CSubConParameterFamily* CSubConParameterFamily::LoadL(CSubConParameterBundle& aBundle, TPtrC8& aBuffer)
/** Creates a new sub-connection parameter set family from a buffer containing the serialised object.

Note:
The aBundle parameter that is passed into this method will take ownership of the newly created 
CSubConParameterFamily object.  When the bundle is destroyed, this family object will also be 
destroyed  (along with any parameter sets that are owned by the family).

@param aBundle Family container (bundle) that this family is to be added to
@param aBuffer Buffer containing the serialised object information
@return a pointer to a sub-connection parameter family if successful, otherwise leaves with system error code.
*/
	{
	TInt32 familyId = 0;
	TPtrC8 containerBuffer(aBuffer.Ptr(), aBuffer.Length());
	familyId = ExtractFamilyAndCreateBufferL(aBuffer, containerBuffer);
	CSubConParameterFamily* family = new (ELeave) CSubConParameterFamily(familyId);
	CleanupStack::PushL(family);
	family->ConstructL(aBundle);
	User::LeaveIfError(family->Load(containerBuffer));
	CleanupStack::Pop(family);
	return family;
	}

EXPORT_C CSubConParameterFamily::~CSubConParameterFamily()
/** Sub-connection parameter family destructor. It cleans up the arrays deleting all
    the paarmeter sets it ownes
*/
	{
	iGenericSets.ResetAndDestroy();
	iExtensionSets[ERequested].ResetAndDestroy();
	iExtensionSets[EAcceptable].ResetAndDestroy();
	iExtensionSets[EGranted].ResetAndDestroy();
	}

EXPORT_C void CSubConParameterFamily::SetGenericSetL(CSubConGenericParameterSet& aGenericSet, TParameterSetType aType)
/** Assigns the generic parameter set of a sub-connection parameter set family.

Note:
The CSubConParameterFamily object takes ownership of the parameter set that is passed into 
this method.  When the family object is destroyed, any parameter sets owned by this object 
will also be destroyed.

@param aGenericSet Generic Parameter Set to be assigned to the family (family takes ownership)
@param aType The type of the set (requested, acceptable or granted)
@exception leaves with KAlreadyExists if a set is already assigned to the family.
*/
	{
	if (iGenericSets[aType] != NULL)
		{
		User::Leave(KErrAlreadyExists);
		}
	iGenericSets[aType] = &aGenericSet;
	}


/**
Adds an extension parameter set to a sub-connection parameter set family.

Note:
The CSubConParameterFamily object takes ownership of the parameter set that is passed into 
this method.  When the family object is destroyed, any parameter sets owned by this object 
will also be destroyed.

@param aExtensionSet Extension Parameter Set to be added to the family (family takes ownership)
@param aType The type of the set (requested, acceptable or granted)
@exception leaves with KErrNoMemory in out of memeory conditions.
*/
EXPORT_C void CSubConParameterFamily::AddExtensionSetL(CSubConExtensionParameterSet& aExtensionSet, TParameterSetType aType)
	{
	iExtensionSets[aType].AppendL(&aExtensionSet);
	}


/**
Searches a sub-connection parameter family for an extension parameter set.
DO NOT USE this form of the method. It is broken. This is only capable of finding extension sets
contained within IP Subconnection Provider, and these extension sets have been deprecated.

@param aSetId Although a TUid, this is the extension class Id 
@param aType The type of the set from TParameterSetType (ERequested, EAcceptable or EGranted)
@return Pointer to the extension set if found, otherwise a NULL pointer.
*/
EXPORT_C CSubConExtensionParameterSet* CSubConParameterFamily::FindExtensionSet(TUid aSetId, TParameterSetType aType)
	{
	// Old Ipscpr Factory IUD  = 0x10204309
	STypeId type = STypeId::CreateSTypeId(0x10204309, aSetId.iUid);
	
	TUint length = iExtensionSets[aType].Count();
	for (TUint i = 0; i < length; ++i)
		{
		if (iExtensionSets[aType][i]->GetTypeId() == type)
			{
			return static_cast<CSubConExtensionParameterSet*>(iExtensionSets[aType][i]);
			}
		}
		
	return NULL;
   }
   
   
/**
Searches a sub-connection parameter family for an extension parameter set.
@param aSetId The STypeId of the extension set
@param aType The type of the set from TParameterSetType (ERequested, EAcceptable or EGranted)
@return Pointer to the extension set if found, otherwise a NULL pointer.
*/
EXPORT_C CSubConExtensionParameterSet* CSubConParameterFamily::FindExtensionSet(STypeId aSetId, TParameterSetType aType)
	{
	TUint length = iExtensionSets[aType].Count();
	for (TUint i = 0; i < length; ++i)
		{
		if (iExtensionSets[aType][i]->GetTypeId() == aSetId)
			{
			return static_cast<CSubConExtensionParameterSet*>(iExtensionSets[aType][i]);
			}
		}
		
/*
 * This is a workaround for the relocated QoS extension parameter set classes. If an
 * extension set class is requested from the new factory and it can't be found another
 * check needs to be made to see if the equivalent deprecated class is there.
 *  
 * Normally this couldn't be done, but in this case its Ok since the classes
 * we're searching on are actually identical. 
 *
 * This can be removed once the deprecated class CSubConQosIPLinkR99ParamSet and
 * its friends are removed from ipscpr.
 *
 * Hardcoded values are being used here since;
 *    i)  They shouldn't be changing
 *    ii) It avoids having to include two unnecessary header files
 *
 * New Umts Factory UID    = 0x1020D460
 * Old Ipscpr Factory IUD  = 0x10204309
 * 
*/
   if (aSetId.iUid == TUid::Uid(0x1020D460))
      {
      STypeId alternateTypeId = STypeId::CreateSTypeId(0x10204309, aSetId.iType);
   	for (TUint i = 0; i < length; ++i)
   		{
   		if (iExtensionSets[aType][i]->GetTypeId() == alternateTypeId)
   			{
   			return static_cast<CSubConExtensionParameterSet*>(iExtensionSets[aType][i]);
   			}
   		}
      }

	return NULL;
	}

EXPORT_C CSubConGenericParameterSet* CSubConParameterFamily::GetGenericSet(TParameterSetType aType)
/** Accessor for the sub-connection generic parameter set of a family.

@param aType The type of the set (requested, acceptable or granted)
@return Pointer to the generic set if set, otherwise a NULL pointer.
*/
	{
	return static_cast<CSubConGenericParameterSet*>(iGenericSets[aType]);
	}

EXPORT_C TUint CSubConParameterFamily::Length() const
/** Calculates the length of buffer required to serialise this parameter set family.

@return Length of buffer required.
*/
	{
	return sizeof(TInt32) + sizeof(TInt32) + iGenericSets.Length() + iExtensionSets[ERequested].Length() + iExtensionSets[EAcceptable].Length() + iExtensionSets[EGranted].Length();
	}

EXPORT_C TInt CSubConParameterFamily::Load(TPtrC8& aBuffer)
/** Instructs this sub-connection family to set its members based on the serialiesd data
    in the buffer passed.

@param aBuffer Buffer containing the serialised family object
@return KErrNone if successful, otherwise system wide error
*/
	{
	TInt ret = iGenericSets.GetTypeId().Check(aBuffer);
	if (ret == KErrNone)
		{
		ret = iGenericSets.Load(aBuffer);
		}
	
	if (ret != KErrNone)
		{
		return ret;
		}

	if (aBuffer.Length() == 0)
		{
		iGenericSets.ResetAndDestroy();
		return KErrArgument;
		}
	
	ret = iExtensionSets[ERequested].GetTypeId().Check(aBuffer);
	if (ret == KErrNone)
		{
		ret = iExtensionSets[ERequested].Load(aBuffer);
		}
	if (ret != KErrNone)
		{
		iGenericSets.ResetAndDestroy();
		return ret;
		}

	if (aBuffer.Length() > 0)
		{
		ret = iExtensionSets[EAcceptable].GetTypeId().Check(aBuffer);
		if (ret == KErrNone)
			{
			ret = iExtensionSets[EAcceptable].Load(aBuffer);
			}
		if (ret != KErrNone)
			{
			iGenericSets.ResetAndDestroy();
			iExtensionSets[ERequested].ResetAndDestroy();
			return ret;
			}
		}

	if (aBuffer.Length() > 0)
		{
		ret = iExtensionSets[EGranted].GetTypeId().Check(aBuffer);
		if (ret == KErrNone)
			{
			ret = iExtensionSets[EGranted].Load(aBuffer);
			}
		if (ret != KErrNone)
			{
			iGenericSets.ResetAndDestroy();
			iExtensionSets[ERequested].ResetAndDestroy();
			iExtensionSets[EAcceptable].ResetAndDestroy();
			return ret;
			}
		}
	return KErrNone;
	}

EXPORT_C TInt CSubConParameterFamily::Store(TDes8& aDes) const
/** Instructs this sub-connection family to create a serialised version of itself
    and append it to the buffer that has been passed.

@param aDes Buffer to append the serialised object to
@return KErrNone if successful, otherwise system wide error (e.g. KErrOverflow if the descriptor is too small)
If unsuccessful, there is no guarantee as to the buffer or data stored in it.
*/
	{	
	// Needs to be at least this size to store len and familyId ptrs
	// aDes can only grow upto MaxLength.
	if (aDes.MaxLength() < (2 * static_cast<TInt>(sizeof(TUint32))))
		{
		return KErrOverflow;
		}
	
	TUint len = Length();

	aDes.Append((TUint8*)&len, sizeof(TUint32));
	aDes.Append((TUint8*)&iFamilyId, sizeof(TUint32));

	TInt ret = iGenericSets.Store(aDes);
	if (ret != KErrNone)
		{
		return ret;
		}

	ret = iExtensionSets[ERequested].Store(aDes);
	if (ret != KErrNone)
		{
		return ret;
		}

	ret = iExtensionSets[EAcceptable].Store(aDes);
	if (ret != KErrNone)
		{
		return ret;
		}

	return iExtensionSets[EGranted].Store(aDes);
	}

EXPORT_C void CSubConParameterFamily::ClearAllParameters(TParameterSetType aType)
/** Clears (removes and deletes) all parameter sets with the type specified

@param aType The type of the set to be deleted(requested, acceptable or granted)
*/
	{
	if (iGenericSets[aType] != NULL)
		{
		delete iGenericSets[aType];
		iGenericSets[aType] = NULL;
		}
	iExtensionSets[aType].ResetAndDestroy();
	}

void CSubConParameterFamily::CopyToFamilyL(RParameterFamily& aDest) const 
	{
	__ASSERT_DEBUG((TInt)CSubConParameterFamily::ENumValues == (TInt)RParameterFamily::ENumValues, User::Panic(KSpecAssert_ESockCSockcssbcn, 1));

	TInt type=0;
	for ( ; type<ENumValues; ++type)
		{
		XParameterSetBase* srcSet =
			 static_cast<XParameterSetBase*>(iGenericSets[type]);
		if(srcSet)
			{
			XParameterSetBase* newSet =
				static_cast<XParameterSetBase*>
				(XParameterSetBase::NewInstanceL(srcSet->GetTypeId()));
			newSet->Copy(*srcSet);
			aDest.AddParameterSetL(newSet,RParameterFamily::TParameterSetType(type));
			}
		}

	for (type=0; type<ENumValues; ++type)
		{
		const RMetaDataEComContainer& srcContainer = iExtensionSets[type];
		for (TInt srcidx=0; srcidx<srcContainer.Count(); ++srcidx)
			{
			XParameterSetBase* srcSet = static_cast<XParameterSetBase*>(srcContainer[srcidx]);
			if(srcSet)
				{
				XParameterSetBase* newSet =
					static_cast<XParameterSetBase*>
					(XParameterSetBase::NewInstanceL(srcSet->GetTypeId()));
				newSet->Copy(*srcSet);
				aDest.AddParameterSetL(newSet,RParameterFamily::TParameterSetType(type));
				}
			}
		}
	}

void CSubConParameterFamily::CopyFromFamilyL(RParameterFamily& aSrc)
	{
		__ASSERT_DEBUG((TInt)CSubConParameterFamily::ENumValues == (TInt)RParameterFamily::ENumValues, User::Panic(KSpecAssert_ESockCSockcssbcn, 2));

	// The excuse for quoting the following UIds is:
	//  They are the only types defined before the switchover to generic 
	//   RFamilyBundles so are the only ones which will be subject to the
	//   differentiation between "Generic" slot and "Extension" list.
	//

	//STypeId gentype_1(CSubConIPAddressInfoParamSet::EUid,CSubConIPAddressInfoParamSet::EType); // CSubConIPAddressInfoParamSet
	STypeId gentype_1 = STypeId::CreateSTypeId(0x102822D5,1); // CSubConIPAddressInfoParamSet

	//STypeId gentype_2(KSubConnGenericParamsImplUid,KSubConnQosGenericParamsType); // CSubConQosGenericParamSet
	STypeId gentype_2 = STypeId::CreateSTypeId(KSubConnGenericParamsImplUid,1); // CSubConQosGenericParamSet

	//STypeId gentype_3(KSubConnGenericParamsImplUid,KSubConnAuthorisationGenericParamsType); // CSubConAuthorisationGenericParamSet
	STypeId gentype_3 = STypeId::CreateSTypeId(KSubConnGenericParamsImplUid,2); // CSubConAuthorisationGenericParamSet

	//STypeId gentype_4(KSubConnGenericParamsImplUid,KFlowRequestParametersType); // CFlowRequestParameters
	STypeId gentype_4 = STypeId::CreateSTypeId(KSubConnGenericParamsImplUid,3); // CFlowRequestParameters

	//STypeId gentype_5(KSubConnGenericParamsImplUid,KSubConnProtocolGenericParamsType); // CSubConnectionProtocolParameterSet
	STypeId gentype_5 = STypeId::CreateSTypeId(KSubConnGenericParamsImplUid,4); // CSubConnectionProtocolParameterSet

	//STypeId gentype_6(KSubConChannelParamsImplUid,KSubConChannelParamsType); // CSubConChannelParamSet
	STypeId gentype_6 = STypeId::CreateSTypeId(0x20019D42,9); // CSubConChannelParamSet
	
	for(TInt type=0; type<ENumValues; ++type)
		{
		TInt nSets = aSrc.CountParameterSets(RParameterFamily::TParameterSetType(type));
		for(TInt srcidx=0 ; srcidx<nSets ; ++srcidx )
			{
			XParameterSetBase* srcSet = aSrc.GetParameterSetAtIndex(srcidx,RParameterFamily::TParameterSetType(type));
			
			TBool isGenericSet = 
				(srcSet->IsTypeOf(gentype_1) || 
			     srcSet->IsTypeOf(gentype_2) || 
			     srcSet->IsTypeOf(gentype_3) || 
			     srcSet->IsTypeOf(gentype_4) || 
			     srcSet->IsTypeOf(gentype_5) || 
			     srcSet->IsTypeOf(gentype_6) ) ;

			XParameterSetBase* newSet =
				static_cast<XParameterSetBase*>
				(XParameterSetBase::NewInstanceL(srcSet->GetTypeId()));
			
			newSet->Copy(*srcSet);
			if(isGenericSet)
				{
				CSubConGenericParameterSet &genPs =
					static_cast<CSubConGenericParameterSet&>(*newSet);
				SetGenericSetL(genPs,TParameterSetType(type));
				}
			else
				{
				CSubConExtensionParameterSet &extPs =
					static_cast<CSubConExtensionParameterSet&>(*newSet);
				AddExtensionSetL(extPs,TParameterSetType(type));
				}
			}
		}
	}


EXPORT_C CSubConParameterFamily::CSubConParameterFamily(TUint32 aFamilyId)
	: iFamilyId(aFamilyId)
/** Empty sub-connection parameter family constructor

@param aFamilyId identifier for the specific family type, currently only 4, QoS (KSubConQoSFamily),Authorisation (KSubConAuthorisationFamily),
CallDescrParams (KSubConnCallDescrParamsFamily) and ContextDescrParams (KSubConnContextDescrParamsFamily), are defined.
*/
	{
	}

EXPORT_C void CSubConParameterFamily::ConstructL(RSubConParameterBundle& aBundle)
/** Set up the sub-connection family with empty sets

Note:
The aBundle parameter that is passed into this method will take ownership of the newly created 
CSubConParameterFamily object.  When the bundle is destroyed, this family object will also be 
destroyed  (along with any parameter sets that are owned by the family).

@param aBundle Bundle (container) the family is to be added to
@exception leaves with KErrNoMemory in out of memory conditions
*/
	{
	iGenericSets.AppendL(NULL);
	iGenericSets.AppendL(NULL);
	iGenericSets.AppendL(NULL);
	aBundle.AddFamilyL(this);
	}

EXPORT_C void CSubConParameterFamily::ConstructL(CSubConParameterBundle& aBundle)
/** Set up the sub-connection family with empty sets

Note:
The aBundle parameter that is passed into this method will take ownership of the newly created 
CSubConParameterFamily object.  When the bundle is destroyed, this family object will also be 
destroyed  (along with any parameter sets that are owned by the family).

@param aBundle Bundle (container) the family is to be added to
@exception leaves with KErrNoMemory in out of memory conditions
*/
	{
	iGenericSets.AppendL(NULL);
	iGenericSets.AppendL(NULL);
	iGenericSets.AppendL(NULL);
	aBundle.AddFamilyL(this);
	}

CSubConParameterBundle::CSubConParameterBundle(const CSubConParameterBundle& /*aBundle*/)
/** Copy Constructor
*/
	{
	__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockCSockcssbcn, 3));//can be Unreachable in future
	}

CSubConParameterBundle& CSubConParameterBundle::operator=(const CSubConParameterBundle& /*aBundle*/)
/** Assignment operator
*/
	{
	__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockCSockcssbcn, 4));//can be Unreachable in future
	return *this;
	}

TInt RSubConParameterBundle::CheckBundle() const
/** Check to see if the handle has a heap object. If it hasn't, it creates one

@return KErrNone if successful, otherwise a system wide error
*/
	{
	if (!iBundle)
		{
		TRAPD(ret, iBundle = CSubConParameterBundle::NewL());
		return ret;
		}
	return KErrNone;
	}

EXPORT_C RSubConParameterBundle::RSubConParameterBundle()
	: iBundle(NULL)
/** Empty sub-connection parameter bundle constructor
*/
	{
	}

RSubConParameterBundle::RSubConParameterBundle(const RSubConParameterBundle& /*aBundle*/)
/** Copy Constructor
*/
	{
	__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockCSockcssbcn, 5));//can be Unreachable in future
	}

RSubConParameterBundle& RSubConParameterBundle::operator=(const RSubConParameterBundle& /*aBundle*/)
/** Assignment operator
*/
	{
	__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockCSockcssbcn, 6));//can be Unreachable in future
	return *this;
	}

EXPORT_C void RSubConParameterBundle::Close()
/** Close the parameter bundle handle
*/
	{
	if (iBundle)
		{
		iBundle->Close();
		iBundle = NULL;
		}
	}

EXPORT_C TUint RSubConParameterBundle::Length() const
/** Calculates the length of buffer required to serialise this parameter set bundle.

@return Length of buffer required.
*/
	{
	TInt err = CheckBundle();
	return (err==KErrNone)? iBundle->Length() : 0;
	}

EXPORT_C TInt RSubConParameterBundle::Load(const TDesC8& aDes)
/** Instructs this sub-connection bundle to set its members based on the serialiesd data
    in the buffer passed.

@param aDes Buffer containing the serialised bundle object
@return KErrNone if successful, otherwise system wide error
*/
	{
	TInt ret = CheckBundle();
	if (ret == KErrNone)
		ret = iBundle->Load(aDes);
	return ret;
	}

EXPORT_C TInt RSubConParameterBundle::Store(TDes8& aDes) const
/** Instructs this sub-connection parameter bundle to create a serilised version of itself
    and append it to the buffer that has been passed.

@param aDes Buffer to append the serialised object to
@return KErrNone if successful, otherwise system wide error (e.g. EKrrOverflow if the descriptor is too small)
If unsuccessful, there is no guarantee as to the buffer or data stored in it.
*/
	{
	TInt ret = CheckBundle();
	if (ret == KErrNone)
		ret = iBundle->Store(aDes);
	return ret;
	}

EXPORT_C void RSubConParameterBundle::AddFamilyL(CSubConParameterFamily* aFamily)
/** Add a sub-connection parameter family to the bundle.

Note:
The RSubConParameterBundle object takes ownership of the parameter family that is passed 
into this method.  When the bundle object is destroyed, any family owned by this object 
will also be destroyed.

@param aFamily Family to be added (bundle takes ownership)
@exception leaves with KErrNoMemory in out of memory conditions
*/
	{
	User::LeaveIfError(CheckBundle());
	iBundle->AddFamilyL(aFamily);
	}

EXPORT_C CSubConParameterFamily* RSubConParameterBundle::FindFamily(TUint32 aFamilyId)
/** Search the sub-connection parameter bundle for a parameter family

@param aFamilyId Id of the family to match against. It is an identifier for the specific family type, currently only 4, QoS (KSubConQoSFamily),Authorisation (KSubConAuthorisationFamily),
CallDescrParams (KSubConnCallDescrParamsFamily) and ContextDescrParams (KSubConnContextDescrParamsFamily), are defined.
@return Matching family or NULL pointer if not found
*/
	{
	if (CheckBundle() != KErrNone)
		return NULL;
	else
		return iBundle->FindFamily(aFamilyId);
	}

EXPORT_C void RSubConParameterBundle::ClearAllParameters(CSubConParameterFamily::TParameterSetType aType)
/** Clears (removes and deletes) all parameter sets with the type specified (delegated to the families)

@param aType The type of the set to be deleted(requested, acceptable or granted)
*/
	{
	if (iBundle != NULL)
		{
		iBundle->ClearAllParameters(aType);
		}
	}


EXPORT_C void RSubConParameterBundle::CopyToFamilyBundleL(RParameterFamilyBundle& aDest) const
	{
	aDest.CreateL();
	User::LeaveIfError(CheckBundle());
	
	TUint length = iBundle->iFamilies.Count();
	for (TUint i = 0; i < length; ++i)
		{
		CSubConParameterFamily* srcScpf = iBundle->iFamilies[i];
		RParameterFamily newPf;
		newPf.CreateL(aDest,srcScpf->Id());
		srcScpf->CopyToFamilyL(newPf);
		}
	}
	
EXPORT_C void RSubConParameterBundle::CopyFromFamilyBundleL(RParameterFamilyBundle& aSrc)
	{
	if (aSrc.IsNull())
		{
		User::Leave(KErrNotReady);
		}
	
	RParameterFamily srcPf;
	TInt index=0;
	while( ! (srcPf=aSrc.GetFamilyAtIndex(index++)).IsNull() )
		{
		CSubConParameterFamily* newScpf = CSubConParameterFamily::NewL(*this,srcPf.Id());
		newScpf->CopyFromFamilyL(srcPf);
		}
	}

EXPORT_C CSubConParameterBundle* CSubConParameterBundle::NewL()
/** Creates a new instance of a sub-connection parameter bundle (heap object)
    Can be used directly as a CObject or via the RSubConParameterBundle handle.

@return newly created instance of a sub-connection parameter bundle
@exception leaves with KErrNoMemory in out of memory conditions
*/
	{
	return new (ELeave) CSubConParameterBundle();
	}

EXPORT_C CSubConParameterBundle* CSubConParameterBundle::LoadL(TDesC8& aDes)
/** Creates a new sub-connection parameter set bunlde from a buffer containing the serialised object.

@param aDes Buffer containing the serialised object information
@return a pointer to a sub-connection parameter bundle if successful, otherwise leaves with system error code.
*/
	{
	CSubConParameterBundle* obj = new (ELeave) CSubConParameterBundle();
	CleanupStack::PushL(obj);
	User::LeaveIfError(obj->Load(aDes));
	CleanupStack::Pop(obj);
	return obj;
	}

EXPORT_C CSubConParameterBundle::~CSubConParameterBundle()
/** Sub-connection parameter bundle destructor, clear up the families
*/
	{
	iFamilies.ResetAndDestroy();
	}

EXPORT_C TUint CSubConParameterBundle::Length() const
/** Calculates the length of buffer required to serialise this parameter set bundle.

@return Length of buffer required.
*/
	{
	TInt length = 0;
	TUint count = iFamilies.Count();
	for (TUint i = 0; i < count; ++i)
		{
		length += iFamilies[i]->Length();
		}
	return length;
	}

EXPORT_C TInt CSubConParameterBundle::Load(const TDesC8& aDes)
/** Instructs this sub-connection bundle to set its members based on the serialiesd data
    in the buffer passed.

@param aDes Buffer containing the serialised bundle object
@return KErrNone if successful, otherwise system wide error
*/
	{
	TPtrC8 buf(aDes);

	while (buf.Length() > 0)
		{
		TRAPD(ret, CSubConParameterFamily::LoadL(*this, buf));
		if (ret != KErrNone)
			{
			iFamilies.ResetAndDestroy();
			return ret;
			}
		}
	return KErrNone;
	}

EXPORT_C TInt CSubConParameterBundle::Store(TDes8& aDes) const
/** Instructs this sub-connection parameter bundle to create a serilised version of itself
    and append it to the buffer that has been passed.

@param aDes Buffer to append the serialised object to
@return KErrNone if successful, otherwise system wide error (e.g. EKrrOverflow if the descriptor is too small)
If unsuccessful, there is no guarantee as to the buffer or data stored in it.
*/
	{
	TUint length = iFamilies.Count();

	for (TUint i = 0; i < length; ++i)
		{
		TInt ret = iFamilies[i]->Store(aDes);
		if (ret != KErrNone)
			{
			aDes.Zero();
			return ret;
			}
		}
	return KErrNone;
	}

EXPORT_C void CSubConParameterBundle::AddFamilyL(CSubConParameterFamily* aFamily)
/** Add a sub-connection parameter family to the bundle.

Note:
The CSubConParameterBundle object takes ownership of the parameter family that is passed 
into this method.  When the bundle object is destroyed, any family owned by this object 
will also be destroyed.

@param aFamily Family to be added (bundle takes ownership)
@exception leaves with KErrNoMemory in out of memory conditions
*/
	{
	iFamilies.AppendL(aFamily);
	}

EXPORT_C CSubConParameterFamily* CSubConParameterBundle::FindFamily(TUint32 aFamilyId)
/** Search the sub-connection parameter bundle for a parameter family

@param aFamilyId Id of the family to match against. It is an identifier for the specific family type, currently only 4, QoS (KSubConQoSFamily),Authorisation (KSubConAuthorisationFamily),
CallDescrParams (KSubConnCallDescrParamsFamily) and ContextDescrParams (KSubConnContextDescrParamsFamily), are defined.
@return Matching family or NULL pointer if not found
*/
	{
	TUint length = iFamilies.Count();
	for (TUint i = 0; i < length; ++i)
		{
		if (iFamilies[i]->Id() == aFamilyId)
			{
			return iFamilies[i];
			}
		}
	return NULL;
	}

EXPORT_C CSubConParameterFamily* CSubConParameterBundle::FindFamily(TUint32 aFamilyId) const
/** Search the sub-connection parameter bundle for a parameter family

@param aFamilyId Id of the family to match against. It is an identifier for the specific family type, currently only 4, QoS (KSubConQoSFamily),Authorisation (KSubConAuthorisationFamily),
CallDescrParams (KSubConnCallDescrParamsFamily) and ContextDescrParams (KSubConnContextDescrParamsFamily), are defined.
@return Matching family or NULL pointer if not found
*/
	{
	TUint length = iFamilies.Count();
	for (TUint i = 0; i < length; ++i)
		{
		if (iFamilies[i]->Id() == aFamilyId)
			{
			return iFamilies[i];
			}
		}
	return NULL;
	}

EXPORT_C void CSubConParameterBundle::ClearAllParameters(CSubConParameterFamily::TParameterSetType aType)
/** Clears (removes and deletes) all parameter sets with the type specified (delegated to the families)

@param aType The type of the set to be deleted(requested, acceptable or granted)
*/
	{
	TUint length = iFamilies.Count();
	for (TUint i = 0; i < length; ++i)
		{
		iFamilies[i]->ClearAllParameters(aType);
		}
	}

EXPORT_C CSubConParameterBundle::CSubConParameterBundle()
/** Empty sub-connection generic parameter bundle constructor
*/
	{
	}

EXPORT_C TNotificationEventBuf::TNotificationEventBuf()
	{
	SetLength(this->MaxLength());
	FillZ();
	}

EXPORT_C TNotificationEventBuf::~TNotificationEventBuf()
	{
	}
	
EXPORT_C TBool TNotificationEventBuf::IsGeneric() const
/** Check whether an event is a generic event (not an extension)

@return ETrue if the event is generic, otherwise EFalse
*/
	{
	return GroupId() == KSubConnGenericEventsImplUid;
	}

EXPORT_C TInt32 TNotificationEventBuf::GroupId() const
/** Provides the group id (Uid) of the event

@return Group id in the form of a UId (Could be generic or any extension uid)
*/
	{
	return *((TInt32*)Ptr());
	}

EXPORT_C TUint32 TNotificationEventBuf::Id() const
/** Provides the sub-type Id of the event

@return sub-type id
*/
	{
	return *((TUint32*)(Ptr() + sizeof(TUint32)));
	}

EXPORT_C CSubConNotificationEvent* CSubConNotificationEvent::NewL(const STypeId& aTypeId)
/** Creates a new sub-connection event using ECOM to load the DLL plugin
    that implements the event.

@param aTypeId Id of the class (Composed of Uid of the implementation and an integer sub-type)
@return a generic pointer to a specific event if successful, otherwise leaves with system error code.
*/
	{
	return static_cast<CSubConNotificationEvent*>(Meta::SMetaDataECom::NewInstanceL(aTypeId));
	}

EXPORT_C CSubConNotificationEvent* CSubConNotificationEvent::NewL(const TNotificationEventBuf& aEventBuffer)
/** Creates a new sub-connection event using ECOM to load the DLL plugin
    that implements the event.

@param aEventBuffer A Buffer containing the serialised form of the event object
@return a generic pointer to a specific event if successful, otherwise leaves with system error code.
*/
	{
	TPtrC8 buf(aEventBuffer.Ptr(), aEventBuffer.Length());
	return static_cast<CSubConNotificationEvent*>(Meta::SMetaDataECom::LoadL(buf));
	}

EXPORT_C TBool CSubConNotificationEvent::IsGeneric() const
/** Check whether an event is a generic event (not an extension)

@return ETrue if the event is generic, otherwise EFalse
*/
	{
	return GetTypeId().iUid.iUid == KSubConnGenericEventsImplUid;
	}

EXPORT_C TInt32 CSubConNotificationEvent::GroupId() const
/** Provides the group id (Uid) of the event

@return Group id in the form of a UId (Could be generic or any extension uid)
*/
	{
	return static_cast<TInt32>(GetTypeId().iUid.iUid);
	}

EXPORT_C TUint32 CSubConNotificationEvent::Id() const
/** Provides the sub-type Id of the event

@return sub-type id
*/
	{
	return GetTypeId().iType;
	}

EXPORT_C RSubConnection::RSubConnection()
/** Empty sub-connection handle constructor
*/
	{
	}

EXPORT_C TInt RSubConnection::Open(RSocketServ& aServer, TSubConnType aSubConnType, RConnection& aConnection)
/** Opens a new RSubConnection instance.

@param aSocketServer Socket Server session.
@param aSubConnType specifies the type of sub connection, the different available types are defined by TSubConnType.
@param aConnection Parent Connection that is to be sub divided by this sub-connection
@return KErrNone if successful, otherwise another of the system wide error codes.
*/
    {
    return Open(aServer, static_cast<TSubConnOpen::TSubConnType>(aSubConnType), aConnection);
    }

EXPORT_C void RSubConnection::Close()
/** Closes the sub-connection.

The sub-connection will not be dropped immediately: it will be dropped when there 
is no more data traffic on the sub-connection. 
*/
	{
	LOG( ESockLog::Printf(_L8("RSubConnection %08x:\tClose() tid %d"), this, (TUint)RThread().Id()););
	CloseSubSession(ESCClose);
	REComSession::FinalClose();
	}
	
EXPORT_C void RSubConnection::Start(TRequestStatus& aStatus)
/**  
Unused
*/
    {
	LOG( ESockLog::Printf(_L8("RSubConnection %08x:\tStart() async tid %d"), this, (TUint)RThread().Id()););
	SendReceive(ESCStart, TIpcArgs(), aStatus);
    }

EXPORT_C TInt RSubConnection::Start()
/**
Unused
*/
    {
	LOG( ESockLog::Printf(_L8("RSubConnection %08x:\tStart() sync tid %d"), this, (TUint)RThread().Id()););
	TRequestStatus status;
	Start(status);
	User::WaitForRequest(status);
	return status.Int();
    }

EXPORT_C TInt RSubConnection::Stop()
/**
Unused
*/
    {
	LOG( ESockLog::Printf(_L8("RSubConnection %08x:\tStop() tid %d"), this, (TUint)RThread().Id()););
	return SendReceive(ESCStop, TIpcArgs());
    }


EXPORT_C TInt RSubConnection::Open(RSocketServ& aServer, TSubConnOpen::TSubConnType aSubConnType, RConnection& aConnection)
/** Opens a new RSubConnection instance.

@param aServer Socket Server session.
@param aSubConnType EAttachToDefault when attaching to default or ECreateNew for new one
@param aConnection Parent Connection that is to be sub divided by this sub-connection
@return KErrNone if successful, otherwise another of the system wide error codes.
*/
	{

	if(SubSessionHandle() == KNullHandle)
		{
		// passing an RConnection which doesn't belong to the same session is an error
		if (!aConnection.SameSession(aServer.Handle()))
			{
			return KErrArgument;
			}

		TSubConnOpen arg;
		arg.iType   = aSubConnType;
		arg.iHandle = aConnection.SubSessionHandle();
		TPckgC<TSubConnOpen> buf(arg);


		const TInt ret = CreateSubSession(aServer, ESCCreate,TIpcArgs(&buf));		
		LOG( ESockLog::Printf(_L8("RSubConnection %08x:\tOpen() tid %d"), this, (TUint)RThread().Id()););
		return ret;
		}
	else
		{
		return KErrInUse;
		}
	}

EXPORT_C void RSubConnection::Add(RSocket& aSocket, TRequestStatus& aStatus)
/** Associate a socket with a subconnection. The socket must already be using the same connection
    or the association attempt will fail. The completion of this method does not indicate that the
	socket is now running over the sub-connection, it only indicates that the association is complete
	and a negotiation has started to move the socket onto the subconnection. An event (via eventNotification)
	will be sent once the socket is running on the sub-connection.

@param aSocket Socket to be associated with the Sub-Connection
@param aStatus Status to complete when socket has been added (or failed to add)
@return KErrNone if successful, otherwise a system wide error
*/
	{
	LOG( ESockLog::Printf(_L8("RSubConnection %08x:\tAdd(RSocket id=%d) tid %d"), this, aSocket.SubSessionHandle(), (TUint)RThread().Id()););
 	SendReceive(ESCAddSocket, TIpcArgs(aSocket.SubSessionHandle()), aStatus);
	}

EXPORT_C void RSubConnection::Remove(RSocket& aSocket, TRequestStatus& aStatus)
/** Remove a socket from a subconnection. The socket will be moved back to the default sub-connection.
	The successful completion of this method does not indicate that the socket has moved back to the
	default sub-connection, it only indicates that the move negotiation has been successful. An event
	(via eventNotification) will be sent once the socket is running on the default sub-connection.

@param aSocket Socket to be removed from the Sub-Connection
@param aStatus Status to complete when socket has been removed (or failed to remove)
@return KErrNone if successful, otherwise a system wide error
*/
	{
	LOG( ESockLog::Printf(_L8("RSubConnection %08x:\tRemove(RSocket id=%d) tid %d"), this, aSocket.SubSessionHandle(), (TUint)RThread().Id()););
	SendReceive(ESCRemoveSocket, TIpcArgs(aSocket.SubSessionHandle()), aStatus);
	}

EXPORT_C TInt RSubConnection::SetParameters(const RSubConParameterBundle& aParameterBundle)
/** Set Parameters of the sub-connection. Successful completion of this method only indicates that
    the negotiation has been successfully started. An event via eventNotification) will be sent once
	the granted parameters are available.

@param aParameterBundle bundle of parameters to be applied to the sub-connection
@return KErrNone if successful, otherwise a system wide error
*/
	{
	RParameterFamilyBundle pfb;
	TRAPD(ret,aParameterBundle.CopyToFamilyBundleL(pfb));
	if(ret==KErrNone)
		{
		ret=SetParameters(pfb);
		}
	pfb.Destroy();
	return ret;
	}

EXPORT_C TInt RSubConnection::GetParameters(RSubConParameterBundle& aParameterBundle)
/** Fetch the sub-connection's current parameters (Requested, Acceptable and Granted)

@param aParameterBundle bundle of parameters to write the current parameters into
@return KErrNone if successful, otherwise a system wide error
*/
	{
	RParameterFamilyBundle pfb;
	TInt ret = GetParameters(pfb);
	if(ret==KErrNone)
		{
		TRAP(ret,aParameterBundle.CopyFromFamilyBundleL(pfb));
		}
	pfb.Destroy();
	return ret;
	}

EXPORT_C TInt RSubConnection::SetParameters(const RParameterFamilyBundle& aParameterBundle)
/** Set Parameters of the sub-connection. Successful completion of this method only indicates that
    the negotiation has been successfully started. An event via eventNotification) will be sent once
	the granted parameters are available.

@param aParameterBundle bundle of parameters to be applied to the sub-connection
@return KErrNone if successful, otherwise a system wide error
*/
	{
	const_cast<RParameterFamilyBundle&>(aParameterBundle).ClearAllParameters(RParameterFamily::EGranted);

	HBufC8* buffer = HBufC8::New(aParameterBundle.Length());
	if(buffer == NULL)
		{
		return KErrNoMemory;
		}

	TPtr8 ptr = buffer->Des();
	TInt ret = aParameterBundle.Store(ptr);
	if (ret == KErrNone)
		{
		ret = SendReceive(ESCSetParameters, TIpcArgs(&ptr));
		}

	delete buffer;
	return ret;
	}

EXPORT_C TInt RSubConnection::GetParameters(RParameterFamilyBundle& aParameterBundle)
/** Fetch the sub-connection's current parameters (Requested, Acceptable and Granted)

@param aParameterBundle bundle of parameters to write the current parameters into
@return KErrNone if successful, otherwise a system wide error
*/
	{
	TInt lengthOrError = SendReceive(ESCGetParametersLength, TIpcArgs());
	if (lengthOrError < KErrNone)
		{
		return lengthOrError;
		}

	HBufC8* buffer = HBufC8::New(lengthOrError);
	if (buffer == NULL)
		{
		return KErrNoMemory;
		}

	TPtr8 ptr = buffer->Des();
	TInt ret = SendReceive(ESCGetParameters, TIpcArgs(&ptr));
	if (ret == KErrNone)
		{
		TRAP(ret,aParameterBundle.LoadL(ptr));
		}

	delete buffer;
	return ret;
	}

EXPORT_C void RSubConnection::EventNotification(TNotificationEventBuf& aEventBuffer, TBool aGenericEventsOnly, TRequestStatus& aStatus)
/** Requests asynchronous event notification for the sub-connection.

@param aEventBuffer A buffer to receive event notification.
@param aStatus On return, the status of the request.
*/
	{
	SendReceive(ESCEventAllNotifications, TIpcArgs(&aEventBuffer, aGenericEventsOnly), aStatus);	
	}

EXPORT_C void RSubConnection::EventNotification(TNotificationEventBuf& aEventBuffer, TEventFilter aEventFilterList[], TUint aEventListLength, TRequestStatus& aStatus)
/** Requests asynchronous notification of a specific list of events for the sub-connection.

@param aEventBuffer A buffer to receive event notification.
@param aEventFilterList list of event filters specifying a subset of events of interest.
@param aEventListLength Length of the filter list provided
@param aStatus On return, the status of the request.
*/
	{
	//Since we are creating the descriptor on the stack we need to ensure that buffer is around for when the reading
	//takes place. We therefore have to upload the buffer synchronously and only then can we ask for the asynchronous
	//notification
	TPtrC8 buffer((const unsigned char*)aEventFilterList, aEventListLength * sizeof(TEventFilter));
	TInt ret = SendReceive(ESCEventNotificationSetup, TIpcArgs(&buffer));
	if (ret == KErrNone)
		{
		SendReceive(ESCEventNotification, TIpcArgs(&aEventBuffer), aStatus);	
		}
	else
		{
		TRequestStatus* statPtr = &aStatus;
		User::RequestComplete(statPtr, ret);
		}	
	
	}

EXPORT_C void RSubConnection::CancelEventNotification()
/** Cancel an outstanding Event Notification request
*/
	{
	SendReceive(ESCEventNotificationCancel);
	}

EXPORT_C TInt RSubConnection::Control(TUint aOptionLevel, TUint aOptionName, TDes8& aOption)
	{
	return SendReceive(ESCControl, TIpcArgs(aOptionLevel, aOptionName, &aOption));
	}

TBool RSubConnection::SameSession(TInt aSessionHandle)
/** Checks that the Session Handle passed is the same as that of this RSubConnection.

Use to verify the RSubConnection argument passed to the RSocket and RHostResolver
Open(..., RSubConnection& aSubConnection) methods.

@param aSessionHandle The handle which is to be checked against that of this RSubConnection
@returns ETrue if handle is the same, else EFalse
@internalComponent
*/
	{
	return (Session().Handle() == aSessionHandle);
	}

