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

#ifndef MMRCERROR_H_
#define MMRCERROR_H_


#define KMMRCErrorNaneNotImplemented _L("Not implemented")
#define KMMRCErrorNameUnkwnonAudioState _L("Unknown audio state")
#define KMMRCErrorNameLogicalChainNull _L("Logical chain should not be null")
#define KMMRCErrorNamesessionLost _L("Session should not be null")

enum TMMRCError
{
	KMMRCErrorNotImplemented,
	EMMRCErrorUnkwnonAudioState,
	EMMRCErrorLogicalChainNull,
	EMMRCErrorSessionLost
};


#endif /*MMRCERROR_H_*/