#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << "#ARITHM!";
}

namespace {
class Formula : public FormulaInterface {
public:
// Реализуйте следующие методы

    explicit Formula(std::string expression) 
        : ast_(ParseFormulaAST(expression)) // Инициализация в списке инициализации
    {
    }

    // bool IsValidFormula(std::string expression) const {

    //     bool op_flag = false;
    //     if (!IsDigit(expression[0])){
    //         if (!IsOp(expression[0])) {
    //             throw FormulaException("FormulaError");
    //         }
    //         else {
    //             op_flag = true;
    //         }
    //     }

    //     for (size_t i = 1; i < expression.size(), ++i) {
    //         if (op_flag) {
    //             if (IsDigit(expression[i])){
    //                 op_flag = false;
    //             }
    //             else {
    //                 throw FormulaException("FormulaError");
    //             }

    //         }
    //         else {
    //             if (IsOp(expression[i])) {
    //                 op_flag = true;
    //             }
    //         }
    //     }
    //     if (op_flag) {
    //         throw FormulaException("FormulaError");
    //     }
    //     return true; 
    // }

    Value Evaluate() const override {
        try {
            return ast_.Execute();
        }
        catch (const FormulaError& fe) {
            return fe;
        }
    }

    std::string GetExpression() const override {
        std::ostringstream output;
        ast_.PrintFormula(output);
        return output.str();
    }

    bool IsOp(char op) const{
        return op == '+' || op == '-' || op == '*' || op == '/';
    }

    bool IsDigit(char dig) const {
        return dig >= '0' && dig <= '9';
    }

private:
    FormulaAST ast_;
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    return std::make_unique<Formula>(std::move(expression));
}