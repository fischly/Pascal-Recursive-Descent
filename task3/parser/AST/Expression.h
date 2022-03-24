
#pragma once

/* Base class for all expressions */
class Expression {
public:
    virtual ~Expression() = default; // adding virtual destructor to make Expression class polymorphic
};