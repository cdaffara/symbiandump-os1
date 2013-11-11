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

#ifndef __DIALUTIL_H__
#define __DIALUTIL_H__

#include <dial.h>
/**
@internalComponent
@deprecated 9.1
*/
enum TCallType { ELocalCall , ELongDistanceCall, EInternationalCall }  ;

class TDialNumber
/**
@internalComponent
@deprecated 9.1
*/
	{
public:
	TDialNumber ( const TDes&, const TDialLocation& , const TChargeCard &);
	TPtrC IntlPrefCode () const;
	TPtrC NatPrefCode () const;
	TPtrC NatCode() const;
	TPtrC AreaCode() const;
	TPtrC PhoneNumber() const;
	TPtrC RuleToUse() const;
	void CallType ( const TDialLocation & ) ;
	void RuleToUse ( const TChargeCard & ) ;
	TBool IsNatCode() const;
	TBool IsAreaCode() const;
	TBool IsRuleToUse() const;
	TCallType CallType() const;
private:	
	TInt String (const TDes& aPhoneNumber, TInt aPos) const ;
	void BracketParser(const TDes& aPhoneNumber) ;
	void SpaceParser(const TDes& aPhoneNumber) ;
	TIntlPrefCode iIntlPrefCode ;
	TNatPrefCode iNatPrefCode;
	TNatCode iNatCode;
	TAreaCode iAreaCode;
	TPhoneNumber iPhoneNumber;
	TChargeCardRule iRuleToUse;
	TCallType iCallType;
	};

class DialUtil
/**
@internalComponent
@deprecated 9.1
*/
{
public:
static TBool AppendDialString(TDes& aDialStr, TPtrC aAppendStr ) ; 
static TBool AppendDialString(TDes& aDialStr, TChar aChar ) ;
static void  RemoveSpace (TDes& aDialString);
//static TBool PhoneFormatOK( TDes& aPhoneNumber) ;
static TInt  GeneralParse(
			 TDialString& aDialString,
			 const TDialLocation & aDialLocation , 
			 const TChargeCard& aChargeCard ,
			 const TDesC& aNumberToDial,
			 TParseMode aParseMode,
 			 TCityDialOptions aOption ) ;
static	void RemoveUndiallableChars (TDes& aDialString) ;
};

/**
@internalComponent
@deprecated 9.1
*/
const TUint KCharT='T';
const TUint KCharP='P';
const TUint KCharW='W';
const TUint KCharComma=',';
const TUint KCharPlus='+';
const TUint KCharSpace=' ';
const TUint KCharELower='e';
const TUint KCharEUpper='E';
const TUint KCharFLower='f';
const TUint KCharFUpper='F';
const TUint KCharILower='i';
const TUint KCharIUpper='I';
const TUint KCharJLower='j';
const TUint KCharJUpper='J';
const TUint KCharGLower='g';
const TUint KCharGUpper='G';
const TUint KCharHLower='h';
const TUint KCharHUpper='H';
const TUint KCharKLower='k';
const TUint KCharKUpper='K';
const TUint KCharStar='*';
const TUint KCharOpenBracket='(';
const TUint KCharCloseBracket=')';
const TUint KCharMinus='-';
const TUint KCharUnderscore='_';
const TUint KCharHash='#';


#endif
