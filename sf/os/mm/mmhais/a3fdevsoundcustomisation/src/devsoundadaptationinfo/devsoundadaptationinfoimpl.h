// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef DEVSOUNDADAPTATIONINFOIMPL_H
#define DEVSOUNDADAPTATIONINFOIMPL_H

#include <e32std.h>
#include <a3f/a3f_trace_utils.h>
#include <a3f/audioprocessingunittypeuids.h>
#include <a3f/devsoundadaptationinfo.h>

class MA3FDevSoundAdaptationInfoObserver;

/*
The first level implementation of CA3FDevsoundAdaptationInfo.
*/
NONSHARABLE_CLASS(CA3FDevSoundAdaptationInfoImpl) : public CA3FDevSoundAdaptationInfo
	{
	class CBody;
	friend class CA3FDevSoundAdaptationInfo;

public:

	/*
	Destructor.
	*/
	~CA3FDevSoundAdaptationInfoImpl();

	// from CA3FDevSoundAdaptationInfo
	TInt RequestMaxGain(TUid aCodecType);
	TInt RequestSupportedFormats(TUid aCodecType, RArray<TUid>& aSupportedFormats);
	TInt Extension_(TUint aExtensionId, TAny*& aExtPtr, TAny*);

private:
	void ConstructL(MA3FDevSoundAdaptationInfoObserver& aAdaptationInfoObserver, CFourCCConvertor& aFourCcConvertor);
	CA3FDevSoundAdaptationInfoImpl();

private:

	CBody* iBody;
	};


#endif // DEVSOUNDADAPTATIONINFOIMPL_H
