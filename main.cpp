#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <random>
#include <fstream>
#include <time.h>

int numVars = 0;
int numClauses = 0;
int WALKSAT_PROBABILITY = 90;//n% of best 1-n% random
std::vector<std::vector<int>> geneticProb;

void display(std::vector<std::vector<int>>);
bool negate(bool);
std::vector<std::vector<int>> parseFile(std::string);//goes through file and puts input into 2d array
std::vector<bool> hillClimb(std::vector<std::vector<int>>);//returns solution using hillClimb
bool solveDisjunction(std::vector<int>, std::vector<bool>);//given a disjunction(arg1) and a set of value(arg2) return if it is true
int getHueristic(std::vector<std::vector<int>>, std::vector<bool>);//given CNF(arg1) and set values (arg2) return number of true clauses
std::vector<bool> walkSAT(std::vector<std::vector<int>>);//returns a solution using walkSat
std::vector<bool> randomLiterals();
std::vector<bool> genetic(std::vector<std::vector<int>>);
int main(int argc, char* argv[])
{
	bool satisfiable = true;
	bool unSatisfiable = true;
	std::string satFileName;
	if (argc == 0)
	{
		std::cout << "Usage ./" << argv[0] << " [SAT filename]" << std::endl;
		return 0;
	}
	else if (argc == 1)
	{
		satFileName = "test.cnf";
	//	satFileName = argv[1];
	}
	std::vector<std::vector<int>> satList;
	std::vector<bool> solution;
	satList = parseFile(satFileName);


	std::cout << "c Trying to find a solution. . .\n";
	//solution = hillClimb(satList);
	//solution = walkSAT(satList);
	solution = genetic(satList);


	if (satisfiable)
	{
		std::cout << "s SATISFIABLE\n";
		//display();
		std::cout << "c Done. Runtime: " << std::endl;
		char c;
		std::cin >> c;
		return 10;
	}
	else if(unSatisfiable)
	{
		std::cout << "s UNSATISFIABLE\n";	
		std::cout << "c Done. Runtime: " << std::endl;
		char c;
		std::cin >> c;
		return 20;
	}
	else
	{
		std::cout << "s UNKNOWN\0";
	}

}

bool modelCompare(std::vector<bool> a, std::vector<bool> b)
{
	return getHueristic(geneticProb, a) < getHueristic(geneticProb, b);
}
std::vector<std::vector<bool>> reproduce(std::vector<std::vector<bool>> parents)
{
	srand(time(NULL));
	int random = rand() % 100;
	std::vector<std::vector<bool>> children;
	for (size_t i = 0; i < parents.size(); i+=2)
	{
		std::vector<bool> child1;
		std::vector<bool> child2;
		for (size_t j = 0; j < parents[i].size(); j++)
		{
			if (j < parents.size() / 2)//first half
			{
				child1.push_back(parents[i][j]);
				child2.push_back(parents[i+1][j]);
			}
			else//second half
			{
				child1.push_back(parents[i+1][j]);
				child2.push_back(parents[i][j]);
			}
		}
		children.push_back(child1);
		children.push_back(child2);
	}
	return children;
}
std::vector<bool> genetic(std::vector<std::vector<int>> cnf)
{
	geneticProb = cnf;
	int populationSize = 100;
	int numKills = 10;
	int numReproduce = 10;//must be even
	std::vector<std::vector<bool>> modelPopulation;
	for (int i = 0; i < populationSize; i++)
	{
		modelPopulation.push_back(randomLiterals());//fill the population with randoms
	}
	
	while (true)
	{
		std::sort(modelPopulation.begin(), modelPopulation.end(), modelCompare);
		if (getHueristic(cnf, modelPopulation[0]))//after sort best hueristic should be first
		{
			return modelPopulation[0];
		}


		modelPopulation._Pop_back_n(numKills);//kill bottom %
		std::vector<std::vector<bool>> parents = modelPopulation;
		parents.resize(numReproduce);//pick top
		std::vector<std::vector<bool>> children = reproduce(parents);
		modelPopulation.insert(modelPopulation.end(), children.begin(), children.end());//add childrent to population

		//sort population
		//pick top 10% for reproducing
		//pop bottom 10%

	}
}
std::vector<bool> walkSAT(std::vector<std::vector<int>> cnf)
{
	std::vector<bool> model;
	model = randomLiterals();
	while (true)//timeout will stop it
	{

		if (getHueristic(cnf, model) == numClauses)
		{
			return model;
		}
		//add terminating if stat

		//get random clause from list
		//get vars in clause
		//rand
		//if rand 50% then flip random var
		//else flip best var in clause kinda like  hill climb but with 1 clause
		int randomClause;
		do
		{
			srand(time(NULL));
			randomClause = rand() % (cnf.size() - 1);
		} while (!solveDisjunction(cnf[randomClause], model));//find a random clause that is false 

		int p = rand() % 100;
		if (p > WALKSAT_PROBABILITY)
		{
			//flip random variable in clause
			int randomVar = rand() % (cnf[randomClause].size() - 1);
			int var = abs(cnf[randomClause][randomVar]);
			model[var - 1] = negate(model[var - 1]);
		}
		else
		{
			//flip variable in clause that improves the most
			int bestHueristic = getHueristic(cnf, model);
			for (size_t i = 0; i < model.size(); i++)
			{
				//make neighbors and find best
				std::vector<bool> neighbor = model;
				neighbor[i] = negate(neighbor[i]);
				int neighborHueristic = getHueristic(cnf, neighbor);
				if (neighborHueristic > bestHueristic)
				{
					bestHueristic = neighborHueristic;
					model = neighbor;
				}
			}
		}
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