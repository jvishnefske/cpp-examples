    #include <iostream>
    #include <streambuf>
    #include <locale>
    #include <cstdio>

    class outbuf : public std::streambuf
    {
      protected:
	/* central output function
	 * - print characters in uppercase mode
	 */
	int_type overflow (int_type c) override {
	    if (c != EOF) {
		// convert lowercase to uppercase
		c = std::toupper(static_cast<char>(c),getloc());

		// and write the character to the standard output
		if (putchar(c) == EOF) {
		    return EOF;
		}
	    }
	    return c;
	}
    };

    int main()
    {
	// create special output buffer
	outbuf ob;
	// initialize output stream with that output buffer
	std::ostream out(&ob);

	static constexpr int kHexValue = 31;
	out << "31 hexadecimal: "
	    << std::hex << kHexValue << std::endl;
	return 0;
    }
