#pragma once

#include <functional>
#include <iosfwd>


class scope_guard {
public:
    scope_guard(std::function<void(void)> func):
        f(std::move(func)) {}

    // Copies aren't meaningful, moves are implicitly disabled for simplicity
    scope_guard(const scope_guard&) = delete;
    scope_guard& operator=(const scope_guard&) = delete;


    ~scope_guard() {
        if (enabled) {
            f();
        }
    }
    void dismiss() {
        enabled = false;
    }
private:
    bool enabled = true;
    std::function<void(void)> f;
};


struct counter {
    counter() = default;
    counter(int dc, int cc, int ca, int mc, int ma, int d, int o):default_constructors(dc),
        copy_constructors(cc), copy_assignments(ca), move_constructors(mc), move_assignments(ma),
        destructors(d), other(o) {}
    int default_constructors = 0;
    int copy_constructors = 0;
    int copy_assignments = 0;
    int move_constructors = 0;
    int move_assignments = 0;
    int destructors = 0;
    int other = 0;
};

bool operator==(const counter& c1, const counter& c2);
counter operator-(counter c1, const counter& c2);
std::ostream& operator<<(std::ostream& out, const counter& cnt);

struct tracker {
    tracker();
    tracker(double val);
    tracker(const tracker&);
    tracker& operator=(const tracker&);
    tracker(tracker&&);
    tracker& operator=(tracker&&);
    ~tracker();

    double value;
    static counter cnt;
};

bool operator==(const tracker& c1, const tracker& c2);
bool operator<(const tracker& c1, const tracker& c2);

struct ordered {
    ordered() = default;
    ordered(int sortby, size_t order):sortby(sortby), order(order) {}

    int sortby;
    size_t order;
};

bool operator==(const ordered& o1, const ordered& o2);
bool operator<(const ordered& o1, const ordered& o2);

struct thrower {
    thrower(int, bool);
    thrower(const thrower&);
    thrower& operator=(const thrower&);
    thrower(thrower&&);
    thrower& operator=(thrower&&);

    void check() const;
    int value;
    bool should_throw;
};

bool operator==(const thrower& t1, const thrower& t2);


template <typename Iterator, bool ForceInvalidate = true>
struct fake_input_iterator {
    using difference_type = typename Iterator::difference_type;
    using value_type = typename Iterator::value_type;
    using reference = typename Iterator::reference;
    using pointer = typename Iterator::pointer;
    using iterator_category = std::input_iterator_tag;

    fake_input_iterator() = default;
    fake_input_iterator(Iterator it) :place(it) {}

    bool operator==(const fake_input_iterator& rhs) const {
        return place == rhs.place;
    }
    bool operator!=(const fake_input_iterator& rhs) const {
        return place != rhs.place;
    }
    reference operator*() {
        return *place;
    }
    fake_input_iterator& operator++() {
        if (ForceInvalidate) {
            *place = value_type{};
        }
        ++place;
        return *this;
    }
    fake_input_iterator operator++(int) {
        if (ForceInvalidate) {
            *place = value_type{};
        }
        auto tmp(*this);
        ++(*this);
        return tmp;
    }
    pointer operator->() {
        return place;
    }

    Iterator place;
};
