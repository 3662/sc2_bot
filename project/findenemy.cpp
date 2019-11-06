void Bot::look_for_enemy(){
    const ObservationInterface *observation = Observation();
    // get enemy units if any
    Units enemy_units = observation->GetUnits(Unit::Alliance::Enemy);
    for(const auto &unit : enemy_units){
        // check if it's enemy worker
        if(unit->unit_type == UNIT_TYPEID::PROTOSS_PROBE)
            continue;
        // not enemy worker, probably enemy base, this needs to improve later
        // send scouts to build pylon
        for(const auto &s : scouts){
            Actions->UnitCommand(s, ABILITY_ID:BUILD_PYLON, Point2D(unit->pos);
        }
    }
}