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
// TGDIELL.H
// 
//

const TInt numellipses=13;
TRect ellrect[numellipses];
ellrect[0].SetRect(0,0,1,1);
ellrect[1].SetRect(0,1,2,3);
ellrect[2].SetRect(0,3,3,6);
ellrect[3].SetRect(0,6,4,10);
ellrect[4].SetRect(0,10,5,15);
ellrect[5].SetRect(0,18,6,24);
ellrect[6].SetRect(6,17,13,24);
ellrect[7].SetRect(5,9,13,17);
ellrect[8].SetRect(4,0,13,9);
ellrect[9].SetRect(13,0,23,10);
ellrect[10].SetRect(13,10,24,21);
ellrect[11].SetRect(13,21,25,33);
ellrect[12].SetRect(0,24,13,37);
