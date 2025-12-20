#pragma once

namespace kissra::test {
struct tracker {
    int ctor{};
    int copy_ctor{};
    int move_ctor{};
    int copy_op{};
    int move_op{};
    int dtor{};

    void reset() {
        *this = tracker{};
    }
};

struct noisy {
    noisy(tracker& track)
        : track(&track) {
        track.ctor++;
    }
    noisy(const noisy& other)
        : track(other.track) {
        track->copy_ctor++;
    }
    noisy(noisy&& other)
        : track(other.track) {
        track->move_ctor++;
    }
    noisy& operator=(const noisy& other) {
        track = other.track;
        track->copy_op++;
        return *this;
    }
    noisy& operator=(noisy&& other) {
        track = other.track;
        track->copy_op++;
        return *this;
    }
    ~noisy() {
        track->dtor++;
    }
    tracker* track{};
};
} // namespace kissra::test