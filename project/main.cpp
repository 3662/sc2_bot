# include <sc2api/sc2_api.h>
# include <iostream>

using namespace sc2;

class Bot : public Agent {
public:
	virtual void OnGameStart() final {
		const GameInfo &game_info = Observation()->GetGameInfo();
		const ObservationInterface *observation = Observation();

		// gets the potential enemy start locations
		std::vector<Point2D> el = game_info.enemy_start_locations;

		// gets our location
		nexus = observation->GetStartLocation();

		// for each of the three potential locations
		for (const auto &l : el) {
			// send a probe to scout 
			scout(l);
		}
	}

	virtual void OnStep() final {
		// for now building pylons and a forge in a random location
		build_pylon(); 
		build_forge();
	}

	virtual void OnUnitIdle(const Unit *unit) final {
		const ObservationInterface *observation = Observation();

		switch (unit->unit_type.ToType()) {
			case UNIT_TYPEID::PROTOSS_NEXUS: {
				// build up to 21 workers. three scouts and 18 miners
				if (observation->GetFoodWorkers() < 21) {
					Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_PROBE);
				}
				break;
			}
			case UNIT_TYPEID::PROTOSS_PROBE: {
				if (is_scout(unit)) {
					break;
				}

				// sends the probe to the nearest nime from the nexus
				const Unit *mineral_target = FindNearestMineralPatch(nexus);
				if (!mineral_target) {
					break;
				}
				Actions()->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);
				break;
			}
			default: {
				break;
			}
		}
	}

	bool build_pylon() {
		const ObservationInterface *observation = Observation();

		// only when supply capped (for now)
		if (observation->GetFoodUsed() <= observation->GetFoodCap() - 2) {
			return false;
		}

		build(ABILITY_ID::BUILD_PYLON, 3, 3);
	}

	bool build_forge() {
		const ObservationInterface *observation = Observation();

		// build only if there is a pylon
		if (CountUnitType(UNIT_TYPEID::PROTOSS_PYLON) < 1) {
			return false; 
		}

		// only build if there is no forge
		if (CountUnitType(UNIT_TYPEID::PROTOSS_FORGE) > 0) {
			return false;
		}

		return build(ABILITY_ID::BUILD_FORGE, 3, 3);
	}

	// builds the given type of structure in the given coordinates
	// for now building in a random location
	bool build(ABILITY_ID ability_type_for_structure, int x, int y) {
		const ObservationInterface *observation = Observation();

		const Unit *unit_to_build = nullptr;

		Units units = observation->GetUnits(Unit::Alliance::Self);

		// get random probe to build
		for (const auto &unit : units) {
			// if probe and not a scout select this unit to build the structure
			if (unit->unit_type == UNIT_TYPEID::PROTOSS_PROBE &&
				!is_scout(unit)) {
				unit_to_build = unit;
			}
			
		}

		float rx = GetRandomScalar();
		float ry = GetRandomScalar();

		Actions()->UnitCommand(unit_to_build, ability_type_for_structure,
			Point2D(unit_to_build->pos.x + rx * 15.0f, unit_to_build->pos.y + ry * 15.0f));

		return true;
	}

	// send a worker to scout given location
	const void scout(Point2D location) {
		const ObservationInterface *observation = Observation();

		Units units = observation->GetUnits(Unit::Alliance::Self);

		// select random worker that is not a scout
		const Unit *scout;

		for (const auto &unit : units) {
			// if probe and not a scout then select this unit 
			if (unit->unit_type == UNIT_TYPEID::PROTOSS_PROBE && 
				!is_scout(unit)) {
				scout = unit;
			}
		}

		// send it to scout given location and add it as a scout
		scouts.push_back(scout);
		Actions()->UnitCommand(scout, ABILITY_ID::SMART, location);
	}

	// copied from tutorial 
	// https://blizzard.github.io/s2client-api/
	size_t CountUnitType(UNIT_TYPEID unit_type) {
		return Observation()->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
	}

	// copied from tutorial 
	// https://blizzard.github.io/s2client-api/
	const Unit *FindNearestMineralPatch(const Point2D &start) {
		Units units = Observation()->GetUnits(Unit::Alliance::Neutral);
		float distance = std::numeric_limits<float>::max();

		const Unit *target = nullptr;

		for (const auto &u : units) {
			if (u->unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD) {
				float d = DistanceSquared2D(u->pos, start);
				if (d < distance) {
					distance = d;
					target = u;
				}
			}
		}

		return target;
	}

	// returns true if unit is a scout 
	const bool is_scout(const Unit *unit) {
		for (const auto &u : scouts) {
			if (u == unit) {
				return true;
			}
		}
		return false;
	}

private:
	// contains the three probe scouts
	std::vector<const Unit*> scouts;

	// start location (of our nexus)
	Point2D nexus;

};

int main(int argc, char* argv[]) {
	Coordinator coordinator;
	coordinator.LoadSettings(argc, argv);

	Bot bot;
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