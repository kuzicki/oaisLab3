#include <iostream>
#include <iomanip>
#include "Logical.h"
#include "KMap.h"

int main() {
	std::string input = "(!a|b|c)&(a|!b|!c)&(a|!b|c)&(a|b|!c)&(a|b|c)";
	std::string a1 = Logical::AlgebraicMethodStr(input, '&');
	std::cout << "Algebraic: " << a1 << std::endl;
	std::string m1 = Logical::McCluskeyMethodStr(input, '&');
	std::cout << "McCluskey: " << m1 << std::endl;
	std::string k1 = Logical::KMapStr(input, '&');
	std::cout << "KMap: " << k1 << std::endl;

	std::cout << "_________________" << std::endl;
	std::string input1 = "(!a&!b&!c)|(!a&!b&c)|(!a&b&!c)|(!a&b&c)|(a&b&!c)";
	std::string a2 = Logical::AlgebraicMethodStr(input1, '|');
	std::cout << "Algebraic: " << a2 << std::endl;
	std::string m2 = Logical::McCluskeyMethodStr(input1, '|');
	std::cout << "McCluskey: " << m2 << std::endl;
	std::string k2 = Logical::KMapStr(input1, '|');
	std::cout << "KMap: " << k2 << std::endl;
}