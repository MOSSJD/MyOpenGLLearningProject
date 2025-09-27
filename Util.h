#pragma once

#include <stdexcept>
#include <string>

namespace ME {
	class MyError : public std::runtime_error {
	public:
		MyError(const std::string message) : runtime_error(message) {}
		virtual ~MyError() = default;
	};
}