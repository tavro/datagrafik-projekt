float generateRandomFloat(float range) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(-range, range);
	
	return dis(gen);
}