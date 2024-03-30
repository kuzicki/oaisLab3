#include "pch.h"
#include "CppUnitTest.h"
#include "Logical.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LogicTest
{
	TEST_CLASS(LogicTest)
	{
	public:

		TEST_METHOD(RPNTest1)
		{
			std::string input = "(a|b)&!c";
			std::set<char> variables;
			std::string RPNResult = Logical::RPN(input, variables);
			std::string expected = "ab|c!&";
			Assert::AreEqual(expected, RPNResult);
		}

		TEST_METHOD(RPNTest2)
		{
			std::string input = "(a&b)->c";
			std::set<char> variables;
			std::string RPNResult = Logical::RPN(input, variables);
			std::string expected = "ab&c-";
			Assert::AreEqual(expected, RPNResult);
		}

		TEST_METHOD(RPNTest3)
		{
			std::string input = "(a~b)&c";
			std::set<char> variables;
			std::string RPNResult = Logical::RPN(input, variables);
			std::string expected = "ab~c&";
			Assert::AreEqual(expected, RPNResult);
		}

		TEST_METHOD(FormsCheck1) {
			std::string input = "(a|b)&!c";
			std::set<char> variables;
			std::string RPNResult = Logical::RPN(input, variables);
			std::vector<std::vector<bool>> table = Logical::getTable(RPNResult, variables);
			std::pair<int, std::string> indexForm = Logical::getIndexForm(table);
			std::string expectedBinary = "00101010";
			int expected = 42;
			std::string expectedD = "(2,4,6) |";
			std::string expectedC = "(0,1,3,5,7) &";
			Assert::AreEqual(expectedBinary, indexForm.second);
			Assert::AreEqual(expected, indexForm.first);
			Assert::AreEqual(expectedD, Logical::getDecimalFormD(table));
			Assert::AreEqual(expectedC, Logical::getDecimalFormC(table));
		}

		TEST_METHOD(FormsCheck2) {
			std::string input = "(a&b)->c";
			std::set<char> variables;
			std::string RPNResult = Logical::RPN(input, variables);
			std::vector<std::vector<bool>> table = Logical::getTable(RPNResult, variables);
			std::pair<int, std::string> indexForm = Logical::getIndexForm(table);
			std::string expectedBinary = "11111101";
			int expected = 253;
			std::string expectedD = "(0,1,2,3,4,5,7) |";
			std::string expectedC = "(6) &";
			Assert::AreEqual(expectedBinary, indexForm.second);
			Assert::AreEqual(expected, indexForm.first);
			Assert::AreEqual(expectedD, Logical::getDecimalFormD(table));
			Assert::AreEqual(expectedC, Logical::getDecimalFormC(table));
		}

		TEST_METHOD(FormsCheck3) {
			std::string input = "(a~b)&c";
			std::set<char> variables;
			std::string RPNResult = Logical::RPN(input, variables);
			std::vector<std::vector<bool>> table = Logical::getTable(RPNResult, variables);
			std::pair<int, std::string> indexForm = Logical::getIndexForm(table);
			std::string expectedBinary = "01000001";
			int expected = 65;
			std::string expectedD = "(1,7) |";
			std::string expectedC = "(0,2,3,4,5,6) &";
			Assert::AreEqual(expectedBinary, indexForm.second);
			Assert::AreEqual(expected, indexForm.first);
			Assert::AreEqual(expectedD, Logical::getDecimalFormD(table));
			Assert::AreEqual(expectedC, Logical::getDecimalFormC(table));
		}

		TEST_METHOD(PDNFPCNFCheck1) {
			std::string input = "(a|b)&!c";
			std::set<char> variables;
			std::string RPNResult = Logical::RPN(input, variables);
			std::vector<std::vector<bool>> table = Logical::getTable(RPNResult, variables);
			
			std::string PDNF = Logical::getPDNF(table, variables);
			std::string PCNF = Logical::getPCNF(table, variables);
			
			std::string PDNFExpected = "(!a&b&!c)|(a&!b&!c)|(a&b&!c)";
			std::string PCNFExpected = "(a|b|c)&(a|b|!c)&(a|!b|!c)&(!a|b|!c)&(!a|!b|!c)";
			Assert::AreEqual(PDNFExpected, PDNF);
			Assert::AreEqual(PCNFExpected, PCNF);
		}

		TEST_METHOD(PDNFPCNFCheck2) {
			std::string input = "(a&b)->c";
			std::set<char> variables;
			std::string RPNResult = Logical::RPN(input, variables);
			std::vector<std::vector<bool>> table = Logical::getTable(RPNResult, variables);

			std::string PDNF = Logical::getPDNF(table, variables);
			std::string PCNF = Logical::getPCNF(table, variables);

			std::string PDNFExpected = "(!a&!b&!c)|(!a&!b&c)|(!a&b&!c)|(!a&b&c)|(a&!b&!c)|(a&!b&c)|(a&b&c)";
			std::string PCNFExpected = "(!a|!b|c)";
			Assert::AreEqual(PDNFExpected, PDNF);
			Assert::AreEqual(PCNFExpected, PCNF);
		}

		TEST_METHOD(PDNFPCNFCheck3) {
			std::string input = "(a~b)&c";
			std::set<char> variables;
			std::string RPNResult = Logical::RPN(input, variables);
			std::vector<std::vector<bool>> table = Logical::getTable(RPNResult, variables);

			std::string PDNF = Logical::getPDNF(table, variables);
			std::string PCNF = Logical::getPCNF(table, variables);

			std::string PDNFExpected = "(!a&!b&c)|(a&b&c)";
			std::string PCNFExpected = "(a|b|c)&(a|!b|c)&(a|!b|!c)&(!a|b|c)&(!a|b|!c)&(!a|!b|c)";
			Assert::AreEqual(PDNFExpected, PDNF);
			Assert::AreEqual(PCNFExpected, PCNF);
		}

		TEST_METHOD(AlgMethod1) {
			std::string original = "(!a&b&c)|(a&!b&!c)|(a&!b&c)|(a&b&!c)|(a&b&c)";
			std::string result = Logical::AlgebraicMethodStr(original, '|');
			std::string expected = "(a)|(b&c)";
			Assert::AreEqual(expected, result);
		}

		TEST_METHOD(McCluskey1) {
			std::string original = "(!a&b&c)|(a&!b&!c)|(a&!b&c)|(a&b&!c)|(a&b&c)";
			std::string result = Logical::McCluskeyMethodStr(original, '|');
			std::string expected = "(a)|(b&c)";
			Assert::AreEqual(expected, result);
		}
		TEST_METHOD(KMap1) {
			std::string original = "(!a&b&c)|(a&!b&!c)|(a&!b&c)|(a&b&!c)|(a&b&c)";
			std::string result = Logical::KMapStr(original, '|');
			std::string expected = "(a)|(b&c)";
			Assert::AreEqual(expected, result);
		}

		TEST_METHOD(AlgMethod2) {
			std::string original = "(!a&!b&!c&d)|(!a&!b&c&d)|(!a&b&!c&!d)|(!a&b&!c&d)|(!a&b&c&!d)|(!a&b&c&d)|(a&!b&!c&!d)|(a&!b&!c&d)|(a&!b&c&!d)|(a&!b&c&d)|(a&b&!c&!d)|(a&b&!c&d)|(a&b&c&!d)|(a&b&c&d)";
			std::string result = Logical::AlgebraicMethodStr(original, '|');
			std::string expected = "(a)|(b)|(d)";
			Assert::AreEqual(expected, result);
		}

		TEST_METHOD(McCluskey2) {
			std::string original = "(!a&!b&!c&d)|(!a&!b&c&d)|(!a&b&!c&!d)|(!a&b&!c&d)|(!a&b&c&!d)|(!a&b&c&d)|(a&!b&!c&!d)|(a&!b&!c&d)|(a&!b&c&!d)|(a&!b&c&d)|(a&b&!c&!d)|(a&b&!c&d)|(a&b&c&!d)|(a&b&c&d)";
			std::string result = Logical::McCluskeyMethodStr(original, '|');
			std::string expected = "(a)|(b)|(d)";
			Assert::AreEqual(expected, result);
		}
		TEST_METHOD(KMap2) {
			std::string original = "(!a&!b&!c&d)|(!a&!b&c&d)|(!a&b&!c&!d)|(!a&b&!c&d)|(!a&b&c&!d)|(!a&b&c&d)|(a&!b&!c&!d)|(a&!b&!c&d)|(a&!b&c&!d)|(a&!b&c&d)|(a&b&!c&!d)|(a&b&!c&d)|(a&b&c&!d)|(a&b&c&d)";
			std::string result = Logical::KMapStr(original, '|');
			std::string expected = "(b)|(a)|(d)";
			Assert::AreEqual(expected, result);
		}
		TEST_METHOD(KMap3) {
			std::string input = "a|b&c->d|e";
			std::set<char> variables;
			std::string RPNResult = Logical::RPN(input, variables);
			std::vector<std::vector<bool>> table = Logical::getTable(RPNResult, variables);

			std::string PDNF = Logical::getPDNF(table, variables);

			std::string result = Logical::KMapStr(PDNF, '|');
			std::string expected = "(e)|(d)|(!a&!b)|(!a&!c)";
			Assert::AreEqual(expected, result);
		}

		TEST_METHOD(KMap4) {
			std::string input = "(!a|b|c)&(a|!b|!c)&(a|!b|c)&(a|b|!c)&(a|b|c)";
			std::string result = Logical::KMapStr(input, '&');
			std::string expected = "(a)&(b|c)";
			Assert::AreEqual(expected, result);
		}

		TEST_METHOD(AlgMethod3) {
			std::string input = "(!a|b|c)&(a|!b|!c)&(a|!b|c)&(a|b|!c)&(a|b|c)";
			std::string result = Logical::AlgebraicMethodStr(input, '&');
			std::string expected = "(a)&(b|c)";
			Assert::AreEqual(expected, result);
		}

		TEST_METHOD(McCluskey3) {
			std::string input = "(!a|b|c)&(a|!b|!c)&(a|!b|c)&(a|b|!c)&(a|b|c)";
			std::string result = Logical::McCluskeyMethodStr(input, '&');
			std::string expected = "(a)&(b|c)";
			Assert::AreEqual(expected, result);
		}
	};
}
