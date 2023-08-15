#include "utilities.h"

std::string get_date_string() {
	auto now = std::chrono::system_clock::now();
	auto current_time = std::chrono::system_clock::to_time_t(now);

	std::tm date;
	localtime_s(&date, &current_time);

	char buffer[100];
	strftime(buffer, 100, "%Y-%m-%d-%H-%M-%S", &date);

	return std::format("[{}]", buffer);
}

std::vector<double> nums(double start, double end, size_t N) {
	if (N < 2 || start > end) {
		throw;
	}
	std::vector<double> v;
	auto mult = std::pow(end / start, 1. / (N - 1));
	for (int i = 0; i < N; i++) {
		v.push_back(start * std::pow(mult, i));
	}
	return v;
};