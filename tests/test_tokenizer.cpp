//#include "catch2/catch.hpp"
//#include "tokenizer/tokenizer.h"
//#include "fmt/core.h"
//
//#include <sstream>
//#include <vector>
//
//TEST_CASE("invalid_characters", "[invalid]") {
//	std::string input = "!\"#$%&',.:<>?@[\\]^_`{|}~";
//	std::stringstream ss;
//	ss.str(input);
//	miniplc0::Tokenizer tkz(ss);
//	auto result = tkz.AllTokens();
//	REQUIRE((result.second.has_value()));
//}
//
//TEST_CASE("empty_file", "[valid]") {
//	std::string input =
//		"";
//	std::stringstream ss;
//	ss.str(input);
//	miniplc0::Tokenizer tkz(ss);
//	auto result = tkz.AllTokens();
//	if (result.second.has_value())
//		FAIL();
//	std::vector<miniplc0::Token> output = {};
//	REQUIRE((result.first == output));
//}
//
//TEST_CASE("identifiers", "[valid]") {
//	std::string input =
//		"a\n"
//		"A\n"
//		"abc\n"
//		"ABC\n"
//		"Abc\n"
//		"aBc\n"
//		"aaawiogfpiusaGPIFsbfbpiweifgbpIAEGPFIewpifgpibpijgbpijbgpbijpbIPJabipPDP\n"
//		"a1\n"
//		"a0989852\n"
//		"A5\n"
//		"A21646452\n"
//		"a2431A\n"
//		"a5s6sa89sa9asf5asf98asf5789asf5789asf9587\n"
//		"a7dt b87TR8D sr780sA7D089 TS87tdxb08 TX08tn\n"
//		"d70SADT087 satdx697R  NX9X2141sga2asfEN08qw\n"
//		"\n"
//		"\n"
//		"BEGIN\n"
//		"END\n"
//		"CONST\n"
//		"VAR\n"
//		"PRINT\n"
//		"\n"
//		"BeGiN\n"
//		"eNd\n"
//		"CONst\n"
//		"vaR\n"
//		"priNT\n"
//		"\n"
//		"beginend\n"
//		"beginEND\n"
//		"CONSTvar\n"
//		"begin123456end\n"
//		"print987654321\n"
//		"const0\n"
//		"var1\n"
//		;
//	std::stringstream ss;
//	ss.str(input);
//	miniplc0::Tokenizer tkz(ss);
//	auto result = tkz.AllTokens();
//	if (result.second.has_value())
//		FAIL();
//	std::vector<miniplc0::Token> output = {};
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("a"), 0, 0, 0, 1));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("A"), 1, 0, 1, 1));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("abc"), 2, 0, 2, 3));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("ABC"), 3, 0, 3, 3));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("Abc"), 4, 0, 4, 3));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("aBc"), 5, 0, 5, 3));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("aaawiogfpiusaGPIFsbfbpiweifgbpIAEGPFIewpifgpibpijgbpijbgpbijpbIPJabipPDP"), 6, 0, 6, 72));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("a1"), 7, 0, 7, 2));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("a0989852"), 8, 0, 8, 8));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("A5"), 9, 0, 9, 2));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("A21646452"), 10, 0, 10, 9));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("a2431A"), 11, 0, 11, 6));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("a5s6sa89sa9asf5asf98asf5789asf5789asf9587"), 12, 0, 12, 41));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("a7dt"), 13, 0, 13, 4));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("b87TR8D"), 13, 5, 13, 12));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("sr780sA7D089"), 13, 13, 13, 25));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("TS87tdxb08"), 13, 26, 13, 36));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("TX08tn"), 13, 37, 13, 43));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("d70SADT087"), 14, 0, 14, 10));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("satdx697R"), 14, 11, 14, 20));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("NX9X2141sga2asfEN08qw"), 14, 22, 14, 43));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("BEGIN"), 17, 0, 17, 5));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("END"), 18, 0, 18, 3));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("CONST"), 19, 0, 19, 5));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("VAR"), 20, 0, 20, 3));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("PRINT"), 21, 0, 21, 5));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("BeGiN"), 23, 0, 23, 5));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("eNd"), 24, 0, 24, 3));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("CONst"), 25, 0, 25, 5));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("vaR"), 26, 0, 26, 3));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("priNT"), 27, 0, 27, 5));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("beginend"), 29, 0, 29, 8));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("beginEND"), 30, 0, 30, 8));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("CONSTvar"), 31, 0, 31, 8));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("begin123456end"), 32, 0, 32, 14));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("print987654321"), 33, 0, 33, 14));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("const0"), 34, 0, 34, 6));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("var1"), 35, 0, 35, 4));
//	REQUIRE((result.first == output));
//}
//
//TEST_CASE("integers", "[valid]") {
//	std::string input =
//		"0\n"
//		"9\n"
//		"0x1234\n"
//		"23456\n"
//		"98765\n"
//		"0X000234\n"
//		"1111111111\n";
//	std::stringstream ss;
//	ss.str(input);
//	miniplc0::Tokenizer tkz(ss);
//	auto result = tkz.AllTokens();
//	if (result.second.has_value())
//		FAIL();
//	std::vector<miniplc0::Token> output = {};
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::DECIMAL, 0, 0, 0, 0, 1));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::DECIMAL, 9, 1, 0, 1, 1));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::HEXADECIMAL, 4660, 2, 0, 2, 6));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::DECIMAL, 23456, 3, 0, 3, 5));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::DECIMAL, 98765, 4, 0, 4, 5));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::HEXADECIMAL, 564, 5, 0, 5, 8));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::DECIMAL, 1111111111, 6, 0, 6, 10));
//	REQUIRE((result.first == output));
//}
//
//TEST_CASE("keywords", "[valid]") {
//	std::string input =
//		"const\n"
//		"int\n"
//		"struct\n"
//		"if\n"
//		"else";
//	std::stringstream ss;
//	ss.str(input);
//	miniplc0::Tokenizer tkz(ss);
//	auto result = tkz.AllTokens();
//	if (result.second.has_value())
//		FAIL();
//	std::vector<miniplc0::Token> output = {};
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::CONST, std::string("const"), 0, 0, 0, 5));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::INT, std::string("int"), 1, 0, 1, 3));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::STRUCT, std::string("struct"), 2, 0, 2, 6));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IF, std::string("if"), 3, 0, 3, 2));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::ELSE, std::string("else"), 4, 0, 4, 4));
//	REQUIRE((result.first == output));
//}
//
//TEST_CASE("no_ws_at_the_end1", "[valid]") {
//	std::string input =
//		"//annotation\n";
//	std::stringstream ss;
//	ss.str(input);
//	miniplc0::Tokenizer tkz(ss);
//	auto result = tkz.AllTokens();
//	if (result.second.has_value())
//		FAIL();
//	std::vector<miniplc0::Token> output = {};
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::SINGLE_ANNOTATION, std::string("//annotation"), 0, 0, 1, 0));
//	REQUIRE((result.first == output));
//}
//
//TEST_CASE("no_ws_at_the_end2", "[valid]") {
//	std::string input =
//		";";
//	std::stringstream ss;
//	ss.str(input);
//	miniplc0::Tokenizer tkz(ss);
//	auto result = tkz.AllTokens();
//	if (result.second.has_value())
//		FAIL();
//	std::vector<miniplc0::Token> output = {};
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::SEMICOLON, ';', 0, 0, 0, 1));
//	REQUIRE((result.first == output));
//}
//
//TEST_CASE("operators", "[valid]") {
//	std::string input =
//		"=\n";
//	std::stringstream ss;
//	ss.str(input);
//	miniplc0::Tokenizer tkz(ss);
//	auto result = tkz.AllTokens();
//	if (result.second.has_value())
//		FAIL();
//	std::vector<miniplc0::Token> output = {};
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::ASSIGN_SIGN, '=', 0, 0, 0, 1));
//	REQUIRE((result.first == output));
//}
//
//TEST_CASE("parentheses", "[valid]") {
//	std::string input =
//		"(\n"
//		")\n"
//		"(()))(";
//	std::stringstream ss;
//	ss.str(input);
//	miniplc0::Tokenizer tkz(ss);
//	auto result = tkz.AllTokens();
//	if (result.second.has_value())
//		FAIL();
//	std::vector<miniplc0::Token> output = {};
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::LEFT_BRACKET, '(', 0, 0, 0, 1));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::RIGHT_BRACKET, ')', 1, 0, 1, 1));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::LEFT_BRACKET, '(', 2, 0, 2, 1));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::LEFT_BRACKET, '(', 2, 1, 2, 2));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::RIGHT_BRACKET, ')', 2, 2, 2, 3));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::RIGHT_BRACKET, ')', 2, 3, 2, 4));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::RIGHT_BRACKET, ')', 2, 4, 2, 5));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::LEFT_BRACKET, '(', 2, 5, 2, 6));
//	REQUIRE((result.first == output));
//}
//
//TEST_CASE("semicolons", "[valid]") {
//	std::string input =
//		";\n"
//		";;;";
//	std::stringstream ss;
//	ss.str(input);
//	miniplc0::Tokenizer tkz(ss);
//	auto result = tkz.AllTokens();
//	if (result.second.has_value())
//		FAIL();
//	std::vector<miniplc0::Token> output = {};
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::SEMICOLON, ';', 0, 0, 0, 1));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::SEMICOLON, ';', 1, 0, 1, 1));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::SEMICOLON, ';', 1, 1, 1, 2));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::SEMICOLON, ';', 1, 2, 1, 3));
//	REQUIRE((result.first == output));
//}
//
//TEST_CASE("test", "[valid]") {
//	std::string input =
//		"int intMin = -2147483647-1;\n"
//		"const intMIN = - 2147483647 - 1;";
//	std::stringstream ss;
//	ss.str(input);
//	miniplc0::Tokenizer tkz(ss);
//	auto result = tkz.AllTokens();
//	if (result.second.has_value())
//		FAIL();
//	std::vector<miniplc0::Token> output = {};
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::INT, std::string("int"), 0, 0, 0, 3));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("intMin"), 0, 4, 0, 10));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::ASSIGN_SIGN, '=', 0, 11, 0, 12));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::MINUS_SIGN, '-', 0, 13, 0, 14));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::DECIMAL, 2147483647, 0, 14, 0, 24));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::MINUS_SIGN, '-', 0, 24, 0, 25));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::DECIMAL, 1, 0, 25, 0, 26));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::SEMICOLON, ';', 0, 26, 0, 27));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::CONST, std::string("const"), 1, 0, 1, 5));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::IDENTIFIER, std::string("intMIN"), 1, 6, 1, 12));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::ASSIGN_SIGN, '=', 1, 13, 1, 14));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::MINUS_SIGN, '-', 1, 15, 1, 16));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::DECIMAL, 2147483647, 1, 17, 1, 27));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::MINUS_SIGN, '-', 1, 28, 1, 29));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::DECIMAL, 1, 1, 30, 1, 31));
//	output.emplace_back(miniplc0::Token(miniplc0::TokenType::SEMICOLON, ';', 1, 31, 1, 32));
//	REQUIRE((result.first == output));
//}
//
//TEST_CASE("whitespaces", "[valid]") {
//	std::string input =
//		"   	     \n"
//		"   \n"
//		"\n"
//		"\n"
//		" \n"
//		"                     \n"
//		"       \n";
//	std::stringstream ss;
//	ss.str(input);
//	miniplc0::Tokenizer tkz(ss);
//	auto result = tkz.AllTokens();
//	if (result.second.has_value())
//		FAIL();
//	std::vector<miniplc0::Token> output = {};
//	REQUIRE((result.first == output));
//}