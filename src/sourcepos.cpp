#include "sourcepos.h"


void SourcePos::add_char(char ch) {
    if (this->last_newline) {
        this->lineno++;
        this->rowno = 0;
    } else {
        this->rowno++;
    }

    this->last_newline = ch == '\n';
}
