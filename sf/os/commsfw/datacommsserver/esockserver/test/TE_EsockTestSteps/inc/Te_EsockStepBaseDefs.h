/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file define all the common values, that are not specific
* to any particular test step(s).
* At the end of this file it is possible to include other header
* files containing any specific definitions used by a particular
* step or a group of steps.
* 
*
*/



/**
 @file Te_EsockStepBaseDefs.h
*/

#if (!defined TE_ESOCKSTEPBASEDEFS_H)
#define TE_ESOCKSTEPBASEDEFS_H

//generic constants that can (are) used by any test step
_LIT(KTe_Name,"Name");
_LIT(KTe_NameRetrieved,"NameRetrieved");
_LIT(KTe_Type,"Type");
_LIT(KTe_Expected,"Expected");
_LIT(KTe_Register,"Register");
_LIT(KTe_ImmediateCancelFlagName,"ImmediateCancel");
_LIT(KTe_All,"All");
_LIT(KTe_Generic,"Generic");

_LIT(KTe_EventMaskName,"EventMask");
_LIT(KTe_ParamsFamilyType,"FamilyType");
_LIT(KExpectedError, "ExpectedError");
_LIT(KExpectedError2, "ExpectedError2");
_LIT(KTe_ExpectedCompareResult, "ExpectedCompareResult");

const TInt KExpectedErrorNotUsed = 1;

//specific constants that are used by specific test steps
#include "BackupRestoreSpecificDefs.h"
#include "QoSSpecificDefs.h"
#include "SblpSpecificDefs.h"
#include "SimTsySpecificDefs.h"
#include "incomingconspecificdefs.h"

#endif //TE_ESOCKSTEPBASEDEFS_H

