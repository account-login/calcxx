#include <tuple>
#include "sourcepos.h"


using std::tie;


bool SourcePos::operator==(const SourcePos &other) const {
    return tie(this->lineno, this->rowno) == tie(other.lineno, other.rowno);
}

bool SourcePos::operator!=(const SourcePos &other) const {
    return !(*this == other);
}

bool SourcePos::is_valid() const {
    return this->lineno >= 0 && this->rowno >= 0;
}

void SourcePos::add_char(char ch) {
    if (this->last_newline) {
        this->lineno++;
        this->rowno = 0;
    } else {
        this->rowno++;
    }

    this->last_newline = ch == '\n';
}
