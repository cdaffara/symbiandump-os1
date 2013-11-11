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
* Description:  Creates the Qt app and bring to foreground
*
*/

#include "audiostuborbit.h"

#include <HbApplication>

int main(int argc, char *argv[])
{
    HbApplication a(argc, argv);
    a.setApplicationName("audiostuborbit");
    audiostuborbit *w = new audiostuborbit();
    w->show();
    int ret = a.exec();
    delete w;
    return ret;
}
