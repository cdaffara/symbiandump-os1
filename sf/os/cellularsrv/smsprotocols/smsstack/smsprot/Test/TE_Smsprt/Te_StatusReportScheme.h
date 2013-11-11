// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef TE_STATUSREPORTSCHEME_H
#define TE_STATUSREPORTSCHEME_H

#include "smsstackbaseteststeps.h"
#include "smsstacktestutilities.h"
#include "smsstacktestconsts.h"

#include "gsmuieoperations.h"
#include "gsmunonieoperations.h"

/**
  Base class housing common functionality used by all test steps exercising 
  status report functionality
*/
class CSmsStatusReportSchemeTestStep : public CSmsBaseTestStep
    {
public:
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();

    CSmsStatusReportSchemeTestStep();
    ~CSmsStatusReportSchemeTestStep();

    //Utilites - legacy methods, will be replaced by ones from the base class
    void CreateSmsL(TCodingScheme, TPtrC);
    void SendSmsMessageL();
    void TriggerReceiveL();
    void GetNumberOfPDUs();

    //TPSRR
    void SetAllTPSRRsL();
    void SetLastTPSRRL();
    void SetSomeTPSRRL(TInt, TInt, TInt);

    //SMS
    void SetAllSMSCL();
    void SetLastSMSCL();
    void SetSomeSMSCL(TInt, TInt, TInt);

protected:
    RSocket iSocket;
    TCodingScheme iDataCodingScheme;
    CSmsMessage* iSmsMessage;
    TInt iNumberOfPDUs;

private:
    void OpenSocketServerL();
    void SetCodingScheme();
    };

#endif // TE_STATUSREPORTSCHEME_H
