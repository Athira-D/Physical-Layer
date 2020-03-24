//TO DO
//define constants for INT,FLT,STR
//Assuming insert function returns 1 for successsful insert and 0 for unsuccessful insert
//Assuming createRel return 1 on success
void import(char *filename)
{
     FILE *file=fopen(filename,"r");
     //attr : first line in the file	
     //count : no of attributes in the file
     char *attr=malloc(sizeof(char));
     int len=1;
     char ch;
     int count=1;
     while((ch = fgetc(file)) != '\n') 
     {
 	if(ch==',')
		count++;
          attr[len-1]=ch;
          len++;
          attr=realloc(attr,(len)*sizeof(char));
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

	char *attr_type=malloc(sizeof(char));
	len=1;
	while((ch = fgetc(file)) != '\n') 
	 {
        		 attr_type[len-1]=ch;
       		 len++;
       		 attr_type=realloc(attr_type,(len)*sizeof(char));
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
			type=STR;
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
          int ret=createRel(newfilename,count,attribute,attribute_type);
          int relid=openRel(newfilename);
         //Assuming createRel return 1 on success
          if(ret!=1)
    	{
		cout<<"IMPORT NOT POSSIBLE\n";
	}
	file=fopen(filename,"r");
	while((ch = fgetc(file)) != '\n')
	 	continue;
          
	char *record=malloc(sizeof(char));
	len=1;

          while(1)
	{
	while((ch = fgetc(file)) != '\n'&&(ch!=EOF)) 
	 {
        		 record[len-1]=ch;
       		 len++;
       		 record=realloc(attr_type,(len)*sizeof(char));
    	}
    	 record[len-1]='\0';
    	 i=0;
           //record contains each record in the file (seperated by commas)
       	int record_array[count];
    	j=0;
	while(j<count)
	{ 	k=0;
       		while(((record_array[i]!=',')&&(record_array[i]!='\0'))&&(k<16))
		{
	    		record_array[j][k++]=record_array[i++];
		}
	}
          union Attribute rec[count];
          for(int l=0;l<count;l++)
	{
	   	if(type[l]==0)
                           type[l].fval=record_array[l];
		if(type[l]==1)
                           type[l].ival=record_array[l];
		if(type[l]==2)
                           strcpy(type[l].strval,record_array[l]);
	}
          int r=ba_insert(relid,rec);
          //Assuming insert function returns 1 for successsful insert and 0 for unsuccessful insert
          if(r==0)
          {
		closeRel(relid);
		deleteRel(newfilename);
          }
	if(ch==EOF)
		break;
          }
          closeRel(relid);
	fclose(file);
}

