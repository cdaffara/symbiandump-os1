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
* This file define all the simtsy specific constants
* 
*
*/



/**
 @file SimTsySpecificDefs.h
*/
#if (!defined SIMTSYSPECIFICDEFS_H)
#define SIMTSYSPECIFICDEFS_H

_LIT(KTe_SimtsyTriggerEventName, "SimTsyEvent");
_LIT(KTe_SimTsyEventName,"EventName");
_LIT(KTe_SimTsyEventIndex,"EventIndex");

	/** Identifiers for each of the simtsy parameters that can be changed using publish and subscribe */
	enum TEtelRequestType
		{
		/** Change QoS parameters */
		ENetworkQoSChange,
		/** Change network status */
		ENetworkChangeRegStatus,
		/** Change context status */
		EContextStatusChange,
		/** Change context config parameters */
		EContextConfigChange,
		/** MBMS Service Availability Change */
		EMbmsServAvailChange,
		/** MBMS Bearer Availability Change */
		EMbmsBearerAvailChange,
		/** MBMS Service Id Removal */
		EMbmsServiceRemove,
		/** MBMS Active Service List Change */
		EMbmsActiveListChange,
		/** MBMS Monitor Service List Change */
		EMbmsMonitorListChange,
		/**Invalid Request */
		EInvalidRequest,
		};


#endif //SIMTSYSPECIFICDEFS_H

