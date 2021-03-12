template <typename State, typename InSymbol, typename OutSymbol>
struct mealy_types {
	struct input {
		State state;
		InSymbol symbol;
	};
	struct output {
		State state;
		OutSymbol data;
	};
};

template <typename State, typename InSymbol, typename OutSymbol, typename Config>
struct mealy_machine {
private:
	State s;
	using types = mealy_types<State, InSymbol, OutSymbol>;
	using input = typename types::input;
	using output = typename types::output;
public:

	constexpr const State& state() { return s; }

	constexpr mealy_machine(): s(Config::initial_state) {}

	// some notes here:
	// while it might be more efficient to update state in-place,
	// forcing it to be a return value should help prevent "forgetting" to update it.
	// especially in the constexpr case where all code paths must initialise and return a value.
	constexpr OutSymbol step(InSymbol indata) {
		input i { s, indata };
		output result = Config::step(i);
		s = result.state;
		return result.data;
	}
};



enum class header_state {
	start_of_line,
	seen_cr,
	seen_second_cr,
	end_of_header,

	missing_cr,
	missing_second_cr,
	empty_request
};

enum class header_insym {
	normal,
	colon,
	space,
	cr,
	lf,
	eof
};

enum class header_outsym {
	none,
	append_field,
	append_value,
	emit
};

struct http_header_parser {
	constexpr static auto initial_state = header_state::start_of_line;
	using types = mealy_types<header_state, header_insym, header_outsym>;
	constexpr typename types::output step(typename types::input instate) {
		using s = header_state;
		using o = header_outsym;
		using i = header_insym;
	
		switch (instate.state) {
			// start with the easy ones that are a latched error or accept state.
			case s::empty_request:
			case s::missing_cr:
			case s::end_of_header:
				return { instate.state, o::none };

			case s::seen_second_cr: {
				// if we've seen the second cr of the ending \r\n\r\n sequence,
				// then we expect the next symbol to be \n.
				// if it's not, then we may have just eaten into the body,
				// AND it's a protocol violation anyway, so bail.
				// also still a protocol violation on end of file at this point.
				// however if it is LF that's the complete header, consume no more.
				// in either case, nothing more to do with any header lines.
				auto state = (instate.symbol == i::lf) ?
					s::end_of_header : s::missing_second_cr;
				return { state, o::none };
			}

			// WIP here...
		}
	}
};


