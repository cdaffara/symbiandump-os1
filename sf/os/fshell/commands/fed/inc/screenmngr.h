// screenmngr.h
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
#ifndef SCREENMNGR_H_
#define SCREENMNGR_H_

#include "common.h"

class MViewController;

/*
 Class responsible for arranging windows on the screen. For now it handles only one main window and one command window.
 New windows can be added by calling AddViewL. The class should rearrange visible windows and resize appropriate views.
 Many views can be shown in the same window - switching between visible views is done by calling AttachViewL. The class
 automatically disables old views and activates new views. If more than one window is supported then there may be
 another function necessary which attaches a provided view to a particular window, not just the default one.
*/
class CScreenManager : public CBase
	{
public:
	//Returns the TWindow occupied by the command window
	const TWindow& GetCommandWindow() {return iCmdWnd;}
	//Sets new coordinates of the screen available to this screen manager
	void ResizeScreenL(const TWindow& aWindow);
	//Resizes all Views to make space (if possible) for the new size of the command window.
	//Returns the new size of the command window.
	const TWindow& ResizeCommandWindowL(TInt aHeight);
	//Opens a new window and attaches the view provided to it. The new view becomes active.
	void AddViewL(MViewController& aView);
	//Attaches the provided view to the currently active window. The attached view becomes active.
	void AttachViewL(MViewController& aView);

	void RefreshScreenL();

	void UnsetCurrentView();

private:
	TWindow iScreenWnd;
	TWindow iCmdWnd;
	TWindow iMainWnd;
	MViewController* iCurrentView;
	};

#endif /*SCREENMNGR_H_*/
