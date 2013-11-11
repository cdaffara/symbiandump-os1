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
* Implements the Enhanced SMS Object Distribution Information Element.
*
*/


/**
 *  @file
 *  @internalComponent
 *  Defines  CEmsObjectDistributionIE  class
 */

#ifndef __EMSObjectDistributionIE_H__
#define __EMSObjectDistributionIE_H__

#include <emspictureie.h>
#include <gsmuelem.h>


class CEmsObjectDistribution : public CEmsInformationElement
/**
 *  CEmsObjectDistribution wraps the creation & modification of the object distribution
 *  information element into a single class.
 *  
 *  Format of Extended Object.
 *  ----------------------------
 *  
 *  Position (Octet)		Bit		Description
 *  1							Number of information Elements
 *  2							Distribution Attr
 *  2					0		NoForwarding Attr (1=NoForward)
 *  2					1..7	Reserved
 *  
 *  @see 3gpp Spec V5 section 9.2.3.24.10.1.14
 *  @internalComponent
 */
	{
public:
	enum {EIESize=2};
	enum TAttrs {EForward=0x0, ENoForward=0x01};

public:
	IMPORT_C static CEmsObjectDistribution* NewL(TUint aObjectCount=1, TAttrs aAttr = EForward);
	IMPORT_C CEmsInformationElement* DuplicateL() const;
	IMPORT_C void CopyL(const CEmsObjectDistribution& aSrc);
	IMPORT_C TUint ObjectCount() const;
	IMPORT_C void SetObjectCount(TUint aObjectCount);
	IMPORT_C TAttrs Forwarding() const;
	IMPORT_C void SetForwarding(TAttrs aAttr);

private:
	virtual void EncodeBodyL(TPtr8 aPtr, TBool aIsForSerialisation) const;
	virtual void DecodeBodyL(const TPtrC8 aPtr, TBool aIsFromSerialisation);
	CEmsObjectDistribution(TUint aObjectCount = 0, TAttrs aAttr = EForward);

private:
	TUint iObjectCount;
	TAttrs iAttr;
	};

#endif // __EMSObjectDistributionIE_H__
