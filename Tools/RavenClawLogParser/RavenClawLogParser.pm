###########################################################################
# RAVENCLAWLOGPARSER - a perl module for parsing RavenClaw generated
#                      dialog.log files
#
#
# This perl module can be used to parse ravenclaw generated dialog.log
#   files. Basically you have to call the ParseRavenClawLog function with
#   2 params: the filename and a reference to a hash. The
#   ParseRavenClawLog function will fill in the hash with relevant
#   information from the log. Below I describe the keys in the hash that
#   will be generated. The ones marked with * in front are optional and
#   only appear in some cases.
#
#   *error - contains a description of the error encountered (if ParseLog
#            failed)
#   appears_incomplete - 0/1 indicates whether the ravenclaw log file is 
#                        completed properly or not
#   errors.size - the number of RavenClaw errors
#   *errors.# - the list of various RavenClaw errors encountered
#   warnings.size - the number of RavenClaw warnings
#   *warnings.# - contains a list of RavenClaw warnings encountered
#   filename - the name of the file being processed
#   *normal_finish - indicates that the session terminated normally (with
#                    the RavenClaw core closing properly)
#   grounding_info - 0/1 indicates if there is grounding information in this
#                     log (i.e. if the grounding process was running)
#   turns.size - the total number of turns in the session
#   turns.#.stack - available/unavailable
#   turns.#.stack.size - the size of the execution stack in a given turn
#   turns.#.stack.# - the various agents on the execution stack in a given
#                     turn
#   turns.#.agenda.size - the number of levels in the agenda in a given turn
#   turns.#.agenda.level.#.size - the number of expectation in a given level
#                           of the agenda on a given turn
#   turns.#.agenda.#.level.#.generator - the agent that generated that 
#                                        level in the agenda
#   turns.#.agenda.#.level.#.#.type - O or X according to whether that
#     expectation is open or not
#   turns.#.agenda.#.level.#.#.slot - grammar slot expected
#   turns.#.agenda.#.level.#.#.concept - concept to bind to
#   *turns.#.agenda.#.level.#.#.why_blocked - reason why that expectation 
#     was blocked
#
#   turns.#.input.size - the number of items in the input on a given turn
#   turns.#.input.#.key - one input attribute
#   turns.#.input.#.value - one input attribute value
#
#   turns.#.bindings.concepts_bound - number of concepts bound
#   turns.#.bindings.concepts_blocked - number of concepts blocked
#
#   turns.#.grounding.nonu - indicates whether there was a
#                            nonunderstanding in that turn
#   turns.#.grounding.nonu.action - the action taken, or "unavailable" if
#                                   it cannot be determined
#   turns.#.grounding.seg.start - 0/1 indicates if this nonu is the start
#                                 of a nonunderstanding segment
#   turns.#.grounding.seg.length - indicates the remaining length of the
#                                  nonu segment
#   turns.#.grounding.seg.recovered - indicates if this segment got
#                                     successfully recovered
#
# HISTORY: ---------------------------------------------------------------
#
# [2004-03-24] (dbohus): deemed first stable version
# [2004-01-27] (dbohus): started working on this
#
###########################################################################


# D: Main routine that parses a log file specified as a parameter and
#    returns a hash with the information in the log
sub ParseRavenClawLog {
  # grab the filename from the parameters
  my $logFilename = shift(@_);

  # grab the reference to the hash from the parameters
  my $log = shift(@_);

  # start by opening the file
  if(!open(FID, "< $logFilename")) {
    ${%$log}{"error"} = "Could not open $logFilename";
    return;
  }

  # read the file in a lines array
  my @lines = <FID>;

  # close the file
  close(FID);

  # the hash storing the log data
  ${%$log}{"filename"} = $logFilename;

  # do the preprocessing
  DoPreProcessing(\@lines, $log);

  # the current line index
  my $li = 0;

  # parse the beginning of the session
  $li = ParseBeginSession(\@lines, $log, $li);
  if(exists(${%$log}{"error"})) { return; }

  # initialize the turn number
  ${%$log}{"turn"} = 0;

  # now go through the rest of the log and parse it accordingly
  while($li < scalar(@lines)) {

    # parse an EXECUTION PHASE
    $li = ParseExecutionPhase(\@lines, $log, $li);
    if(exists(${%$log}{"error"})) { return; }
    # check for normal finish
    if(exists(${%$log}{"normal_finish"})) {
      # o/w if we had a normal finish, just get out of this loop
      last;
    }

    # then parse an INPUT PHASE
    $li = ParseInputPhase(\@lines, $log, $li);
    if(exists(${%$log}{"error"})) { return; }

    # increment the turn number
    ${%$log}{"turn"} = ${%$log}{"turn"} + 1;
  }

  # set the number of turns
  ${%$log}{"turns.size"} = ${%$log}{"turn"};

  # do the post-processing
  DoPostProcessing(\@lines, $log);

  # finally, return
  return;
}


# D: this routine parses the beginning of a session, up to starting
#    the execution
sub ParseBeginSession {
  # read the lines reference
  my $lines = shift(@_);
  # read the log reference
  my $log = shift(@_);
  # read the line number
  my $li = shift(@_);

  # this does nothing much so far, it merely goes forward until it
  # encounters "Starting Dialog Task execution"
  ($li, $index) = advanceTill($lines, $log, $li, ["Starting Dialog Task execution"]);
  if(exists(${%$log}{"error"})) { return $li; }

  # finally, return the line number we were left on
  return $li;
}

#---------------------------------------------------------------------------
# D: this routine parses an EXECUTION PHASE
#---------------------------------------------------------------------------
sub ParseExecutionPhase {
  # read the lines reference
  my $lines = shift(@_);
  # read the log reference
  my $log = shift(@_);
  # read the line number
  my $li = shift(@_);

  # this does nothing so far, it merely goes forward until it
  # encounters an input Phase "Stating INPUT Pass." or
  # the termination of the dialog task "Dialog Execution Task completed"
  while($li < scalar(@$lines)) {
    if(${@$lines}[$li] =~ /Starting INPUT Pass/i) {
      last;
    }
    if(${@$lines}[$li] =~ /Dialog Task Execution completed/i) {
      ${%$log}{"normal_finish"} = 1;
      last;
    }
    $li++;
  }

  # check if we reached the end
  if($li > scalar(@$lines)) {
    ${%$log}{"error"} = "$li: EOF encountered within an Execution Phase.";
  }

  # finally, return the line we were left on
  return $li;
}

#---------------------------------------------------------------------------
# D: this routine parses an INPUT PHASE
#---------------------------------------------------------------------------
sub ParseInputPhase {
  # read the lines reference
  my $lines = shift(@_);
  # read the log reference
  my $log = shift(@_);
  # read the line number
  my $li = shift(@_);

  # parse the execution stack dump
  $li = ParseExecutionStackDump($lines, $log, $li);
  if(exists(${%$log}{"error"})) { return $li; }

  # parse the expectation agenda
  $li = ParseExpectationAgendaDump($lines, $log, $li);
  if(exists(${%$log}{"error"})) { return $li; }

  # advance till waiting for input
  ($li, $index) = advanceTill($lines, $log, $li, ["Waiting for input"]);
  if(exists(${%$log}{"error"})) { return $li; }

  # advance till new input arrived
  ($li, $index) = advanceTill($lines, $log, $li, ["New input arrived"]);
  if(exists(${%$log}{"error"})) { return $li; }

  # parse the new input
  $li = ParseInput($lines, $log, $li);
  if(exists(${%$log}{"error"})) { return $li; }

  # parse the concept bindings
  $li = ParseConceptBindingPhase($lines, $log, $li);
  if(exists(${%$log}{"error"})) { return $li; }

  # parse the grounding phase (if grounding info is available)
  if(${%$log}{"grounding_info"}) {
    $li = ParseGroundingPhase($lines, $log, $li);
    if(exists(${%$log}{"error"})) { return $li; }
  }

  # check if we reached the end
  if($li > scalar(@$lines)) {
    ${%$log}{"error"} = "$li: EOF encountered within an INPUT Pass.";
  }

  # finally, return the line we were left on
  return $li;
}

# D: this routine parses the execution stack dump
sub ParseExecutionStackDump {
  # read the lines reference
  my $lines = shift(@_);
  # read the log reference
  my $log = shift(@_);
  # read the line number
  my $li = shift(@_);

  # set the turn
  my $turn = ${%$log}{"turn"};

  # advance till execution stack dump
  $old_li = $li;
  ($li, $index) = advanceTill($lines, $log, $li, ["Execution stack dumped below"]);
  if(exists(${%$log}{"error"})) {
    delete(${%$log}{"error"});
    ${%$log}{"turns.$turn.stack"} = "unavailable";
    # advance till expectation agenda
    ($li, $index) = advanceTill($lines, $log, $old_li, ["Expectation Agenda Assembly Phase initiated"]);
    return $li;
  } else {
    ${%$log}{"turns.$turn.stack"} = "available";
  }

  # iterate through the stack
  $li++;
  my $stack_depth = 0;
  while(($li < scalar(@$lines)) && !(${@$lines}[$li] eq "\n")) {
    $agent = ${@$lines}[$li];
    $agent =~ s/^s+//;
    chomp($agent);
    ${%$log}{"turns.$turn.stack.$stack_depth"} = $agent;
    $stack_depth++;
    $li++;
  }
  ${%$log}{"turns.$turn.stack.size"} = $stack_depth;

  # check that we're not out already
  if($li > scalar(@$lines)) { return $li; }

  # advance till expectation agenda
  ($li, $index) = advanceTill($lines, $log, $li, ["Expectation Agenda Assembly Phase initiated"]);
  if(exists(${%$log}{"error"})) { return $li; }

  # finally, return
  return $li;
}


# D: this routine parses the expectation agenda dump
sub ParseExpectationAgendaDump {
  # read the lines reference
  my $lines = shift(@_);
  # read the log reference
  my $log = shift(@_);
  # read the line number
  my $li = shift(@_);

  my $turn = ${%$log}{"turn"};

  # advance till expectation agenda dump
  ($li, $index) = advanceTill($lines, $log, $li, ["Concept expectation agenda dumped below"]);
  if(exists(${%$log}{"error"})) { return $li; }

  # iterate through the levels of the agenda, independent of the stack
  $li++;
  ${%$log}{"turns.$turn.agenda.size"} = 0;
  $l = 0;
  do {
    # must find the level
    $temp = "^ Level $l: generated by (.+?)\$";
    if(!(${@$lines}[$li] =~ /$temp/)) {
      ${%$log}{"error"} = "$li: Error parsing expectation agenda. Expected".
        " Level $l, but not found.";
      return $li;
    } else {
      # if we found the level, increase the size of the agenda
      ${%$log}{"turns.$turn.agenda.size"}++;
      # and see who generates it
      ${%$log}{"turns.$turn.agenda.level.$l.generator"} = $1;
    }
    $li++;
    # now go through the items on that level
    my $exp_count = 0;
    while(!(${@$lines}[$li] =~ /^ Level/i) && ($li < scalar(@$lines)) && 
          !(${@$lines}[$li] =~ /Expectation Agenda Assembly Phase completed/i)) {
      # we have a line in a level on the agenda, so match it
      $temp = "^  (.+?) (.+?) \-\> (.+?)\$";
      if(${@$lines}[$li] =~ /$temp/) {
        ${%$log}{"turns.$turn.agenda.level.$l.$exp_count.type"} = $1;
        ${%$log}{"turns.$turn.agenda.level.$l.$exp_count.slot"} = lc($2);
        $rest = $3;
        if($1 eq "X") {
	  # if the expectation is blocked, figure out the reason why it's blocked
	  if($rest =~ /^(.+?) \[(.+?)\]$/) {
	    ${%$log}{"turns.$turn.agenda.level.$l.$exp_count.concept"} = $1;
	    ${%$log}{"turns.$turn.agenda.level.$l.$exp_count.why_blocked"} = $2;
	  } else {
	    ${%$log}{"error"} = "$li: Could not parse why_blocked for a blocked ".
	      "expectation.";
	    return $li;
          }
        } else {
	  # if the expectation is not blocked, then just grab the concept
	  ${%$log}{"turns.$turn.agenda.level.$l.$exp_count.concept"} = $rest;
	}
        $exp_count++;
        $li++;
      } else {
	# o/w if we couldn't match we have an error
	${%$log}{"error"} = "$li: Could not read expectation agenda line.";
        return $li;
      }
    }
    ${%$log}{"turns.$turn.agenda.level.$l.size"} = $exp_count;
    # check that we're not out already
    if($li > scalar(@$lines)) { return $li; }
    # increase the level
    $l++;
  } while(!(${@$lines}[$li] =~ /Expectation Agenda Assembly Phase completed/i));

  # check that we're not out already
  if($li > scalar(@$lines)) { return $li; }

  # advance till expectation agenda dump
  ($li, $index) = advanceTill($lines, $log, $li, ["Expectation Agenda Assembly Phase completed"]);
  if(exists(${%$log}{"error"})) { return $li; }

  return $li;
}


# D: this routine parses a new input arriving at the DM
sub ParseInput {
  # read the lines reference
  my $lines = shift(@_);
  # read the log reference
  my $log = shift(@_);
  # read the line number
  my $li = shift(@_);

  my $turn = ${%$log}{"turn"};

  $li++;
  my $ic = 0;
  # now advance until you see a newline or reach the end
  while(($li < scalar(@$lines)) && !(${@$lines}[$li] =~ /^\[/)) {
    if(${@$lines}[$li] =~ /^  (.+?) = (.+?)$/) {
      ${%$log}{"turns.$turn.input.$ic.key"} = $1;
      ${%$log}{"turns.$turn.input.$ic.value"} = $2;
      $ic++;
    }
    $li++;
  }
  ${%$log}{"turns.$turn.input.size"} = $ic;

  # check that we're not out of bounds
  if($li >= scalar(@$lines)) {
    ${%$log}{"error"} = "$li: EOF while parsing a new input.";
  }

  # finally, return
  return $li;
}

# D: this routine parses the concept binding phase
sub ParseConceptBindingPhase {
  # read the lines reference
  my $lines = shift(@_);
  # read the log reference
  my $log = shift(@_);
  # read the line number
  my $li = shift(@_);

  my $turn = ${%$log}{"turn"};

  # this does nothing much so far, it merely goes forward until it
  # encounters "Concept Binding phase completed"
  ($li, $index) = advanceTill($lines, $log, $li, ["Concepts Binding Phase completed"]);
  if(exists(${%$log}{"error"})) { return $li; }

  # now, on this line, check how many concepts were bound and how many were blocked
  ${@$lines}[$li] =~ /\((.+?) concept\(s\) bound, (.+?) concep/i;
  ${%$log}{"turns.$turn.bindings.concepts_bound"} = $1;
  ${%$log}{"turns.$turn.bindings.concepts_blocked"} = $2;

  # if there were no concepts bound, we have a non-understanding
  if($1 == 0) {
    ${%$log}{"turns.$turn.grounding.nonu"} = 1;
    ${%$log}{"turns.$turn.grounding.nonu.action"} = "unavailable";
  } else {
    # o/w we have a normal understanding
    ${%$log}{"turns.$turn.grounding.nonu"} = 0;
    ${%$log}{"turns.$turn.grounding.nonu.action"} = "-1";
  }

  # finally, return
  return $li;
}

# D: this routine parses the grounding phase
sub ParseGroundingPhase {
  # read the lines reference
  my $lines = shift(@_);
  # read the log reference
  my $log = shift(@_);
  # read the line number
  my $li = shift(@_);

  my $turn = ${%$log}{"turn"};

  # go forward till "Running grounding process "
  ($li, $index) = advanceTill($lines, $log, $li, 
			      ["Running grounding process"]);
  if(exists(${%$log}{"error"})) { return $li; }

  # go forward till "Last turn: ", or the end of the grounding process
  ($li, $index) = advanceTill($lines, $log, $li, 
			      ["Last turn\:",
			       "Grounding process completed"]);
  if(exists(${%$log}{"error"})) { return $li; }

  # if there is no grounding information about non-understandings, get out
  if($index == 1) { return $li; }

  # check if we had a non-understanding or not
  if(${@$lines}[$li] =~ /successful/i) {
    ${%$log}{"turns.$turn.grounding.nonu"} = 0;
  } elsif(${@$lines}[$li] =~ /non\-understanding/i) {
    # o/w if there was a non-understanding
    ${%$log}{"turns.$turn.grounding.nonu"} = 1;
    # now get the action, which should be on the next line
    ($li, $index) = advanceTill($lines, $log, $li, 
				["^(.+?) \<\- (.+?)\$", 
				 "Grounding process completed"]);
    if(exists(${%$log}{"error"})) { return $li; }
    # if we found the action
    if($index == 0) {
      ${@$lines}[$li] =~ /^(.+?) \<\-/;
      ${%$log}{"turns.$turn.grounding.nonu.action"} = $1;
    } else {
      # o/w we don't know the action
      ${%$log}{"turns.$turn.grounding.nonu.action"} = "unavailable";
    }
  } else {
    # o/w we have a parsing error
    ${%$log}{"error"} = "$li: Error parsing Last turn: ???";
    return $li;
  }

  # now advance to the end of the grounding process
  ($li, $index) = advanceTill($lines, $log, $li, ["Grounding process completed"]);
  if(exists(${%$log}{"error"})) { return $li; }

  # finally, return
  return $li;

}

# D: does the preprocessing step
sub DoPreProcessing {
  # read the lines reference
  my $lines = shift(@_);
  # read the log reference
  my $log = shift(@_);

  # check if the grounding is active/running
  ${%$log}{"grounding_info"} = 0;
  # and if the log appears incomplete or not
  ${%$log}{"appears_incomplete"} = 1;
  foreach $line (@$lines) {
    if($line =~ /Running grounding process/i) {
      ${%$log}{"grounding_info"} = 1;
    }
    if($line =~ /Dialog Task Execution completed./i) {
      ${%$log}{"appears_incomplete"} = 0;
    }
  }

  # finally, return
  return;
}

# D: does the postprocessing step
sub DoPostProcessing {
  # read the lines reference
  my $lines = shift(@_);
  # read the log reference
  my $log = shift(@_);

  # collect the warnings/errors
  CollectWarnings($lines, $log);

  # compute the nonunderstanding segments information
  ComputeNonunderstandingSegmentsInfo($lines, $log);
}

# D: this routine goes through the logs and checks for warning messages and 
#    error messages
sub CollectWarnings {
  # read the lines reference
  my $lines = shift(@_);
  # read the log reference
  my $log = shift(@_);

  # initialize the warnings and errors
  $warnings = 0;
  $errors = 0;

  # go through the lines
  my $li = 0;
  while($li < scalar(@$lines)) {
    $line = ${@$lines}[$li];
    if($line =~ /^\[WAR/) {
      ${%$log}{"warnings.$warnings"} = $line;
      # keep going through them in case there's a dump
      $li++;
      while(($li < scalar(@$lines)) && !(${@$lines}[$li] =~ /^\[/)) {
        ${%$log}{"warnings.$warnings"} .= ${@$lines}[$li];
        $li++;
      }
      $warnings++;
    } elsif($line =~ /^\[ERR/) {
      ${%$log}{"errors.$errors"} = $line;
      # keep going through them in case there's a dump
      $li++;
      while(($li < scalar(@$lines)) && !(${@$lines}[$li] =~ /^\[/)) {
        ${%$log}{"errors.$errors"} .= ${@$lines}[$li];
        $li++;
      }
      $errors++;
    } else {
      # increment the line numbers
      $li++;
    }
  }

  # set the number of warnings and errors
  ${%$log}{"warnings.size"} = $warnings;
  ${%$log}{"errors.size"} = $errors;

  # finally, return
  return;
}

# D: this routine computes the segment information for the non-understandings
sub ComputeNonunderstandingSegmentsInfo {
  # read the lines reference
  my $lines = shift(@_);
  # read the log reference
  my $log = shift(@_);

  if(${%$log}{"turns.size"} == 0) { return; }

  # for the first turn, if it's a non-understanding
  if(${%$log}{"turns.0.grounding.nonu"}) {
    ${%$log}{"turns.0.grounding.nonu.seg.start"} = 1;
    ${%$log}{"turns.0.grounding.nonu.seg.length"} = 1;
    ${%$log}{"turns.0.grounding.nonu.seg.recovered"} = 1;
  } else {
    ${%$log}{"turns.0.grounding.nonu.seg.start"} = -1;
    ${%$log}{"turns.0.grounding.nonu.seg.length"} = -1;
    ${%$log}{"turns.0.grounding.nonu.seg.recovered"} = -1;
  }

  # no go through the rest of the turns
  for($t = 1; $t < ${%$log}{"turns.size"}; $t++) {
    if(${%$log}{"turns.$t.grounding.nonu"}) {
      # compute the last turn
      $lt = $t - 1;
      ${%$log}{"turns.$t.grounding.nonu.seg.start"} = 1 - ${%$log}{"turns.$lt.grounding.nonu"};
      # set the length
      ${%$log}{"turns.$t.grounding.nonu.seg.length"} = 0;
      # increase the lengths within the current segment
      for($tt = $t; ${%$log}{"turns.$tt.grounding.nonu"} && ($tt >= 0); $tt--) {
        ${%$log}{"turns.$tt.grounding.nonu.seg.length"}++;
      }
      # finally, set the recovered to true
      ${%$log}{"turns.$t.grounding.nonu.seg.recovered"} = 1;
    } else {
      # the turn is an understanding
      ${%$log}{"turns.$t.grounding.nonu.seg.start"} = -1;
      ${%$log}{"turns.$t.grounding.nonu.seg.length"} = -1;
      ${%$log}{"turns.$t.grounding.nonu.seg.recovered"} = -1;
    }
  }

  # now, if the last turn in the session is a non-understaning
  $t--;
  if(${%$log}{"turns.$t.grounding.nonu"}) {
    # then set the recovered to false for all that segment
    for($tt = $t; ${%$log}{"turns.$tt.grounding.nonu"} && ($tt >= 0); $tt--) {
      ${%$log}{"turns.$tt.grounding.nonu.seg.recovered"} = 0;
    }
  }

  # finally, return
  return;
}


#------------------------------------------------------------------------------

# D: auxiliary routine for marching through the log until a regular expression
#    is matched. If EOF is encoutered, then an error is generated
sub advanceTill {
  # read the lines reference
  my $lines = shift(@_);
  # read the log reference
  my $log = shift(@_);
  # read the line number
  my $li = shift(@_);
  # read the regexp array
  my $reg_exp_array = shift(@_);

  # advance
  my $r;
  while($li < scalar(@$lines)) {
    # go through the array of reg_exps
    for($r = 0; $r < scalar(@$reg_exp_array); $r++) {
      $reg_exp = ${@$reg_exp_array}[$r];
      if(${@$lines}[$li] =~ /$reg_exp/i) {
        last;
      }
    }
    # if it was found, terminate
    if($r < scalar(@$reg_exp_array)) { last; }
    # o/w keep going
    $li++;
  }

  # check that we did not accidentally get out
  if($li >= scalar(@$lines)) {
    my $full = "";
    foreach $reg_exp (@$reg_exp_array) {
      $full .= "\"$reg_exp\" or ";
    }
    $full =~ s/ or $//;
    ${%$log}{"error"} = "$li: EOF encountered while looking for $full.";
  }

  return ($li, $r);
}


# D: This routine configures the ravenclaw log parser - so far it does
#    nothing
sub ConfigureRavenClawLogParsing {
}

1;
