#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>
#include <memory>

using namespace std::literals;

Sheet::~Sheet() {}

void Sheet::SetCell(Position pos, std::string text) {
    if (pos.IsValid()) {
        SizeIncrease(pos);
        if (!cells_[pos.row][pos.col]) {                      // проверяем нет ли уже такой ячейки
            cells_[pos.row][pos.col] = std::make_unique<Cell>();
        }
        cells_[pos.row][pos.col]->Set(std::move(text));
        UpdatePrintableSize(pos);
    }
    else {
        throw InvalidPositionException("Error invalid pos set cell");
    }
}

const CellInterface* Sheet::GetCell(Position pos) const {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid position");
    }

    if (static_cast<size_t>(pos.row) >= cells_.size() || static_cast<size_t>(pos.col) >= cells_[pos.row].size()) {
        return nullptr;
    }

    return static_cast<const CellInterface*>(cells_[pos.row][pos.col].get());    // Возвращаем сырой указатель
}

CellInterface* Sheet::GetCell(Position pos) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid position");
    }

    if (static_cast<size_t>(pos.row) >= cells_.size() || static_cast<size_t>(pos.col) >= cells_[pos.row].size()) {          //Проверка на существование cell в принципе
        return nullptr;
    }

    return cells_[pos.row][pos.col].get();                                   // Возвращаем сырой указатель
}

void Sheet::ClearCell(Position pos) {

    if (!pos.IsValid()) {
        throw InvalidPositionException("Error invalid pos clear cell");
    }

    if (static_cast<size_t>(pos.row) >= cells_.size() || static_cast<size_t>(pos.col) >= cells_[pos.row].size()) {            //Проверка на существование cell в принципе
        return;
    }

    if (!cells_[pos.row][pos.col]) {
        return;
    }

    cells_[pos.row][pos.col].reset();                          //зануляем тек. ячейку

    //std::cout << "ClearOk" << std::endl;
    UpdatePrintSizeAfterClear(pos);
}

Size Sheet::GetPrintableSize() const {
    //std::cout << cells_.size() << " Size" << std:: endl;
    return Size{printable_size_.row,printable_size_.col};

}



void Sheet::UpdatePrintableSize(Position pos)
{
    printable_size_.col = std::max(pos.col + 1,printable_size_.col);
    printable_size_.row = std::max(pos.row + 1,printable_size_.row);
}

void Sheet::UpdatePrintSizeAfterClear(Position pos)
{
    size_t max_row = 0;
    size_t max_col = 0;
    for (size_t row = 0; row < cells_.size(); ++row) {
        for (size_t col = 0; col <  cells_[row].size(); ++col) {
            if (cells_[row][col]) {
                max_row = std::max(max_row, row+1);
                max_col = std::max(max_col, col+1);
            }
        }
    }


    printable_size_ = {static_cast<int>(max_row), static_cast<int>(max_col)};

    cells_.resize(printable_size_.row);
    cells_.resize(printable_size_.col);

}

std::ostream& operator<<(std::ostream& output, CellInterface::Value value) {

    if (std::holds_alternative<FormulaError>(value)){
        output << std::get<FormulaError>(value) ;
    }
    if (std::holds_alternative<double>(value)){
        output << std::get<double>(value) ;
    }
    if (std::holds_alternative<std::string>(value)){
        output << std::get<std::string>(value) ;
    }
    return output;
}

// void Sheet::PrintValues(std::ostream& output) const {
//     for (size_t row = 0; row < cells_.size(); ++row) {
//         for (size_t col = 0; col <  cells_[row].size(); ++col) {
//             if (cells_[row][col]) {
//                 auto text = cells_[row][col]->GetValue();
//                 output << cells_[row][col]->GetValue();

//             }
//             if (col != static_cast<size_t>(printable_size_.col) - 1){
//                 output << "\t";
//             }
//         }
//         output << "\n";
//     }
// }

void Sheet::PrintValues(std::ostream& output) const {
    if (cells_.size() == 0) {return;}
    for (size_t row = 0; row < cells_.size(); ++row) {
        for (size_t col = 0; col <  cells_[row].size(); ++col) {
            if (cells_[row][col]) {
                auto text = cells_[row][col]->GetValue();
                output << cells_[row][col]->GetValue();

            }
            if (col < static_cast<size_t>(printable_size_.col) - 1){   // Для последней колонки, чтобы не выводит
                output << "\t";
            }
        }
        int nullptr_cells_count = cells_[row].size();
        while(nullptr_cells_count < printable_size_.col - 1){
            output << "\t";
            nullptr_cells_count++;
        }
        output << "\n";
    }
}
// void Sheet::PrintTexts(std::ostream& output) const {

//     for (size_t row = 0; row < cells_.size(); ++row) {
//         for (size_t col = 0; col <  cells_[row].size(); ++col) {
//             if (cells_[row][col]) {
//                 std::string s = cells_[row][col]->GetText();
//                 output << cells_[row][col]->GetText();

//             }
//             if (col != static_cast<size_t>(printable_size_.col) - 1){
//                 output << "\t";
//             }
//         }
//         output << "\n";
//     }

// }


// void Sheet::PrintTexts(std::ostream& output) const {

//     for (size_t row = 0; row < cells_.size(); ++row) {
//         std::cout <<cells_.size();
//         for (size_t col = 0; col <  cells_[row].size(); ++col) {
//             if (cells_[row][col]) {
//                 std::string s = cells_[row][col]->GetText();
//                 output << cells_[row][col]->GetText();

//             }
//             if (col < static_cast<size_t>(printable_size_.col)){
//                 output << "\t";
//             }
//         }
//         int nullptr_cells_count = cells_[row].size();
//         while(nullptr_cells_count < printable_size_.col - 1){
//             output << "\t";
//             nullptr_cells_count++;
//         }
//         output << "\n";
//     }

// }


void Sheet::PrintTexts(std::ostream& output) const {
    if (cells_.size() == 0) {return;}
    for (size_t row = 0; row < cells_.size(); ++row) {
        for (size_t col = 0; col <  cells_[row].size(); ++col) {
            if (cells_[row][col]) {
                output << cells_[row][col]->GetText();

            }
            if (col < static_cast<size_t>(printable_size_.col) - 1){   // Для последней колонки, чтобы не выводит
                output << "\t";
            }
        }
        int nullptr_cells_count = cells_[row].size();
        while(nullptr_cells_count < printable_size_.col - 1){
            output << "\t";
            nullptr_cells_count++;
        }
        output << "\n";
    }

}


// SizeIncrease Добавляяем новые элементы в  вектора cells_, каждая строчка(row) имеет
// только столько обьектов сколько нужно, для конктрентой строки а не для таблицы.
// при переборе использовать cell_.size();
void Sheet::SizeIncrease(Position pos){
    if (cells_.size() <= static_cast<size_t>(pos.row)) {
        cells_.resize(pos.row+1);
    }
    if (cells_[pos.row].size() <= static_cast<size_t>(pos.col)) {
        cells_[pos.row].resize(pos.col + 1);
    }
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}
