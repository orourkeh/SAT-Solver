#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <fstream>
#include <time.h>

int numVars = 0;
int numClauses = 0;

void display(std::vector<std::vector<int>>);
bool negate(bool b);
std::vector<std::vector<int>> parseFile(std::string filename);//goes through file and puts input into 2d array
std::vector<bool> hillClimb(std::vector<std::vector<int>>);//returns solution using hillClimb
bool solveDisjunction(std::vector<int>, std::vector<bool>);//given a disjunction(arg1) and a set of value(arg2) return if it is true
int getHueristic(std::vector<std::vector<int>>, std::vector<bool>);//given CNF(arg1) and set values (arg2) return number of true clauses
std::vector<bool> randomLiterals();

int main(int argc, char* argv[])
{
	bool satisfiable = false;
	bool unSatisfiable = true;
	std::string satFileName;
	if (argc == 0)
	{
		std::cout << "Usage ./" << argv[0] << " [SAT filename]" << std::endl;
		return 0;
	}
	else if (argc == 1)
	{
		satFileName = "test2.cnf";
	//	satFileName = argv[1];
	}
	std::vector<std::vector<int>> satList;
	std::vector<bool> solution;
	satList = parseFile(satFileName);
	display(satList);
	/*
	std::vector<bool> testLits = { false, false, false };
	std::cout << getHueristic(satList, testLits);
	*/
	solution = hillClimb(satList);
	for (size_t i = 0; i < solution.size(); i++)
	{
		std::cout << i+1 << " : " << solution[i] << std::endl;
	}


	char c;
	std::cin >> c;
	if (satisfiable)
	{
		std::cout << "s SATISFIABLE\0";
		//display();
		return 10;
	}
	else if(unSatisfiable)
	{
		std::cout << "s UNSATISFIABLE\0";
		return 20;
	}
	else
	{
		std::cout << "s UNKNOWN\0";
	}
}





bool solveDisjunction(std::vector<int> disj, std::vector<bool> literals)
{
	for (size_t i = 0; i < disj.size(); i++)
	{

		bool atom = literals[abs(disj[i])-1];//atom = the literal value of variable
		if (disj[i] < 0)
		{
			//if CNF calls for negation negate
			atom = negate(atom);
		}
		if (atom)
		{
			return true;//if any atoms equiate to true the clause is true
		}
	}
	return false;//if no atoms are true clause is false
}
int getHueristic(std::vector<std::vector<int>> cnf, std::vector<bool> literals)
{
	int hueristic = 0;
	for (size_t i = 0; i < cnf.size(); i++)
	{
		if (solveDisjunction(cnf[i], literals))
		{
			hueristic++;
		}
	}
	return hueristic;
}
bool negate(bool b)
{
	if (b)
	{
		return false;
	}
	else
	{
		return true;
	}
}
std::vector<bool> hillClimb(std::vector<std::vector<int>> cnf)
{
	while (true)//risky business
	{
		std::vector<bool> bestLiteral = randomLiterals();
		int bestHueristic = getHueristic(cnf, bestLiteral);
		for (size_t i = 0; i < bestLiteral.size(); i++)
		{
			//make neighbor and find best
			std::vector<bool> neighbor = bestLiteral;
			neighbor[i] = negate(neighbor[i]);
			int neighborHueristic = getHueristic(cnf, neighbor);
			if (neighborHueristic > bestHueristic)
			{
				bestHueristic = neighborHueristic;
				bestLiteral = neighbor;
			}
		}
		if (bestHueristic == numClauses)//if all clauses solved
		{
			return bestLiteral;
		}
	}
}
std::vector<bool> randomLiterals()
{
	std::vector<bool> randomLit;
	srand(time(NULL));
	for (int i = 0; i < numVars; i++)
	{
		int random = rand() % 2;
		if (random == 0)
		{
			randomLit.push_back(true);
		}
		else
		{
			randomLit.push_back(false);
		}
	}
	return randomLit;
}
std::vector<std::vector<int>> parseFile(std::string filename)
{
	std::vector<std::vector<int>> cnfList;
	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::cout << "File " << filename << " failed to open." << std::endl;
		return cnfList;
	}

	char lineType;
	std::string buf;
	file >> lineType;
	while (file.good())
	{
		if (lineType == 'p')
		{
			file >> buf;
			file >> numVars;
			file >> numClauses;
			std::cout << buf << ' ' << numVars << ' '<< numClauses << '\n';
			getline(file, buf);//dump rest of line
		}
		else if (lineType == 'c')
		{
			getline(file, buf);//dump line
		}
		else
		{
			file.putback(lineType);//first number got picked up by line
			int a;
			file >> a;
			std::vector<int> line;
			while (a != '\0')//go to end of line
			{
				line.push_back(a);
				file >> a;
			}
			cnfList.push_back(line);
		}
		file >> lineType;
	}
	return cnfList;
}
void display(std::vector<std::vector<int>> cnf)
{
	for (size_t i = 0; i < cnf.size(); i++)
	{
		for (size_t j = 0; j < cnf[i].size(); j++)
		{
			std::cout << cnf[i][j] << ' ';
		}
		std::cout << std::endl;
	}
}