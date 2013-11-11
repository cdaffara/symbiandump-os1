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
 *  @internalComponent
 *  Defines  CEmsUserprompt class
 */

#ifndef __EMSUserPromptIE_H__
#define __EMSUserPromptIE_H__


#include <emsinformationelement.h>

class CEmsUserPrompt : public CEmsInformationElement
/**
 *  CEmsUserPrompt wraps the creation & modification of the user prompt
 *  information elements into a single class.
 *  
 *  Format of EMS Object
 *  ------------------------------------------
 *  
 *  Position (Octet)		Bit		Description
 *  1							Object count
 *  
 *  @see 3gpp Spec V5 section 9.2.3.24.10.1.10
 *  @internalComponent
 */
	{
public:
	enum {EIESize=1};

public:
	IMPORT_C static CEmsUserPrompt* NewL(TUint aObjectCount=1);
	IMPORT_C CEmsInformationElement* DuplicateL() const;
	IMPORT_C void CopyL(const CEmsUserPrompt& aSrc);
	IMPORT_C TUint ObjectCount() const;
	IMPORT_C void SetObjectCount(TUint aObjectCount);

private:
	virtual void EncodeBodyL(TPtr8 aPtr, TBool aIsForSerialisation) const;
	virtual void DecodeBodyL(const TPtrC8 aPtr, TBool aIsFromSerialisation);
	CEmsUserPrompt(TUint aObjectCount = 0);
	TUint iObjectCount;
	};

#endif // __EMSUserPromptIE_H__
