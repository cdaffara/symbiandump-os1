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

/**
 @file
 @publishedAll
 @deprecated 9.1
*/

#include "DIALUTIL.H"

/**
@file
@internalComponent
*/
const TInt KCharacterLength=1;


// TelephoneNumber General Parser

/**
@deprecated 9.1
*/
TInt DialUtil::GeneralParse(
			 TDialString& aDialString,
			 const TDialLocation & aDialLocation , 
			 const TChargeCard& aChargeCard ,
			 const TDesC& aNumberToDial,
			 TParseMode aParseMode,
 			 TCityDialOptions aOption )
	{
	
	TPhoneNumber aPhoneNumToParse(aNumberToDial) ;

//	if ( !DialUtil::PhoneFormatOK (aPhoneNumToParse) )
//	return (KErrNotSupported) ;

	// analysing dial string
	TDialNumber dialNumber (aPhoneNumToParse,aDialLocation,aChargeCard) ;
   
	aDialString.Zero(); // restart the dial string 
	if (aDialLocation.IsDisableCallWaitingCode() )
		{
		if (!DialUtil::AppendDialString(aDialString,aDialLocation.DisableCallWaitingCode()))
			return (KErrOverflow) ;
		}
	if (aDialLocation.IsWaitForDialingTone() && aParseMode==EForDialing)
		{
		if (!DialUtil::AppendDialString(aDialString,KCharW))
			return (KErrOverflow) ;
		}
	if ( aDialLocation.IsDialOutRequired() ) 
		{
		if (aParseMode==EForDialing)
			{
			if (aDialLocation.IsPbxUsePulseDial())
				{
				if (!DialUtil::AppendDialString(aDialString,KCharP))
					return (KErrOverflow) ;
				}
			else
				{
				if (!DialUtil::AppendDialString(aDialString,KCharT))
					return (KErrOverflow) ;
				}
			}

		if (dialNumber.CallType() == ELocalCall)
			{
			if (!DialUtil::AppendDialString(aDialString,aDialLocation.DialOutLocalCode()))
				return (KErrOverflow) ;
			}
		else
			{
			if (!DialUtil::AppendDialString(aDialString,aDialLocation.DialOutLongDistanceCode()))
				return (KErrOverflow) ;
			}

		TUint commas = aDialLocation.PauseAfterDialout() ;
		if (commas > 0)		// we should add a comma if the last character in the string is not already a comma
			{
			TBuf<KCharacterLength> lastChar=aDialString.Right(KCharacterLength);
			if (lastChar.Locate(KCharComma)!=KErrNone)		// if not a comma
				{
				if (!DialUtil::AppendDialString(aDialString,KCharComma))
					return (KErrOverflow) ;
				}	
			}

	/*	while (commas--)
			{
			if (!DialUtil::AppendDialString(aDialString,KCharComma))
				return (KErrOverflow) ;
			}
	
	  */}

	if (aDialLocation.IsWaitForProceedTone() && aParseMode==EForDialing)
		{
		if (!DialUtil::AppendDialString(aDialString,KCharW))
			return (KErrOverflow) ;
		}

	if ( !aDialLocation.IsDialOutRequired() || 
		 aDialLocation.IsUsePulseDial () != aDialLocation.IsPbxUsePulseDial() )
		{
		if (aParseMode==EForDialing)
			{
			if (!aDialLocation.IsUsePulseDial ())
				{
				if (!DialUtil::AppendDialString(aDialString,KCharT))
					return (KErrOverflow) ;
				}
			else
				{
				if (!DialUtil::AppendDialString(aDialString,KCharP))
					return (KErrOverflow) ;
				}
			}
		}
	
	if ( aDialLocation.IsMobile() && aChargeCard.ValidateChargeCardInfo() )
		return (KErrNotSupported) ;
		// do not know what to do with this scenario-currently not available !
    
	else if ( aDialLocation.IsMobile() && !aChargeCard.ValidateChargeCardInfo() )
	{
		aDialString.Zero(); // restart the dial string 
        if (aDialLocation.IsReturnPhoneNumberWithPlusFormat()) // fix for release 012
        {
  			if (!DialUtil::AppendDialString(aDialString,KCharPlus))
				return (KErrOverflow) ;
			if (!DialUtil::AppendDialString(aDialString,dialNumber.NatCode()))
				return (KErrOverflow) ;
			if (!DialUtil::AppendDialString(aDialString,dialNumber.AreaCode()))
				return (KErrOverflow) ;
        }
        else
        {
		if ( dialNumber.IsNatCode() )  // Nat code supplied
			{
			if ( dialNumber.NatCode() != aDialLocation.NatCode() )
				{// making interNat code 
				if (!DialUtil::AppendDialString(aDialString,dialNumber.IntlPrefCode()))
					return (KErrOverflow) ;

				if (!DialUtil::AppendDialString(aDialString,dialNumber.NatCode()))
					return (KErrOverflow) ;
				}
			else // call within the same country
				{
				if (!DialUtil::AppendDialString(aDialString,dialNumber.NatPrefCode()))
					return (KErrOverflow) ;
				}
			}
		else // must dial from the same country
			{
			if (!DialUtil::AppendDialString(aDialString,aDialLocation.NatPrefCode()))
				return (KErrOverflow) ;
			}

		if ( dialNumber.IsAreaCode() )  // area code supplied
		{
			if (!DialUtil::AppendDialString(aDialString,dialNumber.AreaCode()))
				return (KErrOverflow) ;
		}
		else
		{
			if (!DialUtil::AppendDialString(aDialString,aDialLocation.AreaCode()))
				return (KErrOverflow) ;
		}
     }
		if (!DialUtil::AppendDialString(aDialString,dialNumber.PhoneNumber()) )
			return (KErrOverflow) ;
		
	}
	
	//----------------------------------------------------
	// Not a mobile phone and not using chargecard
	//----------------------------------------------------
	else if ( !aDialLocation.IsMobile() && !aChargeCard.ValidateChargeCardInfo() ) 
	{
        if (aDialLocation.IsReturnPhoneNumberWithPlusFormat()) // fix for release 012
        {
  			 if (!DialUtil::AppendDialString(aDialString,KCharPlus))
				return (KErrOverflow) ;
			if (!DialUtil::AppendDialString(aDialString,dialNumber.NatCode()))
				return (KErrOverflow) ;
			if (!DialUtil::AppendDialString(aDialString,dialNumber.AreaCode()))
				return (KErrOverflow) ;
    		if (!DialUtil::AppendDialString(aDialString,dialNumber.PhoneNumber()) )
	    		return (KErrOverflow) ;
        }
        else
        {

        if (dialNumber.CallType() == ELocalCall)
			{
			// option to allow areacode 
			if ( aOption==EDialCityAllowDuplicateAreaCode )
				{
				if (!DialUtil::AppendDialString(aDialString,dialNumber.NatPrefCode()))
					return (KErrOverflow) ;
				if (!DialUtil::AppendDialString(aDialString,dialNumber.AreaCode()))
					return (KErrOverflow) ;
				}
			if (!DialUtil::AppendDialString(aDialString,dialNumber.PhoneNumber()))
				return (KErrOverflow) ;
			}
        if (dialNumber.CallType() == ELongDistanceCall) 
			{
			if (!DialUtil::AppendDialString(aDialString,dialNumber.NatPrefCode()))
				return (KErrOverflow) ;

			if (!DialUtil::AppendDialString(aDialString,dialNumber.AreaCode()))
				return (KErrOverflow) ;

			if (!DialUtil::AppendDialString(aDialString,dialNumber.PhoneNumber()))
				return (KErrOverflow) ;
			}
        if (dialNumber.CallType() == EInternationalCall) 
			{
			if (!DialUtil::AppendDialString(aDialString,dialNumber.IntlPrefCode()))
				return (KErrOverflow) ;
			if (!DialUtil::AppendDialString(aDialString,dialNumber.NatCode()))
				return (KErrOverflow) ;
			if (!DialUtil::AppendDialString(aDialString,dialNumber.AreaCode()))
				return (KErrOverflow) ;
			if (!DialUtil::AppendDialString(aDialString,dialNumber.PhoneNumber()))
				return (KErrOverflow) ;
			}
        }
	}
	else if ( !aDialLocation.IsMobile() && aChargeCard.ValidateChargeCardInfo() ) 
		{
		TChargeCardRule iRule=dialNumber.RuleToUse() ;

		for ( TInt i=0 ; i < iRule.Length() ; i++) 
			{ // for each character in rule 
			switch (iRule[i] ) 
				{
			case KCharEUpper: // copy country code into aDialStr 
			case KCharELower:
			if (!DialUtil::AppendDialString(aDialString,dialNumber.NatCode()))
				return (KErrOverflow) ;
			break;
			case KCharFUpper: // copy STD code into aDialStr 
			case KCharFLower:
			if (!DialUtil::AppendDialString(aDialString,dialNumber.AreaCode()))
				return (KErrOverflow) ;
            break;
			case KCharGUpper: // copy local phone number into aDialStr 
			case KCharGLower:
			if (!DialUtil::AppendDialString(aDialString,dialNumber.PhoneNumber()))
				return (KErrOverflow) ;
            break;

			case KCharHUpper: // copy chargecard account number & pin into aDialStr 
			case KCharHLower:
			if (!DialUtil::AppendDialString(aDialString,aChargeCard.Number()))
				return (KErrOverflow) ;
			if (!DialUtil::AppendDialString(aDialString,aChargeCard.Pin()))
				return (KErrOverflow) ;
			break;
			case KCharJUpper: // copy chargecard account number into aDialStr 
			case KCharJLower:
			if (!DialUtil::AppendDialString(aDialString,aChargeCard.Number()))
				return (KErrOverflow) ;
            break;
        
			case KCharKUpper: // copy pin into aDialStr 
			case KCharKLower:
			if (!DialUtil::AppendDialString(aDialString,aChargeCard.Pin()))
				return (KErrOverflow) ;
            break;
		
			default:  // copy character into aDialStr 
			if (!DialUtil::AppendDialString(aDialString,iRule[i]))
				return (KErrOverflow) ;
            break;
				}	
			}
		}
	DialUtil::RemoveSpace (aDialString) ;

	// this is for dtmf dialling string so the string should not have 
	// any undialable characters
	if (aParseMode==EForDisplay) 
		DialUtil::RemoveUndiallableChars (aDialString) ;
	
	
	return(KErrNone) ;
	}


/**
This parser resolves the phone number for Time/World application.

@param aDialString The results of the telephone parser are stored here.
@param aFrom Specifies the area code of the dialling location.
@param aTo Specifies the area code of the city being dialled.
@param aOption This option applies when the phone number being 
       dialled is in the same location as the dialler: 
       It controls whether the area code is included in the final dial string. 
@param aCodeSeparator Specifies the code separator.
@param aPlusFormatOption Specifies whether the dial string is in full 
       or in the ‘+’ char format.

@publishedAll
@deprecated 9.1
*/
EXPORT_C TInt TelephoneNumber::CalculateDialCityPrefix(
						TDialString& aDialString,
						const TCityDialCodes& aFrom,
						const TCityDialCodes& aTo,
						TCityDialOptions aOption,
						TChar aCodeSeparator ,
						TPlusFormatDialOptions aPlusFormatOption )
	{
	aDialString.Zero(); // restart the dial string 
	if (aFrom.iNatCode != aTo.iNatCode) // different country code
		{
		if (aPlusFormatOption==EPlusFormatDialNotAllow)
			{
			if (!DialUtil::AppendDialString(aDialString,aFrom.iIntlPref))
				return (KErrOverflow) ;
			if (!DialUtil::AppendDialString(aDialString,aCodeSeparator))
				return (KErrOverflow) ;
			} 
		else
			{
			if (!DialUtil::AppendDialString(aDialString,KCharPlus))
				return (KErrOverflow) ;
			}
		if (!DialUtil::AppendDialString(aDialString,aTo.iNatCode))
			return (KErrOverflow) ;
		if (!DialUtil::AppendDialString(aDialString,aCodeSeparator))
			return (KErrOverflow) ;
		if (!DialUtil::AppendDialString(aDialString,aTo.iAreaCode))
			return (KErrOverflow) ;
		return(KErrNone) ;
		}
	else if (aFrom.iAreaCode != aTo.iAreaCode) // same country code different area
		{
		if (!DialUtil::AppendDialString(aDialString,aFrom.iNatPref))
			return (KErrOverflow) ;
		if (!DialUtil::AppendDialString(aDialString,aCodeSeparator))
			return (KErrOverflow) ;
		if (!DialUtil::AppendDialString(aDialString,aTo.iAreaCode))
			return (KErrOverflow) ;
		return(KErrNone) ;
		}
	else if (	(aFrom.iIntlPref == aTo.iIntlPref) && 
				(aFrom.iNatPref  == aTo.iNatPref) )
				// must check they are exaclty the same for local call
		{
		if (aOption == EDialCityAllowDuplicateAreaCode)
			{
			if (!DialUtil::AppendDialString(aDialString,aFrom.iNatPref))
				return (KErrOverflow) ;
			if (!DialUtil::AppendDialString(aDialString,aCodeSeparator))
				return (KErrOverflow) ;
			if (!DialUtil::AppendDialString(aDialString,aTo.iAreaCode))
				return (KErrOverflow) ;
			return(KErrNone) ;
			}
		else
			return(KErrNone) ;
		}
	else // in valid info in the structure
		return (KErrOverflow) ;
	}


//
// for DTMF dialing
//

/**
This parser resolves a phone number.  
It assumes that TCityDialOption is 
always set to EDialCityStripDuplicateAreaCode

@param aDialString The results of the telephone parser are stored here.
@param aDialLocation Specifies the dialling location.
@param aPhoneNumber Telephone number is to be parsed in the form of +44 20...

@publishedAll
@deprecated 9.1
*/
EXPORT_C TInt TelephoneNumber::Parse( TDialString& aDialString ,
								  const TDialLocation& aDialLocation, 
								  const TDesC& aPhoneNumber ) 
	{
	TChargeCard chargeCard ; // a NULL charge card not used by DTMF
	return (TelephoneNumber::Parse(
							aDialString,
							aDialLocation,
							chargeCard,
							aPhoneNumber,
							EForDisplay	) ); 
	}


// It is for DTMF dialing with option to strip or allow the area code.

/**
This parser resolves a phone number.

@param aDialString The results of the telephone parser are stored here.
@param aDialLocation Specifies the dialling location.
@param aPhoneNumber Telephone number is to be parsed in the form of +44 20 ...
@param aOption This option applies when the phone number being dialled 
       is in the same location as the dialler: It controls whether the 
       area code is included in the final dial string. 

@publishedAll
@deprecated 9.1
*/
EXPORT_C TInt TelephoneNumber::Parse( TDialString& aDialString ,
								  const TDialLocation& aDialLocation, 
								  const TDesC& aPhoneNumber , 
								  TCityDialOptions aOption )
	{
	TChargeCard chargeCard ; // a NULL charge card not used by DTMF
	return DialUtil::GeneralParse(
							aDialString,
							aDialLocation,
							chargeCard,
							aPhoneNumber,
							EForDisplay	,
							aOption ); 
	}


// Parse for dial up network

/**
This parser resolves a phone number with additional of a TChargeCard.

@param aDialString The results of the telephone parser are stored here.
@param aDialLocation Specifies the dialling location.
@param aChargeCard Specifies charge card information. 
@param aNumberToDial A telephone number is to be parsed in the form of +44 20 ...
@param aParseMode Indicates whether the resulting string 
       is for display or for dialling.

@publishedAll
@deprecated 9.1
*/
EXPORT_C TInt TelephoneNumber::Parse(
			 TDialString& aDialString,
			 const TDialLocation & aDialLocation , 
			 const TChargeCard& aChargeCard ,
			 const TDesC& aNumberToDial,
			 TParseMode aParseMode )
	{
	return DialUtil::GeneralParse (	aDialString,
							aDialLocation , 
							aChargeCard ,
							aNumberToDial,
							aParseMode,
							EDialCityStripDuplicateAreaCode ) ;
	}
