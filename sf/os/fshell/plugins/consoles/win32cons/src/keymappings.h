// keymappings.h
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef KEYMAPPINGS_H__
#define KEYMAPPINGS_H__

class TKeyMapping
	{
public:
	int iWindowsKey;
	TKeyCode iSymbianKey;
	};
	
TKeyCode GetSymbianKeyCode(int aWindowsKeyCode);
TUint GetSymbianModifiers(TUint aWindowsModifiers);

#endif //KEYMAPPINGS_H__
