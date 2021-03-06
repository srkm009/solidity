/*
	This file is part of solidity.

	solidity is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	solidity is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with solidity.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * Contains an abstract base class representing a genetic algorithm and its concrete implementations.
 */

#pragma once

#include <tools/yulPhaser/Population.h>

namespace solidity::phaser
{

/**
 * Abstract base class for genetic algorithms.
 * The main feature is the @a runNextRound() method that executes one round of the algorithm,
 * on the supplied population.
 */
class GeneticAlgorithm
{
public:
	GeneticAlgorithm() {}
	GeneticAlgorithm(GeneticAlgorithm const&) = delete;
	GeneticAlgorithm& operator=(GeneticAlgorithm const&) = delete;
	virtual ~GeneticAlgorithm() = default;

	/// The method that actually implements the algorithm. Should use @a m_population as input and
	/// replace it with the updated state after the round.
	virtual Population runNextRound(Population _population) = 0;
};

/**
 * Completely random genetic algorithm,
 *
 * The algorithm simply replaces the worst chromosomes with entirely new ones, generated
 * randomly and not based on any member of the current population. Only a constant proportion of the
 * chromosomes (the elite) is preserved in each round.
 *
 * Preserves the size of the population. You can use @a elitePoolSize to make the algorithm
 * generational (replacing most members in each round) or steady state (replacing only one member).
 * Both versions are equivalent in terms of the outcome but the generational one converges in a
 * smaller number of rounds while the steady state one does less work per round. This may matter
 * in case of metrics that take a long time to compute though in case of this particular
 * algorithm the same result could also be achieved by simply making the population smaller.
 */
class RandomAlgorithm: public GeneticAlgorithm
{
public:
	struct Options
	{
		double elitePoolSize;        ///< Percentage of the population treated as the elite
		size_t minChromosomeLength;  ///< Minimum length of newly generated chromosomes
		size_t maxChromosomeLength;  ///< Maximum length of newly generated chromosomes

		bool isValid() const
		{
			return (
				0 <= elitePoolSize && elitePoolSize <= 1.0 &&
				minChromosomeLength <= maxChromosomeLength
			);
		}
	};

	explicit RandomAlgorithm(Options const& _options):
		m_options(_options)
	{
		assert(_options.isValid());
	}

	Population runNextRound(Population _population) override;

private:
	Options m_options;
};

/**
 * A generational, elitist genetic algorithm that replaces the population by mutating and crossing
 * over chromosomes from the elite.
 *
 * The elite consists of individuals not included in the crossover and mutation pools.
 * The crossover operator used is @a randomPointCrossover. The mutation operator is randomly chosen
 * from three possibilities: @a geneRandomisation, @a geneDeletion or @a geneAddition (with
 * configurable probabilities). Each mutation also has a parameter determining the chance of a gene
 * being affected by it.
 */
class GenerationalElitistWithExclusivePools: public GeneticAlgorithm
{
public:
	struct Options
	{
		double mutationPoolSize;          ///< Percentage of population to regenerate using mutations in each round.
		double crossoverPoolSize;         ///< Percentage of population to regenerate using crossover in each round.
		double randomisationChance;       ///< The chance of choosing @a geneRandomisation as the mutation to perform
		double deletionVsAdditionChance;  ///< The chance of choosing @a geneDeletion as the mutation if randomisation was not chosen.
		double percentGenesToRandomise;   ///< The chance of any given gene being mutated in gene randomisation.
		double percentGenesToAddOrDelete; ///< The chance of a gene being added (or deleted) in gene addition (or deletion).

		bool isValid() const
		{
			return (
				0 <= mutationPoolSize && mutationPoolSize <= 1.0 &&
				0 <= crossoverPoolSize && crossoverPoolSize <= 1.0 &&
				0 <= randomisationChance && randomisationChance <= 1.0 &&
				0 <= deletionVsAdditionChance && deletionVsAdditionChance <= 1.0 &&
				0 <= percentGenesToRandomise && percentGenesToRandomise <= 1.0 &&
				0 <= percentGenesToAddOrDelete && percentGenesToAddOrDelete <= 1.0 &&
				mutationPoolSize + crossoverPoolSize <= 1.0
			);
		}
	};

	GenerationalElitistWithExclusivePools(Options const& _options):
		m_options(_options)
	{
		assert(_options.isValid());
	}

	Population runNextRound(Population _population) override;

private:
	Options m_options;
};

}
