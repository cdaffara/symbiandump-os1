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
 @file Te_LbsAdminSuiteDefs.h
*/
#ifndef __TE_LBSADMIN_SUITE_DEFS_H__
#define __TE_LBSADMIN_SUITE_DEFS_H__

#include <test/testexecutestepbase.h>

// For Backward Compatibility test
_LIT(KTe_BoolNeedResetFromDefault, "resetall");
// For test step panics
_LIT(KTe_LbsAdminSuitePanic, "Te_LbsAdminSuite");

_LIT(KErrorCreateTELBSAdminTestStep, "The test step can not be created: %d");
_LIT(ErrorUnexpected, "<b>Unexpected error returned:</b>%d");

_LIT(KErrorCheckRunBeforeLeaving, "ErrorCheck run before leaving");
_LIT(KErrorBackwardCompatibilityNotRespected, "<font color=FF0000><b>The backward compatibility is not respected anymore</b></font>");

_LIT(KBackwardCompatibilityPreambleL, "CBackwardCompatibilityTestStep::doTestStepPreambleL()");
_LIT(KBackwardCompatibilitydoTestStepL, "CBackwardCompatibilityTestStep::doTestStepL()");
_LIT(KBackwardCompatibilitydoTestStepLDescription, "This test check that the default value is given when using the Get function, after a calls to NewL() and ResetToDefault().");
_LIT(KBackwardCompatibilityKLbsSettingLogger, "Currently testing: KLbsSettingLogger");
_LIT(KBackwardCompatibilityWrongLoggerSettings, "<b>The wrong logger settings has been returned</b>%d ");
_LIT(KBackwardCompatibilityKLbsSpecialFeatureMaxRequDelay, "Currently testing: KLbsSpecialFeatureMaximumRequestDelay");
_LIT(KBackwardCompatibilityWrongSettings, "<b>The wrong setting value has been returned, the maximum delay for a request currently is: </b>%d");
_LIT(KBackwardCompatibilityKLbsSpecialFeatureAllowTBFinalNetPos, "Currently testing: KLbsSpecialFeatureAllowTBFinalNetPos");
_LIT(KBackwardCompatibilitySpecialFeatureOff, "<b>The special feature should be switched off and it's currently: </b>%d");
_LIT(KBackwardCompatibilityDefaultMechanism, "Currently testing the default mechanism");
_LIT(KBackwardCompatibilityPostambleL, "CBackwardCompatibilityTestStep::doTestStepPostambleL()");

_LIT(KOutOfRangePreambleL, "COutOfRangeStep::doTestStepPreambleL()");
_LIT(KOutOfRangeTestStepL, "COutOfRangeStep::doTestStepL()");
_LIT(KOutOfRangeDescription1, "If the maximum time defined for a request delay is too big, the Get function");
_LIT(KOutOfRangeDescription2, "for the KLbsSpecialFeatureMaximumRequestDelay settings should return KErrArgument");
_LIT(KOutOfRangeDescription3, "and set the delay to 0 (no delay)");
_LIT(KOutOfRangeNotRespected, "<font color=FF0000><b>We currently accept out of range time values</b></font>");
_LIT(KOutOfRangePostambleL, "COutOfRangeStep::doTestStepPostambleL()");
			
_LIT(KUnsupportedSetCalStepPreambleL, "CUnsupportedSetCalStep::doTestStepPreambleL()");
_LIT(KUnsupportedSetCalStepTestStepL, "CUnsupportedSetCalStep::doTestStepL()");
_LIT(KUnsupportedSetCalStepDescription, "This test check that KErrArgument is returned when calling the Set function for the KLbsSpecialFeatureMaximumRequestDelay entry.");
_LIT(KUnsupportedSetCalStepPostambleL, "CUnsupportedSetCalStep::doTestStepPostambleL()");


const TUint KNumberOfProperties = 26;

#endif
