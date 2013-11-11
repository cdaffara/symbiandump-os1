// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __DIAL_CONSTS_H__
/**
@publishedAll
@deprecated 9.1
*/
#define __DIAL_CONSTS_H__

/** Maximum length for TDialString. 
@publishedAll
@released
*/
const TInt KMaxDialString=50;

/** Maximum length for TAreaCode. 
@publishedAll
@released
*/
const TInt KMaxAreaCode=10;

/** Maximum length for TNatCode. 
@publishedAll
@released
*/
const TInt KMaxNatCode=10;

/** Maximum length for TNatPrefCode.
@publishedAll
@released
*/
const TInt KMaxNatPrefCode=10;

/** Maximum length for TIntlPrefCode. 
@publishedAll
@released
*/
const TInt KMaxIntlPrefCode=10;

/** A buffer to contain a telephone number to dial. 
@publishedAll
@released
*/
typedef TBuf<KMaxDialString> TDialString;

/** A buffer for a telephony area code. 
@publishedAll
@released
*/
typedef TBuf<KMaxAreaCode> TAreaCode;

/** A buffer for a telephony national code. 
@publishedAll
@released
*/
typedef TBuf<KMaxNatCode> TNatCode;

/** A buffer for a telephony national prefix code. 
@publishedAll
@released
*/
typedef TBuf<KMaxNatPrefCode> TNatPrefCode;

/** A buffer for a telephony international prefix code. 
@publishedAll
@released
*/
typedef TBuf<KMaxIntlPrefCode> TIntlPrefCode;



#endif
