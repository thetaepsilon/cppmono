#!/bin/sh
set -eu;
expected_dir="$1";
actual_dir="$2";

log() {
	echo "#" "$@" >&2;
}

compare() {
	_name="$1";
	_input="./$_name";
	_actual="$actual_dir/$_name";
	test -e "$_actual" || {
		log "expected file $_name missing in output data.";
		return 1;
	};
	diff "$_input" "$_actual" >&2 || {
		e=$?;
		log "^ note: file data difference detected in $_name";
		return $e;
	};
}

# for every expected file, ensure that file exists in the output,
# and that it's contents match what we expect.
# diff will print out what we need to see if there is a difference.
(
	cd "$expected_dir";
	# note: for in in *... would remain unexpanded if the directory is empty,
	# causing an error as * then isn't a file that exists. >_>
	ls -1 | (IFS='
';
		while read i; do compare "$i"; done;
	);
);

# then, for every file we actually got, check we expect such a file.
# some of this is overlap admittedly but there's not really a better solution in a shell script.
# this will catch the script under test creating files (with or without data)
# that wasn't supposed to be there.
(cd "$actual_dir"; ls -1) | (IFS='
';
	while read i; do {
		test -e "$expected_dir/$i" || {
			log "$i existed in output data but was not expected.";
			exit 1;
		};
	}; done;
);


