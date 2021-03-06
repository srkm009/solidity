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
 * Contains the main class that controls yul-phaser based on command-line parameters and
 * associated factories for building instances of phaser's components.
 */

#pragma once

#include <boost/program_options.hpp>

#include <istream>
#include <memory>
#include <ostream>
#include <string>

namespace solidity::langutil
{

class CharStream;

}

namespace solidity::phaser
{

class FitnessMetric;
class GeneticAlgorithm;
class Population;
class Program;

enum class Algorithm
{
	Random,
	GEWEP,
};

std::istream& operator>>(std::istream& _inputStream, solidity::phaser::Algorithm& _algorithm);
std::ostream& operator<<(std::ostream& _outputStream, solidity::phaser::Algorithm _algorithm);

/**
 * Builds and validates instances of @a GeneticAlgorithm and its derived classes.
 */
class GeneticAlgorithmFactory
{
public:
	struct Options
	{
		Algorithm algorithm;

		static Options fromCommandLine(boost::program_options::variables_map const& _arguments);
	};

	static std::unique_ptr<GeneticAlgorithm> build(
		Options const& _options,
		size_t _populationSize,
		size_t _minChromosomeLength,
		size_t _maxChromosomeLength
	);
};

/**
 * Builds and validates instances of @a FitnessMetric and its derived classes.
 */
class FitnessMetricFactory
{
public:
	static constexpr size_t RepetitionCount = 5;

	static std::unique_ptr<FitnessMetric> build(
		Program _program
	);
};

/**
 * Builds and validates instances of @a Population.
 */
class PopulationFactory
{
public:
	static constexpr size_t PopulationSize = 20;
	static constexpr size_t MinChromosomeLength = 12;
	static constexpr size_t MaxChromosomeLength = 30;

	static Population build(
		std::shared_ptr<FitnessMetric> _fitnessMetric
	);
};

/**
 * Builds and validates instances of @a Program.
 */
class ProgramFactory
{
public:
	struct Options
	{
		std::string inputFile;

		static Options fromCommandLine(boost::program_options::variables_map const& _arguments);
	};

	static Program build(Options const& _options);

private:
	static langutil::CharStream loadSource(std::string const& _sourcePath);
};

/**
 * Main class that controls yul-phaser based on command-line parameters. The class is responsible
 * for command-line parsing, initialisation of global objects (like the random number generator),
 * creating instances of main components using factories and feeding them into @a AlgorithmRunner.
 */
class Phaser
{
public:
	static int main(int argc, char** argv);

private:
	struct CommandLineDescription
	{
		boost::program_options::options_description keywordDescription;
		boost::program_options::positional_options_description positionalDescription;
	};

	struct CommandLineParsingResult
	{
		int exitCode;
		boost::program_options::variables_map arguments;
	};

	static CommandLineDescription buildCommandLineDescription();
	static CommandLineParsingResult parseCommandLine(int _argc, char** _argv);
	static void initialiseRNG(boost::program_options::variables_map const& _arguments);

	static void runAlgorithm(boost::program_options::variables_map const& _arguments);
};

}
