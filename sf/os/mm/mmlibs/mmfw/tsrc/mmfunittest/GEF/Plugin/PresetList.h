
// PresetList.h

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
//

#ifndef PRESETLIST_H
#define PRESETLIST_H

#include <mmf/common/mmfglblaudioeffect.h>

class TPresetEle
	{
public:
	TUid 	iUid;
	HBufC* 	iName;
	};

class CPresetList : public CBase,
                    public MMmfGlobalAudioPresetList
	{
public:
	static CPresetList* NewL();
	~CPresetList();
	void Reset();
	void AppendL(TUid aUid, const TDesC& aName);
	
	// from MMmfGlobalAudioPresetList
    TInt MdcaCount() const;
    TPtrC16 MdcaPoint(TInt aIndex) const;
	TUid GAPUidPoint(TInt aIndex) const;
	
private:
	CPresetList();
	RArray<TPresetEle> iArray;
	};

#endif // PRESETLIST_H
