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
 *  Implements  CEMSUserPromptIE class
 */

#include <emsuserpromptie.h>

EXPORT_C CEmsUserPrompt* CEmsUserPrompt::NewL(TUint aObjectCount)
/**
 *  @capability None
 */
	{
	CEmsUserPrompt* self = new (ELeave) CEmsUserPrompt(aObjectCount);
	return self;
	}

EXPORT_C CEmsInformationElement* CEmsUserPrompt::DuplicateL() const
/**
 *  @capability None
 */
	{
	CEmsUserPrompt* copy = new (ELeave) CEmsUserPrompt();
	CleanupStack::PushL(copy);
	copy->CopyL(*this);
	CleanupStack::Pop(copy);
	return copy;
	}

EXPORT_C void CEmsUserPrompt::CopyL(const CEmsUserPrompt& aSrc)
/**
 *  @capability None
 */
	{
	CEmsInformationElement::CopyL(aSrc);
	iObjectCount = aSrc.iObjectCount;
	}

EXPORT_C TUint CEmsUserPrompt::ObjectCount() const
/**
 *  @capability None
 */
	{
	return iObjectCount;
	}
EXPORT_C void CEmsUserPrompt::SetObjectCount(TUint aObjectCount)
/**
 *  @capability None
 */
	{
	iObjectCount=aObjectCount;
	}


CEmsUserPrompt::CEmsUserPrompt(TUint aObjectCount)
	// pass in a false parameter for start position encoded
	: CEmsInformationElement(CSmsInformationElement::ESmsEnhancedUserPromptIndicator, EFalse), iObjectCount(aObjectCount)
	{
	iEncodedBodyLength=1;
	}

/**
 *  Encodes the information element into its raw format. (no IE id)
 *  
 *  @param aPtr the buffer to be used which is to contain the data
 *  @param TBool boolean to indicate if it is for serialisation or encoding
 */
void CEmsUserPrompt::EncodeBodyL(TPtr8 aPtr, TBool) const
	{
	aPtr.Append(static_cast<TUint8>(iObjectCount));
	}

/**
 *  Decodes the raw data out of an information element into this class.
 *  
 *  @param aPtr The raw predefined animation data
 *  @param TBool boolean to indicate if it is from serialisation
 *  @leave KErrArgument If the size of the data does not match what is expected.
 */
void CEmsUserPrompt::DecodeBodyL(const TPtrC8 aPtr, TBool)
	{
	__ASSERT_ALWAYS(aPtr.Length() == 1, User::Leave(KErrArgument));
	iObjectCount = static_cast<TUint8>(aPtr[0]);
	}
