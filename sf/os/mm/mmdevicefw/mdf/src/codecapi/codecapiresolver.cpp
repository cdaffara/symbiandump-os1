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

#include <ecom/ecom.h>
#include <ecom/ecomerrorcodes.h>
#include <ecom/ecomresolverparams.h>
#include <ecom/implementationinformation.h>
#include <ecom/publicregistry.h>
#include <mdf/codecapiresolverdata.h>
#include <mdf/codecapiresolverutils.h>
#include <mdf/codecapivideoresolverutils.h>
#include <mdf/codecapiuids.hrh>
#include "codecapiresolver.h"

/**
Constructs a CCodecApiResolver object.
@param  aRegistry
		A reference to the instantiated registry information 
@return An instance of the class or NULL.
*/
CCodecApiResolver* CCodecApiResolver::NewL (MPublicRegistry& aRegistry)
	{
	return new(ELeave) CCodecApiResolver (aRegistry);
	}


/**
Class destructor.
*/
CCodecApiResolver::~CCodecApiResolver()
	{
	}

/**
Class constructor.
*/
CCodecApiResolver::CCodecApiResolver (MPublicRegistry& aRegistry) :
	CResolver (aRegistry)
	{
	}

/**
Requests the resolver to identify the most appropriate interface implementation.
@param  aInterfaceUid
		The interface for which an implementation is requested.
@param  aAdditionalParameters
	    The parameters which must match for an implementation to be suitable. 
@return The unique id of the implementation which satisfies the specified parameters. 
 		KNullUid if not implementation is found.
*/ 
TUid CCodecApiResolver::IdentifyImplementationL(TUid /*aInterfaceUid*/,
	const TEComResolverParams& /*aAdditionalParameters*/) const
	{
	return KNullUid;
	}


/**
Lists all the implementations which satisfy the specified interface definition and the 
resolve parameters supplied.
@param  aInterfaceUid
		The interface for which an implementation is requested.
@param  aAdditionalParameters	  	
		The parameters which must match for an implementation to be suitable.
@return Pointer to an array of suitable implementations. Ownership of this array is passed
	    to the calling function. 
*/
RImplInfoArray* CCodecApiResolver::ListAllL(TUid aInterfaceUid,
	const TEComResolverParams& aAdditionalParameters) const
	{
	RImplInfoArray* retList = new (ELeave) RImplInfoArray;
	// coverity[double_push]
	CleanupStack::PushL(retList);
	CleanupClosePushL(*retList);
	
	CCodecApiResolverData* customMatch = CCodecApiResolverData::NewLC(aAdditionalParameters.DataType());
	
	TCodecApiResolverMatchType matchType = customMatch->MatchType();
	TUid implementationTypeUid 			 = customMatch->ImplementationType();
	
	// create a list with all 
	RImplInfoArray& fullList = iRegistry.ListImplementationsL(aInterfaceUid);
	const TInt count = fullList.Count();
	
	for(TInt index = 0; index < count; index++)
		{
		TBool matchFound = EFalse;
		const CImplementationInformation& impData = *(fullList[index]);
		CCodecApiOpaqueData* parse = NULL;
		const TDesC8* opaqueData = &impData.OpaqueData();
		// if opaque data is empty, move to the next element in the list
		if (opaqueData->Length() == 0)
			{
			continue;
			}
		// create an instance of the class which holds the opaque data	
		TInt error = KErrNotFound;
		
		if (implementationTypeUid == TUid::Uid(KUidAudioCodec)) 
			{
			TRAP(error, parse = CCodecApiOpaqueData::NewL(impData.OpaqueData()));	
			}
		else
			{
			if ((implementationTypeUid == TUid::Uid(KUidVideoEncoder)) 
				|| implementationTypeUid == TUid::Uid(KUidVideoDecoder)) 
				{
				TRAP(error, parse = CCodecApiVideoOpaqueData::NewL(impData.OpaqueData()));
				}	
			else
				{
				error = KErrNotSupported;	
				}
			} 
			
		CleanupStack::PushL(parse);	
		
		if (error)
			{
			if ((error == KErrNotSupported) || (error == KErrCorrupt))
				{
				// means that the resource entry was not valid
				// we ignore this plugin and try the next one
				CleanupStack::PopAndDestroy(parse);
				continue;
				}
			else
				{
				User::Leave(error);
				}				
			} //end of 	if (error!=KErrNone)
			
		// check if the uids are different. If they are, move to the next element
		// in the list		
		if (implementationTypeUid != parse->TypeUid())
			{
			CleanupStack::PopAndDestroy(parse);
			continue;
			}
		switch(matchType)
			{	
			// check that the input data type is what we are looking for
			case EMatchInputDataFormat:
				if (parse->CompareInputDataType(customMatch->InputDataFormat()))
					{
					matchFound = ETrue;
					}
				break;
			// check that the output data type is what we are looking for	
			case EMatchOutputDataFormat:
				if (parse->CompareOutputDataType(customMatch->OutputDataFormat()))
					{
					matchFound = ETrue;
					}
				break;
			// check that the input and output data type is what we are looking for	
			case EMatchInputAndOutputDataFormat:
				if (parse->CompareInputDataType(customMatch->InputDataFormat()) 
					&& parse->CompareOutputDataType(customMatch->OutputDataFormat()))
					{
					matchFound = ETrue;
					}
				break;
			default:
				{
				//matchFound was initialised to EFalse, so fall through				
				}				
			}
		// if a match was found, it will be appended to the list returned by this method
		if (matchFound)
			{
			User::LeaveIfError(retList->Append(fullList[index]));	
			}
		CleanupStack::PopAndDestroy(parse);
		} // end of for
	
	CleanupStack::PopAndDestroy(customMatch);
	CleanupStack::Pop(2, retList);
	return retList;	
	}

 
