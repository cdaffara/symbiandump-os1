# Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
# Script to generate IPC /enumNames cppfile
# 
#

$dest_file_ext = ".cpp";
@etelSheets=("etel"); #,"etelmm","etelpckt","etelsat"); # ? to add "etelisv"
@phbksyncSheets=("phbksync");
@c32Sheets=("c32");
@spreadSheets=();


%ipc_table = ();
$count = 0;
$ipc_num=0;
$numOfFiles=scalar @ARGV;
$inputMode="";

$numOfFiles=scalar @ARGV;
if ( $numOfFiles  eq 2)
{
    $source_file=$ARGV[0];
    if(!($source_file =~ /$\.xls/))# excel spreadsheet
    {
     die "$source_file is not xls file !";
    }
    
    if($ARGV[1] eq "etel" ){@spreadSheets=@etelSheets;}
    elsif($ARGV[1] eq "phbksync" ){@spreadSheets=@phbksyncSheets;}
    elsif($ARGV[1] eq "c32" ){@spreadSheets=@c32Sheets;}
    else {die "Sheet not supported: $ARGV[$i]";}
        
    use Win32::OLE;
    # Establish MS Excel Access
    eval {$main::excel= Win32::OLE->GetActiveObject('Excel.Application')};
    		die "Excel not installed" if $@;
    unless (defined $main::excel)
		{$main::excel= Win32::OLE->new('Excel.Application', sub {$_[0]->Quit;})
    		or die "Cant start Excel";}
    printf "Excel access established $source_file\n";

    $main::excel->{DisplayAlerts}=0;     # turn off pesky alerts

    # Open Our Capability Book & Spreadsheet
    $main::excelbook = $main::excel->Workbooks->Open({FileName => $source_file, ReadOnly=>1})
         or die " Can't open $source_file";

    #################################
    $numOfSheets= scalar  spreadSheets;
    for $spreadSheet (@spreadSheets)
    {
    print "\n\nProcessing sheet $spreadSheet\n";
    $main::excelsheet = $main::excelbook->Worksheets($spreadSheet)
      or die " Can't open sheet $spreadSheet";
    $main::excelsheet->Activate();
    $numRows= $main::excelsheet->UsedRange->Rows->{Count};
    $numColumns= $main::excelsheet->UsedRange->Columns->{Count};

    $IpcCol=0;
    $CapsCol=0;
    $col=1;
    while( ($col <= $numColumns) and (($IpcCol == 0) or ($CapsCol == 0)) )
    {
     $nameOfCol= $main::excelsheet->Cells(1,$col)->{'Value'};

       if($nameOfCol =~ /IPC NAME/)
       {
           $CapsCol=$col;
       }
       elsif($nameOfCol =~ /IPC/)
       {
         $IpcCol=$col;
       }
     $col++;
    }; # end while for finding out relevant colimns
    $row=2;
    while( ($row < $numRows)  )
    {
    $ipc_num = $main::excelsheet->Cells($row,$IpcCol)->{'Value'};
    #printf "\n row: $row - ipc: $ipc_num";
      if($ipc_num ne "")
       {
        $capability = $main::excelsheet->Cells($row,$CapsCol)->{'Value'};
  	    $capability =~ s/,(.*)//;
        if($ipc_table{$ipc_num} ne "")
        {
          print "\nWarning - ipc $ipc_num is already defined with capability $ipc_table{$ipc_num} !";
        }
        $ipc_table{$ipc_num} = $capability;
        $ipc_num++;
       }
      $row++;
    };
    $main::excelsheet->Close();
    }; ## end for spreadSheets
  #################################
    $main::excel->ActiveWorkbook->Close(0);
    $main::excel->Quit();
  open(OUTPUT, ">$source_file$dest_file_ext") or die "Can't create output file: $!";
  @keys = sort {$a <=> $b} keys %ipc_table;
  for $key (@keys)
	{
  printf "\n IPC #$key ";
  $caps = $ipc_table{$key};
  for( $caps ){ s/^\s+//; s/\s+$//; }    # remove tleading & trailing white spaces
  print OUTPUT "\nif($caps != $key) printf(\"Warning, $caps != $key\");";
  printf "if($caps != $key) printf(\"Warning, $caps != $key\");";
  }
  close(OUTPUT);
}
else
{
 die "Bad arguments !";
}
