#!/usr/bin/env lua5.1
--[[
given a list of files read into stdin,
filter through them looking for files ending in .test.hpp or .test.$version.hpp,
and decide which of these files gets compiled by a given test pass,
where each test pass in turn compile-tests each input filename against a specific standard revision.
the grouped filenames then get written to more files in a directory specified on the command line,
one file per standard version.

there exist plans to interpret $version == range specially, and read config files,
however this won't be implemented at first. probably not until needed either.
]]

