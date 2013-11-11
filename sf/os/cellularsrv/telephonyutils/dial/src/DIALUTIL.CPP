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
 @internalComponent
 @deprecated 9.1
*/

#include "DIALUTIL.H"

/**
@deprecated 9.1
*/
TInt TDialNumber::String (const TDes& aPhoneNumber, TInt aPos) const
	{
	while (aPhoneNumber[aPos] == KCharSpace  &&  aPos < aPhoneNumber.Length()-1) // already on a space then move to a non-space
		{
		aPos++;
		}

	if (aPos > aPhoneNumber.Length())
		{
		return (KErrNotFound);
		}

	while (aPhoneNumber[aPos] != KCharSpace  &&  aPos < aPhoneNumber.Length()-1)
		{
		aPos++;
		}

	if (aPos >= aPhoneNumber.Length()-1)
		{
		return (KErrNotFound);
		}

	return (aPos);
	}

/**
@deprecated 9.1
*/
void TDialNumber::SpaceParser(const TDes& aPhoneNumber)
	{
	TInt i = aPhoneNumber.Locate(KCharPlus) ;
	TInt j = aPhoneNumber.Locate(KCharSpace) ;
	if (j!=-1) // there space(s)
		j = String ( aPhoneNumber , 0) ;
	else
		j = -1 ;

	if ( i==-1 || j==-1 ) 
	{
		iNatCode = _L("") ;
		iAreaCode = _L("");
		iPhoneNumber = aPhoneNumber.Right(aPhoneNumber.Length()-1 ); // from KCharPlus onward
		return ;
	}
	else if (j>i)
		{
		if ( j-2*i-2 >= KMaxNatCode )
			{
			iNatCode = _L("") ;
			iAreaCode = _L("");
			iPhoneNumber = aPhoneNumber.Right(aPhoneNumber.Length()-1 ); // from KCharPlus onward
			return ;
			}
		else
			iNatCode = aPhoneNumber.Mid(i+1,j-i-1);
		}
	i=j;
	j = String ( aPhoneNumber , i) ;

	if ( i==-1 || j==-1 )
		iAreaCode = _L("");
	else if (j>i)
		{
		if ( j-2*i-2 >= KMaxAreaCode )
			{
			iNatCode = _L("") ;
			iAreaCode = _L("");
			iPhoneNumber = aPhoneNumber.Right(aPhoneNumber.Length()-1 ); // from KCharPlus onward
			return ;
			}
		else
			{
			iAreaCode = aPhoneNumber.Mid(i+1,j-i-1);
			DialUtil::RemoveSpace (iAreaCode) ;
			}
		}

	if (!iNatCode.Length() &&
		!iAreaCode.Length() )
			iPhoneNumber = aPhoneNumber ;
	else if ( !iAreaCode.Length() ) // no area code supplied take the remain from NatCode onward as telephone number
		iPhoneNumber = aPhoneNumber.Mid(i+1,aPhoneNumber.Length()-i-1);
	else
		{
		iPhoneNumber = aPhoneNumber.Mid(j+1,aPhoneNumber.Length()-j-1);
		}
	DialUtil::RemoveSpace (iPhoneNumber) ;
	}

/**
@deprecated 9.1
*/
void TDialNumber::BracketParser(const TDes& aPhoneNumber)
	{
	TInt i = aPhoneNumber.Locate(KCharPlus) ;
	TInt j = aPhoneNumber.Locate(KCharSpace) ; 
	if ( j == KErrNotFound ) // no space found in the string
		j = aPhoneNumber.Locate(KCharOpenBracket) ; // It must has a (
	else
		j = Min (aPhoneNumber.Locate(TChar(KCharSpace)),aPhoneNumber.Locate(TChar(KCharOpenBracket)) ) ;
	
	if ( i==-1 || j==-1 || j<=i )
		{
		iNatCode = _L("") ;
		iAreaCode = _L("");
		iPhoneNumber = aPhoneNumber.Right(aPhoneNumber.Length()-1 ); // from KCharPlus onward
		return ;
		}
	else
		{
		if ( j-2*i-2 >= KMaxNatCode )
			{
			iNatCode = _L("") ;
			iAreaCode = _L("");
			iPhoneNumber = aPhoneNumber.Right(aPhoneNumber.Length()-1 ); // from KCharPlus onward
			return ;
			}
		else
			iNatCode = aPhoneNumber.Mid(i+1,j-i-1);
		}

	i = aPhoneNumber.Locate(KCharOpenBracket) ;
	j = aPhoneNumber.Locate(KCharCloseBracket) ;

	if ( i==-1 || j==-1 || j<=i )
		{
		iNatCode = _L("") ;
		iAreaCode = _L("");
		iPhoneNumber = aPhoneNumber.Right(aPhoneNumber.Length()-1 ); // from KCharPlus onward
		return ;
		}
	else
		{
		if ( j-2*i-2 >= KMaxAreaCode )
			{
			iNatCode = _L("") ;
			iAreaCode = _L("");
			iPhoneNumber = aPhoneNumber.Right(aPhoneNumber.Length()-1 ); // from KCharPlus onward
			return ;
			}
		else
			iAreaCode = aPhoneNumber.Mid(i+1,j-i-1);
		}
	if (!iNatCode.Length() &&
		!iAreaCode.Length() )
			iPhoneNumber = aPhoneNumber ;
	else
			iPhoneNumber = aPhoneNumber.Mid(j+1,aPhoneNumber.Length()-j-1);

	}

/**
@deprecated 9.1
*/
TDialNumber::TDialNumber (const TDes& aPhoneNumber,
						  const TDialLocation& aDialLocation , 
						  const TChargeCard & aChargeCard )
	{
	
	if (aPhoneNumber.Locate(KCharPlus) == KErrNotFound)
		{
		}
	else
		{
		TInt iOpen =aPhoneNumber.Locate(KCharOpenBracket);
		TInt iClose=aPhoneNumber.Locate(KCharCloseBracket);


		if ( iOpen==KErrNotFound ||
			 iClose==KErrNotFound    )
			{
			SpaceParser (aPhoneNumber) ;
			}
		else // found both brackets 
			{
			if (iOpen>iClose || iClose-iOpen==1 ) // close bracket before open bracket or nothing in the brackets
				SpaceParser (aPhoneNumber) ;
			else
				{
				iClose=aPhoneNumber.Locate(KCharSpace);

				if ( iClose==KErrNotFound) // no spaces found in string
					BracketParser(aPhoneNumber);
				else 
					{
					if ( iClose>iOpen) // first space found after open bracket so bracket parser
						BracketParser (aPhoneNumber) ;
					else // spaces found before the (
						{
						while (	iClose < aPhoneNumber.Length()-1 &&
								aPhoneNumber[iClose]==KCharSpace)
							iClose++; // move on until a non-space char found

						if (aPhoneNumber[iClose]==KCharOpenBracket) // is a bracket ?
							BracketParser(aPhoneNumber); // the bracket parser
						else
							SpaceParser (aPhoneNumber) ;
						}
					}

				}
			}
		}
	iIntlPrefCode = aDialLocation.IntlPrefCode () ;
	iNatPrefCode = aDialLocation.NatPrefCode () ;

	if (!iNatCode.Length()) // if national code is not supplied
	{
		iNatCode = aDialLocation.NatCode () ; // set it to the current location
		if (!iAreaCode.Length())
			iAreaCode = aDialLocation.AreaCode () ;
	}
//	else // supplied in the string
//		; // leave the area code alone and assume user has to supplied this

	CallType(aDialLocation) ;
	if (aChargeCard.ValidateChargeCardInfo())
		RuleToUse(aChargeCard) ;

	}

/**
@deprecated 9.1
*/
TPtrC TDialNumber::IntlPrefCode () const 
	{
	return TPtrC(iIntlPrefCode);
	}

/**
@deprecated 9.1
*/
TPtrC TDialNumber::NatPrefCode ()const 
	{
	return TPtrC(iNatPrefCode);
	}

/**
@deprecated 9.1
*/
TPtrC TDialNumber::NatCode()const 
	{
	return TPtrC(iNatCode);
	}

/**
@deprecated 9.1
*/
TPtrC TDialNumber::AreaCode()const 
	{
	return TPtrC(iAreaCode);
	}

/**
@deprecated 9.1
*/
TPtrC TDialNumber::PhoneNumber()const 
	{
	return TPtrC(iPhoneNumber);
	}

/**
@deprecated 9.1
*/
TPtrC TDialNumber::RuleToUse()const 
	{
	return TPtrC(iRuleToUse);
	}

/**
@deprecated 9.1
*/
TCallType TDialNumber::CallType()const 
	{
	return iCallType;
	}

/**
@deprecated 9.1
*/
void TDialNumber::RuleToUse ( const TChargeCard & aChargeCard)
	{
	switch (iCallType)
		{
	case ELocalCall:
		iRuleToUse = aChargeCard.LocalRule() ;
		break;
	case ELongDistanceCall:
		iRuleToUse = aChargeCard.NatRule();
		break;
	case EInternationalCall:
		iRuleToUse = aChargeCard.IntlRule() ;
		break;
		}
	}

/**
@deprecated 9.1
*/
void TDialNumber::CallType ( const TDialLocation & aDialLocation )
	{
	iCallType = ELocalCall ;
	if ( aDialLocation.AreaCode() != iAreaCode)
		{
		iCallType = ELongDistanceCall ;
		}
	if ( aDialLocation.NatCode() != iNatCode)
		{
		iCallType = EInternationalCall ;
		}

	}

/**
@deprecated 9.1
*/
TBool TDialNumber::IsNatCode() const { return iNatCode.Length(); }
/**
@deprecated 9.1
*/
TBool TDialNumber::IsAreaCode() const { return iAreaCode.Length();}
/**
@deprecated 9.1
*/
TBool TDialNumber::IsRuleToUse() const{ return iRuleToUse.Length();}


//
// DialUtil
//

/**
@deprecated 9.1
*/
TBool DialUtil::AppendDialString(TDes& aDialStr, TPtrC aAppendStr )
	{
	if ( (aDialStr.Length()+aAppendStr.Length()) < aDialStr.MaxLength() )
		{
		aDialStr.Append (aAppendStr);
		return ETrue ;
		}
	else
		return EFalse ;
	}

/**
@deprecated 9.1
*/
TBool DialUtil::AppendDialString(TDes& aDialStr, TChar aChar )
	{
	if ( (aDialStr.Length()+1) < aDialStr.MaxLength() )
		{
		aDialStr.Append (aChar);
		return ETrue ;
		}
	else
		return EFalse ;
	}


/**
@deprecated 9.1
*/
void DialUtil::RemoveSpace (TDes& aDialString)
	{
	TInt pos = aDialString.Locate(KCharSpace);

	while ( pos != -1 )
		{
		aDialString.Replace (pos,1,_L("")) ;
		pos = aDialString.Locate(KCharSpace);
		}
	}

#if 0
TBool DialUtil::PhoneFormatOK( TDes& aPhoneNumber)
	{
 	return !(
			(aPhoneNumber.Locate(KCharOpenBracket)!=KErrNotFound && aPhoneNumber.Locate(KCharCloseBracket)==KErrNotFound) 
 	||		(aPhoneNumber.Locate(KCharOpenBracket)==KErrNotFound && aPhoneNumber.Locate(KCharCloseBracket)!=KErrNotFound)
		) ;
	}
#endif

/**
@deprecated 9.1
*/
void DialUtil::RemoveUndiallableChars (TDes& aDialString)
	{
	TInt pos = 0;
	while ( pos < aDialString.Length() )
		{
		if (	aDialString[pos] == KCharStar  ||
				aDialString[pos] == KCharHash  ||
				aDialString[pos] == KCharComma ||
			(	aDialString[pos] >= '0' && aDialString[pos] <='9') )

			;
		else
			{
			aDialString.Replace (pos,1,_L("")) ;
			pos--;
			}
		pos++ ;
		}

	}
