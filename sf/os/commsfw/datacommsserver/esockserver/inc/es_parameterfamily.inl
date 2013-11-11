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
// Inlines for RParameterFamily and RParameterFamilyBundle classes
// 
//

/**
 @file
 @internalTechnology
 @prototype
*/

inline TBool RParameterFamily::IsNull() const
	{
	return (iContainer == NULL) ? ETrue : EFalse;
	}



inline TBool RParameterFamilyBundleC::IsNull() const
	{
	return (iBundle == NULL) ? ETrue : EFalse;
	}


