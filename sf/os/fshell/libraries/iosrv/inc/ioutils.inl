// ioutils.inl
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

template <class SubCmnd> CServerCommandConstructor& CServerCommandFactory::AddLeafCommandL()
	{
	CServerCommandConstructor* cmnd;
	if (iCif)
		{
		CServerCommandBase* commandBase = SubCmnd::NewLC();
		const CCommandInfoFile* cif = iCif->SubCommand(commandBase->Name());
		CleanupStack::PopAndDestroy(commandBase);
		cmnd = CServerCommandConstructor::NewLC(SubCmnd::NameS(), &SubCmnd::NewLC, *this, iReporter, cif);
		}
	else
		{
		cmnd = CServerCommandConstructor::NewLC(SubCmnd::NameS(), &SubCmnd::NewLC, *this, iReporter, NULL);
		}
	AddSubCommandL(cmnd);
	CleanupStack::Pop(cmnd);
	return *cmnd;
	}
	
template <class SubCmnd> CServerCommandFactory& CServerCommandFactory::AddFactoryCommandL()
	{
	CServerCommandFactory* cmnd;
	if (iCif)
		{
		CServerCommandBase* commandBase = SubCmnd::NewLC();
		const CCommandInfoFile* cif = iCif->SubCommand(commandBase->Name());
		CleanupStack::PopAndDestroy(commandBase);
		cmnd = CServerCommandFactory::NewLC(SubCmnd::NameS(), &SubCmnd::NewLC, iReporter, cif);
		}
	else
		{
		cmnd = CServerCommandFactory::NewLC(SubCmnd::NameS(), &SubCmnd::NewLC, iReporter, NULL);
		}
	AddSubCommandL(cmnd);
	CleanupStack::Pop(cmnd);
	return *cmnd;
	}


