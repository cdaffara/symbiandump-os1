// Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _SMSSTACKLOG_H_
#define _SMSSTACKLOG_H_

#include <etelmm.h>


//
// Functions to perform specific logging where necessary.
//

    void LogSmsIfPDUL(const TDesC8& aText, const TDesC8& aPDU, TBool aCommandPdu);
    void LogSmsIfHexBuf(const TDesC8& aText, const TDesC8& aHexBuf);
    void LogSmsIfTypeOfNumber(const TDesC8& aText, RMobilePhone::TMobileTON aTON);
    void LogSmsIfNumberingPlan(const TDesC8& aText, RMobilePhone::TMobileNPI aNPI);
    void LogSmsIfSmsEntry(const TDesC8& aText,
                          const RMobileSmsStore::TMobileGsmSmsEntryV1& aSmsGsmEntryV1);
    void LogSmsIfSendAttributes(const TDesC8& aText,
                                const RMobileSmsMessaging::TMobileSmsSendAttributesV1& aAttrib);
    /**
     * Generates a string with the current time 
     * 
     * @param alogStatement TDes& of at least size 40 
     */
inline void SmsTimeStampL(TDes& aLogStatement)
        {
        TTime now;
        now.UniversalTime();
        TDateTime dateTime(now.DateTime());
        _LIT(KOwnTimeFormat,"TimeStamp=%:0%H%:1%T%:2%S.%*C6%:3");
        now.FormatL(aLogStatement,KOwnTimeFormat);
        //aLogStatement
        };


// Define macros to disable and restore Code Coverage
    
#ifndef BULLSEYE_OFF
#ifdef _BullseyeCoverage
#define BULLSEYE_OFF "BullseyeCoverage save off";
#define BULLSEYE_RESTORE "BullseyeCoverage restore";
#else
#define BULLSEYE_OFF 
#define BULLSEYE_RESTORE 
#endif
#endif

#endif  // _SMSSTACKLOG_H_
