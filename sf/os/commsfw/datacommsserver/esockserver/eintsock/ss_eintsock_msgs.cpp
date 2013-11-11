// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file
// @internalTechnology
// 
//

#include "ss_eintsock_msgs.h"

TInt TEIntSockOpMsgWrapper::WriteDesc(TInt aIndex, const TDesC8& aDesc, TInt aOffset)
	{
	TDes8* des = ( TDes8* )iArgs.iArgs [ aIndex ];
	
	if ( des->MaxLength() < aOffset )
		return KErrOverflow;
	
	des->SetLength ( aOffset );
	if ( ( aDesc.Length() + des->Length() ) > des->MaxLength() )
		return KErrOverflow;
	else	
		des->Append ( aDesc );
	return KErrNone;
	}

