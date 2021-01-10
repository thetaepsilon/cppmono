template <typename State, typename InSymbol, typename OutSymbol, typename Config>
struct mealy_machine {
private:
	State s;
public:
	struct input {
		State state;
		InSymbol symbol;
	};
	struct output {
		State state;
		OutSymbol data;
	};

	constexpr const State& state() { return s; }

	constexpr mealy_machine(): s(Config::initial_state) {}

	constexpr OutSymbol step(InSymbol indata) {
		input i { s, indata };
		output result = Config::step(i);
		s = result.state;
		return result.data;
	}
};

