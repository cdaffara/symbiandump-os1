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
 @test
 @internalComponent - Internal Symbian test code 
*/

#if !defined (__TAMASTARTER_CONSTANTS_H__)
#define __TAMASTARTER_CONSTANTS_H__

//Constants used in AmaStarter test apps
_LIT(KRetryCntr,"RetryCntr");
_LIT(KLaunchCntr,"LaunchCntr");
_LIT(KSuccessCntr,"SuccessCntr");
_LIT(KFailedCntr,"FailedCntr");
_LIT(KLeaveCntr,"LeaveCntr");
_LIT(KPanicCntr,"PanicCntr");

_LIT(KTestAppResultsPath, "c:\\tamastarter\\");
_LIT(KTestLogFileExtension, ".log");
_LIT(KTestLogFileWild, "c:\\tamastarter\\tamastarter_result*.*");

_LIT(KLog, "-Log");
_LIT(KRendezvous, "-Re");
_LIT(KTimeout, "-T");
_LIT(KRetry, "-R");
_LIT(KLeaveErrorValue, "-E");
_LIT(KPanicValue, "-P");
_LIT(KCancelMonitoring, "-CM");


#endif