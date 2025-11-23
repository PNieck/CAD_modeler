#pragma once

#include <stdexcept>
#include <vector>


template <typename T>
class CircularVecWrap {
public:
    explicit CircularVecWrap(const std::vector<T>& data):
        data(data) {}

    const T& operator[](const int idx) const
        { return data[CircularIndex(idx)]; };

    auto begin() const
        { return data.begin(); }

    auto end() const
    { return data.end(); }

    auto size() const
        { return data.size(); }

    const std::vector<T>& to_vector() const
        { return data; }

    [[nodiscard]]
    size_t CircularIndex(const int idx) const {
        if (data.empty())
            throw std::out_of_range("CircularVector is empty");

        const int n = static_cast<int>(data.size());

        int wrapped = idx % n;
        if (wrapped < 0)
            wrapped += n;

        return static_cast<size_t>(wrapped);
    }

private:
    const std::vector<T>& data;
};
