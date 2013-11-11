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
 @file Te_cedSuiteDefs.h
 @internalTechnology
*/
#if (!defined __TE_CED_SUITEDEFS_H__)
#define __TE_CED_SUITEDEFS_H__

// Please modify below value with your project and must match with your configuration ini file which is required to be modified as well
_LIT(KTe_cedSuiteString,"TheString");
_LIT(KTe_cedSuiteInt,"TheInt");
_LIT(KTe_cedSuiteBool,"TheBool");

// For test step panics
_LIT(KTe_cedSuitePanic,"Te_cedSuite");

#ifdef __TOOLS2__
_LIT(KLogFile, "ced.log");
#else
_LIT(KLogFile, "c:\\ced.log");
#endif 
_LIT(KSessionPath, "c:\\");
_LIT(KStringToCheck, "SUCCESS");
const TInt KFileEndSign = 2;


#endif
