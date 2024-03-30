#include "Logical.h"
#include "KMap.h"
#include <set>
#include <string>
#include <stack>
#include <array>
#include <vector>
#include <bitset>
#include <map>

namespace utils {
	void printVariableExpression(const Logical::variableExpression& expr) {
		for (auto& part : expr) {
			for (auto& elem : part) {
				char sign = elem.isNegative ? '-' : ' ';
				std::cout << sign << elem.var << ' ';
			}
			std::cout << std::endl;
		}
	}

	std::string exprPartToString(const std::vector<Logical::Variable>& expr, char delim) {
		std::string result = "(";
		for (int i = 0; i < expr.size(); i++) {
			result += expr[i].ToString() + delim;
		}
		result.erase(result.end() - 1, result.end());
		return result + ")";
	}
}

namespace Logical {
	const std::set<char> sign = { '&', '|', '!', '-', '~' };

	int priority(const char operation) {
		switch (operation) {
		case '!':
			return 3;
		case '&':
			return 2;
		case '|':
			return 1;
		case '-':
		case '~':
			return 0;
		}
		return -1;
	}

	bool calculate(const std::string& RPN, std::map<int, bool> inputVars) {
		bool operand1 = false, operand2 = false;
		bool result = false;
		std::stack<int> st;
		int resultPos = 123;
		char curChar;
		for (int i = 0; i < RPN.length(); i++) {
			curChar = RPN[i];
			if (sign.find(curChar) == sign.end()) {
				st.push(curChar);
				result = inputVars[(int)curChar - 97];
				continue;
			}

			if (curChar == '!') {
				operand1 = inputVars[(int)st.top() - 97];
				st.pop();
				result= !operand1;
				inputVars[resultPos - 97] = result;
				st.push(resultPos);
				resultPos++;
				continue;
			}

			operand1 = inputVars[(int)st.top() - 97];
			st.pop();
			operand2 = inputVars[(int)st.top() - 97];
			st.pop();
			switch (curChar) {
			case '&': result = operand1 && operand2; break;
			case '|': result = operand1 || operand2; break;
			case '-': result = operand1 || !operand2; break;
			case '~': result = operand1 == operand2; break;
			}
			inputVars[resultPos - 97] = result;
			st.push(resultPos);
			resultPos++;
		}
		return result;
	}

	std::string RPN(const std::string& str, std::set<char>& vars) {
		std::stack<int> st;
		char curChar;
		std::string result = "";
		for (int i = 0; i < str.size(); i++) {
			curChar = str[i];
			if (curChar == ' ')
				continue;
			if (curChar == '(') {
				st.push(curChar);
				continue;
			}
			if (curChar == ')') {
				while (!st.empty() && st.top() != '(') {
					result += st.top();
					st.pop();
				}
				if (st.top() == '(')
					st.pop();
				continue;
			}
			if (sign.find(curChar) != sign.end()) {
				if (curChar == '-') {
					while (!st.empty() && priority(st.top()) >= priority(curChar)) {
						result += st.top();
						st.pop();
					}
					st.push(curChar);
					i++;
					continue;
				}

				while (!st.empty() && priority(st.top()) >= priority(curChar)) {
					result += st.top();
					st.pop();
				}
				st.push(curChar);
				continue;
			}
			if (curChar >= 'a' && curChar <= 'z') {
				vars.insert(curChar);
				result += curChar;
				continue;
			}
		}
		while (!st.empty()) {
			if (st.top() != '(')
				result += st.top();
			st.pop();
		}
		return result;
	}

	std::vector<std::vector<bool>> getTable(const std::string& RPN, const std::set<char>& variables) {
		std::vector<std::vector<bool>> table;
		std::vector<bool> currentRow;
		int permutationsAmount = variables.size();
		std::map<int, bool> inputVars;

		for (int mask = 0; mask != (1 << permutationsAmount); mask++) {
			std::bitset<100> bits(mask);
			auto variables_it = variables.end();
			for (int i = 0; i < permutationsAmount; i++) {
				variables_it--;
				inputVars[(int)*variables_it - 97] = bits[i];
				currentRow.insert(currentRow.begin(), bits[i]);
			}
			bool rowResult = calculate(RPN, inputVars);
			currentRow.push_back(rowResult);
			table.push_back(currentRow);
			currentRow.clear();
			inputVars.clear();
		}
		return table;
	}

	std::string getPCNF(std::vector<std::vector<bool>> table, const std::set<char>& variables) {
		std::vector<char> vars;
		for (auto vars_it = variables.begin(); vars_it != variables.end(); vars_it++) {
			vars.push_back(*vars_it);
		}
		std::string result = "";
		for (int i = 0; i < table.size(); i++) {
			if (!table[i][table[i].size() - 1]) {
				result += "(";
				for (int j = 0; j < table[i].size() - 1; j++) {
					result += (table[i][j] == false) ? "" : "!";
					std::string variableChar{ vars[j] };
					result += std::string(variableChar) + "|";
				}
				result.erase(--result.end());
				result += ")&";
			}
		}
		result.erase(--result.end());
		return result;
	}

	std::string getPDNF(std::vector<std::vector<bool>> table, const std::set<char>& variables) {
		std::vector<char> vars;
		for (auto vars_it = variables.begin(); vars_it != variables.end(); vars_it++) {
			vars.push_back(*vars_it);
		}
		std::string result = "";
		for (int i = 0; i < table.size(); i++) {
			if (table[i][table[i].size() - 1]) {
				result += "(";
				for (int j = 0; j < table[i].size() - 1; j++) {
					result += (table[i][j] == false) ? "!" : "";
					std::string variableChar{ vars[j] };
					result += variableChar + "&";
				}
				result.erase(--result.end());
				result += ")|";
			}
		}
		result.erase(--result.end());
		return result;
	}

	std::string getDecimalFormC(const std::vector<std::vector<bool>>& table) {
		std::vector<int> numberForms;
		for (int i = 0; i < table.size(); i++) {
			if (table[i][table[i].size() - 1] == false) {
				std::bitset<100> bits(0);
				int k = 0;
				for (int j = table[i].size() - 2; j >= 0; j--) {
					bits[k++] = table[i][j];
				}
				numberForms.push_back(bits.to_ulong());
			}
		}

		std::string result = "(";
		for (auto& elem : numberForms) {
			result += std::to_string(elem) + ",";
		}
		result.erase(--result.end());
		result += ") &";
		return result;
	}

	std::pair<long long, std::string> getIndexForm(const std::vector<std::vector<bool>>& table) {
		std::string binaryIndexForm = "";
		for (int i = 0; i < table.size(); i++) {
			binaryIndexForm += (table[i][table[i].size() - 1] == false) ? "0" : "1";
		}
		long long result = 0;
		int k = 0;
		for (int i = binaryIndexForm.size() - 1; i >= 0; i--) {
			result += (binaryIndexForm[i] == '1') ? pow(2, k) : 0;
			k++;
		}
		return std::pair<long long, std::string>(result, binaryIndexForm);
	}

	std::string getDecimalFormD(const std::vector<std::vector<bool>>& table) {
		std::vector<int> numberForms;
		for (int i = 0; i < table.size(); i++) {
			if (table[i][table[i].size() - 1] == true) {
				std::bitset<100> bits(0);
				int k = 0;
				for (int j = table[i].size() - 2; j >= 0; j--) {
					bits[k++] = table[i][j];
				}
				numberForms.push_back(bits.to_ulong());
			}
		}

		std::string result = "(";
		for (auto& elem : numberForms) {
			result += std::to_string(elem) + ",";
		}
		result.erase(--result.end());
		result += ") |";
		return result;
	}

	std::string varExprToString(const variableExpression& expr, char delim) {
		std::string stringResult = "";
		char innerDelim = '|';
		if (delim == '|') {
			innerDelim = '&';
		}

		for (int i = 0; i < expr.size(); i++) {
			stringResult += "(";
			for (int j = 0; j < expr[i].size(); j++) {
				stringResult += expr[i][j].ToString() + innerDelim;
			}
			stringResult.erase(stringResult.end() - 1);
			stringResult += std::string(")") + delim;
		}
		stringResult.erase(stringResult.end() - 1);
		return stringResult;
	}

	variableExpression getVariableExpression(const std::string& expression, char separator, std::set<char>& variables) {
		std::vector<std::vector<Variable>> result;
		std::vector<Variable> inBrackets;
		bool isNegative = false;
		int resultPos = 0;
		for (int i = 0; i < expression.size(); i++) {
			char curChar = expression[i];
			if (curChar == '(' || curChar == ')' || curChar == ' ') continue;

			if (curChar == '!') {
				isNegative = true;
				continue;
			}

			if (curChar >= 'a' && curChar <= 'z') {
				variables.insert(curChar);
				Variable var(curChar, isNegative);
				inBrackets.push_back(var);
				isNegative = false;
				continue;
			}

			if (curChar == separator) {
				result.push_back(std::move(inBrackets));
				inBrackets.clear();
			}
		}
		if (!inBrackets.empty()) {
			result.push_back(std::move(inBrackets));
		}
		return result;
	}

	

	void permuteSign(variableExpression& res, std::vector<Variable>& row, int left, int n) {
		if (left == n) {
			return;
		}

		for (int i = left; i < n; i++) {
			row[i].isNegative = true;

			permuteSign(res, row, i + 1, n);
			res.push_back(row);
			row[i].isNegative = false;
		}
	}

	void generateRow(variableExpression& res, std::vector<Variable>& row, int n, int left, int k, const std::vector<char>& variables) {
		if (k == 0) {
			permuteSign(res, row, 0, row.size());
			res.push_back(row);
			return;
		}

		for (int i = left; i < n; i++) {
			Variable newVar = { variables[i], false };
			row.push_back(newVar);
			generateRow(res, row, n, i + 1, k - 1, variables);
			row.erase(row.end() - 1);
		}
	}

	variableExpression makeCombinations(int n, int k, const std::vector<char>& variables) {
		variableExpression res;
		std::vector<Variable> row;
		generateRow(res, row, n, 0, k, variables);
		return res;
	}

	variableExpression makeAllCombinations(const std::vector<char>& variables) {
		variableExpression res;
		for (int i = 1; i < variables.size(); i++) {
			for (auto& comb : makeCombinations(variables.size(), i, variables))
				res.push_back(comb);
		}
		return res;
	}

	bool isIncluded(const std::vector<Variable>& full, const std::vector<Variable>& subset, Variable& notIncluded) {
		int j = 0;
		int n = subset.size();
		int m = full.size();
		if (n != m - 1) {
			return false;
		}
		std::vector<bool> isIncluded(full.size(), false);
		for (int i = 0; i < n; i++) {
			for (j = 0; j < m; j++) {
				if (subset[i] == full[j]) {
					isIncluded[j] = true;
					break;
				}

			}
			if (j == m)
				return false;
		}
		for (int i = 0; i < isIncluded.size(); i++) {
			if (isIncluded[i] == false) {
				notIncluded = full[i];
				notIncluded.isNegative = !notIncluded.isNegative;
				break;
			}
		}
		return true;
	}

	bool isIncludedWithChar(const std::vector<Variable>& full, const std::vector<Variable>& subset, Variable& notIncluded) {
		int j = 0;
		int n = subset.size();
		int m = full.size();
		if (n != m - 1) {
			return false;
		}
		for (int i = 0; i < n; i++) {
			for (j = 0; j < m; j++) {
				if (subset[i] == full[j]) {
					break;
				}

			}
			if (j == m)
				return false;
		}
		for (int i = 0; i < full.size(); i++) {
			if (full[i] == notIncluded) {
				return true;
			}
		}
		return false;
	}

	struct CombinationCompare
	{
		bool operator() (const std::vector<Variable>& lhs, const std::vector<Variable>& rhs) const
		{
			int lSize = lhs.size();
			int rSize = rhs.size();
			if (lSize == rSize) {
				for (int i = 0; i < rSize; i++) {
					if (lhs[i] == rhs[i])
						continue;
					int lMul = (lhs[i].isNegative) ? -1 : 1;
					int rMul = (rhs[i].isNegative) ? -1 : 1;
					return (int)lhs[i].var * lMul < (int)rhs[i].var* rMul;
				}
				return false;
			}
			return lSize < rSize;
		}
	};

	bool isRepeated(const std::vector<Variable>& combination, const variableExpression& original, std::vector<bool>& usedExpr, std::map<
		std::vector<Variable>, std::vector<std::pair<int, int>>, CombinationCompare>& fused_combinations) {
		int firstInd = 0;
		bool wasFused = false;
		for (int i = 0; i < original.size(); i++) {
			
			Variable notIncluded;
			if (isIncluded(original[i], combination, notIncluded)) {
				firstInd = i;
				for (int j = 0; j < original.size(); j++) {
					if (j == firstInd)
						continue;

					for (auto& index : fused_combinations[combination]) {
						if ((index.first == i && index.second == j) || (index.first == j && index.second == i)) {
							wasFused = true;
							break;
						}
					}
					if (wasFused) {
						wasFused = false;
						continue;
					}

					if (isIncludedWithChar(original[j], combination, notIncluded)) {
						usedExpr[j] = true;
						usedExpr[firstInd] = true;
						fused_combinations[combination].push_back(std::make_pair(firstInd, j));
						return true;
					}
				}
			}
		}
		return false;
	}

	variableExpression getSimplifiedForm(variableExpression completeForm, const std::set<char> variables) {
		std::vector<char> variablesVector(variables.begin(), variables.end());
		int variablesSize = variables.size();
		auto combinations = makeAllCombinations(variablesVector);
		for (auto& row : combinations) {
			for (auto& elem : row) {
				char sign = (elem.isNegative) ? '-' : ' ';
			}
		}
		variableExpression result;
		std::map<std::vector<Variable>, std::vector<std::pair<int, int>>, CombinationCompare> fused_combinations;
		bool wasFused = false;
		do {
			wasFused = false;
			result.clear();
			std::vector<bool> usedExpr(completeForm.size(), false);
			for (int i = 0; i < combinations.size(); i++) {
				if (isRepeated(combinations[i], completeForm, usedExpr, fused_combinations)) {
					for (auto& el : combinations[i]) {
						char sign = el.isNegative ? '-' : ' ';
					}
					result.push_back(combinations[i]);
					wasFused = true;
					i--;
				}
			}
			for (int i = 0; i < usedExpr.size(); i++) {
				if (!usedExpr[i]) {
					result.push_back(completeForm[i]);
				}
			}
			completeForm = result;
			for (auto& elem : completeForm) {
				for (auto& el : elem) {
					char sign = el.isNegative ? '-' : ' ';
				}
			}
		} while(wasFused);
		
		return result;
	}

	bool isSubset(const std::vector<Variable>& full, const std::vector<Variable>& subset) {
		int j = 0;
		int n = subset.size();
		int m = full.size();
		
		for (int i = 0; i < n; i++) {
			for (j = 0; j < m; j++) {
				if (subset[i] == full[j]) {
					break;
				}

			}
			if (j == m)
				return false;
		}
		return true;
	}

	variableExpression McCluskeyMethod(variableExpression original, variableExpression simplified) {
		std::vector<std::vector<bool>> table(simplified.size(), std::vector<bool>(original.size(), false));

		for (int i = 0; i < simplified.size(); i++) {
			for (int j = 0; j < original.size(); j++) {
				if (isSubset(original[j], simplified[i])) {
					table[i][j] = true;
				}
			}
		}
		for (auto& row : table) {
			for (bool elem : row) {
				std::cout << elem << " ";
			}
			std::cout << std::endl;
		}

		for (int i = 0; i < table.size(); i++) {
			bool isReplacable = true;
			for (int j = 0; j < table[i].size(); j++) {
				if (!table[i][j]) {
					continue;
				}
				bool onlyZeros = true;
				for (int k = 0; k < table.size(); k++) {
					if (i == k)
						continue;

					if (table[k][j]) {
						onlyZeros = false;
						break;
					}
				}
				if (onlyZeros) {
					isReplacable = false;
					break;
				}
			}
			if (isReplacable) {
				table.erase(table.begin() + i);
				simplified.erase(simplified.begin() + i);
				i--;
			}
		}
		std::cout << "-------------" << std::endl;
		for (auto& row : table) {
			for (bool elem : row) {
				std::cout << elem << " ";
			}
			std::cout << std::endl;
		}
		return simplified;
	}

	bool isRedundantPDNF(const std::vector<Variable>& part, const std::vector<bool>& solution, const std::set<char>& variables, bool isOneEquality) {
		std::map<char, bool> varToBool;
		int ind = 0;
		for (auto& it : variables) {
			varToBool[it] = solution[ind++];
		}
		for (int i = 0; i < part.size(); i++) {
			if (varToBool.find(part[i].var) == varToBool.end()) {
				return false;
			}
			bool areEqual =  varToBool[part[i].var] != part[i].isNegative;
			if (areEqual != isOneEquality) {
				return false;
			}
		}
		return true;
	}

	variableExpression algebraicMethodPDNF(variableExpression simplified, char delim) {
		std::set<char> variables;
		char separator;
		bool isEqualOne;
		if (delim == '&') {
			separator = '|';
			isEqualOne = false;
		}
		else {
			separator = '&';
			isEqualOne = true;
		}

		for (int i = 0; i < simplified.size(); i++) {
			std::string part =  utils::exprPartToString(simplified[i], separator);
			std::string RPNstring = RPN(part, variables);
			std::vector<std::vector<bool>> truthTable = Logical::getTable(RPNstring, variables);

			std::vector<bool> solution(variables.size(), false);
			int lastColumn = truthTable[0].size() - 1;
			for (int i = 0; i < truthTable.size(); i++) {
				if (truthTable[i][lastColumn] == isEqualOne) {
					std::copy(truthTable[i].begin(), truthTable[i].end() - 1, solution.begin());
				}
			}

			for (int j = 0; j < simplified.size(); j++) {
				if (i == j)
					continue;

				if (isRedundantPDNF(simplified[j], solution, variables, isEqualOne)) {
					simplified.erase(simplified.begin() + i);
					i--;
					break;
				}
			}

			variables.clear();
		}
		return simplified;
	}

	std::string AlgebraicMethodStr(std::string original, char delim) {
		std::set<char> newVars;
		auto result = Logical::getVariableExpression(original, delim, newVars);
		auto simplified = Logical::getSimplifiedForm(result, newVars);
		std::cout << "Simplified: " << std::endl;
		std::cout << Logical::varExprToString(simplified, delim) << std::endl;
		return Logical::varExprToString(Logical::algebraicMethodPDNF(simplified, delim), delim);
	}

	std::string McCluskeyMethodStr(std::string original, char delim) {
		std::set<char> newVars;
		auto result = Logical::getVariableExpression(original, delim, newVars);
		auto simplified = Logical::getSimplifiedForm(result, newVars);
		std::cout << "Simplified: " << std::endl;
		std::cout << Logical::varExprToString(simplified, delim) << std::endl;
		auto mcCluskey = Logical::McCluskeyMethod(result, simplified);
		return Logical::varExprToString(mcCluskey, delim);
	}

	std::string KMapStr(std::string original, char delim) {
		std::set<char> kmapVars;
		std::string RPNres = Logical::RPN(original, kmapVars);
		auto truthTable = Logical::getTable(RPNres, kmapVars);
		for (auto& row : truthTable) {
			for (auto elem : row) {
				std::cout << elem << " ";
			}
			std::cout << std::endl;
		}
		KMap map(truthTable, kmapVars);
		std::cout << "The table: " << std::endl;
		map.PrintTable();
		if (delim == '|') {
			return map.GetMinimalPDNF();
		}
		else {
			return map.GetMinimalPCNF();
		}
	}
}