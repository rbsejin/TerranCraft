#pragma once
#include "../Common/typedef.h"

namespace BW
{
    enum class ImageNumber : uint16
    {
        Scourge,
        Scourge_Shadow,
        Scourge_Birth,
        Scourge_Death,
        Scourge_Explosion,
        Broodling,
        Broodling_Shadow,
        Broodling_Remnants,
        Infested_Terran,
        Infested_Terran_Shadow,
        Infested_Terran_Explosion,
        Guardian_Cocoon,
        Guardian_Cocoon_Shadow,
        Defiler,
        Defiler_Shadow,
        Defiler_Birth,
        Defiler_Remnants,
        Drone,
        Drone_Shadow,
        Drone_Birth,
        Drone_Remnants,
        Egg,
        Egg_Shadow,
        Egg_Spawn,
        Egg_Remnants,
        Guardian,
        Guardian_Shadow,
        Guardian_Birth,
        Guardian_Death,
        Hydralisk,
        Hydralisk_Shadow,
        Hydralisk_Birth,
        Hydralisk_Remnants,
        Infested_Kerrigan,
        Infested_Kerrigan_Shadow,
        Needle_Spines,
        Larva,
        Larva_Remnants,
        Mutalisk,
        Mutalisk_Shadow,
        Mutalisk_Birth,
        Mutalisk_Death,
        Overlord,
        Overlord_Shadow,
        Overlord_Birth,
        Overlord_Death,
        Queen,
        Queen_Shadow,
        Queen_Death,
        Queen_Birth,
        Ultralisk,
        Ultralisk_Shadow,
        Ultralisk_Birth,
        Ultralisk_Remnants,
        Zergling,
        Zergling_Shadow,
        Zergling_Birth,
        Zergling_Remnants,
        Zerg_Air_Death_Explosion_Large,
        Zerg_Air_Death_Explosion_Small,
        Zerg_Building_Explosion,
        Cerebrate,
        Cerebrate_Shadow,
        Infested_Command_Center,
        Spawning_Pool,
        Spawning_Pool_Shadow,
        Evolution_Chamber,
        Evolution_Chamber_Shadow,
        Creep_Colony,
        Creep_Colony_Shadow,
        Hatchery,
        Hatchery_Shadow,
        Hive,
        Hive_Shadow,
        Lair,
        Lair_Shadow,
        Sunken_Colony,
        Sunken_Colony_Shadow,
        Mature_Chrysalis,
        Mature_Chrysalis_Shadow,
        Greater_Spire,
        Greater_Spire_Shadow,
        Defiler_Mound,
        Defiler_Mound_Shadow,
        Queens_Nest,
        Queen_Nest_Shadow,
        Nydus_Canal,
        Nydus_Canal_Shadow,
        Overmind_With_Shell,
        Overmind_Remnants,
        Overmind_Without_Shell,
        Ultralisk_Cavern,
        Ultralisk_Cavern_Shadow,
        Extractor,
        Extractor_Shadow,
        Hydralisk_Den,
        Hydralisk_Den_Shadow,
        Spire,
        Spire_Shadow,
        Spore_Colony,
        Spore_Colony_Shadow,
        Infested_Command_Center_Overlay,
        Zerg_Construction_Large,
        Zerg_Building_Morph,
        Zerg_Construction_Medium,
        Zerg_Construction_Small,
        Zerg_Building_Construction_Shadow,
        Zerg_Building_Spawn_Small,
        Zerg_Building_Spawn_Medium,
        Zerg_Building_Spawn_Large,
        Zerg_Building_Rubble_Small,
        Zerg_Building_Rubble_Large,
        Carrier,
        Carrier_Shadow,
        Carrier_Engines,
        Carrier_Warp_Flash,
        Interceptor,
        Interceptor_Shadow,
        Shuttle,
        Shuttle_Shadow,
        Shuttle_Engines,
        Shuttle_Warp_Flash,
        Dragoon,
        Dragoon_Shadow,
        Dragoon_Remnants,
        Dragoon_Warp_Flash,
        High_Templar,
        High_Templar_Shadow,
        High_Templar_Warp_Flash,
        Dark_Templar_Hero,
        Arbiter,
        Arbiter_Shadow,
        Arbiter_Engines,
        Arbiter_Warp_Flash,
        Archon_Energy,
        Archon_Being,
        Archon_Swirl,
        Probe,
        Probe_Shadow,
        Probe_Warp_Flash,
        Scout,
        Scout_Shadow,
        Scout_Engines,
        Scout_Warp_Flash,
        Reaver,
        Reaver_Shadow,
        Reaver_Warp_Flash,
        Scarab,
        Observer,
        Observer_Shadow,
        Observer_Warp_Flash,
        Zealot,
        Zealot_Shadow,
        Zealot_Death,
        Zealot_Warp_Flash,
        Templar_Archives,
        Templar_Archives_Warp_Flash,
        Templar_Archives_Shadow,
        Assimilator,
        Assimilator_Warp_Flash,
        Assimilator_Shadow,
        Observatory,
        Observatory_Warp_Flash,
        Observatory_Shadow,
        Citadel_of_Adun,
        Citadel_of_Adun_Warp_Flash,
        Citadel_of_Adun_Shadow,
        Forge,
        Forge_Overlay,
        Forge_Warp_Flash,
        Forge_Shadow,
        Gateway,
        Gateway_Warp_Flash,
        Gateway_Shadow,
        Cybernetics_Core,
        Cybernetics_Core_Warp_Flash,
        Cybernetics_Core_Overlay,
        Cybernetics_Core_Shadow,
        Khaydarin_Crystal_Formation,
        Nexus,
        Nexus_Warp_Flash,
        Nexus_Overlay,
        Nexus_Shadow,
        Photon_Cannon,
        Photon_Cannon_Shadow,
        Photon_Cannon_Warp_Flash,
        Arbiter_Tribunal,
        Arbiter_Tribunal_Warp_Flash,
        Arbiter_Tribunal_Shadow,
        Pylon,
        Pylon_Warp_Flash,
        Pylon_Shadow,
        Robotics_Facility,
        Robotics_Facility_Warp_Flash,
        Robotics_Facility_Shadow,
        Shield_Battery,
        Shield_Battery_Overlay,
        Shileld_Battery_Warp_Flash,
        Shield_Battery_Shadow,
        Stargate,
        Stargate_Overlay,
        Stargate_Warp_Flash,
        Stargate_Shadow,
        Stasis_Cell_Prison,
        Robotics_Support_Bay,
        Robotics_Support_Bay_Warp_Flash,
        Robotics_Support_Bay_Shadow,
        Protoss_Temple,
        Fleet_Beacon,
        Fleet_Beacon_Warp_Flash,
        Warp_Texture,
        Warp_Anchor,
        Fleet_Beacon_Shadow,
        Explosion1_Small,
        Explosion1_Medium,
        Explosion_Large,
        Protoss_Building_Rubble_Small,
        Protoss_Building_Rubble_Large,
        Battlecruiser,
        Battlecruiser_Shadow,
        Battlecruiser_Engines,
        Civilian,
        Civilian_Shadow,
        Dropship,
        Dropship_Shadow,
        Dropship_Engines,
        Firebat,
        Firebat_Shadow,
        Ghost,
        Ghost_Shadow,
        Ghost_Remnants,
        Ghost_Death,
        Nuke_Beam,
        Nuke_Target_Dot,
        Goliath_Base,
        Goliath_Turret,
        Goliath_Shadow,
        Sarah_Kerrigan,
        Sarah_Kerrigan_Shadow,
        Marine,
        Marine_Shadow,
        Marine_Remnants,
        Marine_Death,
        Wraith,
        Wraith_Shadow,
        Wraith_Engines,
        Scanner_Sweep,
        SCV,
        SCV_Shadow,
        SCV_Glow,
        Siege_Tank_Tank_Base,
        Siege_Tank_Tank_Turret,
        Siege_Tank_Tank_Base_Shadow,
        Siege_Tank_Siege_Base,
        Siege_Tank_Siege_Turret,
        Siege_Tank_Siege_Base_Shadow,
        Vulture,
        Vulture_Shadow,
        Spider_Mine,
        Spider_Mine_Shadow,
        Science_Vessel_Base,
        Science_Vessel_Turret,
        Science_Vessel_Shadow,
        Terran_Academy,
        Academy_Overlay,
        Academy_Shadow,
        Barracks,
        Barracks_Shadow,
        Armory,
        Armory_Overlay,
        Armory_Shadow,
        Comsat_Station,
        Comsat_Station_Connector,
        Comsat_Station_Overlay,
        Comsat_Station_Shadow,
        Command_Center,
        Command_Center_Overlay,
        Command_Center_Shadow,
        Supply_Depot,
        Supply_Depot_Overlay,
        Supply_Depot_Shadow,
        Control_Tower,
        Control_Tower_Connector,
        Control_Tower_Overlay,
        Control_Tower_Shadow,
        Factory,
        Factory_Overlay,
        Factory_Shadow,
        Covert_Ops,
        Covert_Ops_Connector,
        Covert_Ops_Overlay,
        Covert_Ops_Shadow,
        Ion_Cannon,
        Machine_Shop,
        Machine_Shop_Connector,
        Machine_Shop_Shadow,
        Missile_Turret_Base,
        Missile_Turret_Turret,
        Missile_Turret_Base_Shadow,
        Crashed_Batlecruiser,
        Crashed_Battlecruiser_Shadow,
        Physics_Lab,
        Physics_Lab_Connector,
        Physics_Lab_Shadow,
        Bunker,
        Bunker_Shadow,
        Bunker_Overlay,
        Refinery,
        Refinery_Shadow,
        Science_Facility,
        Science_Facility_Overlay,
        Science_Facility_Shadow,
        Nuclear_Silo,
        Nuclear_Silo_Connector,
        Nuclear_Silo_Overlay,
        Nuclear_Silo_Shadow,
        Nuclear_Missile,
        Nuclear_Missile_Shadow,
        Nuke_Hit,
        Starport,
        Starport_Overlay,
        Starport_Shadow,
        Engineering_Bay,
        Engineering_Bay_Overlay,
        Engineering_Bay_Shadow,
        Terran_Construction_Large,
        Terran_Construction_Large_Shadow,
        Terran_Construction_Medium,
        Terran_Construction_Medium_Shadow,
        Addon_Construction,
        Terran_Construction_Small,
        Terran_Construction_Small_Shadow,
        Explosion2_Small,
        Explosion2_Medium,
        Building_Explosion_Large,
        Terran_Building_Rubble_Small,
        Terran_Building_Rubble_Large,
        Dark_Swarm,
        Ragnasaur_Ash,
        Ragnasaur_Ash_Shadow,
        Rhynadon_Badlands,
        Rhynadon_Badlands_Shadow,
        Bengalaas_Jungle,
        Bengalaas_Jungle_Shadow,
        Vespene_Geyser,
        Vespene_Geyser2,
        Vespene_Geyser_Shadow,
        Mineral_Field_Type1,
        Mineral_Field_Type1_Shadow,
        Mineral_Field_Type2,
        Mineral_Field_Type2_Shadow,
        Mineral_Field_Type3,
        Mineral_Field_Type3_Shadow,
        Independent_Starport_Unused,
        Zerg_Beacon,
        Zerg_Beacon_Overlay,
        Terran_Beacon,
        Terran_Beacon_Overlay,
        Protoss_Beacon,
        Protoss_Beacon_Overlay,
        Magna_Pulse_Unused,
        Lockdown_Field_Small,
        Lockdown_Field_Medium,
        Lockdown_Field_Large,
        Stasis_Field_Hit,
        Stasis_Field_Small,
        Stasis_Field_Medium,
        Stasis_Field_Large,
        Recharge_Shields_Small,
        Recharge_Shields_Medium,
        Recharge_Shields_Large,
        Defensive_Matrix_Front_Small,
        Defensive_Matrix_Front_Medium,
        Defensive_Matrix_Front_Large,
        Defensive_Matrix_Back_Small,
        Defensive_Matrix_Back_Medium,
        Defensive_Matrix_Back_Large,
        Defensive_Matrix_Hit_Small,
        Defensive_Matrix_Hit_Medium,
        Defensive_Matrix_Hit_Large,
        Irradiate_Small,
        Irradiate_Medium,
        Irradiate_Large,
        Ensnare_Cloud,
        Ensnare_Overlay_Small,
        Ensnare_Overlay_Medium,
        Ensnare_Overlay_Large,
        Plague_Cloud,
        Plague_Overlay_Small,
        Plague_Overlay_Medium,
        Plague_Overlay_Large,
        Recall_Field,
        Flag,
        Young_Chrysalis,
        Psi_Emitter,
        Data_Disc,
        Khaydarin_Crystal,
        Mineral_Chunk_Type1,
        Mineral_Chunk_Type2,
        Protoss_Gas_Orb_Type1,
        Protoss_Gas_Orb_Type2,
        Zerg_Gas_Sac_Type1,
        Zerg_Gas_Sac_Type2,
        Terran_Gas_Tank_Type1,
        Terran_Gas_Tank_Type2,
        Mineral_Chunk_Shadow,
        Protoss_Gas_Orb_Shadow,
        Zerg_Gas_Sac_Shadow,
        Terran_Gas_Tank_Shadow,
        Data_Disk_Shadow_Ground,
        Data_Disk_Shadow_Carried,
        Flag_Shadow_Ground,
        Flag_Shadow_Carried,
        Crystal_Shadow_Ground,
        Crystal_Shadow_Carried,
        Young_Chrysalis_Shadow_Ground,
        Young_Chrysalis_Shadow_Carried,
        Psi_Emitter_Shadow_Ground,
        Psi_Emitter_Shadow_Carried,
        Acid_Spray_Unused,
        Plasma_Drip_Unused,
        FlameThrower,
        Longbolt_Gemini_Missiles_Trail,
        Burrowing_Dust,
        Shield_Overlay,
        Small_Explosion_Unused,
        Double_Explosion,
        Phase_Disruptor_Hit,
        Nuclear_Missile_Death,
        Spider_Mine_Death,
        Vespene_Geyser_Smoke1,
        Vespene_Geyser_Smoke2,
        Vespene_Geyser_Smoke3,
        Vespene_Geyser_Smoke4,
        Vespene_Geyser_Smoke5,
        Vespene_Geyser_Smoke1_Overlay,
        Vespene_Geyser_Smoke2_Overlay,
        Vespene_Geyser_Smoke3_Overlay,
        Vespene_Geyser_Smoke4_Overlay,
        Vespene_Geyser_Smoke5_Overlay,
        Fragmentation_Grenade_Hit,
        Grenade_Shot_Smoke,
        Anti_Matter_Missile_Hit,
        Scarab_Anti_Matter_Missile_Overlay,
        Scarab_Hit,
        Cursor_Marker,
        Battlecruiser_Attack_Overlay,
        Burst_Lasers_Hit,
        Burst_Lasers_Overlay_Unused,
        High_Templar_Glow,
        Flames1_Type1_Small,
        Flames1_Type2_Small,
        Flames1_Type3_Small,
        Flames2_Type3_Small,
        Flames3_Type3_Small,
        Flames4_Type3_Small,
        Flames5_Type3_Small,
        Flames6_Type3_Small,
        Bleeding_Variant1_Type1_Small,
        Bleeding_Variant1_Type2_Small,
        Bleeding_Variant1_Type3_Small,
        Bleeding_Variant1_Type4_Small,
        Bleeding_Variant2_Type1_Small,
        Bleeding_Variant2_Type2_Small,
        Bleeding_Variant2_Type3_Small,
        Bleeding_Variant2_Type4_Small,
        Flames2_Type1_Small,
        Flames2_Type2_Small,
        Flames7_Type3_Small,
        Flames3_Type1_Small,
        Flames3_Type2_Small,
        Flames8_Type3_Small,
        Flames1_Type1_Large,
        Flames1_Type2_Large,
        Flames1_Type3_Large,
        Flames2_Type3_Large,
        Flames3_Type3_Large,
        Flames4_Type3_Large,
        Flames5_Type3_Large,
        Flames6_Type3_Large,
        Bleeding_Variant1_Type1_Large,
        Bleeding_Variant1_Type2_Large,
        Bleeding_Variant1_Type3_Large,
        Bleeding_Variant1_Type4_Large,
        Bleeding_Variant2_Type1_Large,
        Bleeding_Variant2_Type3_Large,
        Bleeding_Variant3_Type3_Large,
        Bleeding_Variant2_Type4_Large,
        Flames2_Type1_Large,
        Flames2_Type2_Large,
        Flames7_Type3_Large,
        Flames3_Type1_Large,
        Flames3_Type2_Large,
        Flames8_Type3_Large,
        Building_Landing_Dust_Type1,
        Building_Landing_Dust_Type2,
        Building_Landing_Dust_Type3,
        Building_Landing_Dust_Type4,
        Building_Landing_Dust_Type5,
        Building_Lifting_Dust_Type1,
        Building_Lifting_Dust_Type2,
        Building_Lifting_Dust_Type3,
        Building_Lifting_Dust_Type4,
        White_Circle,
        Needle_Spine_Hit,
        Plasma_Drip_Hit_Unused,
        Sunken_Colony_Tentacle,
        Venom_Unused_Zerg_Weapon,
        Venom_Hit_Unused,
        Acid_Spore,
        Acid_Spore_Hit,
        Glave_Wurm,
        Glave_Wurm_Seeker_Spores_Hit,
        Glave_Wurm_Trail,
        Seeker_Spores_Overlay,
        Seeker_Spores,
        Queen_Spell_Holder,
        Consume,
        Guardian_Attack_Overlay,
        Dual_Photon_Blasters_Hit,
        Particle_Beam_Hit,
        Anti_Matter_Missile,
        Pulse_Cannon,
        Phase_Disruptor,
        STA_STS_Photon_Cannon_Overlay,
        Psionic_Storm,
        Fusion_Cutter_Hit,
        Gauss_Rifle_Hit,
        Gemini_Missiles,
        Lockdown_LongBolt_Hellfire_Missile,
        Gemini_Missiles_Explosion,
        C_10_Canister_Rifle_Hit,
        Fragmentation_Grenade,
        Arclite_Shock_Cannon_Hit,
        ATS_ATA_Laser_Battery,
        Burst_Lasers,
        Siege_TankTank_Turret_Attack_Overlay,
        Siege_TankSiege_Turret_Attack_Overlay,
        Science_Vessel_Overlay_Part1,
        Science_Vessel_Overlay_Part2,
        Science_Vessel_Overlay_Part3,
        Yamato_Gun,
        Yamato_Gun_Trail,
        Yamato_Gun_Overlay,
        Yamato_Gun_Hit,
        Hallucination_Hit,
        Scanner_Sweep_Hit,
        Archon_Birth_Unused,
        Psionic_Shockwave_Hit,
        Archon_Beam,
        Psionic_Storm_Part1,
        Psionic_Storm_Part2,
        Psionic_Storm_Part3,
        Psionic_Storm_Part4,
        EMP_Shockwave_Missile,
        EMP_Shockwave_Hit_Part1,
        EMP_Shockwave_Hit_Part2,
        IMG_DEATH_PUFF_SMALL,
        IMG_DEATH_PUFF_MED,
        IMG_DEATH_PUFF_LARGE,
        IMG_HPFLOAT,
        IMG_SELECT_022,
        IMG_SELECT_032,
        IMG_SELECT_048,
        IMG_SELECT_062,
        IMG_SELECT_072,
        IMG_SELECT_094,
        IMG_SELECT_110,
        IMG_SELECT_122,
        IMG_SELECT_146,
        IMG_SELECT_224,
        IMG_TEAM_SELECT_022,
        IMG_TEAM_SELECT_032,
        IMG_TEAM_SELECT_048,
        IMG_TEAM_SELECT_062,
        IMG_TEAM_SELECT_072,
        IMG_TEAM_SELECT_094,
        IMG_TEAM_SELECT_110,
        IMG_TEAM_SELECT_122,
        IMG_TEAM_SELECT_146,
        IMG_TEAM_SELECT_224,
        IMG_LANDING_ZONE,
        Map_Revealer,
        IMG_BOX,
        IMG_PYLON_POWER_TOP,
        IMG_PYLON_POWER_BOTTOM,
        IMG_PYLON_POWER_TOP_FLIP,
        IMG_PYLON_POWER_BOTTOM_FLIP,
        IMG_STARTLOC,
        IMG_DD_ASHW_LOASH_ROCK01,
        IMG_DD_ASHW_LOASH_ROCK01_SHA,
        IMG_DD_ASHW_LOASH_ROCK02,
        IMG_DD_ASHW_LOASH_ROCK02_SHA,
        IMG_DD_ASHW_LOASH_ROCK03,
        IMG_DD_ASHW_LOASH_ROCK03_SHA,
        IMG_DD_ASHW_LOASH_ROCK04,
        IMG_DD_ASHW_LOASH_ROCK04_SHA,
        IMG_DD_ASHW_LOASH_ROCK05,
        IMG_DD_ASHW_LOASH_ROCK05_SHA,
        IMG_DD_ASHW_LOASH_LSPIRE01,
        IMG_DD_ASHW_LOASH_LSPIRE02,
        IMG_DD_ASHW_LOASH_SPIRE01,
        IMG_DD_ASHW_LOASH_SPIRE02,
        IMG_DD_ASHW_LOASH_SPIRE03,
        IMG_DD_ASHW_LOASH_SPIRE04,
        IMG_DD_ASHW_LOASH_SPIRE05,
        IMG_DD_ASHW_LOASH_SPIRE06,
        IMG_DD_ASHW_ROCKS_LAVA01,
        IMG_DD_ASHW_ROCKS_LAVA02,
        IMG_DD_ASHW_ROCKS_LAVA03,
        IMG_JUNGLE_ROCK01,
        IMG_JUNGLE_ROCK01_SHA,
        IMG_JUNGLE_ROCK02,
        IMG_JUNGLE_ROCK02_SHA,
        IMG_JUNGLE_ROCK03,
        IMG_JUNGLE_ROCK03_SHA,
        IMG_JUNGLE_ROCK04,
        IMG_JUNGLE_ROCK04_SHA,
        IMG_JUNGLE_BUSH01,
        IMG_JUNGLE_BUSH01_SHA,
        IMG_JUNGLE_BUSH02,
        IMG_JUNGLE_BUSH02_SHA,
        IMG_JUNGLE_BUSH03,
        IMG_JUNGLE_BUSH03_SHA,
        IMG_JUNGLE_OLDTREE01,
        IMG_JUNGLE_OLDTREE01_SHA,
        IMG_JUNGLE_OLDTREE02,
        IMG_JUNGLE_OLDTREE02_SHA,
        IMG_JUNGLE_OLDTREE03,
        IMG_JUNGLE_OLDTREE03_SHA,
        IMG_JUNGLE_OLDTREE04,
        IMG_JUNGLE_OLDTREE04_SHA,
        IMG_JUNGLE_TREE01,
        IMG_JUNGLE_TREE01_SHA,
        IMG_JUNGLE_TREE02,
        IMG_JUNGLE_TREE02_SHA,
        IMG_JUNGLE_TREE03,
        IMG_JUNGLE_TREE03_SHA,
        IMG_JUNGLE_TREE04,
        IMG_JUNGLE_TREE04_SHA,
        IMG_JUNGLE_025,
        IMG_JUNGLE_026,
        IMG_JUNGLE_027,
        IMG_JUNGLE_028,
        IMG_JUNGLE_029,
        IMG_JUNGLE_030,
        IMG_JUNGLE_031,
        IMG_JUNGLE_055,
        IMG_JUNGLE_056,
        IMG_JUNGLE_075,
        IMG_JUNGLE_076,
        IMG_JUNGLE_077,
        IMG_JUNGLE_078,
        IMG_JUNGLE_079,
        IMG_JUNGLE_080,
        IMG_JUNGLE_081,
        IMG_JUNGLE_091,
        IMG_JUNGLE_203,
        IMG_JUNGLE_204,
        IMG_JUNGLE_205,
        IMG_JUNGLE_206,
        IMG_JUNGLE_207,
        IMG_JUNGLE_209,
        IMG_JUNGLE_210,
        IMG_JUNGLE_211,
        IMG_DD_PLAT_DISH01,
        IMG_DD_PLAT_DISH01_SHA,
        IMG_DD_PLAT_DISH02,
        IMG_DD_PLAT_DISH02_SHA,
        IMG_DD_PLAT_DISH03,
        IMG_DD_PLAT_DISH03_SHA,
        IMG_DD_PLAT_GLOB01,
        IMG_DD_PLAT_GLOB01_SHA,
        IMG_DD_PLAT_GLOB02,
        IMG_DD_PLAT_GLOB02_SHA,
        IMG_DD_PLAT_GLOB03,
        IMG_DD_PLAT_GLOB03_SHA,
        IMG_DD_PLAT_TOWR01,
        IMG_DD_PLAT_TOWR02,
        IMG_PLATFORM_TREE01,
        IMG_PLATFORM_TREE02,
        IMG_DD_PLAT_SIGN01,
        IMG_DD_PLAT_SIGN02,
        IMG_DD_PLAT_SIGN03,
        IMG_DD_PLAT_SIGN04,
        IMG_DD_PLAT_SIGN05,
        IMG_DD_PLAT_SIGN06,
        IMG_DD_PLAT_SIGN07,
        IMG_DD_PLAT_SIGN08,
        IMG_DD_PLAT_SOLAR01,
        IMG_DD_PLAT_REFINERY,
        IMG_BADLANDS_SPIRE01,
        IMG_BADLANDS_SPIRE01_SHA,
        IMG_BADLANDS_SPIRE02,
        IMG_BADLANDS_SPIRE02_SHA,
        IMG_BADLANDS_SPIRE03,
        IMG_BADLANDS_SPIRE03_SHA,
        IMG_BADLANDS_SPIRE04,
        IMG_BADLANDS_SPIRE04_SHA,
        IMG_BADLANDS_ROCK05,
        IMG_BADLANDS_ROCK06,
        IMG_BADLANDS_ROCK07,
        IMG_BADLANDS_ROCK08,
        IMG_BADLANDS_VENT01,
        IMG_BADLANDS_TREE01,
        IMG_BADLANDS_TREE01_SHA,
        IMG_BADLANDS_TREE02,
        IMG_BADLANDS_TREE02_SHA,
        IMG_BADLANDS_TREE03,
        IMG_BADLANDS_TREE03_SHA,
        IMG_BADLANDS_TREE04,
        IMG_BADLANDS_TREE04_SHA,
        IMG_BADLANDS_TREE05,
        IMG_BADLANDS_SHOP01,
        IMG_BADLANDS_SHOP02,
        IMG_BADLANDS_SHOP03,
        IMG_BADLANDS_SHOP04,
        IMG_BADLANDS_SHOP05,
        IMG_BADLANDS_SHOP06,
        IMG_BADLANDS_SHOP07,
        IMG_BADLANDS_SHOP08,
        IMG_BADLANDS_SHOP09,
        IMG_BADLANDS_SHOP10,
        IMG_BADLANDS_SIGN01,
        IMG_BADLANDS_SIGN02,
        IMG_BADLANDS_SIGN03,
        IMG_BADLANDS_SIGN04,
        IMG_BADLANDS_SIGN05,
        IMG_BADLANDS_SIGN06,
        IMG_BADLANDS_SIGN07,
        IMG_BADLANDS_SIGN08,
        IMG_BADLANDS_SIGN09,
        IMG_BADLANDS_SIGN10,
        IMG_BADLANDS_SIGN11,
        IMG_BADLANDS_SIGN12,
        IMG_INSTALL_FLOOR_GUN,
        IMG_INSTALL_FLOOR_MISSILES,
        IMG_INSTALL_FLOOR_MISSILES_T,
        IMG_INSTALL_WALL_MISSILES,
        IMG_INSTALL_WALL_MISSILESF,
        IMG_INSTALL_WALL_FLAMER,
        IMG_INSTALL_WALL_FLAMERF,
        IMG_INSTALL_DOOR1,
        IMG_INSTALL_DOOR2,
        IMG_INSTALL_FAN1,
        IMG_INSTALL_FAN2,
        IMG_INSTALL_CRANE1,
        IMG_INSTALL_CRANE2,
        IMG_INSTALL_CRANE3,
        IMG_INSTALL_CRANE4,
        IMG_INSTALL_SPIKED_DOOR1,
        IMG_INSTALL_SPIKED_DOOR2,
        IMG_INSTALL_GEAR1,
        IMG_INSTALL_GEAR2,
        IMG_INSTALL_HATCH,
        IMG_7_21_Twilight,
        Unknown_Twilight,
        IMG_7_13_Twilight,
        IMG_7_14_Twilight,
        IMG_7_16_Twilight,
        IMG_7_15_Twilight,
        IMG_7_19_Twilight,
        IMG_7_20_Twilight,
        IMG_7_17_Twilight,
        IMG_6_1_Twilight,
        IMG_6_2_Twilight,
        IMG_6_3_Twilight,
        IMG_6_4_Twilight,
        IMG_6_5_Twilight,
        IMG_8_3_Twilight1,
        IMG_8_3_Twilight2,
        IMG_9_29_Ice,
        IMG_9_29_Ice_Shadow,
        IMG_9_28_Ice,
        IMG_9_28_Ice_Shadow,
        IMG_12_38_Ice_,
        IMG_12_38_Ice_Shadow,
        IMG_12_37_Ice,
        IMG_12_37_Ice_Shadow,
        IMG_12_33_Ice1,
        IMG_12_33_Ice1_Shadow,
        IMG_9_21_Ice,
        IMG_9_21_Ice_Shadow,
        IMG_9_15_Ice,
        IMG_9_15_Ice_Shadow,
        IMG_9_16_Ice,
        IMG_9_16_Ice_Shadow,
        Unknown787,
        Unknown788,
        IMG_12_9_Ice1,
        IMG_12_10_Ice1,
        IMG_9_24_Ice,
        IMG_9_24_Ice_Shadow,
        IMG_9_23_Ice,
        IMG_9_23_Ice_Shadow,
        Unknown795,
        Unknown_Ice_Shadow,
        IMG_12_7_Ice,
        IMG_12_7_Ice_Shadow,
        IMG_12_8_Ice,
        IMG_12_8_Ice_Shadow,
        IMG_12_9_Ice2,
        IMG_12_9_Ice2_Shadow,
        IMG_12_10_Ice2,
        IMG_12_10_Ice2_Shadow,
        IMG_12_40_Ice,
        IMG_12_40_Ice_Shadow,
        IMG_12_41_Ice,
        IMG_12_41_Ice_Shadow,
        IMG_12_42_Ice,
        IMG_12_42_Ice_Shadow,
        IMG_12_5_Ice,
        IMG_12_5_Ice_Shadow,
        IMG_12_6_Ice,
        IMG_12_6_Ice_Shadow,
        IMG_12_36_Ice,
        IMG_12_36_Ice_Shadow,
        IMG_12_32_Ice,
        IMG_12_32_Ice_Shadow,
        IMG_12_33_Ice2,
        IMG_12_33_Ice2_Shadow,
        IMG_12_34_Ice,
        IMG_12_34_Ice_Shadow,
        IMG_12_24_Ice1,
        IMG_12_24_Ice1_Shadow,
        IMG_12_25_Ice1,
        IMG_12_25_Ice1_Shadow,
        IMG_12_30_Ice1,
        IMG_12_30_Ice1_Shadow,
        IMG_12_31_Ice,
        IMG_12_31_Ice_Shadow,
        IMG_12_20_Ice,
        IMG_12_30_Ice2,
        IMG_12_30_Ice2_Shadow,
        IMG_9_22_Ice,
        IMG_9_22_Ice_Shadow,
        IMG_12_24_Ice2,
        IMG_12_24_Ice2_Shadow,
        IMG_12_25_Ice2,
        IMG_12_25_Ice2_Shadow,
        Unknown840,
        IMG_4_1_Ice,
        IMG_6_1_Ice,
        IMG_5_6_Ice_,
        IMG_5_6_Ice_Shadow,
        IMG_5_7_Ice_,
        IMG_5_7_Ice_Shadow,
        IMG_5_8_Ice_,
        IMG_5_8_Ice_Shadow,
        IMG_5_9_Ice,
        IMG_5_9_Ice_Shadow,
        IMG_10_10_Desert1,
        IMG_10_12_Desert1,
        IMG_10_12_Desert1_Shadow,
        IMG_10_8_Desert1,
        IMG_10_8_Desert1_Shadow,
        IMG_10_9_Desert1,
        IMG_10_9_Desert1_Shadow,
        IMG_6_10_Desert,
        IMG_6_10_Desert_Shadow,
        IMG_6_13_Desert1,
        IMG_6_13_Desert1_Shadow,
        Unknown_Desert,
        Unknown_Desert_Shadow,
        IMG_10_12_Desert2,
        IMG_10_12_Desert2_Shadow,
        IMG_10_9_Desert2,
        IMG_10_9_Desert2_Shadow,
        IMG_10_10_Desert2,
        IMG_10_10_Desert2_Shadow,
        IMG_10_11_Desert,
        IMG_10_11_Desert_Shadow,
        IMG_10_14_Desert,
        IMG_10_14_Desert_Shadow,
        IMG_10_41_Desert,
        IMG_10_41_Desert_Shadow,
        IMG_10_39_Desert,
        IMG_1_39_Desert_Shadow,
        IMG_10_8_Desert2,
        IMG_10_8_Desert2_Shadow,
        IMG_10_6_Desert,
        IMG_10_7_Desert,
        IMG_10_7_Desert_Shadow,
        IMG_4_6_Desert,
        IMG_4_6_Desert_Shadow,
        IMG_4_11_Desert,
        IMG_4_11_Desert_Shadow,
        IMG_4_10_Desert,
        IMG_4_10_Desert_Shadow,
        IMG_4_9_Desert,
        IMG_4_7_Desert,
        IMG_4_7_Desert_Shadow,
        IMG_4_12_Desert,
        IMG_4_12_Desert_Shadow,
        IMG_4_8_Desert,
        IMG_4_13_Desert,
        IMG_4_13_Desert_Shadow,
        IMG_4_17_Desert,
        IMG_4_15_Desert1,
        IMG_4_15_Desert1_Shadow,
        IMG_10_23_Desert,
        IMG_10_23_Desert_Shadow,
        IMG_10_5_Desert,
        IMG_10_5_Desert_Shadow,
        IMG_6_13_Desert2,
        IMG_6_13_Desert2_Shadow,
        IMG_6_20_Desert,
        IMG_4_15_Desert2,
        IMG_4_15_Desert2_Shadow,
        IMG_8_23_Desert,
        IMG_8_23_Desert_Shadow,
        IMG_12_1_Desert_Overlay,
        IMG_11_3_Desert,
        Unknown913,
        Lurker_Egg,
        Devourer,
        Devourer_Shadow,
        Devourer_Birth,
        Devourer_Death,
        Lurker_Birth,
        Lurker_Remnants,
        Lurker,
        Lurker_Shadow,
        Overmind_Cocoon,
        Overmind_Cocoon_Shadow,
        Dark_Archon_Energy,
        Dark_Archon_Being,
        Dark_Archon_Swirl,
        Dark_Archon_Death,
        Corsair,
        Corsair_Shadow,
        Corsair_Engines,
        Neutron_Flare_Overlay_Unused,
        Dark_Templar_Unit,
        Warp_Gate,
        Warp_Gate_Shadow,
        Warp_Gate_Overlay,
        XelNaga_Temple,
        XelNaga_Temple_Shadow,
        Valkyrie,
        Valkyrie_Shadow,
        Valkyrie_Engines,
        Valkyrie_Engines2_Unused,
        Valkyrie_Afterburners_Unused,
        Medic,
        Medic_Shadow,
        Medic_Remnants,
        Psi_Disrupter,
        Psi_Disrupter_Shadow,
        Power_Generator,
        Power_Generator_Shadow,
        Disruption_Web,
        Scantid_Desert,
        Scantid_Desert_Shadow,
        Kakaru_Twilight,
        Kakaru_Twilight_Shadow,
        Ursadon_Ice,
        Ursadon_Ice_Shadow,
        Uraj,
        Khalis,
        Halo_Rockets_Trail,
        Subterranean_Spines,
        Corrosive_Acid_Shot,
        Corrosive_Acid_Hit,
        Neutron_Flare,
        Halo_Rockets,
        Optical_Flare_Grenade,
        Restoration_Hit_Small,
        Restoration_Hit_Medium,
        Restoration_Hit_Large,
        Unused_Heal_Small,
        Unused_Heal_Medium,
        Unused_Heal_Large,
        Mind_Control_Hit_Small,
        Mind_Control_Hit_Medium,
        Mind_Control_Hit_Large,
        Optical_Flare_Hit_Small,
        Optical_Flare_Hit_Medium,
        Optical_Flare_Hit_Large,
        Feedback_Small,
        Feedback_Medium,
        Feedback_Hit_Large,
        Maelstorm_Overlay_Small,
        Maelstorm_Overlay_Medium,
        Maelstorm_Overlay_Large,
        Subterranean_Spines_Hit,
        Acid_Spores_1_Overlay_Small,
        Acid_Spores_2_3_Overlay_Small,
        Acid_Spores_4_5_Overlay_Small,
        Acid_Spores_6_9_Overlay_Small,
        Acid_Spores_1_Overlay_Medium,
        Acid_Spores_2_3_Overlay_Medium,
        Acid_Spores_4_5_Overlay_Medium,
        Acid_Spores_6_9_Overlay_Medium,
        Acid_Spores_1_Overlay_Large,
        Acid_Spores_2_3_Overlay_Large,
        Acid_Spores_4_5_Overlay_Large,
        Acid_Spores_6_9_Overlay_Large,
        Maelstorm_Hit,
        None
    };

    static_assert(static_cast<uint16>(ImageNumber::None) == 999, "There must be 999 images.");
}
