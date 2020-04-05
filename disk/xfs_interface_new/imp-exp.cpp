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

void exp(char *rel_name,char *exportname)
{
	FILE *fp_export=fopen(exportname,"w");
	struct HeadInfo header;
          int i,first_block;
	union Attribute rec[6]; //for relation catalog entry
	int num_slots,next_block_num,num_attrs,no_attrs;
     	for(i=0;i<20;i++)
      	{
        	       getRecord(rec,4,i);
                 if(strcmp(rec[0].sval,rel_name)==0)
                 {     
		   first_block = rec[3].ival;
		   no_attrs=rec[1].ival;     
                       break;
                 }
      	}
     	if(i==20)
       	{
       	        cout<<"EXPORT NOT POSSIBLE!\n";
	        return;
     	}
	if(first_block==-1)
	{
		cout<<"No records exist!\n";
		return;
	}

	union Attribute attr[6];
	int attr_blk=5;
	char Attr_name[no_attrs][16];
	int Attr_type[no_attrs];
          int j=0;
	while(attr_blk != -1)
	{
		header=getheader(attr_blk);
		next_block_num = header.rblock; 
		for(i=0;i<20;i++)
      		{
        	    	     getRecord(attr,attr_blk,i);
               	     if(strcmp(attr[0].sval,rel_name)==0)
                	     {     
		   	strcpy(Attr_name[j],attr[1].sval); 
                      	Attr_type[j++]=attr[2].ival;
               	     }
      		}
		attr_blk = next_block_num;
	}
          for(j=0;j<no_attrs;j++)
	{         
		fputs(Attr_name[j],fp_export);
		if(j!=no_attrs-1)
		fputs(",",fp_export);
	}
	fputs("\n",fp_export);
          int block_num=first_block;
	while(block_num != -1)
	{
		header=getheader(block_num);
		num_slots = header.numSlots;
		next_block_num = header.rblock; 
		num_attrs=header.numAttrs;
		unsigned char slotmap[num_slots];
		getSlotmap(slotmap,block_num);

		union Attribute A[num_attrs];
		int iter;
		for(iter = 0; iter< num_slots; iter++)
		{
			if(slotmap[iter] ==(unsigned char)1)
			{
				getRecord(A,block_num,iter);
				char s[16];
				for(int l=0;l<no_attrs;l++)
				{
	   				if(Attr_type[l]==FLOAT)
		      	 		{
						sprintf(s, "%f",A[l].fval );
						//cout<<s<<" "<<strlen(s)<<"\n";
						fputs(s,fp_export); 
		       			}
					if(Attr_type[l]==INT)
		     			{
						sprintf(s, "%lld",A[l].ival );
						//cout<<s<<" "<<strlen(s)<<"\n";
						fputs(s,fp_export); 
		      			}
					if(Attr_type[l]==STRING)
		     		          {
                         				 fputs(A[l].sval,fp_export);
		    		          }
					if(l!=no_attrs-1)
					     fputs(",",fp_export);
				}				
				fputs("\n",fp_export);
			}
		}
		block_num = next_block_num;
	}
	fclose(fp_export);
}


int main()
{
createdisk();
formatdisk();
meta();
import("rel1.csv");
exp("rel1","rel2");

}


