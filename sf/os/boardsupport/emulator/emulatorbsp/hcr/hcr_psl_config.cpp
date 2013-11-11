// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file is part of the EPOC Emulator Variant Base Port
// Hardware Configuration Respoitory Platform Specific Layer (PSL) 
//


/** 
@file hcr_psl_config.cpp
File provides the compiled repository (currently empty) EPOC Emulator platform. 

@internalTechnology
*/

// -- HCR INCLUDES ------------------------------------------------------------

#include "hcrconfig.h"

#include "hcr_hai.h"
#include "hcr_uids.h"


using namespace HCR;



// -- HCR BSP SETTING INCLUDES ------------------------------------------------

// Includes for setting published by base port ASSP/SOC interconnections
// e.g. #include "hcr_psl_config_mha_inc.inl"




// -- HCR BSP LARGE SETTING DATA (LSD) VALUES ---------------------------------

// Includes for setting published by base port ASSP/SOC interconnections
// e.g. #include "hcr_psl_config_mha_lsd.inl"




// -- HCR BSP SETTINGS LIST ---------------------------------------------------

SSettingC gSettingsList[] = 
    {
    
// Includes for setting published by base port ASSP/SOC interconnections
// e.g. #include "hcr_psl_config_mha.inl"

	HCR_LAST_SETTING
	};
	
	
SRepositoryBase gHeader = 
    {
    HCR_FINGER_PRINT, 
    EReposCompiled, 
    KRepositoryFirstVersion,
    EReposReadOnly,
    HCR_SETTING_COUNT(gSettingsList)
    };


GLDEF_C SRepositoryCompiled gRepository = 
    { 
    &gHeader, 
    gSettingsList 
    };
