// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains common defination
// 

#ifndef LTSYCOMMONDEFINE_H_
#define LTSYCOMMONDEFINE_H_

// system include
#include <e32std.h>

/**
 * Define the IO event
 * 
 */
enum TAtEventSource
	{
	EReadCompletion,
	EWriteCompletion,
	ETimeoutCompletion
	};

/**
* The default maximum time in which Writing an AT command must be complete
* 5 seconds
*/
const TInt KLtsyDefaultWriteTimeOut = 5;

/**
* The default maximum time in which The response from baseband must be received 
* 10 seconds
*/
const TInt KLtsyDefaultReadTimeOut = 20;

/**
* The invalid call ID
*/
const TInt KLtsyErrorCallId = -1;

/**
 * The minimum call id
 * 
 */
const TInt KLtsyMinCallId = 1;

/**
 * The maximal call id
 * 
 */
const TInt KLtsyMaxCallId = 7;

/**
 * +WIND
 */
const TInt KLtsyUnsolicitedEvent00 = 0;
const TInt KLtsyUnsolicitedEvent01 = 1;
const TInt KLtsyUnsolicitedEvent02 = 2;
const TInt KLtsyUnsolicitedEvent03 = 3;
const TInt KLtsyUnsolicitedEvent04 = 4;
const TInt KLtsyUnsolicitedEvent05 = 5;
const TInt KLtsyUnsolicitedEvent06 = 6;
const TInt KLtsyUnsolicitedEvent07 = 7;
const TInt KLtsyUnsolicitedEvent08 = 8;
const TInt KLtsyUnsolicitedEvent09 = 9;
const TInt KLtsyUnsolicitedEvent10 = 10;
const TInt KLtsyUnsolicitedEvent11 = 11;
const TInt KLtsyUnsolicitedEvent12 = 12;
/**
 *  const for sms 
 */
const TInt KGsmTpduSize = 165;            // 140 bytes user data + 25 bytes TPDU header
const TInt KPduMode = 0;
const TInt KTextMode = 1;
const TInt KCMTMode = 2;
const TInt KMaxAsciiPduLength = 400;       //For write message and send message

/**
 * ReadEntry for phonebook
 */
class TPhonebookReadEntry
    {
public:
	TUint16     iIndex;
    TBuf16<20>  iNumber;
    TUint8      iTon;
    TBuf16<14>  iText;	
    };

// const string defination
_LIT8(KLtsyOkString,"OK");
_LIT8(KLtsyErrorString,"ERROR");
_LIT8(KLtsyCmeErrorString,"+CME ERROR:*");
_LIT8(KLtsyNoCarrierString,"NO CARRIER");
_LIT8(KLtsyGeneralIndMatchString, "+WIND:*");
_LIT8(KAnyATCommand,"AT+*");

_LIT8(KLtsyCtrlZChar,"\032");
_LIT8(KLtsyCommaChar,",");
#endif /*LTSYCOMMONDEFINE_H_*/
