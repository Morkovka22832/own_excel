#include "common.h"
#include "test_runner_p.h"

inline std::ostream& operator<<(std::ostream& output, Position pos) {
    return output << "(" << pos.row << ", " << pos.col << ")";
}

inline Position operator"" _pos(const char* str, std::size_t) {
    return Position::FromString(str);
}

inline std::ostream& operator<<(std::ostream& output, Size size) {
    return output << "(" << size.rows << ", " << size.cols << ")";
}

inline std::ostream& operator<<(std::ostream& output, const CellInterface::Value& value) {
    std::visit(
        [&](const auto& x) {
            output << x;
        },
        value);
    return output;
}

namespace {

void TestEmpty() {
    auto sheet = CreateSheet();
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{0, 0}));
}

void TestInvalidPosition() {
    auto sheet = CreateSheet();
    try {
        sheet->SetCell(Position{-1, 0}, "");
    } catch (const InvalidPositionException&) {
    }
    try {
        sheet->GetCell(Position{0, -2});
    } catch (const InvalidPositionException&) {
    }
    try {
        sheet->ClearCell(Position{Position::MAX_ROWS, 0});
    } catch (const InvalidPositionException&) {
    }
}

void TestSetCellPlainText() {
    auto sheet = CreateSheet();

    auto checkCell = [&](Position pos, std::string text) {
        sheet->SetCell(pos, text);
        CellInterface* cell = sheet->GetCell(pos);
        ASSERT(cell != nullptr);
        ASSERT_EQUAL(cell->GetText(), text);
        ASSERT_EQUAL(std::get<std::string>(cell->GetValue()), text);
    };

    checkCell("A1"_pos, "Hello");
    checkCell("A1"_pos, "World");
    checkCell("B2"_pos, "Purr");
    checkCell("A3"_pos, "Meow");

    const SheetInterface& constSheet = *sheet;
    ASSERT_EQUAL(constSheet.GetCell("B2"_pos)->GetText(), "Purr");

    sheet->SetCell("A3"_pos, "'=escaped");
    CellInterface* cell = sheet->GetCell("A3"_pos);
    ASSERT_EQUAL(cell->GetText(), "'=escaped");
    ASSERT_EQUAL(std::get<std::string>(cell->GetValue()), "=escaped");
}

void TestClearCell() {
    auto sheet = CreateSheet();

    sheet->SetCell("C2"_pos, "Me gusta");
    sheet->ClearCell("C2"_pos);
    ASSERT(sheet->GetCell("C2"_pos) == nullptr);

    sheet->ClearCell("A1"_pos);
    sheet->ClearCell("J10"_pos);
}
void TestPrint() {
    auto sheet = CreateSheet();
    sheet->SetCell("A2"_pos, "meow");
    sheet->SetCell("B2"_pos, "=1+2");
    sheet->SetCell("A1"_pos, "=1/0");

    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{2, 2}));

    std::ostringstream texts;
    sheet->PrintTexts(texts);
    std::string s = texts.str();
    ASSERT_EQUAL(texts.str(), "=1/0\t\nmeow\t=1+2\n");



    std::ostringstream values;
    sheet->PrintValues(values);
    ASSERT_EQUAL(values.str(), "#ARITHM!\t\nmeow\t3\n");

    sheet->ClearCell("B2"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{2, 1}));
}


void TestClearPrint() {
    // auto sheet = CreateSheet();
    // for (int i = 0; i <= 5; ++i) {
    //     sheet->SetCell(Position{i, i}, std::to_string(i));
    // }

    // sheet->ClearCell(Position{3, 3});


    // std::ostringstream text6;

    //     sheet->ClearCell(Position{5, 5});


    //     sheet->ClearCell(Position{4, 4});


    //     sheet->ClearCell(Position{3, 3});


    //     sheet->ClearCell(Position{2, 2});


    //     sheet->ClearCell(Position{1, 1});


    //     sheet->ClearCell(Position{0, 0});
    //     //text << sheet->GetPrintableSize();
    //     //text << "\n";
    //     sheet->PrintTexts(text6);
    //     std::cout << text6.str();


    auto sheet = CreateSheet();
    sheet->SetCell("C5"_pos, "meow");
    sheet->SetCell("E3"_pos, "=1+2");
    sheet->SetCell("E5"_pos, "=1/0");

    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{5, 5}));

    sheet->ClearCell("E5"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{5, 5}));

    sheet->ClearCell("C5"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{3, 5}));

    sheet->SetCell("C5"_pos, "meow");

    sheet->ClearCell("E3"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{5, 3}));



}




}  // namespace

int main() {
    auto sheet = CreateSheet();
    for (int i = 0; i < 10000; ++i) {
        for (int j = 0; j < 10000; ++j) {
            sheet->SetCell(Position{i, j}, std::to_string(i));
        }
    }

}
