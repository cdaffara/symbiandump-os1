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

#ifndef __DIAL_H__
/**
@publishedAll
@deprecated 9.1
*/
#define __DIAL_H__

#include <e32base.h>
#include <dial_consts.h>

/**
@publishedAll
@deprecated 9.1
*/
const TInt KChargeCardRuleSize=32;
/**
@publishedAll
@deprecated 9.1
*/
const TInt KChargeCardAccountNumberSize=32;
/**
@publishedAll
@deprecated 9.1
*/
const TInt KChargeCardPinSize=8;

/**
Represents a dialling rule.
@deprecated 9.1
*/
typedef TBuf<KChargeCardRuleSize> TChargeCardRule ;
/**
Represents the charge card account number.
@deprecated 9.1
*/
typedef TBuf<KChargeCardAccountNumberSize> TChargeCardAccount ;
/**
Represents the charge card PIN number.
@deprecated 9.1
*/
typedef TBuf<KChargeCardPinSize> TChargeCardPin;

/**
Contains information about a charge card.  
There are functions to get and set this information.

@deprecated This class has been deprecated from 9.1 and should no longer be used.
*/
class TChargeCard
	{
public:
	IMPORT_C TChargeCard () ;
	IMPORT_C TChargeCard( 
				const TChargeCardAccount& aNumber,
				const TChargeCardPin&  aPin ,
				const TChargeCardRule& aLocalRule,
				const TChargeCardRule& aNatRule,
				const TChargeCardRule& aIntlRule) ;

	IMPORT_C void SetNumber(const TDesC&);
	IMPORT_C void SetPin(const TDesC&);
	IMPORT_C void SetLocalRule (const TDesC&);
	IMPORT_C void SetNatRule(const TDesC&);
	IMPORT_C void SetIntlRule(const TDesC&);

	IMPORT_C TBool ValidateChargeCardInfo() const;
	IMPORT_C TPtrC Number() const;
	IMPORT_C TPtrC Pin() const;
	IMPORT_C TPtrC LocalRule () const;
	IMPORT_C TPtrC NatRule() const;
	IMPORT_C TPtrC IntlRule() const;
private:

	TChargeCardAccount iNumber;
	TChargeCardPin	iPin;
	TChargeCardRule iLocalRule;
	TChargeCardRule iNatRule;
	TChargeCardRule iIntlRule;
	};

/** 
This can be passed to TelephoneNumber::Parse() to define whether a 
telephone number is for dialling or for display.

@deprecated 9.1
*/
enum TParseMode
	{
	EForDialing,
	EForDisplay
	} ;

/**
Defines the size of TDialOutLocalCode
@deprecated 9.1
*/
const TInt KDialOutLocalCodeSize=4;
/**
Defines the size of TDialOutLongDistanceCode
@deprecated 9.1
*/
const TInt KDialOutLongDistanceCodeSize=4;
/**
Defines the size of TDisableCallWaitingCode
@deprecated 9.1
*/
const TInt KDisableCallWaitingCodeSize=8;
/**
Defines the size of TPhoneNumber
@deprecated 9.1
*/
const TInt KPhoneNumberSize=100;

/**
Descriptor for the Local Dial-Out Code.
@deprecated 9.1
*/
typedef	TBuf<KDialOutLocalCodeSize> TDialOutLocalCode ;
/**
Descriptor for the Long Distance Dial-Out Code.
@deprecated 9.1
*/
typedef	TBuf<KDialOutLongDistanceCodeSize> TDialOutLongDistanceCode ;
/**
Descriptor for code to disable call waiting
@deprecated 9.1
*/
typedef	TBuf<KDisableCallWaitingCodeSize> TDisableCallWaitingCode ;
/**
Descriptor for the phone number.
@deprecated 9.1
*/
typedef TBuf<KPhoneNumberSize> TPhoneNumber ;

/**
These flags are part of the location information stored by TDialLocation.
@deprecated 9.1
*/
enum {
	KPhoneNumberMobile              =0x00000001, 
	KPhoneNumberUsePulseDial        =0x00000002, 
	KPhoneNumberPbxUsePulseDial     =0x00000004, 
	KPhoneNumberWaitForDialingTone  =0x00000008, 
	KPhoneNumberWaitForProceedTone  =0x00000010, 
    KReturnPhoneNumberWithPlusFormat=0x00000020  
	};

/**
@publishedAll
@deprecated 9.1
*/
const TInt KPhoneNumberDefaultFlags=KPhoneNumberUsePulseDial;

/**
Contains information about the current location
such as international prefix code, area code etc.

There are functions to get and set this information.

@deprecated This class has been deprecated from 9.1 and should no longer be used.
*/
class TDialLocation
	{
public:
	IMPORT_C TDialLocation() ;
	IMPORT_C TDialLocation(
		const TIntlPrefCode & aIntlPrefCode,
		const TNatPrefCode& aNatPrefCode,
		const TNatCode& aNatCode,
		const TAreaCode& aAreaCode,
		const TDialOutLocalCode& aDialOutLocalCode,
		const TDialOutLongDistanceCode& aDialOutLongDistanceCode,
		const TDisableCallWaitingCode& aDisableCallWaitingCode,
		TUint aFlags,
		TUint aPauseAfterDialout ) ;

	IMPORT_C	void SetIntlPrefCode(const TDesC&) ;
	IMPORT_C	void SetNatPrefCode(const TDesC&);
	IMPORT_C	void SetNatCode(const TDesC&);
	IMPORT_C	void SetAreaCode(const TDesC&);
	IMPORT_C	void SetDialOutLocalCode(const TDesC&);
	IMPORT_C	void SetDialOutLongDistanceCode(const TDesC&);
	IMPORT_C	void SetDisableCallWaitingCode(const TDesC&);
	IMPORT_C	void SetDialLocationFlags (TUint);
	inline  	void ClearDialLocationFlags (TUint);
	IMPORT_C	void SetPauseAfterDialout ( TUint);
	IMPORT_C TPtrC IntlPrefCode() const ;
	IMPORT_C TPtrC NatPrefCode() const;
	IMPORT_C TPtrC NatCode() const;
	IMPORT_C TPtrC AreaCode() const;
	IMPORT_C TPtrC DialOutLocalCode() const;
	IMPORT_C TPtrC DialOutLongDistanceCode() const;
	IMPORT_C TPtrC DisableCallWaitingCode() const;
	IMPORT_C TInt  PauseAfterDialout () const;
	IMPORT_C TBool IsMobile () const ;
    inline   TBool IsReturnPhoneNumberWithPlusFormat() const;
	IMPORT_C TBool IsUsePulseDial () const;
	IMPORT_C TBool IsPbxUsePulseDial () const ;
	IMPORT_C TBool IsWaitForDialingTone () const ;
	IMPORT_C TBool IsWaitForProceedTone () const;
	IMPORT_C TBool IsDialOutRequired () const;
	IMPORT_C TBool IsDisableCallWaitingCode() const;

private:
	TIntlPrefCode iIntlPrefCode;
	TNatPrefCode iNatPrefCode;
	TNatCode iNatCode;
	TAreaCode iAreaCode;
	TDialOutLocalCode iDialOutLocalCode;
	TDialOutLongDistanceCode iDialOutLongDistanceCode;
	TDisableCallWaitingCode iDisableCallWaitingCode;
	TUint iFlags;
	TUint iPauseAfterDialout;
};

/**
This can be passed to TelephoneNumber::Parse() to define whether the 
dial string is in full or in the ‘+’ char format.

@deprecated 9.1
*/
enum TPlusFormatDialOptions
	{
	EPlusFormatDialNotAllow ,
	EPlusFormatDialAllow
	};

/**
This option applies when the phone number being dialled is in the 
same location as the dialler: It controls whether the area code 
is included in the final dial string. 

@deprecated 9.1
*/
enum TCityDialOptions
	{
	EDialCityStripDuplicateAreaCode,
	EDialCityAllowDuplicateAreaCode
	};

/**
This can be passed to TelephoneNumber::Parse()  to define the 
area code of the dialling and diallers locations.

@deprecated This class has been deprecated from 9.1 and should no longer be used.
*/
class TCityDialCodes
	{
	public:
	TAreaCode iAreaCode;
	TNatCode iNatCode;
	TNatPrefCode iNatPref;
	TIntlPrefCode iIntlPref;
	};


/**
A static class that provides functions to resolve telephone numbers.

@deprecated This class has been deprecated from 9.1 and should no longer be used.
*/
class TelephoneNumber
{
public:
IMPORT_C static TInt Parse( 
              TDialString& aDialString ,
			  const TDialLocation& aDialLocation , 
			  const TChargeCard& aChargeCard,
			  const TDesC& aPhoneNumber,
			  TParseMode aParseMode ) ;

IMPORT_C static TInt Parse ( TDialString& aDialString ,
								const TDialLocation& aDialLocation, 
								const TDesC& aPhoneNumber ); 

IMPORT_C static TInt CalculateDialCityPrefix(
						TDialString& aDialString ,
						const TCityDialCodes& aFrom,
						const TCityDialCodes& aTo,
						TCityDialOptions aOption,
						TChar aCodeSeparator ,
						TPlusFormatDialOptions aPlusOption ) ;
//
IMPORT_C static TInt Parse ( TDialString& aDialString ,
								const TDialLocation& aDialLocation, 
								const TDesC& aPhoneNumber, 
								TCityDialOptions aOption) ;
};

#include <dial.inl>

#endif
