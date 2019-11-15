#include <sc2api/sc2_api.h>
#include <iostream>
#include "bot_master.h"

using namespace sc2;


int main(int argc, char* argv[]) {
	Coordinator coordinator;
	coordinator.LoadSettings(argc, argv);

	bot_master bot;
	coordinator.SetParticipants({
		CreateParticipant(Race::Protoss, &bot),
		CreateComputer(Race::Zerg)
	});

	coordinator.LaunchStarcraft();

	// in order for map to work create folder "Maps" where sc2.exe is located
	// inside that folder place file "CactusValleyLE.SC2Map"
	coordinator.StartGame("CactusValleyLE.SC2Map");

	while (coordinator.Update()) {

	}

	return 0;
}