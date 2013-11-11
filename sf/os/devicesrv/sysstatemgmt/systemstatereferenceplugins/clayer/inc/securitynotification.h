/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Interface   : Domain, Security Notification API
* Parameter and constant definitions for launching
* security-related notifications using the Notifier Framework.
*
*/



#ifndef __SECURITYNOTIFICATION_H__
#define __SECURITYNOTIFICATION_H__

#include <e32cmn.h>

/*
 * @file
* @publishedPartner
* @released
*/

/** UID to use with Security Notifier communication. */
static const TUid KSecurityNotifierUid = { 0x10005988 };

/**
* Parameters for requesting a security code query.
*
* Example:
*   #include <etelmm.h>
*   User::LeaveIfError( iNotifier.Connect() ); // iNotifier is an RNotifier member variable
*   TSecurityNotificationPckg params;
*   params().iStartup = EFalse;
*   params().iEvent = RMobilePhone::EPin1Required;
*   iNotifier.StartNotifierAndGetResponse( iStatus, KSecurityNotifierUid, params, iResult );
*   SetActive();
*/
class TSecurityNotification
    {
public:
    /**
    * One of RMobilePhone::TMobilePhoneSecurityEvent enumeration values.
    * Identifies the code to request.
    */
    TInt iEvent;

    /**
    * Indicates whether the code is requested during system start-up or after it.
    * During system startup the code query dialogs will have no cancel button
    * and emergency call support is enabled.
    * After system startup the code query dialogs will have a cancel possibility.
    */
    TBool iStartup;
    };

typedef TPckgBuf<TSecurityNotification> TSecurityNotificationPckg;

/**
* 'Old-style' parameters. To be used after system start-up phase.
* The code query dialogs will have a cancel button and no emergency call support.
*
* Example:
*   User::LeaveIfError( iNotifier.Connect() ); // iNotifier is an RNotifier member variable
*   iNotifier.StartNotifierAndGetResponse( iStatus, KSecurityNotifierUid, KPin1Required, iResult );
*   SetActive();
*/
_LIT8( KPin1Required, "PINONE" );
_LIT8( KPin2Required, "PINTWO" );
_LIT8( KUPinRequired, "UPIN" );
_LIT8( KPuk1Required, "PUKONE" );
_LIT8( KPuk2Required, "PUKTWO" );
_LIT8( KUPukRequired, "UPUK" );
_LIT8( KNoSIMFound, "NOSIMFOUND" );
_LIT8( KSimTerminated, "SIMTERMINATED" );
_LIT8( KPassPhraseRequired, "SEC" );
_LIT8( KAppPinRequired, "APPPINONE" );
_LIT8( KSecondAppPinRequired, "APPPINTWO" );

#endif // __SECURITYNOTIFICATION_H__
