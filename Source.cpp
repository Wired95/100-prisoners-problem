/*
 * The director of a prison offers 100 death row prisoners, who are
 * numbered from 1 to 100, a last chance. A room contains a cupboard
 * with 100 drawers. The director randomly puts one prisoner's
 * number in each closed drawer. The prisoners enter the room, one
 * after another. Each prisoner may open and look into 50 drawers in
 * any order. The drawers are closed again afterwards. If, during
 * this search, every prisoner finds his number in one of the
 * drawers, all prisoners are pardoned. If just one prisoner does not
 * find his number, all prisoners die. Before the first prisoner
 * enters the room, the prisoners may discuss strategy — but may not
 * communicate once the first prisoner enters to look in the drawers.
 * 
 * What is the prisoners' best strategy?
 *
 * From: https://en.wikipedia.org/wiki/100_prisoners_problem
 */

#include <iostream>
#include <iostream>
#include <random>
#include <time.h>
#include <algorithm>
#include <list>

#ifndef uint8
#define uint8 uint8_t
#endif

// Prison elements
uint8 Boxes[100];

struct Prisonner
{
	float success = false;
	uint8 remainingAttempts = 50;
	std::list<uint8> visited;
};

// Initialize boxes and labels
void randomize_box_labels()
{
	// we only have labels from 1 to 100
	std::list<uint8> labels_avail;
	for (uint8 i = 1; i <= 100; i++)
		labels_avail.push_back(i);

	// gather entropy
	std::random_device dev;
	std::mt19937 rng(dev());

	// shuffle labels into their boxes
	for (uint8 i = 0; i < 100; i++)
	{
		// get random number in range [0, labels_avail.size() - 1]
		std::uniform_int_distribution<std::mt19937::result_type> random(0, labels_avail.size() - 1); 
		uint8 pos = random(rng);

		// assign a random number from labels_avail inside the selected box
		std::list<uint8>::iterator it = labels_avail.begin();
		std::advance(it, pos);
		Boxes[i] = *it;
		labels_avail.erase(it);
	}
}

// Prisoners are executed if they don't find labels matching their ID
bool is_run_failed(std::list<Prisonner> prisonners)
{
	for (auto i : prisonners) {
		if (!i.success)
			return true;
	}
	return false;
}

// A prisonner will proceed with the "loop from my own label" strategy
void prisonner_attempt(std::list<Prisonner>::iterator prisonner, uint8 prisonnerID)
{
	// add entropy in case we have to find another loop
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> random(0, 99); // distribution in range [0, 99]

	// the following of the loop must start from the prisonner's ID
	uint8 box_explore_id = prisonnerID - 1;

	// proceed with "box opening" attempts
	while (prisonner->remainingAttempts != 0 && prisonner->success == false)
	{
		// in case we must look for another loop start, we must find another unexplored loop start
		while (std::find(prisonner->visited.begin(), prisonner->visited.end(), box_explore_id) != prisonner->visited.end())
		{
			box_explore_id = random(rng);
		}
		
		// follow a loop
		while (prisonner->remainingAttempts != 0 && std::find(prisonner->visited.begin(), prisonner->visited.end(), box_explore_id) == prisonner->visited.end() && prisonner->success == false)
		{
			// log checked boxes
			prisonner->visited.push_back(box_explore_id);

			// test box content
			if (Boxes[box_explore_id] == prisonnerID)
			{
				prisonner->success = true;
				break;
			}

			// account the try
			prisonner->remainingAttempts--;

			// proceed to the next box
			box_explore_id = Boxes[box_explore_id] - 1;
		}
	}
}

int main()
{
	// simulation parameters
	const int run_counts = 10000;

	// counters
	int success_runs = 0;
	int failed_runs = 0;
	int tries = 0;

	// runs
	for (int run = 1; run <= run_counts; run++)
	{
		// initialize the prison set
		randomize_box_labels();
		std::list<Prisonner> prisonners(100);
		uint8 prisonnerID = 1;

		// try all prisonners
		for (std::list<Prisonner>::iterator prisonner = prisonners.begin(); prisonner != prisonners.end(); prisonner++)
		{
			prisonner_attempt(prisonner, prisonnerID);

			if (prisonner->success == false)
				break;

			prisonnerID++;
		}

		// output results
		if (is_run_failed(prisonners))
		{
			failed_runs++;
			std::cout << "Run: " << run << " failed at: " << int(prisonnerID) << " n-th prisonner." << std::endl;
			tries += int(prisonnerID);
		}
		else
		{
			success_runs++;
			std::cout << "Run: " << run << " is a success." << std::endl;
		}
	}

	// throw conclusions
	std::cout << "Simulation Done with " << run_counts << " runs." << std::endl;
	std::cout << "Success: " << success_runs << std::endl;
	std::cout << "Failed: " << failed_runs << std::endl;
	std::cout << "Success rate: " << (float(success_runs) / float(run_counts)) * 100 << "%" << std::endl;
	std::cout << "Average prisonners try before failure: " << (float(tries) / float(failed_runs)) << std::endl;
}

