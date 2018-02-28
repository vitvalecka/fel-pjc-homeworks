#pragma once

#include <functional>


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
