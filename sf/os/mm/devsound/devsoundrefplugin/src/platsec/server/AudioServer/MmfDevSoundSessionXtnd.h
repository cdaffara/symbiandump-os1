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

#ifndef __MMFDEVSOUNDSESSIONTND_H__
#define __MMFDEVSOUNDSESSIONTND_H__

#include "MmfDevSoundSession.h"
#include "MmfDevSoundInfo.h"

/*
 *
 *	Class Name:	CMMFDevSoundSessionXtnd
 *
 *  This class provides an extended interface for CMMFDevSound - intended
 *  to provide additional calls that are applicable to a particular implementation
 *  This version is built on RMdaDevSound
 */
class CMMFDevSoundSessionXtnd;	// declared here.

NONSHARABLE_CLASS( CMMFDevSoundSessionXtnd ) : public CMMFDevSoundSession
	{
public:

	// Interface for Audio Policy
	// Sets id for this DevSound instance
	void SetDevSoundId(TInt aDevSoundId);
	// Returns info about this DevSound instance
	TMMFDevSoundInfo DevSoundInfo();
	// Called by Audio Policy Server when play reqeust is granted
	void StartPlayDataL();
	// Called by Audio Policy Server when record reqeust is granted
	void StartRecordDataL();
	// Called by Audio Policy Server when play tone reqeust is granted
	void StartPlayToneL();
	// Called by Audio Policy Server when play dual tone request is granted
	void StartPlayDualToneL();
	// Called by Audio Policy Server when play DTMF reqeust is granted
	void StartPlayDTMFStringL();
	// Called by Audio Policy Server when play tone sequence reqeust is granted
	void StartPlayToneSequenceL();
	// Called by Audio Policy Server when DevSound instance looses device
	void SendEvent(const TMMFEvent& aEvent);
	// Updates the total bytes played from audio device
	void UpdateBytesPlayed();

	// CMMFHwDevice implementation:
	TInt FillThisBuffer(CMMFBuffer& aHwBuffer);
	TInt EmptyThisBuffer(CMMFBuffer& aHwBuffer);
	TInt DeviceMessage(TDesC8& aMsg);
	void DeviceStopped();
	// CMMFHwDevice implementation ends
	};

#endif // __MMFDEVSOUNDSESSIONTND_H__
