#ifndef RESULT_H
#define RESULT_H

#include <ostream>

#include "renderer.h"
#include "labeler.h"

namespace oj {

class Result {
public:
    virtual ~Result() = default;

    Result();
    Result(const Result& other) = default;
    Result(Result&& other) noexcept = default;

    Result& operator=(const Result& other) = default;
    Result& operator=(Result&& other) noexcept = default;

    virtual void        Render(std::ostream& os, const Renderer& renderer) const = 0;
    virtual std::string Label(const Labeler& labeler) const = 0;

    virtual bool        is_success() const = 0;
};

}

#endif