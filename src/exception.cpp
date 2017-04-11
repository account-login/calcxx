#include "exception.h"


const char *BaseException::what() const throw() {
    return this->msg.data();
}
