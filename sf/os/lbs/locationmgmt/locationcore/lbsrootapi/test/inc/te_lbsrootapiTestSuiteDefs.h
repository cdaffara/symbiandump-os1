/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file Te_LbsRootApiTestSuiteDefs.h
*/
#if (!defined __TE_LBSROOTAPITEST_SUITEDEFS_H__)
#define __TE_LBSROOTAPITEST_SUITEDEFS_H__

// Please modify below value with your project and must match with your configuration ini file which is required to be modified as well
_LIT(KTe_LbsRootApiTestSuiteString,"TheString");
_LIT(KTe_LbsRootApiTestSuiteInt,"TheInt");
_LIT(KTe_LbsRootApiTestSuiteBool,"TheBool");

// For test step panics
_LIT(KTe_LbsRootApiTestSuitePanic,"Te_LbsRootApiTestSuite");

const TUid KLbsTestProcessUid = { 0x10282255 };

const TUint KTestLbsPanicRequestKey				= 0x12345678 ;
const TUint KTestLbsPanicOnCloseDownRequestKey	= 0x12345679 ;
const TUint KTestLbsSecondCloseDownRequestKey	= 0x12345680 ;
const TUint KTestLbsResetFunctionCalledKey		= 0x12345681 ;

_LIT(KNrhName, "t_lbsrootapi_lbsnetworkrequesthandler.exe");
_LIT(KAgpsName, "t_lbsrootapi_lbsgpslocmanager.exe");
_LIT(KNgName,   "t_lbsrootapi_lbsnetgateway.exe");
_LIT(KPanickingProcessName,   "t_lbsrootapi_panickingprocess.exe");

const TInt32 KNrhUidValue = 0x10282272;
const TUid KNrhUid = { KNrhUidValue };

const TInt32 KAgpsUidValue = 0x10282270;
const TUid KAgpsUid = { KAgpsUidValue };

const TInt32 KNgUidValue = 0x10282271;
const TUid KNgUid = { KNgUidValue };

const TInt32 KPanickingUidValue = 0x10282275;
const TUid KPanickingUid = { KPanickingUidValue };

_LIT(KNrhProcessName, "t_lbsrootapi_lbsnetworkrequesthandler.exe[10282272]0001");
_LIT(KAgpsProcessName, "t_lbsrootapi_lbsgpslocmanager.exe[10282270]0001");
_LIT(KNgProcessName,   "t_lbsrootapi_lbsnetgateway.exe[10282271]0001");

#endif
