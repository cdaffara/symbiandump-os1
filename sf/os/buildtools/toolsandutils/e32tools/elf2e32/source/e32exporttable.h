// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Class for E32 Export Table implementation of the elf2e32 tool
// @internalComponent
// @released
// 
//

#ifndef __E32EXPORTTABLE__
#define __E32EXPORTTABLE__

#include <tools/elfdefs.h>

#include "pl_elfexports.h"

class ElfExecutable;

/**
class for E32 Export Table
@internalComponent
@released
*/
class E32ExportTable {
 public:
  E32ExportTable() : 
	   iElfExecutable(0), iSize(0), iTable(0), iExportTableAddress(0),
	   iAllocateP(true), iNumExports(0)
	   {};
  ~E32ExportTable();
  void CreateExportTable(ElfExecutable * aElfExecutable, ElfExports::ExportList & aExportList);
  size_t GetNumExports();
  bool AllocateP();
  size_t GetExportTableSize();
  unsigned int * GetExportTable();

 public:
  ElfExecutable * iElfExecutable;
  size_t iSize;
  unsigned int * iTable;
  // NB. This a virtual address (within the RO segment).
  Elf32_Addr iExportTableAddress;
  // True if the postlinker must allocate the export table in the E32Image.
  // This should only be false for custom built ELF executables.
  bool iAllocateP;
  size_t iNumExports;
};


#endif
