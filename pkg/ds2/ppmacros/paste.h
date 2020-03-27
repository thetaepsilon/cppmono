#pragma once
/* This will likely need new entries for differing numbers of arguments. */


#define __ds2_ppmacros_paste8___(x, y, z, a, b, c, d, e) \
    x ## y ## z ## a ## b ## c ## d ## e

#define __ds2_ppmacros_paste8(x, y, z, a, b, c, d, e) \
	__ds2_ppmacros_paste8___(x, y, z, a, b, c, d, e)

