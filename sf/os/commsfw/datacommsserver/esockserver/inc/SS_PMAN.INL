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

/**
 @file
 @internalComponent 
*/

inline void CProtocolRef::LoadAndBindL()
	{LoadL(EFalse);}

inline CProtocolBase* CProtocolRef::Protocol()
	{ return iProtocol;}

inline void CProtocolRef::ProtocolClosed()
	{
	iProtocol=NULL;
	iFlags=0;
	iFamily->Close();
	}

inline void CProtocolRef::Close()
	{
	if (iProtocol)
		iProtocol->Close();
	}

inline TPtrC CProtocolRef::Tag()
	{ return iTag->Des();}

inline TServerProtocolDesc& CProtocolRef::Info()
	{ return iInfo; }
	
inline void CProtocolFamilyRef::Close()
	{
	if (iProtocolFamily)
		iProtocolFamily->Close();
	}

inline void CProtocolFamilyRef::FamilyClosed()
	{
	iProtocolFamily=NULL;
	iFlags=0;
	}

inline CProtocolFamilyBase* CProtocolRef::Family()
	{ return &(iFamily->ProtocolFamilyL()); }
