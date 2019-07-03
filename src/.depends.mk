algorithm.o: algorithm.cpp algorithm.h tactic.h game_go.h goban.h \
  const_goban.h move.h const_move.h const_manager_game.h chain.h \
  const_geography.h tools_ge.h const_tools_ge.h bibliotheque.h marker.h \
  amaf.h simulation.h const_rule.h amaf2.h mc_locations_informations.h \
  fast_goban.h evaluation_score.h const_tactic.h strategy.h \
  param_strategy.h database_games_go.h file_sgf.h filter.h const_filter.h \
  statistic.h const_statistic.h block.h const_block.h param_block.h \
  location_information.h
amaf2.o: amaf2.cpp amaf2.h tools_ge.h const_tools_ge.h const_goban.h \
  const_move.h move.h game_go.h goban.h const_manager_game.h chain.h \
  const_geography.h bibliotheque.h marker.h
amaf.o: amaf.cpp amaf.h tools_ge.h const_tools_ge.h const_goban.h \
  const_move.h move.h
amaf_tree.o: amaf_tree.cpp amaf_tree.h amaf.h tools_ge.h const_tools_ge.h \
  const_goban.h const_move.h move.h simulation.h game_go.h goban.h \
  const_manager_game.h chain.h const_geography.h bibliotheque.h marker.h \
  const_rule.h amaf2.h mc_locations_informations.h
Arbre13.o: Arbre13.cpp Arbre13.h Forme_kppv13.h
Arbre.o: Arbre.cpp Arbre.h Forme_kppv.h goexperiments_mg.h typedefs_mg.h \
  goban_mg.h fastsmallvector_mg.h gobanurgency_mg.h vectormemorypool_mg.h \
  stringinformation_mg.h locationinformation_mg.h cfg_mg.h gotools_mg.h \
  smallpatternsdatabase_mg.h yadtinterface_mg.h
basic_instinct_fast_mg.o: basic_instinct_fast_mg.cpp \
  basic_instinct_fast_mg.h fastboard_mg.h typedefs_mg.h \
  fastsmallvector_mg.h tools.h const_tools.h goban_mg.h gobanurgency_mg.h \
  vectormemorypool_mg.h stringinformation_mg.h locationinformation_mg.h \
  cfg_mg.h gotools_mg.h smallpatternsdatabase_mg.h yadtinterface_mg.h \
  gobannode_mg.h pattern.h tools_ge.h const_tools_ge.h Arbre.h \
  Forme_kppv.h goexperiments_mg.h Forme_kppv13.h Element_kppv.h
bibliotheque.o: bibliotheque.cpp bibliotheque.h move.h const_move.h \
  const_geography.h
block.o: block.cpp block.h goban.h const_goban.h move.h const_move.h \
  const_geography.h const_block.h param_block.h
cfg_mg.o: cfg_mg.cpp cfg_mg.h stringinformation_mg.h typedefs_mg.h \
  fastsmallvector_mg.h locationinformation_mg.h gotools_mg.h goban_mg.h \
  gobanurgency_mg.h vectormemorypool_mg.h smallpatternsdatabase_mg.h \
  yadtinterface_mg.h
chain.o: chain.cpp chain.h goban.h const_goban.h move.h const_move.h \
  const_geography.h tools_ge.h const_tools_ge.h bibliotheque.h marker.h
coeff_pattern.o: coeff_pattern.cpp coeff_pattern.h
cputimechronometer_mg.o: cputimechronometer_mg.cpp \
  cputimechronometer_mg.h typedefs_mg.h
database_games_go.o: database_games_go.cpp database_games_go.h file_sgf.h \
  move.h const_move.h const_goban.h tools_ge.h const_tools_ge.h
Element_kppv.o: Element_kppv.cpp Element_kppv.h
evaluation_score.o: evaluation_score.cpp evaluation_score.h game_go.h \
  goban.h const_goban.h move.h const_move.h const_manager_game.h chain.h \
  const_geography.h tools_ge.h const_tools_ge.h bibliotheque.h marker.h \
  const_rule.h
fastboard_mg.o: fastboard_mg.cpp fastboard_mg.h typedefs_mg.h \
  fastsmallvector_mg.h tools.h const_tools.h goban_mg.h gobanurgency_mg.h \
  vectormemorypool_mg.h stringinformation_mg.h locationinformation_mg.h \
  cfg_mg.h gotools_mg.h smallpatternsdatabase_mg.h yadtinterface_mg.h \
  goexperiments_mg.h cputimechronometer_mg.h gobannode_mg.h \
  initiatefastboard_mg.h
fast_goban.o: fast_goban.cpp fast_goban.h const_goban.h goban.h move.h \
  const_move.h const_geography.h evaluation_score.h game_go.h \
  const_manager_game.h chain.h tools_ge.h const_tools_ge.h bibliotheque.h \
  marker.h const_rule.h
fast_mc_locations_informations.o: fast_mc_locations_informations.cpp \
  fast_mc_locations_informations.h fast_goban.h const_goban.h goban.h \
  move.h const_move.h const_geography.h evaluation_score.h game_go.h \
  const_manager_game.h chain.h tools_ge.h const_tools_ge.h bibliotheque.h \
  marker.h const_rule.h referee.h manager_game.h
fastsmallvector_mg.o: fastsmallvector_mg.cpp fastsmallvector_mg.h
file_go.o: file_go.cpp file_go.h const_goban.h
file_sgf.o: file_sgf.cpp file_sgf.h move.h const_move.h const_goban.h \
  tools_ge.h const_tools_ge.h
filter.o: filter.cpp filter.h tools_ge.h const_tools_ge.h const_filter.h \
  const_goban.h database_games_go.h file_sgf.h move.h const_move.h \
  statistic.h const_statistic.h goban.h
Forme_kppv13.o: Forme_kppv13.cpp Arbre13.h Forme_kppv13.h Element_kppv.h
Forme_kppv.o: Forme_kppv.cpp Arbre.h Forme_kppv.h goexperiments_mg.h \
  typedefs_mg.h goban_mg.h fastsmallvector_mg.h gobanurgency_mg.h \
  vectormemorypool_mg.h stringinformation_mg.h locationinformation_mg.h \
  cfg_mg.h gotools_mg.h smallpatternsdatabase_mg.h yadtinterface_mg.h \
  Element_kppv.h coeff_pattern.h
game_go.o: game_go.cpp game_go.h goban.h const_goban.h move.h \
  const_move.h const_manager_game.h chain.h const_geography.h tools_ge.h \
  const_tools_ge.h bibliotheque.h marker.h
goban2_mg.o: goban2_mg.cpp goban_mg.h typedefs_mg.h fastsmallvector_mg.h \
  gobanurgency_mg.h vectormemorypool_mg.h stringinformation_mg.h \
  locationinformation_mg.h cfg_mg.h gotools_mg.h \
  smallpatternsdatabase_mg.h yadtinterface_mg.h randomgoplayer_mg.h \
  goplayer_mg.h openingdatabase_mg.h gobannode_mg.h goexperiments_mg.h \
  innermcgoplayer_mg.h gogametree_mg.h moveselector_mg.h treeupdator_mg.h
goban3_mg.o: goban3_mg.cpp goban_mg.h typedefs_mg.h fastsmallvector_mg.h \
  gobanurgency_mg.h vectormemorypool_mg.h stringinformation_mg.h \
  locationinformation_mg.h cfg_mg.h gotools_mg.h \
  smallpatternsdatabase_mg.h yadtinterface_mg.h gobannode_mg.h \
  innermcgoplayer_mg.h goplayer_mg.h openingdatabase_mg.h gogametree_mg.h \
  moveselector_mg.h treeupdator_mg.h
goban.o: goban.cpp goban.h const_goban.h move.h const_move.h
goban_mg.o: goban_mg.cpp goban_mg.h typedefs_mg.h fastsmallvector_mg.h \
  gobanurgency_mg.h vectormemorypool_mg.h stringinformation_mg.h \
  locationinformation_mg.h cfg_mg.h gotools_mg.h \
  smallpatternsdatabase_mg.h yadtinterface_mg.h randomgoplayer_mg.h \
  goplayer_mg.h openingdatabase_mg.h gobannode_mg.h goexperiments_mg.h \
  innermcgoplayer_mg.h gogametree_mg.h moveselector_mg.h treeupdator_mg.h
gobannode_mg.o: gobannode_mg.cpp gobannode_mg.h typedefs_mg.h goban_mg.h \
  fastsmallvector_mg.h gobanurgency_mg.h vectormemorypool_mg.h \
  stringinformation_mg.h locationinformation_mg.h cfg_mg.h gotools_mg.h \
  smallpatternsdatabase_mg.h yadtinterface_mg.h
gobanurgency_mg.o: gobanurgency_mg.cpp gobanurgency_mg.h typedefs_mg.h \
  fastsmallvector_mg.h vectormemorypool_mg.h goban_mg.h \
  stringinformation_mg.h locationinformation_mg.h cfg_mg.h gotools_mg.h \
  smallpatternsdatabase_mg.h yadtinterface_mg.h gobannode_mg.h
goexperiments_mg.o: goexperiments_mg.cpp scoreevaluator_mg.h \
  typedefs_mg.h goban_mg.h fastsmallvector_mg.h gobanurgency_mg.h \
  vectormemorypool_mg.h stringinformation_mg.h locationinformation_mg.h \
  cfg_mg.h gotools_mg.h smallpatternsdatabase_mg.h yadtinterface_mg.h \
  treeupdator_mg.h gobannode_mg.h gogametree_mg.h goexperiments_mg.h \
  gogame_mg.h innermcgoplayer_mg.h goplayer_mg.h openingdatabase_mg.h \
  moveselector_mg.h randomgoplayer_mg.h gtpengine_mg.h \
  goplayerfactory_mg.h basic_instinct_fast_mg.h fastboard_mg.h tools.h \
  const_tools.h pattern.h tools_ge.h const_tools_ge.h \
  localpatternsdatabase_mg.h pbmb_io.h smallsquarepatternsdb_mg.h \
  openingbook_mg.h Forme_kppv.h
gogame_mg.o: gogame_mg.cpp gogame_mg.h typedefs_mg.h goban_mg.h \
  fastsmallvector_mg.h gobanurgency_mg.h vectormemorypool_mg.h \
  stringinformation_mg.h locationinformation_mg.h cfg_mg.h gotools_mg.h \
  smallpatternsdatabase_mg.h yadtinterface_mg.h
gogametree_mg.o: gogametree_mg.cpp gogametree_mg.h gobannode_mg.h \
  typedefs_mg.h goban_mg.h fastsmallvector_mg.h gobanurgency_mg.h \
  vectormemorypool_mg.h stringinformation_mg.h locationinformation_mg.h \
  cfg_mg.h gotools_mg.h smallpatternsdatabase_mg.h yadtinterface_mg.h \
  openingdatabase_mg.h
go_player.o: go_player.cpp go_player.h referee.h goban.h const_goban.h \
  move.h const_move.h manager_game.h const_manager_game.h chain.h \
  const_geography.h tools_ge.h const_tools_ge.h bibliotheque.h marker.h \
  strategy.h param_strategy.h game_go.h database_games_go.h file_sgf.h \
  filter.h const_filter.h statistic.h const_statistic.h block.h \
  const_block.h param_block.h param_go_player.h const_rule.h \
  manager_moves.h param_manager_moves.h simulation.h amaf.h amaf2.h \
  mc_locations_informations.h amaf_tree.h algorithm.h tactic.h \
  fast_goban.h evaluation_score.h const_tactic.h location_information.h \
  fast_mc_locations_informations.h uct1.h semeai.h
goplayerfactory_mg.o: goplayerfactory_mg.cpp goplayerfactory_mg.h \
  typedefs_mg.h randomgoplayer_mg.h goplayer_mg.h goban_mg.h \
  fastsmallvector_mg.h gobanurgency_mg.h vectormemorypool_mg.h \
  stringinformation_mg.h locationinformation_mg.h cfg_mg.h gotools_mg.h \
  smallpatternsdatabase_mg.h yadtinterface_mg.h openingdatabase_mg.h \
  innermcgoplayer_mg.h gogametree_mg.h gobannode_mg.h moveselector_mg.h \
  treeupdator_mg.h
goplayer_mg.o: goplayer_mg.cpp goplayer_mg.h typedefs_mg.h goban_mg.h \
  fastsmallvector_mg.h gobanurgency_mg.h vectormemorypool_mg.h \
  stringinformation_mg.h locationinformation_mg.h cfg_mg.h gotools_mg.h \
  smallpatternsdatabase_mg.h yadtinterface_mg.h openingdatabase_mg.h
gotools_mg.o: gotools_mg.cpp gotools_mg.h typedefs_mg.h \
  fastsmallvector_mg.h innermcgoplayer_mg.h goplayer_mg.h goban_mg.h \
  gobanurgency_mg.h vectormemorypool_mg.h stringinformation_mg.h \
  locationinformation_mg.h cfg_mg.h smallpatternsdatabase_mg.h \
  yadtinterface_mg.h openingdatabase_mg.h gogametree_mg.h gobannode_mg.h \
  moveselector_mg.h treeupdator_mg.h
gtpengine_mg.o: gtpengine_mg.cpp gtpengine_mg.h goban_mg.h typedefs_mg.h \
  fastsmallvector_mg.h gobanurgency_mg.h vectormemorypool_mg.h \
  stringinformation_mg.h locationinformation_mg.h cfg_mg.h gotools_mg.h \
  smallpatternsdatabase_mg.h yadtinterface_mg.h goplayer_mg.h \
  openingdatabase_mg.h gogame_mg.h innermcgoplayer_mg.h gogametree_mg.h \
  gobannode_mg.h moveselector_mg.h treeupdator_mg.h \
  cputimechronometer_mg.h fastboard_mg.h tools.h const_tools.h
handicap_mg.o: handicap_mg.cpp gtpengine_mg.h goban_mg.h typedefs_mg.h \
  fastsmallvector_mg.h gobanurgency_mg.h vectormemorypool_mg.h \
  stringinformation_mg.h locationinformation_mg.h cfg_mg.h gotools_mg.h \
  smallpatternsdatabase_mg.h yadtinterface_mg.h goplayer_mg.h \
  openingdatabase_mg.h gogame_mg.h innermcgoplayer_mg.h gogametree_mg.h \
  gobannode_mg.h moveselector_mg.h treeupdator_mg.h \
  cputimechronometer_mg.h
initiatefastboard_mg.o: initiatefastboard_mg.cpp initiatefastboard_mg.h
innermcgoplayer_mg2.o: innermcgoplayer_mg2.cpp innermcgoplayer_mg.h \
  goplayer_mg.h typedefs_mg.h goban_mg.h fastsmallvector_mg.h \
  gobanurgency_mg.h vectormemorypool_mg.h stringinformation_mg.h \
  locationinformation_mg.h cfg_mg.h gotools_mg.h \
  smallpatternsdatabase_mg.h yadtinterface_mg.h openingdatabase_mg.h \
  gogametree_mg.h gobannode_mg.h moveselector_mg.h treeupdator_mg.h \
  openingbook_mg.h
innermcgoplayer_mg.o: innermcgoplayer_mg.cpp innermcgoplayer_mg.h \
  goplayer_mg.h typedefs_mg.h goban_mg.h fastsmallvector_mg.h \
  gobanurgency_mg.h vectormemorypool_mg.h stringinformation_mg.h \
  locationinformation_mg.h cfg_mg.h gotools_mg.h \
  smallpatternsdatabase_mg.h yadtinterface_mg.h openingdatabase_mg.h \
  gogametree_mg.h gobannode_mg.h moveselector_mg.h treeupdator_mg.h \
  cputimechronometer_mg.h scoreevaluator_mg.h localpatternsdatabase_mg.h \
  basic_instinct_fast_mg.h fastboard_mg.h tools.h const_tools.h pattern.h \
  tools_ge.h const_tools_ge.h libfuseki.h openingbook_mg.h
libfuseki.o: libfuseki.cpp libfuseki.h
localpatternsdatabase_mg.o: localpatternsdatabase_mg.cpp \
  localpatternsdatabase_mg.h typedefs_mg.h goban_mg.h \
  fastsmallvector_mg.h gobanurgency_mg.h vectormemorypool_mg.h \
  stringinformation_mg.h locationinformation_mg.h cfg_mg.h gotools_mg.h \
  smallpatternsdatabase_mg.h yadtinterface_mg.h
location_information.o: location_information.cpp location_information.h \
  const_tools_ge.h tactic.h game_go.h goban.h const_goban.h move.h \
  const_move.h const_manager_game.h chain.h const_geography.h tools_ge.h \
  bibliotheque.h marker.h amaf.h simulation.h const_rule.h amaf2.h \
  mc_locations_informations.h fast_goban.h evaluation_score.h \
  const_tactic.h
locationinformation_mg.o: locationinformation_mg.cpp \
  locationinformation_mg.h typedefs_mg.h fastsmallvector_mg.h \
  stringinformation_mg.h goban_mg.h gobanurgency_mg.h \
  vectormemorypool_mg.h cfg_mg.h gotools_mg.h smallpatternsdatabase_mg.h \
  yadtinterface_mg.h
main.o: main.cpp typedefs_mg.h goban_mg.h fastsmallvector_mg.h \
  gobanurgency_mg.h vectormemorypool_mg.h stringinformation_mg.h \
  locationinformation_mg.h cfg_mg.h gotools_mg.h \
  smallpatternsdatabase_mg.h yadtinterface_mg.h gogame_mg.h \
  gobannode_mg.h randomgoplayer_mg.h goplayer_mg.h openingdatabase_mg.h \
  gogametree_mg.h gtpengine_mg.h cputimechronometer_mg.h \
  goplayerfactory_mg.h goexperiments_mg.h fastboard_mg.h tools.h \
  const_tools.h
manager_game.o: manager_game.cpp manager_game.h const_manager_game.h \
  goban.h const_goban.h move.h const_move.h chain.h const_geography.h \
  tools_ge.h const_tools_ge.h bibliotheque.h marker.h
manager_moves.o: manager_moves.cpp manager_moves.h tools_ge.h \
  const_tools_ge.h param_manager_moves.h const_goban.h game_go.h goban.h \
  move.h const_move.h const_manager_game.h chain.h const_geography.h \
  bibliotheque.h marker.h database_games_go.h file_sgf.h statistic.h \
  const_statistic.h strategy.h param_strategy.h filter.h const_filter.h \
  block.h const_block.h param_block.h
marker.o: marker.cpp marker.h
mc_locations_informations.o: mc_locations_informations.cpp \
  mc_locations_informations.h tools_ge.h const_tools_ge.h const_goban.h \
  const_move.h move.h game_go.h goban.h const_manager_game.h chain.h \
  const_geography.h bibliotheque.h marker.h
mogomutex.o: mogomutex.cpp mogomutex.h typedefs_mg.h
move.o: move.cpp move.h const_move.h
moveselector_mg.o: moveselector_mg.cpp basic_instinct_fast_mg.h \
  fastboard_mg.h typedefs_mg.h fastsmallvector_mg.h tools.h const_tools.h \
  goban_mg.h gobanurgency_mg.h vectormemorypool_mg.h \
  stringinformation_mg.h locationinformation_mg.h cfg_mg.h gotools_mg.h \
  smallpatternsdatabase_mg.h yadtinterface_mg.h gobannode_mg.h pattern.h \
  tools_ge.h const_tools_ge.h moveselector_mg.h treeupdator_mg.h \
  gogametree_mg.h randomgoplayer_mg.h goplayer_mg.h openingdatabase_mg.h \
  scoreevaluator_mg.h localpatternsdatabase_mg.h innermcgoplayer_mg.h
newRandomModes_mg.o: newRandomModes_mg.cpp goban_mg.h typedefs_mg.h \
  fastsmallvector_mg.h gobanurgency_mg.h vectormemorypool_mg.h \
  stringinformation_mg.h locationinformation_mg.h cfg_mg.h gotools_mg.h \
  smallpatternsdatabase_mg.h yadtinterface_mg.h
openingbook_mg.o: openingbook_mg.cpp gogametree_mg.h gobannode_mg.h \
  typedefs_mg.h goban_mg.h fastsmallvector_mg.h gobanurgency_mg.h \
  vectormemorypool_mg.h stringinformation_mg.h locationinformation_mg.h \
  cfg_mg.h gotools_mg.h smallpatternsdatabase_mg.h yadtinterface_mg.h \
  openingbook_mg.h
openingdatabase_mg.o: openingdatabase_mg.cpp openingdatabase_mg.h \
  typedefs_mg.h goban_mg.h fastsmallvector_mg.h gobanurgency_mg.h \
  vectormemorypool_mg.h stringinformation_mg.h locationinformation_mg.h \
  cfg_mg.h gotools_mg.h smallpatternsdatabase_mg.h yadtinterface_mg.h \
  gogame_mg.h
pattern.o: pattern.cpp pattern.h tools_ge.h const_tools_ge.h
pbmb_io.o: pbmb_io.cpp pbmb_io.h
randomgoplayer_mg.o: randomgoplayer_mg.cpp randomgoplayer_mg.h \
  goplayer_mg.h typedefs_mg.h goban_mg.h fastsmallvector_mg.h \
  gobanurgency_mg.h vectormemorypool_mg.h stringinformation_mg.h \
  locationinformation_mg.h cfg_mg.h gotools_mg.h \
  smallpatternsdatabase_mg.h yadtinterface_mg.h openingdatabase_mg.h
referee.o: referee.cpp referee.h goban.h const_goban.h move.h \
  const_move.h manager_game.h const_manager_game.h chain.h \
  const_geography.h tools_ge.h const_tools_ge.h bibliotheque.h marker.h
scoreevaluator_mg.o: scoreevaluator_mg.cpp scoreevaluator_mg.h \
  typedefs_mg.h goban_mg.h fastsmallvector_mg.h gobanurgency_mg.h \
  vectormemorypool_mg.h stringinformation_mg.h locationinformation_mg.h \
  cfg_mg.h gotools_mg.h smallpatternsdatabase_mg.h yadtinterface_mg.h \
  treeupdator_mg.h gobannode_mg.h gogametree_mg.h goexperiments_mg.h \
  fastboard_mg.h tools.h const_tools.h smallsquarepatternsdb_mg.h
semeai.o: semeai.cpp semeai.h chain.h goban.h const_goban.h move.h \
  const_move.h const_geography.h tools_ge.h const_tools_ge.h \
  bibliotheque.h marker.h game_go.h const_manager_game.h fast_goban.h \
  evaluation_score.h const_rule.h const_tactic.h location_information.h \
  tactic.h amaf.h simulation.h amaf2.h mc_locations_informations.h
simulation.o: simulation.cpp simulation.h game_go.h goban.h const_goban.h \
  move.h const_move.h const_manager_game.h chain.h const_geography.h \
  tools_ge.h const_tools_ge.h bibliotheque.h marker.h const_rule.h amaf.h \
  amaf2.h mc_locations_informations.h
smallpatternsdatabase_mg.o: smallpatternsdatabase_mg.cpp \
  smallpatternsdatabase_mg.h typedefs_mg.h yadtinterface_mg.h \
  gotools_mg.h fastsmallvector_mg.h goban_mg.h gobanurgency_mg.h \
  vectormemorypool_mg.h stringinformation_mg.h locationinformation_mg.h \
  cfg_mg.h
smallsquarepatternsdb_mg.o: smallsquarepatternsdb_mg.cpp \
  smallsquarepatternsdb_mg.h typedefs_mg.h goban_mg.h \
  fastsmallvector_mg.h gobanurgency_mg.h vectormemorypool_mg.h \
  stringinformation_mg.h locationinformation_mg.h cfg_mg.h gotools_mg.h \
  smallpatternsdatabase_mg.h yadtinterface_mg.h
statistic.o: statistic.cpp statistic.h database_games_go.h file_sgf.h \
  move.h const_move.h const_goban.h tools_ge.h const_tools_ge.h \
  const_statistic.h goban.h
strategy.o: strategy.cpp strategy.h goban.h const_goban.h move.h \
  const_move.h tools_ge.h const_tools_ge.h const_manager_game.h \
  param_strategy.h game_go.h chain.h const_geography.h bibliotheque.h \
  marker.h database_games_go.h file_sgf.h filter.h const_filter.h \
  statistic.h const_statistic.h block.h const_block.h param_block.h
stringinformation_mg.o: stringinformation_mg.cpp stringinformation_mg.h \
  typedefs_mg.h fastsmallvector_mg.h goban_mg.h gobanurgency_mg.h \
  vectormemorypool_mg.h locationinformation_mg.h cfg_mg.h gotools_mg.h \
  smallpatternsdatabase_mg.h yadtinterface_mg.h
super_manager_game.o: super_manager_game.cpp super_manager_game.h \
  referee.h goban.h const_goban.h move.h const_move.h manager_game.h \
  const_manager_game.h chain.h const_geography.h tools_ge.h \
  const_tools_ge.h bibliotheque.h marker.h go_player.h strategy.h \
  param_strategy.h game_go.h database_games_go.h file_sgf.h filter.h \
  const_filter.h statistic.h const_statistic.h block.h const_block.h \
  param_block.h param_go_player.h const_rule.h manager_moves.h \
  param_manager_moves.h simulation.h amaf.h amaf2.h \
  mc_locations_informations.h amaf_tree.h algorithm.h tactic.h \
  fast_goban.h evaluation_score.h const_tactic.h location_information.h \
  fast_mc_locations_informations.h uct1.h semeai.h
tactic.o: tactic.cpp tactic.h game_go.h goban.h const_goban.h move.h \
  const_move.h const_manager_game.h chain.h const_geography.h tools_ge.h \
  const_tools_ge.h bibliotheque.h marker.h amaf.h simulation.h \
  const_rule.h amaf2.h mc_locations_informations.h fast_goban.h \
  evaluation_score.h const_tactic.h
territory.o: territory.cpp territory.h
tools.o: tools.cpp tools.h const_tools.h
tools_ge.o: tools_ge.cpp tools_ge.h const_tools_ge.h
treeupdator_mg.o: treeupdator_mg.cpp treeupdator_mg.h goban_mg.h \
  typedefs_mg.h fastsmallvector_mg.h gobanurgency_mg.h \
  vectormemorypool_mg.h stringinformation_mg.h locationinformation_mg.h \
  cfg_mg.h gotools_mg.h smallpatternsdatabase_mg.h yadtinterface_mg.h \
  gobannode_mg.h gogametree_mg.h moveselector_mg.h fastboard_mg.h tools.h \
  const_tools.h gogame_mg.h
uct1.o: uct1.cpp uct1.h const_rule.h fast_goban.h const_goban.h goban.h \
  move.h const_move.h const_geography.h evaluation_score.h game_go.h \
  const_manager_game.h chain.h tools_ge.h const_tools_ge.h bibliotheque.h \
  marker.h fast_mc_locations_informations.h referee.h manager_game.h \
  location_information.h tactic.h amaf.h simulation.h amaf2.h \
  mc_locations_informations.h const_tactic.h
vectormemorypool_mg.o: vectormemorypool_mg.cpp vectormemorypool_mg.h \
  fastsmallvector_mg.h
yadtinterface_mg.o: yadtinterface_mg.cpp yadtinterface_mg.h typedefs_mg.h \
  gotools_mg.h fastsmallvector_mg.h
