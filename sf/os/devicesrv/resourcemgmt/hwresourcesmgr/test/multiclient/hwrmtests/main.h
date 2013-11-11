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

#ifndef MAIN_H
#define MAIN_H

class CConsoleBase;

GLREF_D CConsoleBase* console;


//
// Main executable functions
//
void mainL();
void consoleMainL();
GLDEF_C TInt E32Main();

#endif
