// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Configurator Server Security Policy Definitions
// 
//

/**
 @file
 @internalComponent
*/

_LIT_SECURE_ID(KC32EXEProcessSecureId,0x101F7989);

const TUint CRsConfiguratorServer::iPolicyRangeCount = 4;

const TInt CRsConfiguratorServer::iPolicyRanges[iPolicyRangeCount] = 
    {
    0,					// Must start at zero to prevent CPolicyServer from panicing
    CSEnumerateModules, //NetworkControl
    CSUnloadCpm, 		//NetworkControl + PowerMgmt
    CSCancelUnloadCpm+1, 
    };

// Index numbers into PolicyElements[]

const TInt policyNetworkControl		  = 0;
const TInt policyPowerNetworkControl  = 1; 



const TUint8 CRsConfiguratorServer::iPolicyElementsIndex[iPolicyRangeCount] = 
    {
   	CPolicyServer::ENotSupported,	// For fake IPC 0 - CS IPC calls start at CSEnumerateModules
    policyNetworkControl,			// CSEnumerateModules (1)
    								// CSGetModuleIniData(2)
									// CSLoadCpm(3)
									// CSCancelLoadCpm (4)
    policyPowerNetworkControl,    	// CSUnloadCpm(5)
   									// CSCancelUnloadCpm (6)
    CPolicyServer::ENotSupported,   // CSCancelUnloadCpm + 1 to KMaxTInt
    };

const CPolicyServer::TPolicyElement CRsConfiguratorServer::iPolicyElements[] = 
    {
	{_INIT_SECURITY_POLICY_S1(KC32EXEProcessSecureId,ECapabilityNetworkControl), CPolicyServer::EFailClient },
	{_INIT_SECURITY_POLICY_S2(KC32EXEProcessSecureId,ECapabilityNetworkControl, ECapabilityPowerMgmt), CPolicyServer::EFailClient },
	};	

const CPolicyServer::TPolicy CRsConfiguratorServer::iPolicy =
    {
    policyNetworkControl, //  all connect attempts require NetworkControl (all IPCs require it)
    iPolicyRangeCount,                   
    iPolicyRanges,
    iPolicyElementsIndex,
    iPolicyElements,
    };
        

