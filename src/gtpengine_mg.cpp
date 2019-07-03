//
// C++ Implementation: gtpengine_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "gtpengine_mg.h"
#include "gotools_mg.h"
#include <stdio.h>
#include "time.h"
#include "innermcgoplayer_mg.h"
#include "cputimechronometer_mg.h"
#include <time.h>
#include "openingdatabase_mg.h"
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <sstream>

#include "fastboard_mg.h"



#ifdef MOGOMPI
#include "mpi.h"


int MPI_NUMERO;
int MPI_NUMBER;
#endif

int handicapLevel=0;

FILE *debugGTPEngine=0;
using namespace MoGo;
using namespace std;

int MoGo::GTPEngine::undoMode = 1;
int MoGo::GTPEngine::specialGTPMode = 0;

namespace MoGo {



#ifdef MPIMOGO

  
  void mpiSendMessage(int tag, int target)
  {//assert(0);
    //assert(debugGTPEngine);
    fprintf(debugGTPEngine,"I am proc %d, I send a message with tag %d to proc %d\n",MPI_NUMERO,tag,target);
    fflush(debugGTPEngine);
  }
  
  void mpiWantRecvMessage(int tag, int source)
  {//assert(0);
    MPI_Status status;
    int out;
    MPI_Iprobe(source,tag,MPI_COMM_WORLD,&out,&status);
    static int nbTry=0;
    
    if (!out) 
      {//assert(0);
	fprintf(debugGTPEngine,"I am proc %d, I want a message with tag %d from proc %d NOT YET RECIEVED (%d)!!!\n",MPI_NUMERO,tag,source,nbTry);
	nbTry++;
      }
    else
      {//assert(0);
	fprintf(debugGTPEngine,"I am proc %d, I want a message with tag %d from proc %d\n",MPI_NUMERO,tag,source);
	nbTry=0;
      }
    fflush(debugGTPEngine);
  }
  
  void mpiRecvMessage(int tag, int source)
  {//assert(0);
    fprintf(debugGTPEngine,"I am proc %d, I recieve a message with tag %d from proc %d\n",MPI_NUMERO,tag,source);

    fflush(debugGTPEngine); //JBH ADDING
  }


  void mpi_fgets(char *param ,int size,FILE *stream)
  {//assert(0);
    if (MPI_NUMERO==0)
      {//assert(0);
	fprintf(debugGTPEngine,"proc 0, waiting for stdin...\n"); fflush(stdout); fflush(debugGTPEngine); //JBH ADD
	fgets(param,size,stream);
	fprintf(debugGTPEngine,"################### %s\n",param); fflush(debugGTPEngine);
	fprintf(debugGTPEngine,"reading ...");
	fprintf(debugGTPEngine,param); fprintf(debugGTPEngine,"\n");fflush(stdout); fflush(debugGTPEngine); //JBH ADD
	for (int i=1;i<MPI_NUMBER;i++)
	  {//assert(0);
	    mpiSendMessage(MPI_TAG_GTP_PARAM,i);
	    assert(MPI_Send((void*)param,MAX_GTP_SIZE,MPI_CHAR,i,MPI_TAG_GTP_PARAM,MPI_COMM_WORLD)==MPI_SUCCESS);
	  }
      
      }
    else
      {//assert(0);
	mpiWantRecvMessage(MPI_TAG_GTP_PARAM,0);
	assert(MPI_Recv((void*)param,MAX_GTP_SIZE,MPI_CHAR,0,MPI_TAG_GTP_PARAM,MPI_COMM_WORLD,MPI_STATUS_IGNORE)==MPI_SUCCESS);
	fprintf(debugGTPEngine,"$$$$$$$$$$$$$$$$$$$ %s\n",param);fflush(debugGTPEngine);

	mpiRecvMessage(MPI_TAG_GTP_PARAM,0);
      }
	
  }
  void mpi_scanf(char *param)
  {//assert(0);
    if (MPI_NUMERO==0)
      {//assert(0);
	fprintf(debugGTPEngine,"proc 0, waiting for stdin...\n");fflush(stdout); fflush(debugGTPEngine); //JBH ADD
	scanf("%s",param);
	fprintf(debugGTPEngine,"################### %s\n",param);fflush(debugGTPEngine);
	fprintf(debugGTPEngine,"reading ...");
	fprintf(debugGTPEngine,param);fprintf(debugGTPEngine,"\n");fflush(stdout); fflush(debugGTPEngine); //JBH ADD
	for (int i=1;i<MPI_NUMBER;i++)
	  {//assert(0);
	    mpiSendMessage(MPI_TAG_GTP_PARAM,i);
	    assert(MPI_Send((void*)param,MAX_GTP_SIZE,MPI_CHAR,i,MPI_TAG_GTP_PARAM,MPI_COMM_WORLD)==MPI_SUCCESS);
	  }
	
      }
    else
      {//assert(0);
	mpiWantRecvMessage(MPI_TAG_GTP_PARAM,0);
	assert(MPI_Recv((void*)param,MAX_GTP_SIZE,MPI_CHAR,0,MPI_TAG_GTP_PARAM,MPI_COMM_WORLD,MPI_STATUS_IGNORE)==MPI_SUCCESS);
	fprintf(debugGTPEngine,"$$$$$$$$$$$$$$$$$$$ %s\n",param);fflush(debugGTPEngine); //JBH HERE 
	
	mpiRecvMessage(MPI_TAG_GTP_PARAM,0);
      }
	
}
#endif
#ifdef MANUALREPORT
double GTPEngine::manualReport=1800.00;
#endif

 // TEMPORARY COMMENT: THIS FUNCTION IS USED
  GTPEngine::GTPEngine(GoPlayer * goPlayer, int playsAgainstHuman, double timeMultiplier, double timeDivider, double timeShift, double _komi, double forgetKomiValue, int defaultParameterSize): playerGoban(9,9) {
    this->timeMultiplier=timeMultiplier;
    if (debugGTPEngine==0)
    {
#ifdef MOGOMPI
      char *myString=new char[100];
      //sprintf(myString,"debugGTPEngine%d",MPI_NUMERO);
      //sprintf(myString,"__debugGTPEngine%d.deb",MPI_NUMERO);
      sprintf(myString,"/dev/null");
      debugGTPEngine = fopen(myString,"w");
      delete [] myString;
#else
      //debugGTPEngine = fopen("__debugGTPEngine.deb","w");
      //debugGTPEngine = fopen("debugGTPEngine","w");
      debugGTPEngine = fopen("/dev/null","w");
#endif
    }
    this->goPlayer=goPlayer;
    initiateCommandList();
    setDelimiters();
    timeAllowed = 15;
    timeLeft = 20;
    gameSaver = 0;
    this->komi = _komi;
    saveMode = false;
    isTheOtherPlaying=false;
    thinkOverOtherTime=false;
    keepTreeIfPossible=false;
    if (dynamic_cast<InnerMCGoPlayer *>(goPlayer))
      keepTreeIfPossible=dynamic_cast<InnerMCGoPlayer *>(goPlayer)->getKeepTreeIfPossible();
    temporaryStopThinking=false;
    databaseName = "openingZ";
    this->timeDivider=timeDivider;
    this->playsAgainstHuman=playsAgainstHuman;
    originalResignThreshold=InnerMCGoPlayer::resignThreshold;
    handicapAgainst=false;
    currentHandicapCompensation=0;
    this->timeShift=timeShift;
    this->forgetKomiValue=forgetKomiValue;
    this->defaultParameterSize=defaultParameterSize;
    timeManagementMode=0;
    maxBeginning=0;
    numMoveBeginning=0;
  }


  GTPEngine::~GTPEngine() {//assert(0);
    if (gameSaver) {//assert(0);
      saveGame();
      delete gameSaver;
    }

    if (debugGTPEngine) {//assert(0);
      fclose(debugGTPEngine);
      debugGTPEngine=0;
    }
  }
}




 // TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::GTPEngine::start( ) {

  char *command=new char[MAX_GTP_SIZE];

#ifdef MPIMOGO
  std::string logName;
  ostringstream oss;
  //oss<<"__MoGoGTP"<<MPI_NUMERO<<".log";
  oss<<"/dev/null";

  logName = oss.str();
  //std::cout<<"PLOUF#"<<logName<<std::endl;

  /*
    if (MPI_NUMERO==0)
    logName = "MoGoGTP.log";
    else
    logName = "/dev/null";
  */
#else
  std::string logName;
  logName = "/dev/null";
#endif

  //std::cout<<logName<<std::endl;

  GoTools::changeFileName(logName,false);
  GoTools::setOutputMode(FILE_MODE);
  fprintf(debugGTPEngine,"MoGo GTP log file.\n");
  GoTools::changeFileName(logName,true);
  GoTools::setOutputMode(STDOUT_MODE);

  //std::cout<<logName<<std::endl;

  while(1) {
    if (dynamic_cast<InnerMCGoPlayer *>(goPlayer) != 0)
      /*if (timeLeft < 30)
        dynamic_cast<InnerMCGoPlayer *>(goPlayer)->setKeepTreeIfPossible(false);
      else*/
        dynamic_cast<InnerMCGoPlayer *>(goPlayer)->setKeepTreeIfPossible(keepTreeIfPossible);
    bool continueTrainingCalled=false;

    if (!readCommand(command)) {

#ifdef MPIMOGO
//#ifdef MPI_INTENSIVE
     if (MPI_NUMERO == 0)
// if (0==0)
//
//#endif
#endif
     {

      if (thinkOverOtherTime)
        if (isTheOtherPlaying)
          //if (timeLeft > 30)
            if (!temporaryStopThinking) {
              CPUTimeChronometer c;
              int shishoCheckModeSave=MoGo::InnerMCGoPlayer::shishoCheckMode; int shishoCheckModeNewSave=MoGo::InnerMCGoPlayer::shishoCheckModeNew;
              MoGo::InnerMCGoPlayer::shishoCheckMode = 0; MoGo::InnerMCGoPlayer::shishoCheckModeNew = 0;

#ifdef MPIMOGO
//#ifdef MPI_INTENSIVE
		for (int i=1;i<MPI_NUMBER;i++)
		{
			assert(MPI_Send(NULL,0,MPI_INT,i,MPI_TAG_PONDERING,MPI_COMM_WORLD)==MPI_SUCCESS);
		}
#endif
//#endif

std::cerr << "machine 0 begins pondering\n";fflush(stderr);
#ifdef MPIMOGO
              continueTraining(0.5);//FIXME TIME
#else
	      continueTraining(0.5);//FIXME TIME	
#endif
std::cerr << "machine 0 ends pondering\n";fflush(stderr);

              MoGo::InnerMCGoPlayer::shishoCheckMode=shishoCheckModeSave; MoGo::InnerMCGoPlayer::shishoCheckModeNew=shishoCheckModeNewSave;
              continueTrainingCalled=true;
              if (c.getTime() < 0.1)
                temporaryStopThinking=true;
            }
     }
#ifdef MPIMOGO
//#ifdef MPI_INTENSIVE
     else
     {
		assert(MPI_NUMERO > 0);
		MPI_Status status;
		int out;
		MPI_Iprobe(0,MPI_TAG_PONDERING,MPI_COMM_WORLD,&out,&status);
		if (out)
		{
			assert(MPI_Recv(NULL,0,MPI_INT,0,MPI_TAG_PONDERING,MPI_COMM_WORLD,MPI_STATUS_IGNORE)==MPI_SUCCESS);
			//std::cerr << "machine " << MPI_NUMERO << " begins pondering\n";
			continueTraining(0.5);
              		continueTrainingCalled=true;
			//std::cerr << "machine " << MPI_NUMERO << " ends pondering\n";;
		} 
		
     }
//#endif
#endif


      //       if (!continueTrainingCalled && dynamic_cast<InnerMCGoPlayer *>(goPlayer))
      //         dynamic_cast<InnerMCGoPlayer *>(goPlayer)->treatComingMachines();
    } else
      if (!executeCommand(command)) break;



  }
  delete []command;
  return false;
}

 // TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GTPEngine::setSaveMode( bool saveMode ) {
  this->saveMode = saveMode;
  if (gameSaver) {//assert(0);
    saveGame(); delete gameSaver;gameSaver=0;
  }
  if (saveMode)
    gameSaver = new GoGame(playerGoban.height());
}

 // TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::GTPEngine::readCommand(char * command ) const {
  fd_set set;
  struct timeval timeout;
  // Initialize the file descriptor set.
  FD_ZERO (&set);
  FD_SET (0, &set);
  // Initialize the timeout data structure.
  timeout.tv_sec = 0;
  timeout.tv_usec = 100;



#ifdef MPIMOGO
  //printf("mpimogo %d \n",MPI_NUMERO);fflush(stdout);
  if (MPI_NUMERO==0)
    {//assert(0);   // this is the main process
      
      //fprintf(debugGTPEngine,"I am proc %d\n", MPI_NUMERO);fflush(stdout);
      if (select (FD_SETSIZE,
		  &set, NULL, NULL,
		  &timeout)) 
	{//assert(0);
	  scanf("%s",command);
	  //    TODO broadcast plus propre
	  //   MPI_Bcast((void*)command,MAX_GTP_SIZE,MPI_char,0,MPI_COMM_WORLD);
	  fprintf(debugGTPEngine,"################ %s\n",command);fflush(debugGTPEngine);
	  for (int i=1;i<MPI_NUMBER;i++)
	    {//assert(0);
	//      	    printf("sending command to proc %d\n",i);fflush(stdout);
	      mpiSendMessage(MPI_TAG_GTP,i);
	      assert(MPI_Send((void*)command,MAX_GTP_SIZE,MPI_CHAR,i,MPI_TAG_GTP,MPI_COMM_WORLD)==MPI_SUCCESS);
	      //MPI_Wait(mpiRequest,NULL);
	    }
	  //  printf("sent done, ok\n");fflush(stdout);
	  return true;
	}
      else
	return false;

      
    }
  else
    {//assert(0);  // this is a process != 0
      //  printf("I am proc %d\n", MPI_NUMERO);fflush(stdout);
      int out;
      MPI_Status status;
      //       printf("MPI_iprobe almost done, ok\n");fflush(stdout);
      //assert(MPI_Iprobe(0, MPI_TAG_GTP, MPI_COMM_WORLD, &out, MPI_STATUS_IGNORE)==MPI_SUCCESS);
      MPI_Iprobe(0,MPI_TAG_GTP,MPI_COMM_WORLD,&out,&status);
      if (out)
	{//assert(0);
	  //        printf("recv almost done, ok\n");fflush(stdout);
	
	  mpiWantRecvMessage(MPI_TAG_GTP,0);
	  MPI_Recv((void*)command,MAX_GTP_SIZE,MPI_CHAR,0,MPI_TAG_GTP,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	  fprintf(debugGTPEngine,"$$$$$$$$$$$$$$$$ %s\n",command);fflush(debugGTPEngine);
	  mpiRecvMessage(MPI_TAG_GTP,0);
	  //        printf("recv done, ok\n");fflush(stdout);
	  return true;
	}
      else
	return false;
    }
#else
  
  if (select (FD_SETSIZE,
              &set, NULL, NULL,
              &timeout)) {
    scanf("%s",command);
    return true;
  }
  else
    return false;
  
#endif
  
  //fprintf(debugGTPEngine,"command << %s >> has length %d\n",command,strlen(command));
  //printf("GTPEngine read: %s\n",command);
  //for (int i=0;i<(int)commandList.size();i++)
  //  if (!strcmp(command,commandList[i]))
  //    return;
  
  //failureResponse();
}

 // TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::GTPEngine::executeCommand( char * command ) {
  fprintf(debugGTPEngine, "command read = %s\n", command);fflush(debugGTPEngine);
  //fprintf(debugGTPEngine,"command read = %s\n", command);fflush(stderr);
  temporaryStopThinking=false;
  if (!strncmp(command,"play",4)) {
    playMoveOnGoban();
    isTheOtherPlaying=false;
    return true;
  }
  if (!strncmp(command,"genmove",7)) {

    generateMove(playsAgainstHuman);
    // PATCH SALE POUR AFFICHAGE
    /*{
	    Goban tmpPlayerGoban(playerGoban);
    Vector<Vector<Location> > locationsOfDeadStrings;
    double threshold=0.2; //if (considerPassBeforeEnd==2) threshold=0.1;
	tmpPlayerGoban.retrieveDeadStrings(300,threshold,locationsOfDeadStrings,komi);
    }*/
    // FIN DU PATCH SALE
    return true;
  }
  if (!strncmp(command,"kgs-genmove_cleanup",19)) {
    generateMove(playsAgainstHuman==1);
    return true;
  }
  if (!strncmp(command,"protocol_version",16)) {
    send("2");
    return true;
  }
  if (!strncmp(command,"quit",4)) {
    //printf("= stop GTPEngine.\n\n");
    send("");
    return false;
  }
  if (!strncmp(command,"name",4)) {
    std::string name=goPlayer->getName();
    send(name.c_str());
    return true;
  }
  if (!strncmp(command,"boardsize",9)) {
    if (gameSaver)
      createNewGameSaver();
    readGobanSize();
    return true;

  }
  if (!strncmp(command,"known_command",13)) {
#ifdef MOGOMPI
	  //fprintf("attention on vient la\n");fflush(stdout);exit(-1);
	  mpi_scanf(command);
#else
    scanf("%s",command);
#endif
    if (isCommandKnown(command)) send("true");
    else send("false");
    //     successResponseEnd();
    return true;
  }
  if (!strncmp(command,"list_commands",13)) {
    std::string res=createCommandList();

    send(res.c_str());

    return true;
  }
  if (!strncmp(command,"version",7)) {
#ifdef RELEASE
    send("MoGo release 1. Please read http://www.lri.fr/~gelly/MoGo.htm for more information. That is NOT an official developpement MoGo version, but it is a public release. Its strength highly depends on your hardware and the time settings.");
#else
    send("MoGo version 4.86.Soissons: I'll resign if you win and playing until you pass if I win. If you passed and I did not, it often means that the status of some stones are not clear enough for me (I want to be sure we will agree on dead stones). Clarify them. Maybe I am losing, but have not enough time to be sure I should resign. In this case play until the end :'( (very rare cases)");
#endif

    return true;
  }
  if (!strncmp(command,"clear_board",11)) {
    time_t timeV=time(NULL);
    char *dateStr=asctime(gmtime(&timeV));
    dateOfGameStart=std::string(dateStr);
    for (int i=0;i<(int)dateOfGameStart.size();i++) if (dateOfGameStart[i]==' ') dateOfGameStart[i]='_';  if (dateOfGameStart.size() > 0) dateOfGameStart[dateOfGameStart.size()-1]='_';
    if (gameSaver) {//assert(0);
      GoTools::changeFileName("MoGo_log_"+dateOfGameStart+".log",false);
      createNewGameSaver();
    }
    playerGoban.clearGoban();
    goPlayer->clearHistoryGoban();
    historyGoban.clear();
    historyGoban.push_back(playerGoban);
    openingGeneratingModeOn = false;
    handicapAgainst=false;
    currentHandicapCompensation=0;
    InnerMCGoPlayer::resignThreshold=originalResignThreshold;
    GoTools::setOutputMode(LOG_MODE);
    goPlayer->start(playerGoban);
    GoTools::setOutputMode(STDOUT_MODE);

    send("");
    return true;
  }
  if (!strncmp(command,"komi",4)) {
    readKomi();
    return true;
  }
  if (!strncmp(command,"time_left",9)) {//assert(0);
    readTimeLeft();
    return true;

  }
  if (!strncmp(command,"final_status_list",17)) {
    returnFinalStatus();
    return true;
  }
  if (!strncmp(command,"time_settings",13)) {//assert(0);
    readTimeSettings();
    return true;
  }
  if (!strncmp(command,"reg_genmove",11)) {//assert(0);
    generateMoveOnly();
    return true;
  }
  if (!strncmp(command,"undo",4) &&
      undoMode == 1) {//assert(0);
    undoOneMove();
    return true;
  }
  if (!strncmp(command,"show_sequence",13)) {//assert(0);
    showPreferedSequence();
    return true;
  }
  if (!strncmp(command,"showboard",10)) {
    showGoban();
    return true;
  }
  if (!strncmp(command,"show_history",12)) {//assert(0);
    showHistory();
    return true;
  }
  if (!strncmp(command,"show_free_location",18)) {//assert(0);
    showFreeLocation();
    return true;
  }
  if (!strncmp(command,"update_location_list",20)) {//assert(0);
    updateLocationList();
    return true;
  }
  if (!strncmp(command,"is_atari_saving_move",20)) {//assert(0);
    isAtariSavingMove();
    return true;
  }
  if (!strncmp(command,"is_still_eating_move",20)) {//assert(0);
    isStillEatingMove();
    return true;
  }
  if (!strncmp(command,"set_random_mode",15)) {//assert(0);
    setRandomMode();
    return true;
  }
  if (!strncmp(command,"show_territory",14)) {//assert(0);
    showTerritory();
    return true;
  }

  if (!strncmp(command,"mode",12)) {//assert(0);
    setOpeningGeneratingMode();
    return true;
  }
  if (!strncmp(command,"set_database_name",17)) {//assert(0);
    setDatabaseName();
    return true;
  }
  if (!strncmp(command,"show_evaluation",15)) {//assert(0);
    show_evaluation();
    return true;
  }
  if (!strncmp(command,"place_free_handicap",19)) {//assert(0);
    place_free_handicap();
    return true;
  }
  if (!strncmp(command,"set_free_handicap",17)) {//assert(0);
    set_free_handicap();
    return true;
  }
  if (string(command)==string("VT-getArgumentNames")) {//assert(0);
    send("node_name lastMove value nbSimulations RAVE_value RAVE_nb");
    return true;
  }
  if (string(command)==string("VT-getChildren")) {//assert(0);
    handleVT();
    return true;
  }


  failureResponse();
  return true;

}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GTPEngine::failureResponse( ) const {
  send("error_message",false);
}

 // TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GTPEngine::initiateCommandList( ) {
  commandList.clear();
  addCommand("protocol_version");
  addCommand("name");
  addCommand("version");
  addCommand("known_command");
  addCommand("list_commands");
  addCommand("quit");
  addCommand("play");
  addCommand("genmove");
  addCommand("time_left");
  addCommand("final_status_list");
  addCommand("clear_board");
  addCommand("time_settings");
  addCommand("reg_genmove");
  if (undoMode == 1)
    addCommand("undo");
  addCommand("show_sequence");
  addCommand("showboard");
  addCommand("show_history");
  addCommand("show_free_location");
  addCommand("update_location_list");
  addCommand("is_atari_saving_move");
  addCommand("is_still_eating_move");
  addCommand("set_random_mode");
  addCommand("showTerritory");
  addCommand("kgs-genmove_cleanup");
  addCommand("show_evaluation");
  addCommand("place_free_handicap");
  addCommand("set_free_handicap");
  addCommand("komi");


}

void MoGo::GTPEngine::deleteCommandList( ) {//assert(0);
}

std::string MoGo::GTPEngine::createCommandList( ) const {
  std::string res;
  for (int i=0;i<(int)commandList.size()-1;i++) {

    res += commandList[i]+std::string("\n");
  }
  res += commandList[commandList.size()-1];
  return res;
}

 // TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GTPEngine::addCommand( const char * commandName) {
  commandList.push_back(new char[30]);
  strcpy(commandList[commandList.size()-1],commandName);
}

bool MoGo::GTPEngine::isCommandKnown( char * commandName ) {//assert(0);
  for (int i=0;i<(int)commandList.size();i++)
    if (!strcmp(commandList[i],commandName)) return true;
  return false;
}

 // TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GTPEngine::setDelimiters( ) {
  delimiters[0]=' ';
  delimiters[1]='\t';
  delimiters[2]='\r';
  delimiters[3]='\n';
  delimiters[4]='\0';
}

 // TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GTPEngine::playMoveOnGoban( ) {
  char color[MAX_GTP_SIZE],position[MAX_GTP_SIZE];
#ifdef MOGOMPI
	  mpi_scanf(color);
	  mpi_scanf(position);
#else
  scanf("%s %s", color, position);

#endif
  playWaitingMove(color);

  Location location=changeToLocation(position);
  if (specialGTPMode>0)
    if (location == PASSMOVE) {//assert(0);
      openingGeneratingModePassMoveRecieved();
      send("");
      return;
    } else if (openingGeneratingModeOn) {//assert(0);
      openingGeneratingModeOneMoveRecieved(location);
      if (specialGTPMode != 3) {//assert(0);
        send("");
        return;
      }
    }



  if (!playerGoban.isLegal(location)) {
    send("illegal move", false);
    return;
  }

  playerGoban.playMoveOnGoban(location);
  if (gameSaver) gameSaver->addNextMove(location);

  goPlayer->addHistoryGoban(playerGoban);
  historyGoban.push_back(playerGoban);

  //GoTools::setOutputMode(STDERR_MODE);
  GoTools::setOutputMode(LOG_MODE);
  //   playerGoban.textModeShowGoban();
  fflush(stderr);
  //playerGoban.showFreeLocation();
  fflush(stderr);

  GoTools::setOutputMode(STDOUT_MODE);

  send("");
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GTPEngine::generateMove(int considerPassBeforeEnd) 
{
  
#ifdef MOGOMPI
  double time_gmove_beginning = MPI_Wtime(); 
#endif
  
  char color[MAX_GTP_SIZE];
#ifdef MOGOMPI
  mpi_scanf(color);
#else
  scanf("%s",color);
#endif
  fprintf(debugGTPEngine,"scanfcolor fini");fflush(debugGTPEngine);
  if (specialGTPMode == 1 || specialGTPMode == 2) 
    {//assert(0);
      send("");
      return;
    }
  fprintf(debugGTPEngine,"send() fini");fflush(debugGTPEngine);
  playWaitingMove(color);

  fprintf(debugGTPEngine,"playWaitingMove fini");fflush(debugGTPEngine);
  Location location = PASSMOVE;
  GoTools::setOutputMode(LOG_MODE);

  fprintf(debugGTPEngine,"avant handicapagainst");fflush(debugGTPEngine);
  if (handicapAgainst)
    if (playerGoban.moveNumber()<playerGoban.height()*playerGoban.width()/2)
      InnerMCGoPlayer::resignThreshold=0.999; // we don't resign at the begginning if we have a handicap against us
    else
      InnerMCGoPlayer::resignThreshold=originalResignThreshold;


  bool wantsToPass=false;
  bool goPlayerHasPlayed=false;
  // fprintf(debugGTPEngine,"considerPassBeforeEnd %i\n", considerPassBeforeEnd);
  fprintf(debugGTPEngine,"avant pass-story");fflush(debugGTPEngine);
  if (considerPassBeforeEnd && (playerGoban.moveNumber()>0) && (playerGoban.lastMove()==PASSMOVE)) { // the other has passed and we are asked to consider an early pass
    Vector<Vector<Location> > locationsOfDeadStrings;
    Goban tmpPlayerGoban(playerGoban);tmpPlayerGoban.playMoveOnGoban(PASSMOVE);
    double threshold=0.2; if (considerPassBeforeEnd==2) threshold=0.1;
    
    //JBH PATCH
    //Only machine 0 can decide if mogo passes or not after a pass of his opponent.
#ifdef MPIMOGO
    if(MPI_NUMERO==0)
      {
#endif
	wantsToPass=tmpPlayerGoban.retrieveDeadStrings(300,threshold,locationsOfDeadStrings,komi);
	if (wantsToPass) {
	  double predictedScore=tmpPlayerGoban.scoreKGS(locationsOfDeadStrings);
	  fprintf(debugGTPEngine,"score predicted is %f (komi %f), the win for black is %i\n", predictedScore, komi, predictedScore>komi);fflush(debugGTPEngine);
	  wantsToPass=((predictedScore>komi) && playerGoban.isBlacksTurn()) || ((predictedScore<komi) && (!playerGoban.isBlacksTurn()));
	}
	fprintf(debugGTPEngine,"try to see if it wants to pass: %i\n", (int)wantsToPass);fflush(debugGTPEngine);
	
#ifdef MPIMOGO
	
	int iWantsToPass = (wantsToPass) ? 1 : 0;
	
	for (int i=1;i<MPI_NUMBER;i++)
	  {//assert(0);
	    mpiSendMessage(MPI_TAG_DECISION,i);
	    //assert(MPI_Send((void*)&wantsToPass,1,MPI_INT,i,MPI_TAG_DECISION,MPI_COMM_WORLD)==MPI_SUCCESS);
	    assert(MPI_Send((void*)&iWantsToPass,1,MPI_INT,i,MPI_TAG_DECISION,MPI_COMM_WORLD)==MPI_SUCCESS);
	  }
      }
    else
      {
	
	int iWantsToPass = 0;
	
	mpiWantRecvMessage(MPI_TAG_DECISION,0);
	//assert(MPI_Recv((void*)&wantsToPass,1,MPI_INT,0,MPI_TAG_DECISION,MPI_COMM_WORLD,MPI_STATUS_IGNORE)==MPI_SUCCESS);
	assert(MPI_Recv((void*)&iWantsToPass,1,MPI_INT,0,MPI_TAG_DECISION,MPI_COMM_WORLD,MPI_STATUS_IGNORE)==MPI_SUCCESS);
	mpiRecvMessage(MPI_TAG_DECISION,0);
	
	wantsToPass = (iWantsToPass==0) ? false : true;  
      }
    
#endif
    //JBH END PATCH

 
  }
  fprintf(debugGTPEngine,"apres pass-story");fflush(debugGTPEngine);
  
  if (!wantsToPass) {
    
#ifdef MANUALREPORT
    timeLeft=manualReport;
    fprintf(stderr,"timeLeft=%g\n",manualReport);
    //cerr << "clock=" << clock() <<endl;
    //manualReport+=((clock())/1000.0);
    manualReport+=time(0);
   //CPUTimeChronometer myclock;
#endif
    double time_=timeEstimated(playerGoban,timeAllowed);

    if (timeLeft>0)
      fprintf(debugGTPEngine,"time left: %d secs\n",timeLeft);
    fprintf(debugGTPEngine,"time given for this move:%5.1f sec time\n",time_);
    fflush(debugGTPEngine);
    if (1==0)//(timeLeft>=0 && timeLeft<20)//The v v final case with too limited time left. In this case MoGo plays randomly because it is too risky !!
      {//assert(0);
	//     location=goPlayer->generateOneMove(playerGoban,0);//0 means just choose one move quickly enough. However some thousand simulations is necessary in my opinion.
	//     goPlayer->showPreferedMoves(playerGoban,15);
	location=playerGoban.randomOutOneMove(201544);
	fprintf(debugGTPEngine,"fast end mode gives one move:");
	playerGoban.textModeShowPosition(location);
	fprintf(debugGTPEngine,"\n");
      }
    //location=playerGoban.randomOutOneMove();
    else {
      
#ifdef MODE_PANIC

    if (timeManagementMode != 1) {

      if(FBS_handler::board_size!=9)
	{
	  if((timeLeft<60)&&(timeLeft>3))
	    {
	      location=goPlayer->generateOneMove(playerGoban,0.5); 
	      goPlayerHasPlayed=true;
	    }
	  
	  if(timeLeft<=3)
	    location=PASSMOVE; 
	  
	  if(timeLeft>=60)
	    {
	      location=goPlayer->generateOneMove(playerGoban,time_);
	      goPlayerHasPlayed=true;
	    }
	}
      else
	{ 
	  
	  if((timeLeft<20)&&(timeLeft>3))
	    {
	      location=goPlayer->generateOneMove(playerGoban,0.5); 
	      goPlayerHasPlayed=true;
	    }

	  if(timeLeft<=3)
	    location=PASSMOVE; 
	  
	  if(timeLeft>=20)
	    {
	     location=goPlayer->generateOneMove(playerGoban,time_);
	      goPlayerHasPlayed=true;
	    }
	}
    } else {
	    location=goPlayer->generateOneMove(playerGoban,time_);
	    goPlayerHasPlayed=true;
    }	

#else //MODE NO PANIC
      
      location=goPlayer->generateOneMove(playerGoban,time_);
      goPlayerHasPlayed=true;
	
#endif //END MODE PANIC
      
    }
#ifdef MANUALREPORT
//    cerr << "apres, clock=" << clock() <<endl;
    manualReport-=5+time(0);//(myclock.getTime()/timeMultiplier+3.);
#endif
  } else
    location=PASSMOVE;
  //fprintf(debugGTPEngine,"generateMove finished.\n");
  
#ifdef MOGOMPI
  MPI_Bcast((void*)&location, 1, MPI_INT, 0, MPI_COMM_WORLD);
  //assert(MPI_Recv((void*)&location,1,MPI_INT,0,MPI_TAG_DECISION,MPI_COMM_WORLD,MPI_STATUS_IGNORE)==MPI_SUCCESS);
#endif

  if (!(goPlayerHasPlayed && goPlayer->hasResigned())) { // if no resign
    playerGoban.playMoveOnGoban(location);

    if (gameSaver) gameSaver->addNextMove(location);
    goPlayer->addHistoryGoban(playerGoban);
    historyGoban.push_back(playerGoban);
  }
  if (!InnerMCGoPlayer::dontDisplay) {
    playerGoban.textModeShowPosition(location);fprintf(debugGTPEngine,"\n");
//    // playerGoban.textModeShowGoban(location);
  }
  
    
  
  fflush(stderr);

  GoTools::setOutputMode(STDOUT_MODE);
  if (goPlayerHasPlayed && goPlayer->hasResigned())
    send("resign");
  else {
    char token[10];
    changeFromLocation(location,token);
    send(token);
  }
  if (goPlayerHasPlayed)
    isTheOtherPlaying=true;
  else
    isTheOtherPlaying=false;


#ifdef MOGOMPI
double time_gmove_end = MPI_Wtime();
double time_used =  time_gmove_end - time_gmove_beginning;
if (!InnerMCGoPlayer::dontDisplay)
std::cerr << "proc " << MPI_NUMERO << " used " << time_used << "s to generate his move\n";
#endif

}


void MoGo::GTPEngine::generateMoveOnly( ) {//assert(0);
  char color[MAX_GTP_SIZE];
#ifdef MOGOMPI
  mpi_scanf(color);
#else
  scanf("%s",color);
#endif
  playWaitingMove(color);

  //printf("%s\n",color);
  //GoTools::setOutputMode(STDERR_MODE);
  GoTools::setOutputMode(LOG_MODE);
  double time_=timeAllowed;
  fprintf(debugGTPEngine,"time given for this move:%5.1f sec time\n",time_);

  Location location=goPlayer->generateOneMove(playerGoban,time_);

  fprintf(debugGTPEngine,"\n");
  fflush(stderr);
  GoTools::setOutputMode(STDOUT_MODE);

  char token[10];
  changeFromLocation(location,token);
  send(token);
}


 // TEMPORARY COMMENT: THIS FUNCTION IS USED
Location MoGo::GTPEngine::changeToLocation( const char * token ) {
  if (!strncmp(token,"PASS",4) || !strncmp(token,"pass",4) || !strncmp(token,"Pass",4)) return PASSMOVE;
  int i,j=0;
  if ('A'<=token[0] && token[0]<='Z') {
    j=(int)(token[0]-(int)'A');
    if ((int)token[0]>=(int)'I') j--;// Be careful! Why there is no 'I' in the GNUGO? Or for all the protocol(I dont think so)?
  } else
    if ('a'<=token[0] && token[0]<='z') {
      j=(int)(token[0]-'a');
      if ((int)token[0]>=(int)'i') j--;
    } else return -1;

  i=playerGoban.height()-atoi(token+1);
  return playerGoban.indexOnArray(i,j);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GTPEngine::changeFromLocation( const Location location, char * token) {
  if (location == PASSMOVE) {
    strcpy(token,"PASS");
    return;
  }

  int i=playerGoban.xIndex(location),j=playerGoban.yIndex(location);
  char tmp[10];
  snprintf(tmp,8,"%c%i",'A'+j,playerGoban.height()-i);
  if (tmp[0] >= 'I') tmp[0]++;
  tmp[9]=0;
  strcpy(token,tmp);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GTPEngine::readGobanSize( ) {
  int size;
  char tmp[MAX_GTP_SIZE];
#ifdef MOGOMPI
  mpi_scanf(tmp);
#else
  scanf("%s",tmp);
#endif
  size=atoi(tmp);
  //fprintf(stderr,"<<<< %s --> %d >>>>\n",tmp,size);fflush(stderr);


  playerGoban=Goban(size,size);
  GobanLocation::resize(playerGoban);
  GobanBitmap::initiate(playerGoban);
  goPlayer->setGobanSize(size);
  //fprintf(stderr,"I am sending something empty\n");fflush(stderr);
  send("");
  //fprintf(stderr,"I have sent something empty\n");fflush(stderr);
  //TODO a funcion is needed for the player to change their goban.
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GTPEngine::send( const char * msg, bool isSuccessful) {
  //fprintf(stderr,"Mogo sends << %s >> \n",msg);fflush(stderr);
  
  if (isSuccessful) {

#ifdef MOGOMPI
	  if (MPI_NUMERO==0)
#endif
	  {
    fprintf(debugGTPEngine,"sending to stdout = %s\n\n",msg);
    GoTools::print("= %s\n\n",msg);
	  }
  } else {
#ifdef MOGOMPI
	  if (MPI_NUMERO==0)
#endif
	  {
       	  fprintf(debugGTPEngine,"? %s\n\n",msg);
    GoTools::print("? %s\n\n",msg);
	  }
  }
  fflush(stdout); fflush(debugGTPEngine); //JBH ADD
}

// TEMPORARY COMMENT: GOGUI
// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GTPEngine::readKomi( ) {
  float komi_;
#ifdef MOGOMPI
  char * param = new char[MAX_GTP_SIZE];
  mpi_scanf(param);
  sscanf(param,"%f",&komi_);
  delete [] param;
#else
  scanf("%f",&komi_);
#endif
  if (forgetKomiValue>-1000.) {//assert(0);
    komi_=float(forgetKomiValue);
    //     GoTools::print("Warning the komi is hard coded here: %f\n", komi_);
  }
  goPlayer->setKomi(double(komi_));
  // GoTools::print("%f\n", komi_);
  komi = komi_;
  // GoTools::print("%f\n", komi_);
  send("");
}

 // TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GTPEngine::setEachMoveTime( const double time_, int timeManagementMode, double maxBeginning, int numMoveBeginning ) {
  timeAllowed=time_;
  this->timeManagementMode=timeManagementMode;
  this->maxBeginning=maxBeginning;
  this->numMoveBeginning=numMoveBeginning;
}

void MoGo::GTPEngine::readTimeLeft( ) {//assert(0);
  char color[MAX_GTP_SIZE],time[MAX_GTP_SIZE],timeOnSaisPas[MAX_GTP_SIZE];
#ifdef MOGOMPI
  mpi_scanf(color);
  mpi_scanf(time);
  mpi_scanf(timeOnSaisPas);
#else
  scanf("%s %s %s", color, time, timeOnSaisPas);
#endif
  timeLeft=atoi(time);
  timeLeft-=int(timeShift);
  //fprintf(debugGTPEngine,"time left:%d seconds\n",timeLeft);
  send("");
}

double MoGo::GTPEngine::timeEstimated( const Goban & goban, double timeAllowed ) {
  double timeMultiplierToUse=timeMultiplier;
  if (goban.approximateSizeOfPossibleMoves()<InnerMCGoPlayer::securityThreads)
    timeMultiplierToUse=1.;

  //   double temperature=goban.temperatureOfLastMove();
  //   fprintf(debugGTPEngine,"temperature %f, experimental time: %f, classical time %f\n", temperature, timeLeft*(0.9*temperature+0.1)/timeDivider*timeMultiplierToUse, timeLeft/timeDivider*timeMultiplierToUse);

  //printf("My time left is %d\n",timeLeft);fflush(stdout);exit(-1);
 
  // if timeManagementMode==1, then Mogo uses only --time to know how much time it should use 
  // (it doesn't use the remaining time sent by kgs or other) 
  if (timeManagementMode==1) {
	  return timeAllowed*timeMultiplierToUse;
  }


  if (timeLeft<0.)
    return timeAllowed*timeMultiplierToUse;
  if (timeLeft>40.) {
    if (timeManagementMode==0) {
      double tmp=(timeLeft-20.)/timeDivider;
      if (tmp>timeAllowed)
        return timeAllowed*timeMultiplierToUse;
      else
        return tmp*timeMultiplierToUse;
    } else {//assert(0);
      double tmp=(timeLeft-20.)/timeDivider;
      double max=timeAllowed;
      if (goban.moveNumber()<numMoveBeginning) {//assert(0);
        double x=double(goban.moveNumber())/double(numMoveBeginning);
        max=maxBeginning+(timeAllowed-maxBeginning)*x;
      }
      tmp=GoTools::min(max, tmp);
      return tmp*timeMultiplierToUse;
    }
  } else {//assert(0);
    double tmp=(timeLeft)/timeDivider;
    return tmp*timeMultiplierToUse;
  }

  /*    if ( (goban.height() == 9 && timeLeft>int(6*timeAllowed)) ||
           (goban.height() == 13 && timeLeft>300) ||
           (goban.height() == 19 && timeLeft>600) ||
           timeLeft<0 ) return timeAllowed;
    double tmp=timeLeft*.9/double(goban.possibleMoveNumber()+1)*2.;
    if (timeLeft < 20. && tmp > 2.)
      return 2.;
    return tmp;*/
}



void MoGo::GTPEngine::readTimeSettings( ) {//assert(0);
  int main_time, byo_yomi_time, byo_yomi_stones;
#ifdef MOGOMPI
  char * param = new char[MAX_GTP_SIZE];
  mpi_scanf(param);
  sscanf(param,"%d",&main_time);
  mpi_scanf(param);
  sscanf(param,"%d",&byo_yomi_time);
  mpi_scanf(param);
  sscanf(param,"%d",&byo_yomi_stones);
  delete [] param;
#else
  scanf("%d %d %d",&main_time,&byo_yomi_time,&byo_yomi_stones);
#endif
  timeLeft=main_time*60;
 // printf("my time left is %f\n",timeLeft);
  timeLeft-=int(timeShift);
  send("");
}

 // TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GTPEngine::setAllGameTime( const int time ) {
  timeLeft=time*60;
  timeLeft-=int(timeShift);
}

void MoGo::GTPEngine::saveGame( ) {//assert(0);
  assert(gameSaver);
  if (gameSaver->nextMoveNumber()==1) return;
  //   time_t rawtime;
  //   time ( &rawtime );
  //   String gameName = "GTPGame_"+String(ctime (&rawtime));
  if (goPlayer)
    if (!goPlayer->hasResigned())
      return; // We now save only the lost games
  gameSaver->saveGame("GTPGame_"+dateOfGameStart,"");
}

void MoGo::GTPEngine::createNewGameSaver( ) {//assert(0);
  if (gameSaver) {//assert(0);
    saveGame(); delete gameSaver ; gameSaver=0;
  }

  gameSaver = new GoGame(playerGoban.height());
  gameSaver->setKomi(komi);
}
void MoGo::GTPEngine::returnFinalStatus() {
                              char status[10];
#ifdef MOGOMPI
			      char *param= new char[MAX_GTP_SIZE];
			      mpi_scanf(param);
			      sscanf(param,"%s",status);
			      delete [] param;
#else
                              scanf("%s",status);
#endif
                              if (!strncmp(status,"dead",4)) {//assert(0);
                                //GoTools::setOutputMode(STDOUT_MODE);
                                GoTools::setOutputMode(LOG_MODE);
                                // GoTools::print("komi=%f\n", komi);
                                Vector<Vector<Location> > locationsOfDeadStrings;
                                playerGoban.retrieveDeadStrings(500,0.1,locationsOfDeadStrings,komi);
                                double predictedScore=playerGoban.scoreKGS(locationsOfDeadStrings);
                                GoTools::print("score predicted is %f (komi %f), the win for black is %i\n", predictedScore, komi, predictedScore>komi);
                                GoTools::setOutputMode(STDOUT_MODE);
#ifdef MOGOMPI
				if (MPI_NUMERO==0)
                                GoTools::print("= ");
#else
                                GoTools::print("= ");
#endif
                                //                                 playerGoban.showOneLibertyStringsInGTPMode();
#ifdef MOGOMPI
				if (MPI_NUMERO==0)
#endif
				{//assert(0);
                                bool oneDead=false;
                                for (int i=0;i<(int)locationsOfDeadStrings.size();i++) {//assert(0);
                                  for (int j=0;j<(int)locationsOfDeadStrings[i].size();j++)
                                    playerGoban.textModeShowPosition(locationsOfDeadStrings[i][j]);
                                  GoTools::print("\n");
                                  oneDead=true;
                                }
                                if (!oneDead)
                                  GoTools::print("\n");
                                GoTools::print("\n");
				}
                                //GoTools::setOutputMode(STDERR_MODE);
                              }
                              //send("");
                            };

void MoGo::GTPEngine::continueTraining( double time ) {
  if (dynamic_cast<InnerMCGoPlayer *>(goPlayer))
    if (dynamic_cast<InnerMCGoPlayer *>(goPlayer)->isTreeAtSizeLimit())
      return ;

  double timeMultiplierToUse=timeMultiplier;
  if (playerGoban.approximateSizeOfPossibleMoves()<InnerMCGoPlayer::securityThreads)
    timeMultiplierToUse=1.;

  GoTools::setOutputMode(LOG_MODE);
  bool tmp=InnerMCGoPlayer::dontDisplay;
  InnerMCGoPlayer::dontDisplay=true;
  int tmpEarlyCut=InnerMCGoPlayer::earlyCut;
  InnerMCGoPlayer::earlyCut=0;
  goPlayer->generateOneMove(playerGoban,time*timeMultiplierToUse); //FIXME MPI
  InnerMCGoPlayer::dontDisplay=tmp;
  InnerMCGoPlayer::earlyCut=tmpEarlyCut;
  GoTools::setOutputMode(STDOUT_MODE);
}

void MoGo::GTPEngine::continueTrainingNOMPI( double time ) {
  if (dynamic_cast<InnerMCGoPlayer *>(goPlayer))
    if (dynamic_cast<InnerMCGoPlayer *>(goPlayer)->isTreeAtSizeLimit())
      return ;

  double timeMultiplierToUse=timeMultiplier;
  if (playerGoban.approximateSizeOfPossibleMoves()<InnerMCGoPlayer::securityThreads)
    timeMultiplierToUse=1.;

  GoTools::setOutputMode(LOG_MODE);
  bool tmp=InnerMCGoPlayer::dontDisplay;
  InnerMCGoPlayer::dontDisplay=true;
  int tmpEarlyCut=InnerMCGoPlayer::earlyCut;
  InnerMCGoPlayer::earlyCut=0;
  goPlayer->generateOneMoveNOMPI(playerGoban,time*timeMultiplierToUse); //FIXME MPI
  InnerMCGoPlayer::dontDisplay=tmp;
  InnerMCGoPlayer::earlyCut=tmpEarlyCut;
  GoTools::setOutputMode(STDOUT_MODE);
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GTPEngine::setThinkWhileOtherThinks( bool thinkOverOtherTime ) {
  this->thinkOverOtherTime=thinkOverOtherTime;
}

void MoGo::GTPEngine::undoOneMove( ) {//assert(0);
  GoTools::setOutputMode(LOG_MODE);
  //   GoTools::print("here are the historyGoban\n");
  //   for (int i=0;i<(int)historyGoban.size();i++)
  //     Goban(historyGoban[i]).textModeShowGoban();
  GoTools::setOutputMode(STDOUT_MODE);
  if (historyGoban.size()>1) {//assert(0);
    goPlayer->undoForHistory(playerGoban);
    playerGoban = historyGoban[historyGoban.size()-2];
    historyGoban.pop_back();
  } else if (historyGoban.size() == 1) {//assert(0);
    playerGoban.clearGoban();
    historyGoban.clear();
    historyGoban.push_back(playerGoban);
    goPlayer->clearHistoryGoban();
  }
  if (gameSaver)
    gameSaver->undoMove();
  showGoban();
}

void MoGo::GTPEngine::showPreferedSequence( ) {//assert(0);
  char position[MAX_GTP_SIZE];
#ifdef MOGOMPI
  mpi_scanf(position);
#else
  scanf("%s", position);
#endif
  Location location=changeToLocation(position);
  GoTools::setOutputMode(LOG_MODE);
  goPlayer->showPreferedSequence(playerGoban,location);
  GoTools::setOutputMode(STDOUT_MODE);
  send("");
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GTPEngine::showGoban( ) {
  GoTools::setOutputMode(LOG_MODE);
  playerGoban.textModeShowGoban();
  GoTools::print("\n");
  fflush(stderr);
  GoTools::setOutputMode(STDOUT_MODE);
  send("");
}

void MoGo::GTPEngine::showHistory( ) {//assert(0);
  GoTools::setOutputMode(LOG_MODE);
  GoTools::print("%d history gobans\n",historyGoban.size());
  for (int i=0;i<(int)historyGoban.size();i++) {//assert(0);
    //     Goban tmpGoban(historyGoban[i]);
    historyGoban[i].textModeShowGoban();
  }
  GoTools::setOutputMode(STDOUT_MODE);
  send("");
}

void MoGo::GTPEngine::showFreeLocation( ) {//assert(0);
  GoTools::setOutputMode(LOG_MODE);
  playerGoban.textModeShowGoban(playerGoban.lastMove());
  playerGoban.showFreeLocation();
  GoTools::setOutputMode(STDOUT_MODE);
  send("");
}

void MoGo::GTPEngine::updateLocationList( ) {//assert(0);
  GoTools::setOutputMode(LOG_MODE);
  playerGoban.updateLocationList();
  playerGoban.showFreeLocation();
  GoTools::setOutputMode(STDOUT_MODE);
  send("");
}

void MoGo::GTPEngine::isAtariSavingMove( ) {//assert(0);
  char color[MAX_GTP_SIZE],position[MAX_GTP_SIZE];
#ifdef MOGOMPI
  mpi_scanf(color);
  mpi_scanf(position);
#else
  scanf("%s %s", color, position);
#endif
  playWaitingMove(color);

  Location location=changeToLocation(position);
  if (!playerGoban.isLegal(location)) {//assert(0);
    send("illegal move", false);
    return;
  }
  if (playerGoban.isAtariSavingMove(location))
    send("yes");
  else
    send("no");
}

void MoGo::GTPEngine::isStillEatingMove( ) {//assert(0);
  char color[MAX_GTP_SIZE],position[MAX_GTP_SIZE];
#ifdef MOGOMPI
  mpi_scanf(color);
  mpi_scanf(position);
#else
  scanf("%s %s", color, position);
#endif
  playWaitingMove(color);


  Location location=changeToLocation(position);
  if (!playerGoban.isLegal(location)) {//assert(0);
    send("illegal move", false);
    return;
  }
  if (playerGoban.isStillEatingMove(location))
    send("yes");
  else
    send("no");

}

void MoGo::GTPEngine::setRandomMode( ) {//assert(0);
  int mode = 0;
#ifdef MOGOMPI
  char * param= new char[MAX_GTP_SIZE];
  mpi_scanf(param);
  sscanf(param,"%d",&mode);
  delete [] param;
#else
  scanf("%d",&mode);
#endif
  GoTools::setOutputMode(LOG_MODE);
  goPlayer->setRandomModeByGTPEngine(mode,playerGoban);
  GoTools::setOutputMode(STDOUT_MODE);
  send("");
}

void MoGo::GTPEngine::showTerritory( ) {//assert(0);
  GoTools::setOutputMode(LOG_MODE);
  if (playerGoban.isCFGFunctionOn()) {//assert(0);
    playerGoban.getCFG().showTerritory(&playerGoban);
    playerGoban.getCFG().showTerritoryInformation(&playerGoban);
  }
  GoTools::setOutputMode(STDOUT_MODE);
  send("");

}

void MoGo::GTPEngine::setGTPMode( int mode ) {//assert(0);
  specialGTPMode = mode;
  GoTools::setOutputMode(LOG_MODE);
  GoTools::print("GTPMode : %d\n",mode);
  GoTools::setOutputMode(STDOUT_MODE);

}

void MoGo::GTPEngine::setOpeningGeneratingMode( ) {//assert(0);
  int mode = 0;
#ifdef MOGOMPI
  char * param= new char[MAX_GTP_SIZE];
  mpi_scanf(param);
  sscanf(param,"%d",&mode);
  delete [] param;
#else
  scanf("%d",&mode);
#endif

  GoTools::setOutputMode(LOG_MODE);
  setGTPMode(mode);
  GoTools::setOutputMode(STDOUT_MODE);
  openingGeneratingModeOn = false;
  send("");
}

void MoGo::GTPEngine::openingGeneratingModePassMoveRecieved( ) {//assert(0);
  openingGeneratingModeOn = !openingGeneratingModeOn;
  GoTools::setOutputMode(LOG_MODE);
  if (openingGeneratingModeOn) {//assert(0);
    GoTools::print("OpeningGenerating enabled!\n");
    tmpMoves.clear();
    tmpDatabase.clear();
  } else {//assert(0);
    GoTools::print("OpeningGenerating disabled!\n%d moves are saved in the last session: ",tmpMoves.size());
    for (int i=0;i<(int)tmpMoves.size();i++)
      playerGoban.textModeShowPosition(tmpMoves[i]);
    GoTools::print("\nThe corresponding Goban:\n");
    playerGoban.textModeShowGoban();
    if (specialGTPMode == 1)
      openingGeneratingMode1();
    else if (specialGTPMode == 2)
      openingGeneratingMode2();
    else if (specialGTPMode == 3)
      openingGeneratingMode3();
  }
  GoTools::setOutputMode(STDOUT_MODE);

}

void MoGo::GTPEngine::openingGeneratingModeOneMoveRecieved( const Location location ) {//assert(0);
  GoTools::setOutputMode(LOG_MODE);
  GoTools::print("Coucou! one move is played (and thus is saved) at :");
  playerGoban.textModeShowPosition(location);
  tmpMoves.push_back(location);
  GoTools::print("\n");
  GoTools::setOutputMode(STDOUT_MODE);
}

void MoGo::GTPEngine::openingGeneratingMode1( ) {//assert(0);
  GoTools::print("Coucou, now we need to save the moves in the tmpMoves as the good moves for the Goban playerGoban.\n");
  double value0 = 0., value1 = 0.;
  if (playerGoban.isBlacksTurn())
    value0 = 100.;
  else value1 = 100.;
  for (int i=0;i<(int)tmpMoves.size();i++)
    //    if (!tmpDatabase.isGobanAndMoveInDatabase(playerGoban.getGobanKey(),tmpMoves[i]))
    tmpDatabase.addValues(playerGoban,tmpMoves[i],value0,value1);
  if (tmpDatabase.saveToFile(databaseName.c_str(),false))
    GoTools::print("database %s updated.\n",databaseName.c_str());
  else
    GoTools::print("database %s updating failed.\n",databaseName.c_str());

}

void MoGo::GTPEngine::openingGeneratingMode2( ) {//assert(0);
  GoTools::print("Coucou, now we need to save the moves in the tmpMoves as the bad moves for the Goban playerGoban.\n");
  double value0 = 0., value1 = 0.;
  if (playerGoban.isBlacksTurn()) value1 = 100.;
  else value0 = 100.;
  for (int i=0;i<(int)tmpMoves.size();i++)
    //    if (!tmpDatabase.isGobanAndMoveInDatabase(playerGoban.getGobanKey(),tmpMoves[i]))
    tmpDatabase.addValues(playerGoban,tmpMoves[i],value0,value1);
  if (tmpDatabase.saveToFile(databaseName.c_str(),false))
    GoTools::print("database %s updated.\n",databaseName.c_str());
  else
    GoTools::print("database %s updating failed.\n",databaseName.c_str());


}

void MoGo::GTPEngine::openingGeneratingMode3( ) {//assert(0);
  GoTools::print("Coucou, now we need to save the moves in the tmpMoves as one good sequence for the Goban playerGoban.\n");
}

void MoGo::GTPEngine::setDatabaseName( ) {//assert(0);
  char name[MAX_GTP_SIZE];
#ifdef MOGOMPI
  mpi_scanf(name);
#else
  scanf("%s",name);
#endif
  databaseName = std::string(name);

  GoTools::setOutputMode(LOG_MODE);
  GoTools::print("newDatabaseName: %s\n",databaseName.c_str());
  GoTools::setOutputMode(STDOUT_MODE);
  send("");

}

 // TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GTPEngine::playWaitingMove(char *color) {
  GoTools::setOutputMode(LOG_MODE);
  if ((!strncmp(color,"b",1) && !playerGoban.isBlacksTurn()) || (!strncmp(color,"w",1) && playerGoban.isBlacksTurn())) {
    //     GoTools::print("playWaitingMove !\n");
    //     for (int i=0;i<(int)historyGoban.size();i++) {//assert(0);
    //       historyGoban[i].textModeShowGoban();
    //     }
    playerGoban.playMoveOnGoban(PASSMOVE);
    if (gameSaver) gameSaver->addNextMove(PASSMOVE);
    goPlayer->addHistoryGoban(playerGoban);
    historyGoban.push_back(playerGoban);
  }
  GoTools::setOutputMode(STDOUT_MODE);
}

void MoGo::GTPEngine::show_evaluation( ) {//assert(0);
  GoTools::setOutputMode(LOG_MODE);
  int nbSimulations=0; double value=0.;
  if (dynamic_cast<InnerMCGoPlayer *>(goPlayer))
    value=dynamic_cast<InnerMCGoPlayer *>(goPlayer)->evaluatePosition(playerGoban, nbSimulations);
  GoTools::print("evaluation for black: %g/%i\n", value, nbSimulations);
  GoTools::setOutputMode(STDOUT_MODE);
  send("");
}

void MoGo::GTPEngine::place_free_handicap( ) {//assert(0);

  int nbStones=0;
#ifdef MOGOMPI
  char * param= new char[MAX_GTP_SIZE];
  mpi_scanf(param);
  sscanf(param,"%i",&nbStones);
  delete [] param;
#else
  scanf("%i",&nbStones);
#endif


  if (nbStones<2) {//assert(0);
    send("invalid handicap", false);
    return;
  }
  handicapLevel=nbStones;
  // = D5 E3 = E6 C4 E4 = D5 B3 D3 F3 = D5 F5 C4 B3 D3 = E5 F5 C4 D4 F4 E3 = D6 D5 D4 C3 E3 C2 D2 = D6 D5 D4 E4 C3 E3 C2 D2 = C6 C5 D5 E5 C4 E4 D3 E3 E2 = D6 E6 B5 C5 D5 B4 D4 E4 C3 D3 = D6 E6 B5 C5 D5 B4 D4 E4 F4 C3 D3 = D6 E6 C5 E5 B4 C4 D4 F4 D3 E3 F3 D2 = C6 D6 E6 C5 E5 B4 D4 B3 C3 D3 E3 = D6 E6 B5 C5 D5 B4 D4 E4 F4 C3 D3 B2 C2 E2 = D6 E6 F6 B5 C5 D5 F5 B4 D4 E4 C3 D3 = D6 E6 F6 B5 C5 D5 F5 B4 D4 E4 C3 D3 = C6 E6 F6 B5 C5 D5 E5 C4 E4 F4 B3 C3 D3 F3 C2 D2 E2 //7

  GoTools::setOutputMode(LOG_MODE);
  Vector<std::string> stones=getHandicaps(playerGoban.height(), nbStones);
  if (stones.size()==0) {//assert(0);
    GoTools::setOutputMode(STDOUT_MODE);
    send("invalid handicap or handicap not supported in this size", false);
    return;
  }
  if (nbStones>(int)stones.size()) nbStones=stones.size();
  playerGoban.clearGoban();
  goPlayer->clearHistoryGoban();
  historyGoban.clear();
  historyGoban.push_back(playerGoban);
  for (int i=0;i<nbStones;i++) {//assert(0);
    playerGoban.playMoveOnGoban(changeToLocation(stones[i].c_str()));
    if (i<nbStones-1)
      playerGoban.playMoveOnGoban(PASSMOVE);
  }
  currentHandicapCompensation=-nbStones;
  komi+=nbStones;
  goPlayer->setKomi(komi);

  GoTools::setOutputMode(STDOUT_MODE);
#ifdef MOGOMPI
  if (MPI_NUMERO==0)
#endif
		  {//assert(0);
  GoTools::print("= ");
  for (int j=0;j<nbStones;j++) {//assert(0);
    GoTools::print(stones[j].c_str());
    if (j<nbStones-1)
      GoTools::print(" ");
  }
  GoTools::print("\n");
  if (nbStones>0)
    GoTools::print("\n");
}
}

void MoGo::GTPEngine::set_free_handicap( ) {//assert(0);
  Goban tmp(playerGoban);
  char position[30],tmpLine[10000];
  int count=0;
  GoTools::setOutputMode(LOG_MODE);
#ifdef MOGOMPI
  mpi_fgets(tmpLine,10000,stdin);
#else
  fgets(tmpLine, 10000, stdin);
#endif

  int c=0;
  while (true) {//assert(0);
    int t=sscanf(tmpLine+c,"%s ", position);
    if (t<=0)
      break;
    c+=strlen(position)+1;
    //     GoTools::print("position %s (%i,%i\n", position, c,t );
    Location location=changeToLocation(position);
    if ((location<0) || (!tmp.isLegal(location))) {//assert(0);
      GoTools::setOutputMode(STDOUT_MODE);
      send("invalid coordinate", false);
      return;
    }
    if (count>0)
      tmp.playMoveOnGoban(PASSMOVE);
    tmp.playMoveOnGoban(location);
    count++;
    handicapLevel=count;
  }
  if (count<2) {//assert(0);
    GoTools::setOutputMode(STDOUT_MODE);
    send("invalid coordinate", false);
    return;
  }

  playerGoban=tmp;
  goPlayer->clearHistoryGoban();
  historyGoban.clear();
  historyGoban.push_back(playerGoban);
  handicapAgainst=true;
  currentHandicapCompensation=count;
  komi+=count;
  goPlayer->setKomi(komi);
  InnerMCGoPlayer::resignThreshold=0.999; // we don't resign at the begginning if we have a handicap against us


  GoTools::setOutputMode(STDOUT_MODE);
  send("");
}

void MoGo::GTPEngine::handleVT( ) {//assert(0);
  InnerMCGoPlayer *player=dynamic_cast<InnerMCGoPlayer *>(goPlayer);
  if (!player) {//assert(0);
    GoTools::setOutputMode(STDOUT_MODE);
    send("", false);
    return;
  }
  GobanNode *root=player->getTree().getRootNodeAddress();
  if (!root) {//assert(0);
    GoTools::setOutputMode(STDOUT_MODE);
    send("", false);
    return;
  }

  char /*position[30],*/tmpLine[10000];
  //   int count=0;
  GoTools::setOutputMode(LOG_MODE);
#ifdef MOGOMPI
  mpi_fgets(tmpLine, 10000, stdin);
#else
  fgets(tmpLine, 10000, stdin);
#endif
  std::string tmpLineString=string(tmpLine);
  if (tmpLineString.length()>0)
    tmpLineString=string(tmpLine+1);
  //   GoTools::print("tmpLineStringBefore = |%s|\n", tmpLineString.c_str());
  Vector<std::string> vTmp=GoTools::split(tmpLine, '\n');
  if (vTmp.size()>0) tmpLineString=vTmp[0]; else tmpLineString=" ";
  //   vTmp=GoTools::split(tmpLineString.c_str(), ' ');
  //   assert(vTmp.size()>0); tmpLineString=vTmp[0];
  //   GoTools::print("tmpLineString = |%s|\n", tmpLineString.c_str());
  bool askRoot=tmpLineString==" ";
  if (!askRoot) {//assert(0);
    Vector<std::string> v=GoTools::split(tmpLineString.c_str(), '_');
    Vector<Location> sequence;
    for (int i=1;i<(int)v.size();i++) {//assert(0);
      //       GoTools::print("v[%i]=|%s|\n", i, v[i].c_str());
      if (v[i].length()>1)
        sequence.push_back(changeToLocation(v[i].c_str()));
    }

    //     GoTools::print("before sequence\n");
    GobanNode *node=player->getTree().getNodeFromSequence(sequence);
    //     GoTools::print("root %i, node %i\n", root, node);
    //     GoTools::print(sequence);
    //     GoTools::setOutputMode(STDOUT_MODE);

    if (!node || node->childNode().size()==0) {//assert(0);
      GoTools::setOutputMode(STDOUT_MODE);
      //       sendRaw("= isLeafNode\n\n");
      send("", false);
      return;
    }

    //     GoTools::print("before \n");
    sendRaw("= ");
    //     GoTools::print("after \n");

    for (int i=0;i<(int)node->childNode().size();i++) {//assert(0);
      //       GoTools::print("for (int i=0;i<(int)node->childNode().size();i++) {//assert(0);\n");
      Location move=node->childMove()[i];
      GobanNode *child=node->childNode()[i];
      double value=child->nodeValue()[1]/double(child->numberOfSimulation());
      int nbSimulations=int(child->numberOfSimulation());
      if (child->isBlacksTurn()!=root->isBlacksTurn()) value=1.-value;
      //       GoTools::print("before changeFromLocation\n");
      changeFromLocation(move, tmpLine);
      //       GoTools::print("after changeFromLocation\n");
      //       GoTools::setOutputMode(STDOUT_MODE);
      sendRaw("%s ", (tmpLineString+"_"+string(tmpLine)).c_str());
      sendRaw("%s %f %i ", tmpLine, value, nbSimulations);
      if (node->nodeValue().size()>10) {//assert(0);
        double nbRave=node->nodeValue()[move+3];
        double raveValue=node->nodeUrgency()[move]/nbRave;
        if (child->isBlacksTurn()==root->isBlacksTurn()) raveValue=1.-raveValue;
        sendRaw("%f %i\n", raveValue, int(nbRave));
      } else
        sendRaw("0 0\n");
    }
    //     GoTools::setOutputMode(LOG_MODE);
    //     GoTools::print("end\n");
    //     GoTools::setOutputMode(STDOUT_MODE);
    if (node->childNode().size()==0)
      sendRaw("\n");
    sendRaw("\n");
  } else {//assert(0);
    GoTools::print("Ask root\n");
    GoTools::setOutputMode(STDOUT_MODE);
    int nbSimulations=int(root->numberOfSimulation());
    double value=root->nodeValue()[1]/double(nbSimulations);
    sendRaw("= Root ");
    sendRaw("T20 %f %i ", value, nbSimulations);
    sendRaw("0 0\n");
    sendRaw("\n");
  }


  //   send("");
}







/*  int c=0;
  while (true) {//assert(0);
    int t=sscanf(tmpLine+c,"%s ", position);
    if (t<=0)
      break;
    c+=strlen(position)+1;
    //     GoTools::print("position %s (%i,%i\n", position, c,t );
    Location location=changeToLocation(position);
    if ((location<0) || (!tmp.isLegal(location))) {//assert(0);
      GoTools::setOutputMode(STDOUT_MODE);
      send("invalid coordinate", false);
      return;
    }
    if (count>0)
      tmp.playMoveOnGoban(PASSMOVE);
    tmp.playMoveOnGoban(location);
    count++;
  }*/


void MoGo::GTPEngine::sendRaw( const char * msg, ... ) {//assert(0);
  char a[10000];
  va_list args;
  va_start(args,msg);
  vsnprintf(a, 10000, msg, args);
#ifdef MOGOMPI
  if (MPI_NUMERO==0)
#endif
  {//assert(0);
  fprintf(stdout, "%s", a);
  fprintf(debugGTPEngine, "sending to stdout%s", a);
  }
  va_end(args);
  fflush(stdout); fflush(debugGTPEngine); //JBH ADD
  fflush(stderr);
}
