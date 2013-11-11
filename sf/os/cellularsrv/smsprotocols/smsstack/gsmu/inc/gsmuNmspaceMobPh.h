// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Multimode SMS common API header file.
// Describes common Telephony interface types.
// The types are duplicated from ETELMM.h in an effort to separate SMS from ETEL.
// Correspondent ETEL types could be found in ETELMM.h, class RMobilePhone.
// 
//

/**
 @file
 @publishedAll
 @released
*/


#ifndef __GSMUNMSPACEMOBPH_H__
#define __GSMUNMSPACEMOBPH_H__

/**
 *  Address types.
 *  
 *  Modes: Common
 *  
 *  @see class RMobilePhone
 */
namespace NMobilePhone
	{
	enum TMobileTON
		{
		/**
		 *  User or the network has no knowledge of the type of number.
		 *  
		 *  Modes: Common
		 */
		EUnknownNumber,			// 0

		/**
		 *  International number.
		 *  
		 *  Modes: Common
		 */
		EInternationalNumber,	// 1

		/**
		 *  National number.
		 *  
		 *  Modes: Common
		 */
		ENationalNumber,		// 2

		/**
		 *  Administration/service number specific to the serving network,
		 *  e.g. used to access an operator.
		 *  
		 *  Modes: Common
		 */
		ENetworkSpecificNumber, // 3

		/**
		 *  Subscriber number.
		 *  
		 *  Modes: Common
		 */
		ESubscriberNumber,		// 4 - Also defined as "dedicated, short code" in GSM 04.08

		/**
		 *  Alphanumeric number coded according to 3GPP TS 123 038 GSM
		 *  7-bit default alphabet.
		 *  
		 *  Modes: GSM/WCDMA
		 */
		EAlphanumericNumber,	// 5

		/**
		 *  Abbreviated number.
		 *  
		 *  Modes: Common
		 */
		EAbbreviatedNumber		// 6
		};

	/**
	 *  Number Plan Indicator.
	 *  
	 *  Modes: Common
	 */
	enum TMobileNPI
		{
		/**
		 *  User or the network has no knowledge of the numbering plan.
		 *  
		 *  Modes: Common
		 */
		EUnknownNumberingPlan =0,

		/**
		 *  ISDN/telephony numbering plan.
		 *  
		 *  Modes: Common
		 */
		EIsdnNumberPlan=1,

		/**
		 *  Data numbering plan.
		 *  
		 *  Modes: Common
		 */
		EDataNumberPlan=3,

		/**
		 *  Telex numbering plan.
		 *  
		 *  Modes: Common
		 */
		ETelexNumberPlan=4,

		/**
		 *  Service centre specific plan used to indicate a numbering plan
		 *  specific to external Short Message entities attached to	the
		 *  SMSC.
		 */
		EServiceCentreSpecificPlan1=5,

		/**
		 *  Service centre specific plan used to indicate a numbering plan
		 *  specific to external Short Message entities attached to	the
		 *  SMSC.
		 *  
		 *  Modes: GSM/WCDMA
		 */
		EServiceCentreSpecificPlan2=6,

		/**
		 *  National numbering plan.
		 *  
		 *  Modes: GSM/WCDMA
		 */
		ENationalNumberPlan=8,

		/**
		 *  Private numbering plan.
		 *  
		 *  Modes: Common
		 */
		EPrivateNumberPlan=9,

		/**
		 *  ERMES numbering plan.
		 *  
		 *  Modes: GSM/WCDMA
		 */
		EERMESNumberPlan=10
		};
	}

#endif // __GSMUNMSPACEMOBPH_H__
