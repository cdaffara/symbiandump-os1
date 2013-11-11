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
Creates an empty TDialLocation.
@deprecated 9.1
*/
EXPORT_C TDialLocation::TDialLocation()
	:
	iFlags(0),
	iPauseAfterDialout(1)
	{}

/**
Creates a TDialLocation containing the parameters.

@param aIntlPrefCode International Prefix Code. E.g. for the UK this is 00
@param aNatPrefCode National Prefix Code.  E.g. for the UK this is 44
@param aNatCode National Code. E.g. within the UK this is 0
@param aAreaCode Area Code. E.g. for London this is 20
@param aDialOutLocalCode Local Dial Out Code.  
       Sometimes, the dial out codes differ depending upon 
       whether dialling a local call or a long distance call.
       Typically ‘8’ or ‘9’ is used.
@param aDialOutLongDistanceCode Long Distance Dial-Out Code.
@param aDisableCallWaitingCode This is the code to disable Call Waiting
@param aFlags Flags to set various options.  It should contain the following flags: 
       KPhoneNumberMobile, 
       KPhoneNumberUsePulseDial, 
       KPhoneNumberPbxUsePulseDial, 
       KPhoneNumberWaitForDialingTone, 
       KPhoneNumberWaitForProceedTone, 
       KReturnPhoneNumberWithPlusFormat
@param aPauseAfterDialout Specified the delay after dial out. 
       In the dial string the delay is represented as a 
       comma character. This parameter indicates the 
       length of the comma character.

@deprecated 9.1
*/
EXPORT_C TDialLocation::TDialLocation(
	const TIntlPrefCode& aIntlPrefCode,
	const TNatPrefCode& aNatPrefCode,
	const TNatCode& aNatCode,
	const TAreaCode& aAreaCode,
	const TDialOutLocalCode& aDialOutLocalCode,
	const TDialOutLongDistanceCode& aDialOutLongDistanceCode,
	const TDisableCallWaitingCode& aDisableCallWaitingCode,
	TUint aFlags,
	TUint aPauseAfterDialout )
	:
	iIntlPrefCode(aIntlPrefCode),
	iNatPrefCode(aNatPrefCode),
	iNatCode(aNatCode),
	iAreaCode(aAreaCode),
	iDialOutLocalCode(aDialOutLocalCode),
	iDialOutLongDistanceCode(aDialOutLongDistanceCode),
	iDisableCallWaitingCode(aDisableCallWaitingCode),
	iFlags(aFlags),
	iPauseAfterDialout(aPauseAfterDialout)
	{}

/**
Set the International Prefix Code. E.g. for the UK this is 00
@deprecated 9.1
*/
EXPORT_C void TDialLocation::SetIntlPrefCode(const TDesC& aIntlPrefCode){iIntlPrefCode.Copy(aIntlPrefCode);}
/**
Set the National Prefix Code.  E.g. for the UK this is 44
@deprecated 9.1
*/
EXPORT_C void TDialLocation::SetNatPrefCode(const TDesC& aNatPrefCode){iNatPrefCode.Copy(aNatPrefCode);}
/**
Set the National Code. E.g. within the UK this is 0
@deprecated 9.1
*/
EXPORT_C void TDialLocation::SetNatCode(const TDesC& aNatCode){iNatCode.Copy(aNatCode);}
/**
Set the Area Code. E.g. for London this is 20
@deprecated 9.1
*/
EXPORT_C void TDialLocation::SetAreaCode(const TDesC& aAreaCode){iAreaCode.Copy(aAreaCode);}
/**
Set the Local Dial Out Code.  Sometimes, the dial out codes differ depending upon whether dialling a local call or a long distance call. Typically ‘8’ or ‘9’ is used.
@deprecated 9.1
*/
EXPORT_C void TDialLocation::SetDialOutLocalCode(const TDesC& aDialOutLocalCode){iDialOutLocalCode.Copy(aDialOutLocalCode);}
/**
Set the Long Distance Code.
@deprecated 9.1
*/
EXPORT_C void TDialLocation::SetDialOutLongDistanceCode(const TDesC& aDialOutLongDistanceCode){iDialOutLongDistanceCode.Copy(aDialOutLongDistanceCode);}
/**
Set the code to disable Call Waiting
@deprecated 9.1
*/
EXPORT_C void TDialLocation::SetDisableCallWaitingCode(const TDesC& aDisableCallWaitingCode){iDisableCallWaitingCode.Copy(aDisableCallWaitingCode);}
/**
Set the location flags.  Any of the following flags can be set:

KPhoneNumberMobile

KPhoneNumberUsePulseDial

KPhoneNumberPbxUsePulseDial

KPhoneNumberWaitForDialingTone

KPhoneNumberWaitForProceedTone

KReturnPhoneNumberWithPlusFormat

@deprecated 9.1
*/
EXPORT_C void TDialLocation::SetDialLocationFlags (TUint aFlags) { iFlags |= aFlags;}
/**
Set the delay after dial out. In the dial string the delay is represented as a comma character. This parameter indicates the length of the comma character.
@deprecated 9.1
*/
EXPORT_C void TDialLocation::SetPauseAfterDialout ( TUint aPauseAfterDialout) { iPauseAfterDialout=aPauseAfterDialout;}

/**
Returns the International Prefix Code. E.g. for the UK this is 00
@deprecated 9.1
*/
EXPORT_C TPtrC TDialLocation::IntlPrefCode() const {return TPtrC(iIntlPrefCode);}
/**
Returns the National Prefix Code.  E.g. for the UK this is 44
@deprecated 9.1
*/
EXPORT_C TPtrC TDialLocation::NatPrefCode() const {return TPtrC(iNatPrefCode);}
/**
Returns the National Code. E.g. within the UK this is 0
@deprecated 9.1
*/
EXPORT_C TPtrC TDialLocation::NatCode() const{return TPtrC(iNatCode);}
/**
Returns the Area Code. E.g. for London this is 20
@deprecated 9.1
*/
EXPORT_C TPtrC TDialLocation::AreaCode() const{return TPtrC(iAreaCode);}
/**
Returns the Local Dial-Out Code.  Sometimes, the dial out codes differ depending upon whether dialling a local call or a long distance call. Typically ‘8’ or ‘9’ is used.
@deprecated 9.1
*/
EXPORT_C TPtrC TDialLocation::DialOutLocalCode() const{return TPtrC(iDialOutLocalCode);}
/**
Returns the Long Distance Dial-Out Code.
@deprecated 9.1
*/
EXPORT_C TPtrC TDialLocation::DialOutLongDistanceCode() const{return TPtrC(iDialOutLongDistanceCode);}
/**
Returns the code to disable Call Waiting
@deprecated 9.1
*/
EXPORT_C TPtrC TDialLocation::DisableCallWaitingCode() const{return TPtrC(iDisableCallWaitingCode);}
/**
Returns the delay after dial out. In the dial string the delay is represented as a comma character. This parameter indicates the length of the comma character.
@deprecated 9.1
*/
EXPORT_C TInt  TDialLocation::PauseAfterDialout () const{return iPauseAfterDialout ;}
/**
True if the KPhoneNumberMobile flag is set
@deprecated 9.1
*/
EXPORT_C TBool TDialLocation::IsMobile () const {return(iFlags & KPhoneNumberMobile);}
/**
True if the KPhoneNumberUsePulseDial flag is set
@deprecated 9.1
*/
EXPORT_C TBool TDialLocation::IsUsePulseDial () const{return(iFlags & KPhoneNumberUsePulseDial);}
/**
True if the KPhoneNumberPbxUsePulseDial flag is set
@deprecated 9.1
*/
EXPORT_C TBool TDialLocation::IsPbxUsePulseDial () const {return(iFlags & KPhoneNumberPbxUsePulseDial);}
/**
True if the KPhoneNumberWaitForDialingTone flag is set
@deprecated 9.1
*/
EXPORT_C TBool TDialLocation::IsWaitForDialingTone () const {return(iFlags & KPhoneNumberWaitForDialingTone);}
/**
True if the KPhoneNumberWaitForProceedTone flag is set
@deprecated 9.1
*/
EXPORT_C TBool TDialLocation::IsWaitForProceedTone () const{return(iFlags & KPhoneNumberWaitForProceedTone);}
/**
True if call waiting is disabled.  If no code was supplied to disable call waiting then false is returned.
@deprecated 9.1
*/
EXPORT_C TBool TDialLocation::IsDisableCallWaitingCode() const 
	{
	return (iDisableCallWaitingCode.Length()) ;
	}
/**
True if a code is needed to dial out either local or long distance calls.
@deprecated 9.1
*/
EXPORT_C TBool TDialLocation::IsDialOutRequired() const 
	{
	return ( iDialOutLocalCode.Length() ||
		 iDialOutLongDistanceCode.Length() ) ;
	}
