/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file te_settingsmanagerdefs.h
 @internalTechnology
*/
#ifndef TE_SETTINGSMANAGERDEFS_H
#define TE_SETTINGSMANAGERDEFS_H

// Please modify below value with your project and must match with your configuration ini file which is required to be modified as well
_LIT(KTe_LbsSisInstallSisLocation,"TheSisLocation");
_LIT(KTe_LbsSisInstallAppId,"TheAppId");

// For test step panics
_LIT(KTe_lbssisinstallSuitePanic,"Te_lbssisinstallSuite");

// For test step creation panic
_LIT(KStepCreation,"The following step: %S failed: %d");

// For the "Check btpsy from location server" step
_LIT(KOpenMLFWTxt, "Open connection to MLFW server");
_LIT(KOpenPSYTxt, "Open BT GPS PSY" );

_LIT(KGetPsyInformationTxt, "Get BT GPS PSY module information");
_LIT(KPsyInformationAreCorrectTxt, "The BT GPS PSY module Informations are correct");
_LIT(KWaitModuleReady, "Wait module to be ready");

_LIT(KSettingUpdateFailedTxt, "Setting the update options to BT GPS failed, code %d" );
_LIT(KSettingRequestorFailedTxt, "Setting the requestor failed for BT GPS PSY, code %d" );
_LIT(KGettingModuleStatusFailedTxt, "Getting module status of BT GPS PSY failed, code %d");
_LIT(KPSYNotRetreivedTxt, "BT GPS PSY could not be retrieved, code %d" );
_LIT(KPSYNotRetreivedButGracefullTxt, "BT GPS PSY could not be found, but we leave as expected with the error code %d");
_LIT(KPSYNotOpenTxt, "Connection to BT GPS PSY could not be opened, code %d" );
_LIT(KMLFWNotOpenTxt, "MLFW server could not be opened, code %d" );
_LIT(KOpenFailedTxt, "Open failed, got %d " );
_LIT(KRequestFailedTxt, "Request failed, got %d " );
_LIT(KPositionRequestFailedTxt, "position request failed with code %d" );
_LIT(KGetValueFailedTxt, "Get value failed, code %d" );

#endif
