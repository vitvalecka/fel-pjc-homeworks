#include "tests-helpers.hpp"

#include <ostream>

bool operator==(const counter& c1, const counter& c2) {
    return c1.default_constructors == c2.default_constructors &&
        c1.copy_constructors == c2.copy_constructors &&
        c1.copy_assignments == c2.copy_assignments &&
        c1.move_constructors == c2.move_constructors &&
        c1.move_assignments == c2.move_assignments &&
        c1.destructors == c2.destructors &&
        c1.other == c2.other;
}
counter operator-(counter c1, const counter& c2) {
    c1.default_constructors -= c2.default_constructors;
    c1.copy_constructors -= c2.copy_constructors;
    c1.copy_assignments -= c2.copy_assignments;
    c1.move_constructors -= c2.move_constructors;
    c1.move_assignments -= c2.move_assignments;
    c1.destructors -= c2.destructors;
    c1.other -= c2.other;
    return c1;
}

std::ostream & operator<<(std::ostream & out, const counter & cnt) {
    out << "| dc: " << cnt.default_constructors
        << " | cc: " << cnt.copy_constructors
        << " | ca: " << cnt.copy_assignments
        << " | mc: " << cnt.move_constructors
        << " | ma: " << cnt.move_assignments
        << " | ~d: " << cnt.destructors
        << " | o:  " << cnt.other
        << " |";
    return out;
}

tracker::tracker():value(0) {
    cnt.default_constructors++;
}

tracker::tracker(double val):value(val) {
    cnt.other++;
}

tracker::tracker(const tracker& rhs):value(rhs.value) {
    cnt.copy_constructors++;
}
tracker& tracker::operator=(const tracker& rhs) {
    value = rhs.value;
    cnt.copy_assignments++;
    return *this;
}
tracker::tracker(tracker&& rhs):value(rhs.value) {
    cnt.move_constructors++;
}
tracker& tracker::operator=(tracker&& rhs) {
    value = rhs.value;
    cnt.move_assignments++;
    return *this;
}
tracker::~tracker() {
    cnt.destructors++;
}

counter tracker::cnt = {};


bool operator<(const tracker& c1, const tracker& c2) {
    return c1.value < c2.value;
}

bool operator==(const tracker& c1, const tracker& c2) {
    return c1.value == c2.value;
}

bool operator==(const ordered & o1, const ordered & o2) {
    return o1.sortby == o2.sortby;
}

bool operator<(const ordered& o1, const ordered& o2) {
    return o1.sortby < o2.sortby;
}

bool operator==(const thrower& t1, const thrower& t2) {
    return t1.value == t2.value && t1.should_throw == t2.should_throw;
}

thrower::thrower(int value, bool should_throw):value{ value }, should_throw { should_throw } {}

thrower::thrower(const thrower& rhs):value{ rhs.value }, should_throw{ rhs.should_throw } {
    rhs.check();
}

thrower& thrower::operator=(const thrower& rhs) {
    rhs.check();
    value = rhs.value;
    should_throw = rhs.should_throw;
    return *this;
}

thrower::thrower(thrower&& rhs):value{ rhs.value }, should_throw{ rhs.should_throw } {
    rhs.check();
}

thrower& thrower::operator=(thrower&& rhs) {
    rhs.check();
    value = rhs.value;
    should_throw = rhs.should_throw;
    return *this;
}

void thrower::check() const {
    if (should_throw) {
        throw 123;
    }
}
