%{
#include <stdio.h>
#include "y.tab.h"
#include "frontend.h"
#include "frontend.cpp"
char s[16];
%}

%%
select|SELECT			{return SELECT;}
into|INTO			{return INTO;}
from|FROM			{return FROM;}
where|WHERE			{return WHERE;}
project|PROJECT			{return PROJECT;}
insert|INSERT			{return INSERT;}
create|CREATE			{return CREATE;}
drop|DROP			{return DROP;}
table|TABLE			{return TABLE;}
index|INDEX			{return INDEX;}
int|INT|integer|INTEGER 		{return INTTYPE;}
float|FLOAT			{return FLOATTYPE;}
string|STRING|str|STR 		{return STRINGTYPE;}
alter|ALTER			{return ALTER;}
rename|RENAME			{return RENAME;}
column|COLUMN			{return COLUMN;}
to|TO				{return TO;}
join|JOIN			{return JOIN;}
exit				{return EXIT;}
"*"				{return *yytext;}
"="				{return EQTK;}
"<"				{return LTTK;}
">"				{return GTTK;}
"<="				{return LETK;}
">="				{return GETK;}
"!="				{return NETK;}
[(),;\.]				{return *yytext;}
[ \n\t]				{}
[a-zA-Z0-9_]+			{
				 strcpy(s,strdup(yytext));
				 yylval.no=getNode(s, 0, NULL, NULL);
				 return ID;
				}
%%

