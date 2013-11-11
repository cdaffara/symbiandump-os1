/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Backend for the Qt app
*
*/

#ifndef AUDIOSTUBORBIT_H
#define AUDIOSTUBORBIT_H

#include <HbMainWindow>

class CTspClientMapper;
class CAudioClientStubRemConObserver;

class audiostuborbit : public HbMainWindow
{
    Q_OBJECT

public:
	audiostuborbit(QWidget *parent = 0);
    ~audiostuborbit();

private:
    CTspClientMapper* iTspClientMapper;
    CAudioClientStubRemConObserver* iRemConObserver;
};

#endif // AUDIOSTUBORBIT_H
