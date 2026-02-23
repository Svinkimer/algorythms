// Hammurabi.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <locale.h>
#include <random>
#include <fstream>
#include <windows.h>


std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> landCostDist(17, 26);
std::uniform_int_distribution<int> wheatPerAcreDist(1, 6);
std::uniform_real_distribution<double> ratsEatingDist(0, 0.07);
std::uniform_int_distribution<int> plagueProbabilityDist(0, 100);




struct GameState {
	int wheat;
	int wheatReserved;
	int population;
	int land;
	int landCost;
	int landSeeded;
	int year;
	int diedFromHunger;
	int diedFromPlague;
	int wheatPerAcre;
	int wheatCollectedThisYear;
	int arrived;
	int wheatEatenByRats;
	double ratsEatingRate;
	int plagueProbabilityScore;
};



bool failedToReadNumIntoVar(int& num) {
	if (!(std::cin >> num)) {
		std::cout << "Повелитель, я вас не понял... Ответьте числом!" << std::endl;
		std::cin.clear();
		std::cin.ignore(10000, '\n');
		return true;
	}
	return false;
}

bool checkNegative(int num) {
	if (num < 0) {
		std::cout << "Повелитель, мы еще не изобрели отрицательные числа!" << std::endl;
		return true;
	}
	return false;
}

bool notEnoughLand(const GameState& gs, int landAmount) {
	if (landAmount > gs.land) {
		std::cout << "Повелитель, у нас нет земель, чтобы выполнить ваш приказ! Умерьте ваши амбиции..." << std::endl;
		return true;
	}
	return false;
}

bool notEnoughWheatForPurchase(const GameState& gs, int landAmount) {
	if (gs.landCost * landAmount > gs.wheat) {
		std::cout << "Повелитель, нам недостает пшеницы для такой покупки... Умерьте амбиции!";
		return true;
	}
	return false;
}

bool notEnoughWheatForReservation(const GameState& gs, int wr) {
	if (gs.wheat < wr) {
		std::cout << "Повелитель, в наших амбарах недостаточно пшеницы для такого расточительства!";
		return true;
	}
	return false;
}


void updateRandomGameStateParameters(GameState& gs) {
	gs.landCost = landCostDist(gen);
	gs.wheatPerAcre = wheatPerAcreDist(gen);
	gs.ratsEatingRate = ratsEatingDist(gen);
	gs.plagueProbabilityScore = plagueProbabilityDist(gen);
	//std::cout << "Rats Eaten: " << gs.ratsEatingRate << std::endl;


}



void processLandSale(GameState& gs, int& l) {
	while (true) {
		std::cout << "Сколько акров земли продать?" << std::endl << ">>> ";
		if (failedToReadNumIntoVar(l) || checkNegative(l) || notEnoughLand(gs, l)) {
			continue;
		}
		else {
			std::cout << "Ваша воля исполнится!" << std::endl;
			break;
		}

	}
	gs.land -= l;
	gs.wheat += l * gs.landCost;
}

void processLandPurchase(GameState& gs, int& l) {
	while (true) {
		std::cout << "Сколько акров земли купить?" << std::endl << ">>> ";
		if (failedToReadNumIntoVar(l) || checkNegative(l) || notEnoughWheatForPurchase(gs, l)) {
			continue;
		}
		else {
			std::cout << "Ваша воля исполнится!" << std::endl;
			break;
		}
	}

	gs.land += l;
	gs.wheat -= l * gs.landCost;

}

void processFoodReservation(GameState& gs, int& wr) {
	while (true) {
		std::cout << "Сколько пшеницы отложить на питание крестьян?" << std::endl << ">>> ";

		if (failedToReadNumIntoVar(wr) || checkNegative(wr) || notEnoughWheatForReservation(gs, wr)) {
			continue;
		}

		//gs.wheat -= wr;
		gs.wheatReserved = wr;
		break;
	}
}

void processLandSeeding(GameState& gs, int& l) {
	while (true) {
		std::cout << "Сколько акров земли засеять ?" << std::endl << ">>> ";

		if (failedToReadNumIntoVar(l) || checkNegative(l) || notEnoughLand(gs, l)) {
			continue;
		}

		// Check, if we can `засеять` without reserved wheat

		int validatedWheatAmount = gs.wheat - gs.wheatReserved;

		int wheatRequirement = (l + 1) / 2;
		int populationRequirement = l / 10;
		if (l % 10 != 0) populationRequirement++;

		if (wheatRequirement > validatedWheatAmount) {
			std::cout << "О, повелитель, нам не хватит пшеницы для засевания! [1 бушель на 2 акра]" << std::endl;
			continue;
		}
		else if (populationRequirement > gs.population) {
			std::cout << "О повелитель, нам не хватает смердов [1 чел на 10 акров]!" << std::endl;
			continue;
		}

		gs.wheat -= wheatRequirement;

		gs.landSeeded = l;
		break;
	}
}



void gameStateReport(const GameState& gs) {

	std::cout << "Повелитель, дозволь поведать тебе!" << std::endl
		<< "Идет твой " << gs.year << " год правления." << std::endl
		<< "Таковы результаты: " << std::endl;

	if (gs.diedFromHunger > 0) {
		std::cout << gs.diedFromHunger << " поданных пали от голода;" << std::endl;
	}

	if (gs.arrived > 0) {
		std::cout << gs.arrived << " новых жителя прибыли в наш город;" << std::endl;
	}

	if (gs.diedFromPlague > 0) {
		std::cout << gs.diedFromPlague << " поданных пали от чумы;" << std::endl;
	}

	std::cout << "Крысы пожрали " << gs.wheatEatenByRats << " бушелей пшеницы. Чтоб они подавились ею!" << std::endl;

	std::cout << "\tУрожайность:\t" << gs.wheatPerAcre << " бушелей с акра" << std::endl;
	std::cout << "\tВсего собрано:\t" << gs.wheatCollectedThisYear << " бушелей пшеницы" << std::endl;
	std::cout << "\tНаселение:\t" << gs.population << " крестьян" << std::endl;
	std::cout << "\tВ кладовой:\t" << gs.wheat << " бушелей пшеницы" << std::endl;
	std::cout << "\tПахотной земли:\t" << gs.land << " акров" << std::endl;
	std::cout << "\tЦена земли:\t" << gs.landCost << " за акр" << std::endl;


	//std::cout << "По истечении года у нас есть:";



}

void gameStateReportAfterOrder(const GameState& gs) {

	std::cout << "О, повелитель! После прошлого приказа у нас остается:\n\t"
		<< gs.population << " крестьян\n\t"
		<< gs.wheat << " бушелей пшеницы\n\t"
		<< gs.land << " акров земли\n\t"
		<< "Текущая цена акра земли: " << gs.landCost << " бушелей\\акр\n" << std::endl;
}


bool saveFileExists() {
	std::ifstream file("save.txt");
	return file.good();
}

void saveGame(const GameState& gs) {
	std::ofstream file("save.txt");

	if (!file) {
		std::cout << "Ошибка сохранения игры!" << std::endl;
		return;
	}

	file << gs.wheat << ' '
		<< gs.population << ' '
		<< gs.land << ' '
		<< gs.landCost << ' '
		<< gs.landSeeded << ' '
		<< gs.year << ' '
		<< gs.diedFromHunger << ' '
		<< gs.diedFromPlague << ' '
		<< gs.wheatReserved << ' '
		<< gs.wheatPerAcre << ' '
		<< gs.wheatCollectedThisYear << ' '
		<< gs.arrived << ' '
		<< gs.wheatEatenByRats << ' '
		<< gs.ratsEatingRate << ' '
		<< gs.plagueProbabilityScore;
}

bool loadGame(GameState& gs) {
	std::ifstream file("save.txt");

	if (!file) {
		return false;
	}

	if (!(file >> gs.wheat
		>> gs.population
		>> gs.land
		>> gs.landCost
		>> gs.landSeeded
		>> gs.year
		>> gs.diedFromHunger
		>> gs.diedFromPlague
		>> gs.wheatReserved
		>> gs.wheatPerAcre
		>> gs.wheatCollectedThisYear
		>> gs.arrived
		>> gs.wheatEatenByRats
		>> gs.ratsEatingRate
		>> gs.plagueProbabilityScore))
	{
		return false;
	}

	return true;
}



int main()
{
	// Fixing console encoding for russian letters
	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);
	

	int landBought{ 0 };
	int landSold{ 0 };
	int wheatReservedForFood{ 0 };
	bool receivedInputFlag{ false };
	int landSeeded{ 0 };
	double hungerDeathTollTotal = 0;


	GameState gameState;

	// Not really maintainable :-(
	const GameState defaultStart{
	2800, // wheat
	0,    // wheatReserved
	100,  // population
	1000, // land
	20,   // landCost
	0,    // landSeeded
	0,    // year
	0,    // diedFromHunger
	0,    // diedFromPlague
	0,    // wheatPerAcre
	0,    // wheatCollectedThisYear
	0,    // arrived
	0,    // wheatEatenByRats
	0.0,  // ratsEatingRate
	0     // plagueProbabilityScore
	};

	bool justLoadedGameFlag = false;

	// Try to load save - wth I even wrote this comment? I mean, everything is already in text literal, no?
	if (saveFileExists()) {
		std::cout << "Обнаружено сохранение. Продолжить игру? (y/n): ";
		char c;
		std::cin >> c;

		if (c == 'y' || c == 'Y') {
			if (!loadGame(gameState)) {
				std::cout << "Ошибка загрузки сохранения. Начинаем новую игру." << std::endl;
				gameState = defaultStart;
			}
			else {
				justLoadedGameFlag = true;
			}
		}
		else {
			gameState = defaultStart;
		}
	}
	else {
		// Нет сохранения — новая игра
		gameState = defaultStart;
	}


	for (gameState.year; gameState.year < 10; gameState.year++)
	{
		updateRandomGameStateParameters(gameState);

		if (gameState.year > 0) {
			if (!justLoadedGameFlag) {
				std::cout << "Начинается новый год правления. Продолжить игру? (y/n): ";
				char confirmation;
				std::cin >> confirmation;

				if (confirmation == 'n' || confirmation == 'N') {
					saveGame(gameState);
					return 0;
				}

			}
			else {
				justLoadedGameFlag = false;
			}


			// Collected Wheat
			gameState.wheatCollectedThisYear = gameState.landSeeded * gameState.wheatPerAcre;
			gameState.wheat += gameState.wheatCollectedThisYear;

			// Rats eating
			gameState.wheatEatenByRats = ceil(gameState.ratsEatingRate * gameState.wheat);
			gameState.wheat -= gameState.wheatEatenByRats;

			// Feeding Population
			int availableFood;
			gameState.diedFromHunger = 0;
			std::cout << "Wheat Reserved: " << gameState.wheatReserved << std::endl;
			if (gameState.wheatReserved <= gameState.wheat) {
				availableFood = gameState.wheatReserved;
			}
			else {
				availableFood = gameState.wheat;
			}


			int feededAmount = availableFood / 20;
			double currentHungerDeathToll = 0.0;

			if (feededAmount < gameState.population) {
				gameState.diedFromHunger = gameState.population - feededAmount;
				currentHungerDeathToll = static_cast<double>(gameState.diedFromHunger) / gameState.population;
				std::cout << "HungerDeathToll: " << currentHungerDeathToll << std::endl;
				if (currentHungerDeathToll > 0.45) {
					std::cout << "Game Over: в вашей стране случился голодный мор. Толпа изголодавшихся крестьян порвала в клочья вашу охрану, а затем сделала то же самое и с вами..." << std::endl;
					return 0;
				}
				hungerDeathTollTotal += currentHungerDeathToll;

				gameState.population = feededAmount;
				gameState.wheat -= availableFood;
			}

			else {
				gameState.diedFromHunger = 0;
				gameState.wheat -= gameState.population * 20;
			}

			gameState.wheatReserved = 0;

			// New arrivals
			int newArrivalsCame = static_cast<int>(gameState.diedFromHunger / 2 + (5 - gameState.wheatPerAcre) * gameState.wheat / 600 + 1);
			if (newArrivalsCame < 0) newArrivalsCame = 0;
			else if (newArrivalsCame > 50) newArrivalsCame = 50;
			gameState.arrived = newArrivalsCame;

			// Plague (considering, that people got sick before arrivals came)
			gameState.diedFromPlague = 0;
			if (gameState.plagueProbabilityScore <= 15) {
				gameState.diedFromPlague = gameState.population - gameState.population / 2;
				gameState.population -= gameState.diedFromPlague;
			}

			gameState.population += newArrivalsCame;



		}


		GameState gameStateCopy = gameState;


		gameStateReport(gameStateCopy);

		processLandSale(gameStateCopy, landSold);
		gameStateReportAfterOrder(gameStateCopy);

		processLandPurchase(gameStateCopy, landBought);
		gameStateReportAfterOrder(gameStateCopy);

		processFoodReservation(gameStateCopy, wheatReservedForFood);
		gameStateReportAfterOrder(gameStateCopy);

		processLandSeeding(gameStateCopy, landSeeded);
		gameStateReportAfterOrder(gameStateCopy);

		gameState = gameStateCopy;

	}

	// Checking game Results
	double P = hungerDeathTollTotal / 10 * 100;
	double L = static_cast<double>(gameState.land) / gameState.population;

	if (P > 33.0 && L < 7.0) {
		std::cout
			<< "Из-за вашей некомпетентности в управлении, народ устроил бунт, "
			<< "и изгнал вас их города. Теперь вы вынуждены влачить жалкое "
			<< "существование в изгнании."
			<< std::endl;
	}
	else if (P > 10.0 && L < 9.0) {
		std::cout
			<< "Вы правили железной рукой, подобно Нерону и Ивану Грозному. "
			<< "Народ вздохнул с облегчением, и никто больше не желает видеть "
			<< "вас правителем."
			<< std::endl;
	}
	else if (P > 3.0 && L < 10.0) {
		std::cout
			<< "Вы справились вполне неплохо, у вас, конечно, есть "
			<< "недоброжелатели, но многие хотели бы увидеть вас во главе "
			<< "города снова."
			<< std::endl;
	}
	else {
		std::cout
			<< "Фантастика! Карл Великий, Дизраэли и Джефферсон вместе не "
			<< "справились бы лучше."
			<< std::endl;
	}
}
