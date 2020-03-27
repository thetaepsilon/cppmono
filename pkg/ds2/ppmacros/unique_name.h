#pragma once
#include <ds2/ppmacros/paste.h>

/*
Produces a hopefully reasonably unique name for variables/storage created by other macros.
x and label are respectively the "name" of something that refers to this anonymous name,
and a qualifying label that describes the category of this anonymous name (e.g. storage).
*/
#define __ds2__ppmacros__unique_name(x, label) \
	__ds2_ppmacros_paste8(__, x, , , __anon__line, __LINE__, __, label)

