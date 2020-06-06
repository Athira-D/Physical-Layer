/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_FE_HH_INCLUDED
# define YY_YY_FE_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    SELECT = 258,
    INTO = 259,
    FROM = 260,
    WHERE = 261,
    PROJECT = 262,
    INTTYPE = 263,
    FLOATTYPE = 264,
    STRINGTYPE = 265,
    CREATE = 266,
    DROP = 267,
    TABLE = 268,
    INDEX = 269,
    ALTER = 270,
    RENAME = 271,
    TO = 272,
    JOIN = 273,
    ID = 274,
    EXIT = 275,
    EQTK = 276,
    LTTK = 277,
    GTTK = 278,
    LETK = 279,
    GETK = 280,
    NETK = 281,
    COLUMN = 282,
    AND = 283,
    INSERT = 284,
    VALUES = 285
  };
#endif
/* Tokens.  */
#define SELECT 258
#define INTO 259
#define FROM 260
#define WHERE 261
#define PROJECT 262
#define INTTYPE 263
#define FLOATTYPE 264
#define STRINGTYPE 265
#define CREATE 266
#define DROP 267
#define TABLE 268
#define INDEX 269
#define ALTER 270
#define RENAME 271
#define TO 272
#define JOIN 273
#define ID 274
#define EXIT 275
#define EQTK 276
#define LTTK 277
#define GTTK 278
#define LETK 279
#define GETK 280
#define NETK 281
#define COLUMN 282
#define AND 283
#define INSERT 284
#define VALUES 285

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 25 "fe.y" /* yacc.c:1909  */

	struct fnode *no;

#line 118 "fe.hh" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_FE_HH_INCLUDED  */
