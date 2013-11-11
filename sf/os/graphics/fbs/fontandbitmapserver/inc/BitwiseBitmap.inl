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

inline CBitwiseBitmap::TExtra* CBitwiseBitmap::Extra() const
	{
	return (CBitwiseBitmap::TExtra*)(this + 1);
	}

inline void CBitwiseBitmap::TSettings::SetDirtyBitmap()
	{
	iData |= EBitmapFlagDirty;
	}

inline TBool CBitwiseBitmap::TSettings::IsDirtyBitmap() const
	{
	return iData & EBitmapFlagDirty;
	}

inline void CBitwiseBitmap::TSettings::SetVolatileBitmap()
	{
	iData |= EBitmapFlagVolatile;
	}

inline TBool CBitwiseBitmap::TSettings::IsVolatileBitmap() const
	{
	return iData & EBitmapFlagVolatile;
	}
