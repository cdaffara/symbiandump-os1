#! /bin/perl
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
# readmrp - API to parse mrp files (but do no further processing)
# 
#

package ReadMrp;

sub New($)
	{
	my $proto = shift;
	my $class = ref($proto) || $proto;
	my $self = {};
	bless $self, $class;

	my ($fileName) = @_;
	$self->{filename} = $fileName;
	$self->Read();

	return $self;
	}

sub Read()
	{
	my $self = shift;
	my $fileName = $self->{filename};

	die "ERROR: MRP file '$fileName' does not exist\n" unless (-e $fileName);

	my $srcitems = [];
	my $binexpitems = [];
	my $component;
	my $notes;

	open MRP, "$fileName" or die "ERROR: Couldn't open '$fileName' for reading: $!\n";
	
	while (my $line = <MRP>)
		{
		chomp $line;

		$line =~ s/(?<!\\)#.*$//;  # remove comments
		$line =~ s/^\s+//;
		next if (!$line); # blank lines

		my @operands;

		my $string = $line;
		while ($string)
		{
			if ($string =~ s/^\"(.*?)\"// # Match and remove next quoted string
			or $string =~ s/^(.*?)\s+//   # or, match and remove next (but not last) unquoted string
		 	or $string =~ s/^(.*)\s*$//)  # or, match and remove last unquoted string.
			{
				push (@operands, $1);
				$string =~ s/^\s+//; # Remove delimiter if present.
			}
		}

		my $keyword = shift @operands;

		my $minus = ($keyword =~ s/^-//);

		if ($keyword eq "component")
			{
			die "-component is not a valid command in file '$fileName'\n" if $minus;
			$component = shift @operands;
			}
		elsif ($keyword eq "notes_source")
			{
			die "-notes_source is not a valid command in file '$fileName'\n" if $minus;
			$notes = shift @operands
			# N.B. This may require source mapping, so we don't check for existence here
			}
		elsif ($keyword eq "source")
			{
			die "-source is not supported by this parser yet in file '$fileName'\n" if $minus;
			my $srcItem = join ' ', @operands;
			push @$srcitems, $srcItem;
			}
		elsif ($keyword eq "binary")
			{
			if (scalar @operands == 1)
				{
				push @$binexpitems, shift @operands;
				}
			else
				{
				# This release doesn't handle bld.inf binary lines; no parsing here
				}
			}
		elsif ($keyword eq "testbinary")
			{
			if (scalar @operands == 1)
				{
				push @$binexpitems, shift @operands;
				}
			else
				{
				# This release doesn't handle bld.inf binary lines; no parsing here
				}
			}
		elsif ($keyword eq "exports")
			{
			# This release doesn't handle bld.inf exports lines; no parsing here
			}
		elsif ($keyword eq "testexports")
			{
			# This release doesn't handle bld.inf exports lines; no parsing here
			}
		elsif ($keyword eq "export_file")
			{
			push @$binexpitems, $operands[1];
			}
		elsif ($keyword eq "ipr")
			{
			# This release doesn't handle ipr lines; no parsing here
			}
		else
			{
			die "ERROR: In file '$fileName', command not understood in line: $line\n";
			}
		}
	die "ERROR: Component not specified in file '$fileName'\n" unless defined($component);
	die "ERROR: Notes_source not specified in file '$fileName'\n" unless defined($notes);
	$self->{srcitems} = $srcitems;
	$self->{component} = $component;
	$self->{binexpitems} = $binexpitems;
	$self->{notes} = $notes;
	}

sub GetComponent()
	{
	my $self = shift;
	return $self->{component};
	}

sub GetSrcItems()
	{
	my $self = shift;
	return $self->{srcitems};
	}

sub GetBinExpItems()
	{
	my $self = shift;
	return $self->{binexpitems};
	}

sub GetNotes()
	{
	my $self = shift;
	return $self->{notes};
	}

sub _SplitOnSpaces($)
	{
	my $self = shift;
	my ($operands) = (@_);
	
	# Break down operands
	my @operands = ();
	my $operand = "";
	my $first;
	while ($operands =~ /\S/)
		{
		$operands =~ /^(\s*\S+)(\s+.*)?$/ or die "Semantic error (broken regexp)";

		($first, $operands) = ($1, $2);
		$operand .= $first;

		$operand =~ s/^\s*//; # Remove preceding whitespace

		if (substr($operand,0,1) ne '"')
			{
			# Not quoted
			push @operands, $operand;
			$operand = "";
			}
		else
			{
			# Quoted
			if (substr($operand,scalar($operand-1),1) eq '"')
				{
				# Complete quoted operand
				$operand = substr ($operand, 1, scalar($operand-1));
				push @operands, $operand;
				$operand = "";
				}
			# Else leave the operand to have the next word added
			}
		}

	if ($operand ne "")
		{
		die "ERROR: Missing end quote from '$operand'\n";
		}
	
	return @operands;	
	}
1;

__END__

=pod

=head1 NAME

readmrp - Simple parser for MRP fils

=head1 SYNOPSIS

 use readmrp;

 my $mrpFile = '\someFolder\anMrpFile.mrp';

 # Create an instance of a readmrp object
 my $mrp = new readmrp($mrpFile);

 # Get data out
 my $name = $mrp->GetComponent();
 my @srcitems = @{$mrp->GetSrcItems()};
 my @binexpitems = @{$mrp->GetBinExpItems()};
 my $notessrc = $mrp->GetNotes();

=head1 DESCRIPTION

This module is used to parse MRP files and to store the basic data.  It does not do any further processing of the data, and so it deliberately does not depend on the referenced files being present.  It records source statements, as well as simple binary statements and the target of export_file statements.  It ignores exports and complex binary statements, which refer to a group directory and would require further processing to determine their targets.

=head1 METHODS

=head2 New (mrpFile)

Constructor.  Takes an MRP filename, and immediately parses it.  Dies if there is a syntax error in the MRP file.

=head2 GetComponent ()

Returns the parsed component name.

=head2 GetSrcItems ()

Returns an array ref of the source paths of the 'source' statements in the MRP file.

=head2 GetBinExpItems ()

Returns an array ref of the target paths of the simple 'binary' and 'export_file' statements in the MRP file.  It does not distringuish between binary files and exports.

=head2 GetNotes ()

Returns the path of the 'notes_source' file.

=head1 COPYRIGHT

Copyright (c) 2004-2007 Symbian Software Ltd. All Rights Reserved.

=cut
