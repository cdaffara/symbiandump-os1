/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/
 
#ifndef __TE_LBSLOCATIONMONITOR_SUITE_DEFS_H__
#define __TE_LBSLOCATIONMONITOR_SUITE_DEFS_H__

#include <test/TestExecuteStepBase.h>

// For Backward Compatibility test
_LIT(KTe_BoolNeedResetFromDefault, "resetall");
// For test step panics
_LIT(KTe_LbsLocationMonitorSuitePanic, "Te_LbsLocationMonitorSuite");

_LIT(KErrorCreateTELBSLocationMonitorTestStep, "The test step can not be created: %d");
_LIT(ErrorUnexpected, "<b>Unexpected error returned:</b>%d");

_LIT(KErrorCheckRunBeforeLeaving, "ErrorCheck run before leaving");
_LIT(KErrorBackwardCompatibilityNotRespected, "<font color=FF0000><b>The backward compatibility is not respected anymore</b></font>");



#endif
