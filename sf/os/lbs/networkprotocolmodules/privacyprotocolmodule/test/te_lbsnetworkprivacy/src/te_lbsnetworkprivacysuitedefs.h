/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file te_lbsnetworkprivacysuitedefs.h
*/
#if (!defined __TE_LBSNETWORKPRIVACY_SUITEDEFS_H__)
#define __TE_LBSNETWORKPRIVACY_SUITEDEFS_H__

// Please modify below value with your project and must match with your configuration ini file which is required to be modified as well
_LIT(KTe_LbsNetworkPrivacySuiteString,"TheString");
_LIT(KTe_LbsNetworkPrivacySuiteInt,"TheInt");
_LIT(KTe_LbsNetworkPrivacySuiteBool,"TheBool");


// The name of the server
_LIT(KPrivacyServerName,"LBSPrivacyServer");


// For test step panics
_LIT(KTe_LbsNetworkPrivacySuitePanic,"Te_LbsNetworkPrivacySuite");

// For requestor constants
_LIT16(KTestRequestorName,	"te_networkprivcy" );
_LIT16(KTestClientName, "te_networkprivcy client name");


/**
UID and value of Dummy Network gateway
*/
const TUint KDummyNetworkGatewayUidValue   	= 0x10283740;
const TUid  KDummyNetworkGatewayUid    		= {KDummyNetworkGatewayUidValue};

#endif
