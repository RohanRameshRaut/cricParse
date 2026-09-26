/*
Name	 : Rohan Ramesh Raut
Roll no  : 25112027
Course   : MCA-I
Assignmen: 01
Topic	 : Definition of the objects as discussed in the class on 24-01-26
Date	 : 26-01-26
 */

/*
   Identified objects:

   -match = type, ballsPerOver, city, date, gender, outcome.<winner>, over, playerOfTheMatch, umpires, venue
   -outcome = win_type, winner
   -team = name, players, runs.<tota_runs>, wicket.<total_wickets>, extras.<total_extras>
   -toss = decision, winner
   -innings = inning_number, team.<team_elements>
   -over = number, deliveries.<deliveries_elements>
   -deliveries = number, batsaman, bowler, non_striker, runs.<runs_element>, wicket.<wicket_elements>
   -runs = batsman, extras.<tota_extras>, total
   -extras = wides, legbyes, byes, noball
   -wicket = kind, player_out

 */


/*

   1.Match: we can get the info like type(odi, t20), over, gender etc.
   2.Outcome: we can get the win_type(by runs), winner name
   3.Team: it contains total teams with names, players, has total runs, wickets, extras as sub-ojects
   4.toss: it contains decision(bat, field), winner
   5.Innings: it has innings number, teams name
   6.Over: this object contains over numbers, deliveries as sub-object
   7.Deliveries: will contain delevery numbers, batsman, bowler, non stricker etc for detail information
   8.Runs: in this object we can store the batsman runs and extras total or individual as sub-object
   9.Wicket: kind(run out, lbw), name of the player 

 */


typedef struct Match{
	unsigned char type;
	short int balls_per_over;
	unsigned char city;
	unsigned char date;
	unsigned char gender;
	short int over;
	unsigned char player_of_the_match;
	unsigned char umpires;
	unsigned char venue;
}Match;


typedef struct Outcome{
	unsigned char win_type;
	unsigned char winner;
}Outcome;

typedef struct Team{
	unsigned char name;
	unsigned char players;
}Team;

typedef Toss{
	unsigned char decision;
	unsigned char winner;
}Toss;

typedef Innings{
	short int number;
}Innings;

typedef Over{
	short int number;
}Over;

typedef Deliveries{
	short int number;
	unsigned char batsman;
	unsigned char bowler;
	unsigned char non_striker;

}Deliveries;

typedef Runs{
	unsigned char batsman;
	short int total;
}Runs;

typedef Extras{
	short int wides;
	short int legbyes;
	short int byes;
	short int noballs;
}Extras;

typedef Wicket{
	unsigned char kind;
	unsigned char player_out;
}Wicket;

