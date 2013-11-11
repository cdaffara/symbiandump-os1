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
//

/**
 @file
 @internalComponent
*/

#include <remconcoreapitargetobserver.h>

EXPORT_C void MRemConCoreApiTargetObserver::MrccatoCommand(TRemConCoreApiOperationId /*aOperationId*/, 
		TRemConCoreApiButtonAction /*aButtonAct*/)
	{
	}
	
EXPORT_C void MRemConCoreApiTargetObserver::MrccatoPlay(TRemConCoreApiPlaybackSpeed /*aSpeed*/, 
		TRemConCoreApiButtonAction /*aButtonAct*/)
	{
	}

EXPORT_C void MRemConCoreApiTargetObserver::MrccatoTuneFunction(TBool /*aTwoPart*/, 
		TUint /*aMajorChannel*/, 
		TUint /*aMinorChannel*/,
		TRemConCoreApiButtonAction /*aButtonAct*/)
	{
	}

EXPORT_C void MRemConCoreApiTargetObserver::MrccatoSelectDiskFunction(TUint /*aDisk*/,
		TRemConCoreApiButtonAction /*aButtonAct*/)
	{
	}
	
EXPORT_C void MRemConCoreApiTargetObserver::MrccatoSelectAvInputFunction(TUint8 /*aAvInputSignalNumber*/,
		TRemConCoreApiButtonAction /*aButtonAct*/)
	{
	}

EXPORT_C void MRemConCoreApiTargetObserver::MrccatoSelectAudioInputFunction(TUint8 /*aAudioInputSignalNumber*/,
		TRemConCoreApiButtonAction /*aButtonAct*/)
	{
	}
