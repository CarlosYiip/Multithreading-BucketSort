#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

void print(std::vector<std::vector<unsigned>> vv) {
	for (auto& v : vv) {
		std::cout << "[ ";
		for (auto& i : v) {
			std::cout << i << ' ';
		}
		std::cout << "]\n";
	}
}


int main(int argc, char const *argv[])
{

	std::vector<std::vector<unsigned>> vv;
	std::mutex m;
	unsigned p = 300;

	for (int i = 0; i < p; ++i)
	{
		vv.emplace_back(std::vector<unsigned> {});
	}

	std::vector<std::thread> containerOfThreads;

	unsigned n = 0;
	for (unsigned i = 0; i < p; ++i)
	{
		containerOfThreads.emplace_back(std::thread {[&vv, &p, i] {
			// std::lock_guard<std::mutex> guard {m};
			for (unsigned j = 0; j < p; ++j)
			{
				vv.at(i).push_back(j);
			}
		}});


		// containerOfThreads.back().join();
	}

	for (auto it = containerOfThreads.begin(); it != containerOfThreads.end(); ++it)
		it->join();



	print(vv);


	return 0;
}