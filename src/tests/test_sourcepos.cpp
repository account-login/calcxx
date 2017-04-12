#include "catch.hpp"
#include "../sourcepos.h"


TEST_CASE("Test SourcePos") {
    SourcePos pos;
    CHECK_FALSE(pos.is_valid());

    pos.add_char('a');
    CHECK(pos.lineno == 0);
    CHECK(pos.rowno == 0);
    CHECK(pos.is_valid());

    pos.add_char('b');
    CHECK(pos.lineno == 0);
    CHECK(pos.rowno == 1);

    pos.add_char('\n');
    CHECK(pos.lineno == 0);
    CHECK(pos.rowno == 2);

    pos.add_char('c');
    CHECK(pos.lineno == 1);
    CHECK(pos.rowno == 0);

    pos = SourcePos();
    pos.add_char('\n');
    CHECK(pos == SourcePos(0, 0));

    pos.add_char('a');
    CHECK(pos == SourcePos(1, 0));
}
