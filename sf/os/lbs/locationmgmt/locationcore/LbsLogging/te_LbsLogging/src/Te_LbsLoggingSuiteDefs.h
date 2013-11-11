/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file define all the common values thoughout your test project
* 
*
*/



/**
 @file Te_LbsLoggingSuiteDefs.h
*/
#if (!defined __TE_LBSLOGGIONG_SUITEDEFS_H__)
#define __TE_LBSLOGGING_SUITEDEFS_H__

#define APGS_MODINFO_MODULE_ID      			271064388
#define NETWORK_MODINFO_MODULE_ID   			271064387 

const TTimeIntervalMicroSeconds32 KWait2second(2000000); // 2 second
const TTimeIntervalMicroSeconds32 KWait5second(5000000); // 5 second
const TTimeIntervalMicroSeconds32 KWait10second(10000000); // 10 second
const TTimeIntervalMicroSeconds32 KWait15second(15000000); // 15 second

// constants definition
_LIT(KLbsLogFile, "C:\\LbsLog_TEST.TXT");
_LIT(KLbsLogResultFile, "C:\\LbsLogResult_TEST.TXT");
_LIT(KTe_LbsLoggingSuiteLocalAppUid,"LocalAppUid");
_LIT(KTe_LbsLoggingSuiteCost,"Cost");
_LIT(KTe_LbsLoggingSuiteRequestOutcome,"RequestOutcome");
_LIT(KTe_LbsLoggingSuiteModuleId,"ModuleId");
_LIT(KTe_LbsLoggingSuitePosUpdateType,"PosUpdateType");
_LIT(KTe_LbsLoggingSuiteTechType,"TechType");
_LIT(KTe_LbsLoggingSuiteModeReason,"ModeReason");
_LIT(KTe_LbsLoggingSuiteVerAccuracy,"VerAccuracy");
_LIT(KTe_LbsLoggingSuiteHorAccuracy,"HorAccuracy");
_LIT(KTe_LbsLoggingSuiteLat,"Lat");
_LIT(KTe_LbsLoggingSuiteLong,"Long");
_LIT(KTe_LbsLoggingSuiteRequestMask,"RequestMask");
_LIT(KTe_LbsLoggingSuiteReceiveMask,"ReceiveMask");
_LIT(KTe_LbsLoggingSuiteX3PUid,"X3PUid");
_LIT(KTe_LbsLoggingSuiteDest,"Dest");
_LIT(KTe_LbsLoggingSuiteSessionNum,"SessionNum");
_LIT(KTe_LbsLoggingSuiteSessionOwner,"SessionOwner");
_LIT(KTe_LbsLoggingSuiteOriginator,"Originator");
_LIT(KTe_LbsLoggingSuiteAction,"Action");
_LIT(KTe_LbsLoggingSuiteAdvice,"Advice");
_LIT(KTe_LbsLoggingSuiteClientName,"ClientName");
_LIT(KTe_LbsLoggingSuiteExternalId,"ExternalId");
_LIT(KTe_LbsLoggingSuiteRequestor,"Requestor");
_LIT(KTe_LbsLoggingSuiteResult,"Result");
_LIT(KTe_LbsLoggingSuiteMax,"Max");
// For test step panics
_LIT(KTe_LbsLoggingSuitePanic,"Te_LbsLoggingSuite");

#endif
