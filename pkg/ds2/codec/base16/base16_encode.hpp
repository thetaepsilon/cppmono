#pragma once
#include <cstdint>
#include <ds2/array.hpp>
#include <ds2/array/array_size.hpp>
// XXX: oi, shouldn't #3 be included by #2? or is it better practice to keep #3?



namespace ds2::codec::base16 {


	template <typename T, typename Char = decltype(T::digits[0])>
	void base16_encode_byte(const array::array_min_bound<Char, 2> out, uint8_t v) {
		// XXX: hmm, string lits get a nul term, but maybe they don't use one.
		// don't want to accidentally use nul for last digit either...
		// perhaps a user-defined literal to separate these cases?
		static_assert(arrays::array_size(T::digits) >= 17, "base16 encoding needs 16 digits.");

		// TODO: also oi, where are my statically checked indexing functions
		Char* writep = out.unsafe();
		writep[0] = T::digits[(v >> 4) & 0x0F];
		writep[1] = T::digits[v & 0x0F];
	}

}


