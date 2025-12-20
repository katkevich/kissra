#pragma once

namespace kissra::test {
struct tracker {
    int ctor{};
    int copy_ctor{};
    int move_ctor{};
    int copy_op{};
    int move_op{};
    int dtor{};

    constexpr void reset() {
        *this = tracker{};
    }
};

struct noisy {
    constexpr noisy(tracker& track)
        : track(&track) {
        track.ctor++;
    }
    constexpr noisy(const noisy& other)
        : track(other.track) {
        track->copy_ctor++;
    }
    constexpr noisy(noisy&& other)
        : track(other.track) {
        track->move_ctor++;
    }
    constexpr noisy& operator=(const noisy& other) {
        track = other.track;
        track->copy_op++;
        return *this;
    }
    constexpr noisy& operator=(noisy&& other) {
        track = other.track;
        track->copy_op++;
        return *this;
    }
    constexpr ~noisy() {
        track->dtor++;
    }
    tracker* track{};
};
} // namespace kissra::test