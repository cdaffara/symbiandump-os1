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
 
#ifndef ABSTRACTCOMMAND_H
#define ABSTRACTCOMMAND_H

class CComReadWrite;

/*
 * Name:		AbstractCommand
 *
 * Description:	Virtual definition file for all commands
 *
 */

NONSHARABLE_CLASS(AbstractCommand)
	{
public:
	virtual ~AbstractCommand();
	virtual void DoIt() = 0;
	virtual void GetReply() = 0;

	virtual TDesC8& GetData() = 0; // debugging purposes
	
	// Pointer to the CComReadWrite communications layer
	CComReadWrite* iComReadWrite;
	};

#endif // ABSTRACTCOMMAND_H
