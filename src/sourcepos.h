#ifndef CALCXX_SOURCEPOS_H
#define CALCXX_SOURCEPOS_H


#include <string>
#include <tuple>
#include "utils.hpp"


using std::to_string;
using std::tie;


struct SourcePos {
    int lineno;
    int rowno;

private:
    bool last_newline = true;

public:
    SourcePos(int lineno, int rowno)
        : lineno(lineno), rowno(rowno)
    {}

    // invalid
    SourcePos() : lineno(-1), rowno(-1) {}

    bool operator == (const SourcePos &other) const {
        return tie(this->lineno, this->rowno) == tie(other.lineno, other.rowno);
    }

    bool operator != (const SourcePos &other) const {
        return !(*this == other);
    }

    bool is_valid() const {
        return this->lineno >= 0 && this->rowno >= 0;
    }

    void add_char(char ch);
};


REPR(SourcePos) {
    return "<Pos " + to_string(value.lineno) + ":" + to_string(value.rowno) + ">";
}


#endif //CALCXX_SOURCEPOS_H
