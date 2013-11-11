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


inline TInt CProtocolBase::RefCount() const
/**
Reference counting by protocols created by protocol families.

@return number of protocols
*/
	{ return iRefCount; }

inline TInt CProtocolFamilyBase::RefCount() const
	{
	return iRefCount;
	}

inline void CResolverProvdBase::SetNotify(MResolverNotify * aNotifier)
	{ iNotify=aNotifier; }

