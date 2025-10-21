namespace Plants {

template <class T> struct Extra {
	
};

struct OtherBase {
	
};

struct Fruit : Extra<Fruit>, OtherBase {
	double mass = 0.0; // in grams
	// This class is the base class for all fruits e.g. Orange, Apple
	Fruit() {}
	void SetMass(double grams) {mass = grams;}
	double GetMass() const {return mass;}
	virtual ~Fruit() {}
	virtual const char* GetName() const {return "Fruit";}
	void Consume(double grams);
};

struct Orange : Fruit {
	Orange() {}
	const char* GetName() const override;
};

struct Apple : Fruit {
	Apple() {}
	const char* GetName() const {return "Apple";}
};

void Fruit::Consume(double grams) {
	if (grams > mass)
		grams = mass;
	mass -= grams;
}

const char* Orange::GetName() const {
	return "Orange";
}

}


#define TEST(x) if (o.GetMass() == 0) return 1;

int main(int argc, const char *argv[])
{
	using namespace Plants;
	
	// We are eating oranges, not all of them
	Orange o;
	o.SetMass(120); // grams
	o.Consume(20);
	if (argc > 1)
		o.Consume(argc);
	void (Fruit::*fn_ptr)(double) = &Orange::Consume;
	TEST(o);
	
	return 0;
}
/*
namespace Plants {

struct Cucumber {
	Cucumber() {}
	void Fn(int i) {i++;}
};

}
*/
