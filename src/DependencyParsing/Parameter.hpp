#ifndef _PARAMETER_H_
#define _PARAMETER_H_

/************************************** Simulator **************************************/

#define LEARNTIMES	        200	        /*times of learning*/
#define MUTATEPRO   	        0.001	        /*Mutated probability*/
#define PRECISION	        100000000	/*3 points after 0: 0.000*/
#define ACCURACYTHRESHOLD       0.96            /*threshold for mutated accuracy*/
#define TIMETHRESHOLD           10              /*time threshold for terminating immune responses to control
                                                the training time*/

/************************************** Simulator **************************************/

/************************************** Word Agent **************************************/

/*Status*/
#define ACTIVE		1	/*resting*/
#define MATCH		2	/*match*/
#define MUTATE		3	/*mutated*/
#define MATURE		4	/*matured*/
#define DIE		5	/*dead*/
#define REGULATE        6       /*regulated*/
/*Status*/

/*Behavior*/
#define MOVING		1	/*moving*/
#define INTERACTING	2	/*interacting*/
#define MUTATING	3	/*mutating*/
#define SELECTING	4	/*selecting*/
#define CLONING		5	/*cloning*/
#define	REGULATING	6	/*regulating*/
#define DYING		7	/*dying*/
/*Behavior*/

/*Category*/
#define ANTIGEN		0	/*antigen word-agent*/
#define BCELL		1	/*B cell word-agent*/
#define MEMORYBCELL     2       /*memory B cell word-agents*/
/*Category*/

/************************************** Word Agent**************************************/


/************************************** Enviroment **************************************/

#define ROWS 15                 /*rows in the environment*/
#define COLS 15                 /*columns in the environment*/
#define MAXNUMAGENT     100     /*maximum number of agents in a grid*/

/************************************** Enviroment**************************************/

#endif
