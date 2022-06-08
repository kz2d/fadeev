#include <vector> 
#include <cmath>

constexpr  int test_constexpr(const std::initializer_list <int> array, const int x) {
	int data = 0;
	for ( int i = 0; i < array.size(); i++) {
		data += *(array.begin() + i) * std::pow(x, i);
	}

	return data;
}


int main() {
	int d = test_constexpr({ 2, 7, 1 }, 2);

	return 0;
}