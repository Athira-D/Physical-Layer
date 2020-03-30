#include<stdio.h>
#include<stdlib.h>
#include<bits/stdc++.h>
using namespace std;

#include "block_access.cpp"
#include "disksimulator.cpp"
#include "schema.cpp"
//#include "disk_structure.h"

//TO DO
//define constants for INT,FLT,STR
//Assuming insert function returns 1 for successsful insert and 0 for unsuccessful insert
//Assuming createRel return 1 on success

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
	//printf("%d:%d:%d:%d",count_int,count_dot,count_string,i);
	if(count_dot==1&&count_int==(strlen(data)-1))
		return 0; //FLOAT
	if(count_int==strlen(data))
		return 1; //INT
	else
		return 2; //STRING
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
		if(t==0)
			type[j]=FLT;
		if(t==1)
			type[j]=INT;
		if(t==2)
			type[j]=STR;
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
          ret=createRel(newfilename,count,attribute,attribute_type);
          int relid=openRel(newfilename);
         //Assuming createRel return 1 on success
         if(ret!=1)
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
	   	if(type[l]==0)
		       {
		       cout<<record_array[l];
                           rec[l].fval=stof(record_array[l]);
		       }
		if(type[l]==1)
		       {
		       cout<<record_array[l];
                           rec[l].ival=atoi(record_array[l]);
		        }
		if(type[l]==2)
		        {
		        cout<<record_array[l];
                            strcpy(rec[l].strval,record_array[l]);
		        }
                    cout<<"\n";
	}
       
          int r;
          r=ba_insert(relid,rec);
          //Assuming insert function returns 1 for successsful insert and 0 for unsuccessful insert
          if(r==0)
          {
		closeRel(relid);
		ba_delete(newfilename);
          }
	 if(ch==EOF)	
                   break;
          }
          closeRel(relid);
	fclose(file);
}

int main()
{
import("rel1.csv");
}
