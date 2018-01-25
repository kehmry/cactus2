#include "Common.h"
#include "CCBot.h"
#include "JSONTools.h"
#include "Util.h" 

#include "sc2utils/sc2_manage_process.h"
#include "sc2api/sc2_api.h" 

#ifndef LADDEREXE
int main(int argc, char* argv[])
{
	sc2::Coordinator coordinator;
	if (!coordinator.LoadSettings(argc, argv))
	{
		std::cout << "Unable to find or parse settings." << std::endl;
		return 1;
	}

	std::string config = JSONTools::ReadFile("BotConfig.txt");
	if (config.length() == 0)
	{
		std::cerr << "Config file could not be found, and is required for starting the bot\n";
		std::cerr << "Please read the instructions and try again\n";
		exit(-1);
	}

	std::ifstream file("BotConfig.txt");
	json j;
	file >> j;

	/*if (parsingFailed)
	{
	std::cerr << "Config file could not be parsed, and is required for starting the bot\n";
	std::cerr << "Please read the instructions and try again\n";
	exit(-1);
	}*/

	std::string botRaceString;
	std::string enemyRaceString;
	std::string mapString;
	int stepSize = 1;
	sc2::Difficulty enemyDifficulty = sc2::Difficulty::Easy;

	if (j.count("SC2API") && j["SC2API"].is_object())
	{
		const json & info = j["SC2API"];
		JSONTools::ReadString("BotRace", info, botRaceString);
		JSONTools::ReadString("EnemyRace", info, enemyRaceString);
		JSONTools::ReadString("MapFile", info, mapString);
		JSONTools::ReadInt("StepSize", info, stepSize);
		JSONTools::ReadInt("EnemyDifficulty", info, enemyDifficulty);
	}
	else
	{
		std::cerr << "Config file has no 'Game Info' object, required for starting the bot\n";
		std::cerr << "Please read the instructions and try again\n";
		exit(-1);
	}

	// Add the custom bot, it will control the players.
	CCBot bot;


	// WARNING: Bot logic has not been thorougly tested on step sizes > 1
	//          Setting this = N means the bot's onFrame gets called once every N frames
	//          The bot may crash or do unexpected things if its logic is not called every frame
	coordinator.SetStepSize(stepSize);
	coordinator.SetRealtime(false);

	coordinator.SetParticipants({
		sc2::CreateParticipant(Util::GetRaceFromString(botRaceString), &bot),
		sc2::CreateComputer(Util::GetRaceFromString(enemyRaceString), enemyDifficulty)
	});

	// Start the game.
	coordinator.LaunchStarcraft();
	coordinator.StartGame(mapString);

	// Step forward the game simulation.
	while (coordinator.Update())
	{
	}

	return 0;
}
#else
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_arg_parser.h"

struct ConnectionOptions
{
	int32_t GamePort;
	int32_t StartPort;
	std::string ServerAddress;
};

bool ParseArguments(int argc, char *argv[], ConnectionOptions &connect_options)
{
	sc2::ArgParser arg_parser(argv[0]);
	arg_parser.AddOptions({
		{ "-g", "--GamePort", "Port of client to connect to", false },
		{ "-o", "--StartPort", "Starting server port", false },
		{ "-l", "--LadderServer", "Ladder server address", false },
	});
	arg_parser.Parse(argc, argv);
	std::string GamePortStr;
	if (arg_parser.Get("GamePort", GamePortStr)) {
		connect_options.GamePort = atoi(GamePortStr.c_str());
	}
	std::string StartPortStr;
	if (arg_parser.Get("StartPort", StartPortStr)) {
		connect_options.StartPort = atoi(StartPortStr.c_str());
	}
	arg_parser.Get("LadderServer", connect_options.ServerAddress);
	return true;
}

//*************************************************************************************************

std::string filePath;
std::string fileName;

int main(int argc, char* argv[]) {
	//We need this in Util to find the BotConfig.txt
	std::string fileWithPath = argv[0];
	filePath = fileWithPath.substr(0, fileWithPath.rfind("/"));
	fileName = fileWithPath.substr(fileWithPath.rfind("/") + 1, fileWithPath.rfind(".exe") - fileWithPath.rfind("/") - 1);

	ConnectionOptions Options;
	ParseArguments(argc, argv, Options);
	sc2::Coordinator coordinator;
	if (!coordinator.LoadSettings(argc, argv)) {
		return 1;
	}
	std::string configPath = Util::ExePath() + "\\BotConfig.txt";
	std::string config = JSONTools::ReadFile(configPath);
	if (config.length() == 0)
	{
		std::cerr << "Config file could not be found, and is required for starting the bot\n";
		std::cerr << "Please read the instructions and try again\n";
		while (true);
		exit(-1);
	}

	std::ifstream file(configPath);
	json j;
	file >> j;

	std::string botRaceString;

	if (j.count("SC2API") && j["SC2API"].is_object())
	{
		const json & info = j["SC2API"];
		JSONTools::ReadString("BotRace", info, botRaceString);
	}
	else
	{
		std::cerr << "Config file has no 'Game Info' object, required for starting the bot\n";
		std::cerr << "Please read the instructions and try again\n";
		exit(-1);
	}


	// Add the custom bot, it will control the players.
	CCBot bot;
	coordinator.SetParticipants({
		CreateParticipant(Util::GetRaceFromString(botRaceString), &bot),
	});

	// Start the game.

	// Step forward the game simulation.
	std::cout << "Connecting to port " << Options.GamePort << std::endl;
	coordinator.Connect(Options.GamePort);
	coordinator.SetupPorts(2, Options.StartPort, false);
	// Step forward the game simulation.
	coordinator.JoinGame();
	coordinator.SetTimeoutMS(10000);
	std::cout << " Successfully joined game" << std::endl;
	while (coordinator.Update()) {
	}

	return 0;
}
#endif // !LADDEREXE