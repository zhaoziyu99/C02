#include "catch2/catch.hpp"

#include "instruction/instruction.h"
#include "tokenizer/tokenizer.h"
#include "analyser/analyser.h"
#include "simple_vm.hpp"

#include <sstream>
#include <vector>

std::ostream& output = std::cout;

TEST_CASE("1", "[valid]") {
	std::string input =
		"int cnt = 0;\n"
		"void move(int id, int from, int to) {\n"
		"cnt = cnt + 1;\n"
		"print(cnt, id, from, to);\n"
		"}\n"
		"void hanoi(int n, int x, int y, int z) {\n"
		"if (n == 0)\n"
		"return;\n"
		"hanoi(n - 1, x, z, y);\n"
		"move(n, x, z);\n"
		"hanoi(n - 1, y, x, z);\n"
		"}\n"
		"int main()\n"
		"{\n"
		"int n;\n"
		"cnt = 0;\n"
		"scan(n);\n"
		"hanoi(n, 1, 2, 3);\n"
		"return 0;\n"
		"}\n"
		;
	std::stringstream ss;
	ss.str(input);
	miniplc0::Tokenizer tkz(ss);
	auto tokens = tkz.AllTokens();
	if (tokens.second.has_value())
		FAIL();
	miniplc0::Analyser analyser(tokens.first);
	auto instructions = analyser.Analyse(output, false);
	REQUIRE(!(instructions.second.has_value()));
}