/*

typedef struct LineT{
        unsigned int start_index;
        unsigned char length;
}LineT;

Validation: 

1. start_index >= 0
2. 0 <= length <= 255
3. line[0].length < line[1].start_index
4. (line[0].length < line[1].start_index) ---> (1-0 = 1) i.e the difference between two consecutive lines should be 1.
-----------------------------------------------------------------------------------------------------------------------------
typedef struct ContentT{
        unsigned short cnl; //line number
        unsigned char start;
}ContentT;

ContentT teams[2];

Validation:

1. 0 <= cnl <= nl(number of lines)
2. 0 <= start <= line[cnl].length
----------------------------------------------------------------------------------------------------------------------------
typedef struct delT{
        unsigned short start;
        ContentT del_no;
        ContentT del_type;
        ContentT runs;
        ContentT outcome;
        unsigned char player_index;
        unsigned char player_index2;
}delT;

Validation:

1. inning[2].overs[novers].start < start
2. 0 <= inning[2].overs[novers].del[ndel].del_no <= 11
3. (del_no > 6) --> (del_type => extras={wides, noballs}) AND (del_no <= 6) --> (del_type => -1)
4. 0 <= runs <= 15
5. outcome(player_out, fielder) => inning[2].plr[11].index
6. player_index AND player_index2 => inning[2].plr[11].index
-------------------------------------------------------------------------------------------------------------
typedef struct overT{
        unsigned short start;
        delT *ds;
        unsigned char player_index;
        unsigned char over_no, ndel;
}overT;

Validation:

1. inning[0].start < start < inning[1].start
2. player_index => {inning[0].plr[11].index or inning[1].plr[11].index)
3. 0 <= innin[0].overs[novers].over_no <= 52
4. 0 <= inning[0].overs[novers].ndel <= 550
5. 0 <= innin[1].overs[novers].over_no <= 52
6. 0 <= inning[1].overs[novers].ndel <= 550
--------------------------------------------------------------------------------------------------------------
typedef struct Players{
        unsigned char index;
        ContentT name;
}Players;

Validation:

1. 0 <= index <= 15
2. 5 <= name.cnl <= 100
-----------------------------------------------------------------------------------------------------------------
typedef struct Toss{
	ContentT decision;
	ContentT winner;
}Toss;

Validation:

1. decision => (bat or field (bowl))
2. winner => (teams[0].cnl or team[1].cnl)
-----------------------------------------------------------------------------------------------------------------
typedef struct InningT{
        _BatsmanT *bat;
        _bowlerT *blr;
        overT *overs;
        Players *plr;
        unsigned char novers, nbat, nblr;
        unsigned short start, end;
}InningT;

Validation: 

1. 0 <= inning[0].novers <= 51 && 0 <= inning[0].novers <= 51
2. 2 <= inning[0].nbat <= 11 && 2 <= inning[0].nbat <= 11
3. 5 <= inning[0].nblr <= 11 && 5 <= inning[0].nblr <= 11 
4. Toss.decision.cnl < inning[0].start 
5. inning[0].start <= inning[0].end < inning[1].start <= inning[1].end <= nl(number of lines)
6. inning[0].plr[10].name.cnl + 1 < inning[1].plr[0].name.cnl
----------------------------------------------------------------------------------------------------------------
*/
