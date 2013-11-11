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

#include <e32def.h>

/*
 * patchable const data values
 * 
 * these values control whether apparc tries to save memory by unloading data
 * recognizers when they are not being used
 *
 * To patch these values, add a line to an iby or oby file that is included in the rom being built using the following format:
 *
 * "patchdata <dll> @ <symbol> <newvalue>"
 *
 * e.g. to enable on-demand recognizer loading, use the following line:
 * "patchdata mmfdrmpluginserverproxy.dll @ KMmfDrmPluginServerTimeOut 10000000"
 */


/**
 * Patchable constant.  Determines how long (in microseconds) the DRM Plugin server may remain active 
 * before it gets shutdown.
 * 
 * @SYMPatchable
 * @see KMmfDrmPluginServerTimeOut 
 *
 */ 
EXPORT_C extern const TInt KMmfDrmPluginServerTimeOut = 5000000; // 5 seconds in microseconds

