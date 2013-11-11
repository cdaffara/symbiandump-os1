// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BITGCEXTRADATA_H
#define BITGCEXTRADATA_H

/**
Class used to extend the CFbsBitGc class to avoid BC break. Any data member which needs 
to be added in CFbsBitGc should be added to this class.
@internalAll
@released
*/
class CFbsBitGcExtraData:public CBase
	{
public:
	CFbsBitGcExtraData();
	~CFbsBitGcExtraData();
	void Reset();
	inline TInt* PenArray(){return iPenArray;};
	inline TRgb ShadowColor(){return iShadowColor;}
	inline void SetPenArray(TInt* aPenArray);
	inline void SetShadowColor(const TRgb& aShadowColor);
	inline void ResetPenArray();
private:
	TInt* iPenArray;
	TRgb iShadowColor;
	};

inline void CFbsBitGcExtraData::SetPenArray(TInt* aPenArray)
	{
	delete[] iPenArray;
	iPenArray = aPenArray;
	}

inline void CFbsBitGcExtraData::SetShadowColor(const TRgb& aShadowColor)
	{
	iShadowColor = aShadowColor;
	}

inline void CFbsBitGcExtraData::ResetPenArray()
	{
	delete[] iPenArray;
	iPenArray = NULL;
	}

#endif // BITGCEXTRADATA_H
