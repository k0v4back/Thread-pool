#pragma once

// #include <functional>
#include <future>

namespace tp {

// using Task = std::function<void()>;
using Task = std::future<void>;

}