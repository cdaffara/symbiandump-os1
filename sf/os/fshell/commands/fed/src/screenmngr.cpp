// screenmngr.cpp
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <e32base.h>

#include "screenmngr.h"
#include "viewbase.h"

const TInt KCmdWndMinHeight = 3;
const TInt KCmdWndMinWidth = 20;
const TInt KRM = 1000; //const multiplication to avoid inexact calculations on small numbers

void CScreenManager::ResizeScreenL(const TWindow& aWindow)
	{
	if(aWindow.iHeight < KCmdWndMinHeight
		|| aWindow.iWidth < KCmdWndMinWidth)
		User::Leave(KErrNotSupported);

	if(aWindow == iScreenWnd)
		return;

	//We need to leave one char on each side for the border around the console window, so the width/height is actually less by 2
	iMainWnd.iX = iCmdWnd.iX = aWindow.iX;
	iMainWnd.iWidth = iCmdWnd.iWidth = aWindow.iWidth + KConsoleWidthCorrection;
	iMainWnd.iY = aWindow.iY;

	//Based on proportions of windows on the old screen calculate proportions on the new screen
	TInt newcmdh = 1;
	if(iScreenWnd.iHeight)
		{
		newcmdh = ((iCmdWnd.iHeight*KRM) * (aWindow.iHeight*KRM)) / iScreenWnd.iHeight*KRM*KRM;
		if(newcmdh >= aWindow.iHeight)
			newcmdh = aWindow.iHeight/2;
		}
	iCmdWnd.iHeight = newcmdh > 0 ? newcmdh : 1;
	iMainWnd.iHeight = aWindow.iHeight - iCmdWnd.iHeight + KConsoleWidthCorrection;
	iCmdWnd.iY = aWindow.iY + iMainWnd.iHeight;
	iScreenWnd = aWindow;

	if(iCurrentView)
		iCurrentView->ResizeL(iMainWnd);
	}

const TWindow& CScreenManager::ResizeCommandWindowL(TInt aHeight)
	{
	if(aHeight == iCmdWnd.iHeight)
		return iCmdWnd;

	//resize the main and cmd windows
	if(aHeight > iScreenWnd.iHeight)
		aHeight = iScreenWnd.iHeight;

	iCmdWnd.iHeight = aHeight;
	iMainWnd.iHeight = iScreenWnd.iHeight - iCmdWnd.iHeight;
	iCmdWnd.iY = iScreenWnd.iY + iMainWnd.iHeight;

	if(iCurrentView)
		iCurrentView->ResizeL(iMainWnd);

	return iCmdWnd;
	}

void CScreenManager::AddViewL(MViewController& aView)
	{
	//Here create a new window if necessary
	AttachViewL(aView);
	}

void CScreenManager::AttachViewL(MViewController& aView)
	{
	if(iCurrentView)
		{
		iCurrentView->DeactivateL();
		}

	iCurrentView = &aView;
	iCurrentView->RedrawL(iMainWnd);
	}

void CScreenManager::UnsetCurrentView()
	{
	iCurrentView = NULL;
	}

void CScreenManager::RefreshScreenL()
	{
	if (iCurrentView) iCurrentView->RedrawL(iMainWnd);
	}
