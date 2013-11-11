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
// The P&S properties used by Sim.Tsy are defined by this component.
// 
//

/**
 @file
*/

#include <e32base.h>
#include <e32property.h>
#include <simtsy.h>

/**
This function defines all P&S properties used by Sim.Tsy.
*/
void InitSimTsyCpm()
	{
	TSecurityPolicy readPolicy(TSecurityPolicy::EAlwaysPass);
	TSecurityPolicy writePolicy(TSecurityPolicy::EAlwaysPass);

	RProperty::Define(KPSSimTsyTestNumber, KPSSimTsyTestNumberKeyType, readPolicy, writePolicy);
	RProperty::Define(KPSSimTsyTestNumberInUse, KPSSimTsyTestNumberInUseKeyType, readPolicy, writePolicy);
	RProperty::Define(KPSSimTsyIncomingVoiceCall, KPSSimTsyIncomingVoiceCallKeyType, readPolicy, writePolicy);
	RProperty::Define(KPSSimTsyIncomingDataCall, KPSSimTsyIncomingDataCallKeyType, readPolicy, writePolicy);
	RProperty::Define(KPSSimTsyRemoteHangup, KPSSimTsyRemoteHangupKeyType, readPolicy, writePolicy);
	RProperty::Define(KPSSimTsyRegStatChange, KPSSimTsyRegStatChangeKeyType, readPolicy, writePolicy);
	RProperty::Define(KPSSimTsyTimersReduceTime, KPSSimTsyTimersReduceTimeSignalKeyType, readPolicy, writePolicy);
	RProperty::Define(KPSSimTsyNetworkQoSChange, KPSSimTsyNetworkQoSChangeKeyType, readPolicy, writePolicy);
	RProperty::Define(KPSSimtsyPacketServiceNtwkRegStatusChange, KPSSimtsyPacketServiceNtwkRegStatusChangeKeyType, readPolicy, writePolicy);
	RProperty::Define(KPSSimtsyPacketContextStatusChange, KPSSimtsyPacketContextStatusChangeKeyType, readPolicy, writePolicy);
	RProperty::Define(KPSSimTsyForceIncomingContextActivation, KPSSimTsyForceIncomingContextActivationType, readPolicy, writePolicy);
	RProperty::Define(KPSSimTsyMbmsService, KPSSimTsyMbmsServiceType, readPolicy, writePolicy);
	RProperty::Define(KPSSimTsySignalStrengthChange, KPSSimTsySignalStrengthChangeKeyType, readPolicy, writePolicy); //this must be the LAST one to be defined
	}

/**
Sim.Tsy's CPM thread entry point.
*/
EXPORT_C TInt SimTsyCpmThread(TAny*)
	{
	// Signal to the RootServer that init is completed successfully
	RThread::Rendezvous(KErrNone);

	InitSimTsyCpm();
	return 0;
	}
