#include <stdio.h>
#include <memory>
#include <string>

class A {
public:
	int age;
	int weight;

public:

	A(int age, int weight) {
		this->age = age;
		this->weight = weight;

		printf("Memory Created!!! for age = %d, weight = %d \n", age, weight);
	}

	~A() {
		printf("Memory Deleted!!! for age = %d, weight = %d \n", age, weight);
	}
};

int main() {

	{
		//storing a string in unique pointer

		//std::unique_ptr<std::string> str(new std::string("hello"));

		//std::unique_ptr<std::string> str2;

		//printf("%s", str->c_str());

		//str2 = std::make_unique<std::string>("Rose");

		//printf("%s", str2->c_str());


		////storing a class in unique pointer
		//std::unique_ptr<A> a = std::make_unique<A>(10, 15);
	}

}