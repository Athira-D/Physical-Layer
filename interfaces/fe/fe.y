%{
extern "C"
{
        int yyparse(void);
        int yylex(void);  
        int yywrap()
        {
                return 1;
        }
}
#include<bits/stdc++.h>
using namespace std;
#include <string.h>
#include <stdio.h>
#include "frontend.h"
#include "frontend.cpp"
#include "y.tab.h"
void yyerror(const char *str)
{
        fprintf(stderr,"error: %s\n",str);
}
%}

%union
{
	struct fnode *no;
}
%token SELECT INTO FROM WHERE PROJECT INTTYPE FLOATTYPE STRINGTYPE CREATE DROP TABLE INDEX ALTER RENAME TO JOIN ID EXIT EQTK LTTK GTTK LETK GETK NETK COLUMN AND INSERT VALUES

%%
Input : Commands EXIT 		{ return 0; }
       | EXIT 			{ return 1; }
       ;

Commands : Commands Command { }
 	   | Command 		{ }
 	   ;

Command : ddlCommand { }
	  | dmlCommand { }
	  ;

ddlCommand : CREATE TABLE ID '(' ColList ')' ';' 
					 { 
					 fe_createrel($<no>3, $<no>5); freeall($<no>3); freeall($<no>5); 
					 }
	      | DROP TABLE ID ';'                  
					 {      
					 fe_droprel($<no>3); freeall($<no>3); 
					 }
	      | ALTER TABLE ID RENAME TO ID ';' 
					{ 
					fe_renamerel($<no>3, $<no>6); freeall($<no>3), freeall($<no>6); 
					}
	      | ALTER TABLE ID RENAME COLUMN ID TO ID ';' 
					{ 
					fe_renamecol($<no>3, $<no>6, $<no>8); freeall($<no>3); freeall($<no>6); 						freeall($<no>8); 
					}
	      | CREATE INDEX  ID '.' ID ';' 
					{
					fe_createind($<no>4, $<no>6); freeall($<no>4); freeall($<no>6); 
					}
	      | DROP INDEX  ID '.' ID ';' 	{
					fe_dropind($<no>4, $<no>6); freeall($<no>4); freeall($<no>6);
					}
			;

ColList : ColList ',' Column { 
				$<no>$=setright($<no>1, $<no>3); 
			  }
		| Column {
			 	$<no>$=$<no>1;
			 }
		;

Column : ID INTTYPE             {
				$<no>$=$<no>1;
				$<no>$->ival = INT;
				}
	  | ID FLOATTYPE    { $<no>$=$<no>1; $<no>$->ival = FLOAT; } 
	  | ID STRINGTYPE   { $<no>$=$<no>1; $<no>$->ival = STRING; }
	   ;

dmlCommand : SELECT '*' INTO ID FROM ID WHERE CondList ';' { fe_select(NULL, $<no>4, $<no>6, $<no>8); freeall($<no>4); freeall($<no>6); freeall($<no>8); }
		   | SELECT AttrList INTO ID FROM ID WHERE CondList ';' { fe_select($<no>2, $<no>4, $<no>6, $<no>8); freeall($<no>2); freeall($<no>4); freeall($<no>6); freeall($<no>8); }
		   | SELECT AttrList INTO ID FROM ID ';' { fe_select($<no>2, $<no>4, $<no>6, NULL); freeall($<no>4); freeall($<no>6); freeall($<no>2); }
		   | SELECT '*' INTO ID FROM ID ';' { cout<<"Currently not supported"<<endl; }
		   | SELECT '*' INTO ID FROM ID JOIN ID WHERE ID '=' ID ';' { }
		   | SELECT AttrList INTO ID FROM ID JOIN ID WHERE ID '=' ID ';' { }
		   | INSERT INTO ID VALUES RecList ';' { }
		   ;

AttrList : IdList { $<no>$=$<no>1; };

CondList : CondList AND Condition { $<no>$=setnext($<no>1, $<no>3);  }
		 | Condition { $<no>$=$<no>1; }
		 ;

Condition 	  : ID EQTK ID { $<no>$=setright($<no>1, $<no>3);  $<no>$->ival = EQ; }
		  | ID LETK ID { $<no>$=setright($<no>1, $<no>3);  $<no>$->ival = LE; }
		  | ID GETK ID { $<no>$=setright($<no>1, $<no>3);  $<no>$->ival = GE; }
		  | ID LTTK ID { $<no>$=setright($<no>1, $<no>3);  $<no>$->ival = LT; }
		  | ID GTTK ID { $<no>$=setright($<no>1, $<no>3);  $<no>$->ival = GT; }
		  | ID NETK ID { $<no>$=setright($<no>1, $<no>3);  $<no>$->ival = NE; }
		  ;

RecList : RecList ',' Record  { setnext($<no>1, $<no>3); $<no>$=$<no>1; }
         | Record              { $<no>$=$<no>1; }
         ;

Record : '(' IdList ')'  { $<no>$=$<no>2; };

IdList : IdList ',' ID   { $<no>$=setright($<no>1, $<no>3); }
        | ID             { $<no>$=$<no>1;}
        ;
%%

int main()
{
        yyparse();  
        return 0;
} 
