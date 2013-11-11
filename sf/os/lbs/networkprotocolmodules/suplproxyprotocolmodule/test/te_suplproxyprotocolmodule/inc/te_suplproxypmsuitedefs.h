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
 @file te_suplproxypmsuitedefs.h
*/

#ifndef TE_SUPLPROXYPMSUITEDEFS_H
#define TE_SUPLPROXYPMSUITEDEFS_H

// The labels must match those in the ini file
_LIT(KTe_SuiteTestId,"TestId");
_LIT(KTe_SuiteBool,"TheBool");


/**
UID and value of Dummy Network gateway
*/
const TUint KDummyNetworkGatewayUidValue    = 0x10283740;
const TUid  KDummyNetworkGatewayUid         = {KDummyNetworkGatewayUidValue};


// TODO 
// The name of the server
//_LIT(KSuplProxyServerName,"LBSSuplProxyServer");
// ---------------------------------------------------------

// For test step panics
_LIT(KTe_SuplProxyPMSuitePanic,"Te_SuplProxyPMSuite");


#endif
