#!perl
# CommandInfoFile.pm
# 
# Copyright (c) 2010 Accenture. All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# 
# Initial Contributors:
# Accenture - Initial contribution
#

# Description:
# A Perl implementation of the C++ class CCommandInfoFile (in ioutils.dll).

# Note, this code is intended to behave the same as the C++ version. It therefore used C++ like ways
# doing things rather than Perl-like ways to keep the code as similar are possible.

package CommandInfoFile;

use strict;
use IO::File;
use File::Basename;


#
# Public Interface.
#

sub New {
  my $pkg = shift;
  my $fileName = shift;
  my $self = _Construct($pkg);
  $self->_ReadFile($fileName);
  return $self;
}

sub Name($) {
  my $self = shift;
  return $self->{name};
}

sub FullName($) {
  my $self = shift;
  my $name = shift;

  if (defined $name) {
    $name = "$self->{name} $name";
  }
  else {
    $name = $self->{name};
  }

  if ($self->{parent}) {
    $name = $self->{parent}->FullName($name);
  }

  return $name;
}

sub ShortDescription($) {
  my $self = shift;
  return $self->{short_description};
}

sub LongDescription($) {
  my $self = shift;
  return $self->{long_description};
}

sub SeeAlso($) {
  my $self = shift;
  return $self->{see_also};
}

sub Copyright($) {
  my $self = shift;
  return $self->{copyright};
}

sub Arguments($) {
  my $self = shift;
  return $self->{arguments};
}

sub Options($) {
  my $self = shift;
  return $self->{options};
}

sub NumSubCommands($) {
  my $self = shift;
  my $numSubCommands = 0;
  if (defined $self->{sub_commands}) {
    $numSubCommands = scalar (@{$self->{sub_commands}});
  }
  return $numSubCommands;
}

sub SubCommand($$) {
  my $self = shift;
  my $index = shift;
  die unless (($index >= 0) and ($index < $self->NumSubCommands()));
  return $self->{sub_commands}->[$index];
}


#
# Private.
#

sub _ReadFile($$) {
  my $self = shift;
  my $fileName = shift;

  my $file = IO::File->new($fileName) or die "Error: Couldn't open '$fileName' for reading: $!\n";
  my $pos = 0;
  my $fileLength = -s $fileName;

  while ($pos < $fileLength) {
    if ($self->{process_include} or not defined $self->{current_child}) {
      $self->_ReadDetails($file, $fileName);
    }
    else {
      $self->{current_child}->_ReadDetails($file, $fileName);
    }
    $pos = $file->tell();
  }
  close ($file);
}

sub _ReadDetails($$$) {
  my $self = shift;
  my $file = shift;
  my $fileName = shift;

  my $pos = $file->tell();
  TextToNextCommand($file); # Ignore everything before the first '==' command.
  while (my $line = <$file>) {
    if ($line =~ /^==name\s+(\S+)/) {
      $self->{name} = $1;
    }
    elsif ($line =~ /^==short-description\s*$/) {
      $self->{short_description} = TextToNextCommand($file);
    }
    elsif ($line =~ /^==long-description\s*$/) {
      $self->{long_description} = TextToNextCommand($file);
    }
    elsif ($line =~ /^==see-also\s*$/) {
      $self->{see_also} = TextToNextCommand($file);
    }
    elsif ($line =~ /^==copyright\s*$/) {
      $self->{copyright} = TextToNextCommand($file);
    }
    elsif ($line =~ /^==argument\s+(.*)$/) {
      push (@{$self->{arguments}}, ReadArgument($file, $1));
    }
    elsif ($line =~ /^==option\s+(.*)$/) {
      push (@{$self->{options}}, ReadOption($file, $1));
    }
    elsif ($line =~ /^==include\s+(.*)$/) {
      if (not exists $self->{parent}) {
	$self->{process_include} = 0;
	my $includeFileName = dirname($fileName) . "/$1";
	$self->_ReadFile($includeFileName);
	last;
      }
      else {
	# We're a sub-command. Let control return to the root to handle the include.
	$self->{parent}->_ProcessInclude($self);
	$file->seek($pos, 0);
	last;
      }
    }
    elsif ($line =~ /^==sub-command\s+(.*)$/) {
      if (not exists $self->{parent}) {
	my @subCommandNames = split (/\s+/, $1);
	$self->_AddSubCommand(\@subCommandNames, $file, $fileName);
      }
      else {
	# We're a sub-command. Let control return to the root to handle the include.
	$self->{parent}->_ProcessNewChild();
	$file->seek($pos, 0);
	last;
      }
    }

    $pos = $file->tell();
  }
}

sub _ProcessNewChild($) {
  my $self = shift;

  if ($self->{parent}) {
    $self->{parent}->_ProcessNewChild();
  }
  else {
    die if ($self->{process_include});
    undef $self->{current_child};
  }
}

sub _ProcessInclude($$) {
  my $self = shift;
  my $child = shift;

  if ($self->{parent}) {
    $self->{parent}->_ProcessInclude($child);
  }
  else {
    $self->{process_include} = 1;
    $self->{current_child} = $child;
  }
}

sub _AddSubCommand($$$$) {
  my $self = shift;
  my $subCommandNames = shift;
  my $file = shift;
  my $fileName = shift;
  my $subCommandName = shift @$subCommandNames;

  my $found = 0;
  for (my $i = ($self->NumSubCommands() - 1); $i >= 0; --$i) {
    if ($self->{sub_commands}->[$i]->{name} eq $subCommandName) {
      $self->{sub_commands}->[$i]->_AddSubCommand($subCommandNames, $file, $fileName);
      $found = 1;
      last;
    }
  }

  die unless ($found or (@$subCommandNames == 0));

  if (not $found) {
    my $newCif = _Construct('CommandInfoFile');
    $newCif->{name} = $subCommandName;
    $newCif->{parent} = $self;
    $newCif->_ReadDetails($file, $fileName);
    push (@{$self->{sub_commands}}, $newCif);
  }
}

sub _Construct($) {
  my $pkg = shift;
  my $self = {};
  bless $self, $pkg;

  push (@{$self->{options}}, {
			      type => 'bool',
			      short_name => 'h',
			      long_name => 'help',
			      description => 'Display help.'
			     });

  return $self;
}

sub ReadArgument($$) {
  my $file = shift;
  my @args = split (/\s+/, shift);

  my $argumentEntry = {};

  $argumentEntry->{type} = shift @args;
  $argumentEntry->{name} = shift @args;

  foreach my $arg (@args) {
    if ($arg eq 'optional') {
      $argumentEntry->{flags}->{optional} = 1;
    }
    elsif ($arg eq 'multiple') {
      $argumentEntry->{flags}->{multiple} = 1;
    }
    elsif ($arg eq 'last') {
      $argumentEntry->{flags}->{last} = 1;
    }
    else {
      $argumentEntry->{env_var} = $arg;
    }
  }

  die "Error: Argument missing type\n" unless defined $argumentEntry->{type};
  die "Error: Argument missing name\n" unless defined $argumentEntry->{name};

  $argumentEntry->{description} = TextToNextCommand($file);
  $argumentEntry->{description} =~ s/\s*$//;

  if ($argumentEntry->{type} eq 'enum') {
    $argumentEntry->{enum_values} = GetEnumValues($file);
  }

  return $argumentEntry;
}

sub ReadOption($$) {
  my $file = shift;
  my @args = split (/\s+/, shift);

  my $optionEntry = {};

  $optionEntry->{type} = shift @args;
  $optionEntry->{short_name} = shift @args;
  $optionEntry->{long_name} = shift @args;

  foreach my $arg (@args) {
    if ($arg eq 'multiple') {
      $optionEntry->{flags}->{multiple} = 1;
    }
    else {
      $optionEntry->{env_var} = $arg;
    }
  }

  die "Error: Option missing type\n" unless defined $optionEntry->{type};
  die "Error: Option missing short name\n" unless defined $optionEntry->{short_name};
  die "Error: Option short name not a single character\n" unless length ($optionEntry->{short_name}) == 1;
  die "Error: Option missing long name\n" unless defined $optionEntry->{long_name};

  $optionEntry->{description} = TextToNextCommand($file);

  if ($optionEntry->{type} eq 'enum') {
    $optionEntry->{enum_values} = GetEnumValues($file);
  }

  return $optionEntry;
}

sub GetEnumValues($) {
  my $file = shift;

  my @values;

  my $pos = $file->tell();
  while (my $line = <$file>) {
    if ($line =~ /^==enum-value\s+(\S+)/) {
      my $value = $1;
      my $description = TextToNextCommand($file);
      push (@values, {
		      value => $value,
		      description => $description
		     });
    }
    else {
      $file->seek ($pos, 0);
      last;
    }
    $pos = $file->tell();
  }

  return \@values;
}

sub TextToNextCommand($) {
  my $file = shift;
  my $text = '';
  my $pos = $file->tell();
  while (my $line = <$file>) {
    if ($line =~ /^==/) {
      $file->seek($pos, 0);
      last;
    }
    else {
      $text .= $line;
    }
    $pos = $file->tell();
  }
  return $text;
}

1;
