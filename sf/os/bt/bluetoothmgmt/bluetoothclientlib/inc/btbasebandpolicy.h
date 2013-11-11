// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BTBASEBANDPOLICY_H
#define BTBASEBANDPOLICY_H

/**
@file
@publishedAll
@released
*/

#include <bttypes.h>
#include <btdevice.h>

/** Paging policy for baseband.*/
enum TBasebandPageTimePolicy
    {
	EPagingDontCare,	/*!< Don't care setting */
    EPagingNormal,		/*!< Normal setting */
    EPagingBestEffort,	/*!< Best effort setting */
    EPagingQuick,		/*!< Quick paging setting */
    };

struct TBasebandPolicyParams
/** Baseband policy parameters.*/
	{
	TBasebandPageTimePolicy		iPageTimePolicy;	/*!< Page time policy */
	};

struct TSetBasebandPolicy
/** Set baseband policy.

@deprecated
@see RBTBaseband, TPhysicalLinkQuickConnectionToken
*/
	{
	TBTDevAddr				iDevAddr;	/*!< Device Address */
	TBasebandPolicyParams	iPolicy;	/*!< Policy parameters */
	};

/** Package for SetBasebandPolicy structure
@deprecated
*/
typedef TPckgBuf<TSetBasebandPolicy> TSetBasebandPolicyBuf;	

struct TPhysicalLinkQuickConnectionToken
/** Specifies details for faster connection.*/
	{
	TBTNamelessDevice		iDevice;	/*!< Nameless device */
	TBasebandPolicyParams	iPolicy;	/*!< New policy */
	};
	
typedef TPckgBuf<TPhysicalLinkQuickConnectionToken> TPhysicalLinkQuickConnectionTokenBuf;	/*!< Package for TPhysicalLinkQuickConnectionToken structure */

#define KBasebandSlotTime 0.000625	/*!< Baseband timeslot duration (0.000625 seconds) */
static const TUint KDefaultBasebandConnectionTimeout = 10;  /*!< Default baseband connection timeout (10 seconds) */

#endif // BTBASEBANDPOLICY_H
