// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <dial.h>

/** 
@file
@publishedAll
@deprecated 9.1
*/

/**
Creates an empty TChargeCard.
@deprecated 9.1
*/
EXPORT_C TChargeCard::TChargeCard()
	{}

/**
Creates a TChargeCard containing the parameters.

@param aNumber This string represents the charge card account number.
@param aPin This string represents the charge card PIN number.
@param aLocalRule This is a string representing the rule for dialling 
       a local number. Each phone operator has its own rules for 
       dialling local, national and international numbers.
@param aNatRule As for aLocalRule but for a national call.
@param aIntlRule As for aLocalRule but for an international call.

@deprecated 9.1
*/
EXPORT_C TChargeCard::TChargeCard(	
	const TChargeCardAccount& aNumber,
	const TChargeCardPin&     aPin,
	const TChargeCardRule&    aLocalRule,
	const TChargeCardRule&    aNatRule,
	const TChargeCardRule&    aIntlRule)
	:
	iNumber(aNumber),
	iPin(aPin),
	iLocalRule(aLocalRule),
	iNatRule(aNatRule),
	iIntlRule(aIntlRule)
	{}


/**
Set the string that represents the charge card account number.
@deprecated 9.1
*/
EXPORT_C void TChargeCard::SetNumber(const TDesC& aNumber) { iNumber.Copy(aNumber);}
/**
Set the string that represents the charge card PIN number.
@deprecated 9.1
*/
EXPORT_C void TChargeCard::SetPin(const TDesC& aPin){iPin.Copy(aPin);}
/**
Set the string that represents the rule for dialling a local number. 
Each phone operator has its own rules for dialling local, 
national and international numbers.
@deprecated 9.1
*/
EXPORT_C void TChargeCard::SetLocalRule (const TDesC& aLocalRule){iLocalRule.Copy(aLocalRule);}
/**
Set the string that represents the rule for dialling a national number. 
Each phone operator has its own rules for dialling local, 
national and international numbers.
@deprecated 9.1
*/
EXPORT_C void TChargeCard::SetNatRule(const TDesC& aNatRule){iNatRule.Copy(aNatRule);}
/**
Set the string that represents the rule for dialling an international number. 
Each phone operator has its own rules for dialling local, 
national and international numbers.
@deprecated 9.1
*/
EXPORT_C void TChargeCard::SetIntlRule(const TDesC& aIntlRule){iIntlRule.Copy(aIntlRule);}

/**
True if the charge card number, PIN number, and 
the dialling rules (local, national and international) have been set.
@deprecated 9.1
*/
EXPORT_C TBool TChargeCard::ValidateChargeCardInfo () const
	{
	return ( iNumber.Length() &&
			 iPin.Length() &&
			 iLocalRule.Length() &&
			 iNatRule.Length() &&
			 iIntlRule.Length() ) ;
	}

/**
Returns the string that represents the charge card account number.
@deprecated 9.1
*/
EXPORT_C TPtrC TChargeCard::Number() const {return TPtrC(iNumber);}
/**
Returns the string that represents the charge card PIN number.
@deprecated 9.1
*/
EXPORT_C TPtrC TChargeCard::Pin() const {return TPtrC(iPin);}
/**
Returns the string that represents the rule for dialling a local number. 
Each phone operator has its own rules for dialling local,
national and international numbers.
@deprecated 9.1
*/
EXPORT_C TPtrC TChargeCard::LocalRule () const {return TPtrC(iLocalRule);}
/**
Returns the string that represents the rule for dialling a national number. 
Each phone operator has its own rules for dialling local, 
national and international numbers.
@deprecated 9.1
*/
EXPORT_C TPtrC TChargeCard::NatRule() const {return TPtrC(iNatRule);}
/**
Returns the string that represents the rule for dialling an 
international number. Each phone operator has its own rules 
for dialling local, national and international numbers.
@deprecated 9.1
*/
EXPORT_C TPtrC TChargeCard::IntlRule() const {return TPtrC(iIntlRule);}
