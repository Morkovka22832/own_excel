#pragma once

#include "cell.h"
#include "common.h"

#include <functional>

class Sheet : public SheetInterface {
public:
    explicit Sheet() = default;



    ~Sheet();

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void UpdatePrintableSize(Position pos);
    void UpdatePrintSizeAfterClear(Position pos);


    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

    // Можете дополнить ваш класс нужными полями и методами
private:
    void SizeIncrease(Position pos);

private:
    std::vector< std::vector< std::unique_ptr<Cell>>> cells_;
    Position printable_size_ = Position{0,0};
    size_t max_row_ = 0;
    size_t max_col_ = 0;
};
