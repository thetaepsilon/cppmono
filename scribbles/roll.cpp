inline unsigned int roll_right(unsigned int v, unsigned short _amount) {
    // inb4 someone shoots me about sizeof() * 8 not being correct... I'm tired damnit
    static constexpr unsigned short modulus = sizeof(int) * 8;
    unsigned short amount_low = _amount % modulus;
    unsigned short amount_high = modulus - amount_low;
    return (v >> amount_low) | (v << amount_high);
}


unsigned int test(unsigned int v) {
    return roll_right(v, 5);
}

