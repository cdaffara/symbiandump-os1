/*
* Copyright (c) 2009 Sony Ericsson Mobile Communications AB
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Sony Ericsson Mobile Communications AB - initial contribution.
* Nokia Corporation - additional changes.
* 
* Contributors:
* 
* Description:
* Implements the Enhanced SMS User Prompt Information Element.
*
*/


/**
 *  @file
 *  
 *  Implements  CEmsObjectDistributionIE class
 */

#include <emsobjectdistributionie.h>

EXPORT_C CEmsObjectDistribution* CEmsObjectDistribution::NewL(TUint aObjectCount, TAttrs aAttr)
/**
 *  @capability None
 */
	{
	CEmsObjectDistribution* self = new (ELeave) CEmsObjectDistribution(aObjectCount, aAttr);
	return self;
	}

EXPORT_C CEmsInformationElement* CEmsObjectDistribution::DuplicateL() const
/**
 *  @capability None
 */
	{
	CEmsObjectDistribution* copy = new (ELeave) CEmsObjectDistribution();
	CleanupStack::PushL(copy);
	copy->CopyL(*this);
	CleanupStack::Pop(copy);
	return copy;
	}

EXPORT_C void CEmsObjectDistribution::CopyL(const CEmsObjectDistribution& aSrc)
/**
 *  @capability None
 */
	{
	CEmsInformationElement::CopyL(aSrc);
	iObjectCount = aSrc.iObjectCount;
	iAttr = aSrc.iAttr;
	}

EXPORT_C TUint CEmsObjectDistribution::ObjectCount() const
/**
 *  @capability None
 */
	{
	return iObjectCount;
	}

EXPORT_C void CEmsObjectDistribution::SetObjectCount(TUint aObjectCount)
/**
 *  @capability None
 */
	{
	iObjectCount=aObjectCount;
	}

EXPORT_C CEmsObjectDistribution::TAttrs CEmsObjectDistribution::Forwarding() const
/**
 *  @capability None
 */
	{
	return iAttr;
	}

EXPORT_C void CEmsObjectDistribution::SetForwarding(TAttrs aAttr)
/**
 *  @capability None
 */
	{
	iAttr = aAttr;
	}

CEmsObjectDistribution::CEmsObjectDistribution(TUint aObjectCount, TAttrs aAttr)
	// pass in a false parameter for start position encoded
    : CEmsInformationElement(CSmsInformationElement::ESmsEnhancedODI, EFalse), iObjectCount(aObjectCount), iAttr(aAttr)
	{
	iEncodedBodyLength=EIESize;
	}

/**
 *  Encodes the information element into its raw format. (no IE id)
 *  
 *  @param aPtr the buffer to be used which is to contain the data
 *  @param TBool boolean to indicate if it is for serialisation or encoding
 */
void CEmsObjectDistribution::EncodeBodyL(TPtr8 aPtr, TBool) const
	{
	aPtr.Append(static_cast<TUint8>(iObjectCount));
	aPtr.Append(iAttr);
	}

/**
 *  Decodes the raw data out of an information element into this class.
 *  
 *  @param aPtr The raw predefined animation data
 *  @param TBool boolean to indicate if it is from serialisation
 *  @leave KErrArgument If the size of the data does not match what is expected.
 */
void CEmsObjectDistribution::DecodeBodyL(const TPtrC8 aPtr, TBool)
	{
	__ASSERT_ALWAYS(aPtr.Length() == EIESize, User::Leave(KErrArgument));
	iObjectCount = static_cast<TUint8>(aPtr[0]);
	iAttr = static_cast<TAttrs>(aPtr[1]);
	}
