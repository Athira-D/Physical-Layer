#include<stdio.h>
#include<stdlib.h>
#include<bits/stdc++.h>
using namespace std;

#include "schema.cpp"

//TO DO
//Assuming insert function returns 1 for successsful insert and 0 for unsuccessful insert
//check in delete blk, delete rel on the type entered in blk allocation map
//Tell Aparnas team that ECACHEFULL and ERELNOTEXISTS cannot take values from 0 to 11


int check_type(char *data)
{	
	int count_int=0,count_dot=0,count_string=0,i;
	for(i=0;data[i]!='\0';i++)
	{
		
		if(data[i]>='0'&&data[i]<='9')
			count_int++;
		if(data[i]=='.')
			count_dot++;
		else
			count_string++;
	}
	
	if(count_dot==1&&count_int==(strlen(data)-1))
		return FLOAT;
	if(count_int==strlen(data))
	{
		return INT;
	}
	else
		return STRING;
}

void import(char *filename)
{
     FILE *file=fopen(filename,"r");
     //attr : first line in the file	
     //count : no of attributes in the file
     char *attr=(char*) malloc(sizeof(char));
     int len=1;
     char ch;
     int count=1;
     while((ch = fgetc(file)) != '\n') 
     {
 	if(ch==',')
		count++;
          attr[len-1]=ch;
          len++;
          attr=(char*) realloc(attr,(len)*sizeof(char));
    }
     attr[len-1]='\0';
     int i=0,j,k;
     char attribute[count][16];
     j=0;
	while(j<count)
	{ 	k=0;
       		while(((attr[i]!=',')&&(attr[i]!='\0'))&&(k<16))
		{
	    		attribute[j][k++]=attr[i++];
		}
		attribute[j][k]='\0';
		j++;i++;
	}
	i=0;
      //attribute array contains the names of all attributes
    

	char *attr_type=(char*) malloc(sizeof(char));
	len=1;
	while((ch = fgetc(file)) != '\n') 
	 {
        		 attr_type[len-1]=ch;
       		 len++;
       		 attr_type=(char*) realloc(attr_type,(len)*sizeof(char));
    	}
    	 attr_type[len-1]='\0';
    	 i=0;
           //attr_type contains second line in the file (to know the type) 
       	char attribute_type[count][16];
          int type[count];
    	j=0;
	while(j<count)
	{ 	k=0;
       		while(((attr_type[i]!=',')&&(attr_type[i]!='\0'))&&(k<16))
		{
	    		attribute_type[j][k++]=attr_type[i++];
		}
		attribute_type[j][k]='\0';
		int t=check_type(attribute_type[j]);
		if(t==FLOAT)
			type[j]=FLOAT;
		if(t==INT)
			type[j]=INT;
		if(t==STRING)
			type[j]=STRING;
		j++;i++;
	}
     
	i=0;
	//type array contains the types of all attributes
	char newfilename[(strlen(filename))-4];
	int loopv=0;
	while(filename[loopv]!='.')
		{
		newfilename[loopv]=filename[loopv];
		loopv++;
		}
	newfilename[loopv]='\0';
          int ret;
          ret=createRel(newfilename,count,attribute,type);
          if(ret!=SUCCESS)
	{
		cout<<"IMPORT NOT POSSIBLE\n";
		return;
	}	
          int relid=openRel(newfilename);
          if(relid==E_CACHEFULL||relid==E_RELNOTEXIST)
	{
		cout<<"IMPORT NOT POSSIBLE\n";
		return;
	}
	file=fopen(filename,"r");
	while((ch = fgetc(file)) != '\n')
	 	continue;
          
	char *record=(char*) malloc(sizeof(char));
	len=1;

          while(1)
	{
	ch= fgetc(file);
          len=1;
	while((ch  != '\n')&&(ch!=EOF)) 
	 {
        		 record[len-1]=ch;
       		 len++;
       		 record=(char*) realloc(record,(len)*sizeof(char));
                     ch=fgetc(file);
    	}
          record[len-1]='\0';
    	i=0;
         //record contains each record in the file (seperated by commas)
          char record_array[count][16];
    	j=0;
	while(j<count)
	{ 	k=0;
       		while(((record[i]!=',')&&(record[i]!='\0'))&&(k<16))
		{
	    		record_array[j][k++]=record[i++];        
		}
                    i++;
                    record_array[j][k]='\0';
                    j++;
	}
          union Attribute rec[count];
          for(int l=0;l<count;l++)
	{
	   	if(type[l]==FLOAT)
		       {
                           rec[l].fval=atof(record_array[l]);
		       }
		if(type[l]==INT)
		       {
                           rec[l].ival=strtoull(record_array[l],NULL,10);
		       }
		if(type[l]==STRING)
		       {
                           strcpy(rec[l].sval,record_array[l]);
		       }
	}
          int r; 
          r=ba_insert(relid,rec);
          //Assuming insert function returns 1 for successsful insert and 0 for unsuccessful insert
          if(r!=SUCCESS)
          {
		closeRel(relid);
		ba_delete(newfilename);
                    return;
          }
	 if(ch==EOF)	
                   break;
          }
          closeRel(relid);
	fclose(file);
}

int main()
{
createdisk();
formatdisk();
meta();
import("rel1.csv");
}
