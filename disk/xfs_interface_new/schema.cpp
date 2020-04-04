//  Note
// in CreateRel updated no of entries to 0 when relcat entry is created.was not there before.

#include "block_access.cpp"


int createRel(char relname[16],int nAttrs, char attrs[][ATTR_SIZE],int attrtype[])
{
    union Attribute attrval;union Attribute relcat_rec[6];
    strcpy(attrval.sval,relname);
    union Attribute relcatrec[6];//relname,#attrs,#records,firstblk,slotsperblk
    int flag=ba_search(0,relcatrec,"RelName", attrval, EQ);
    //cout<<"Flag: "<<flag<<"\n";
    if(flag==SUCCESS)
   {
        return E_RELEXIST; //Relation name already exists.
    }
    int iter;
    for(iter=0;iter<nAttrs;iter++)
    {
        int iter2;
        for(iter2=iter+1;iter2<nAttrs;iter2++)
       {
            if(strcmp(attrs[iter],attrs[iter2])==0)
            {
                return E_DUPLICATEATTR;  //distinct attributes having same name.
            }
        }
    }
    strcpy(relcatrec[0].sval,relname);
    relcatrec[1].ival=nAttrs;
    relcatrec[2].ival=0;
    relcatrec[3].ival=-1;//first block=-1 ,earlier it was 0
    relcatrec[4].ival=-1;
    relcatrec[5].ival=(2016/(16*nAttrs+1));
    flag=ba_insert(0,relcatrec);
    getRecord(relcat_rec,4,0);
    relcat_rec[2].ival=relcat_rec[2].ival+1;
    setRecord(relcat_rec,4,0);
	
    if(flag!=SUCCESS)
    {
        ba_delete(relname);
        return flag;
    }
    for(iter=0;iter<nAttrs;iter++)
    {
        union Attribute attrcatrec[6];//relname,attrname,attrtype,primaryflag,rootblk,offset
        strcpy(attrcatrec[0].sval,relname);
        strcpy(attrcatrec[1].sval,attrs[iter]);
        attrcatrec[2].ival=attrtype[iter];
        attrcatrec[3].ival=-1;
        attrcatrec[4].ival=-1;
        attrcatrec[5].ival=iter;
        flag=ba_insert(1,attrcatrec);
       getRecord(relcat_rec,4,1);
       relcat_rec[2].ival=relcat_rec[2].ival+1;
       setRecord(relcat_rec,4,1);
        if(flag!=SUCCESS)
       {
            ba_delete(relname);
            return flag;
        }
    }
    return SUCCESS; 
}

int openRel(char RelName[16])
{
      union Attribute rec[6]; //for relation catalog entry
      int i;
      for(i=0;i<20;i++)
      {
          getRecord(rec,4,i);
          if(strcmp(rec[0].sval,RelName)==0)
          {          
                       break;
          }
       }
       if(i==20)
       {
           return E_RELNOTEXIST;
       }
       for(i=0;i<12;i++)
       {  
	if(strcmp(RelName,OpenRelTable[i])==0)
            return i;
        }
       for(i=0;i<12;i++)
       {
          if(strcmp(OpenRelTable[i],"NULL")==0)
          {  
              strcpy(OpenRelTable[i],RelName);
              return i;   
          }
       }
       if(i==12)
          {       
                 return E_CACHEFULL; 
          }     
}

int closeRel(int relid)    //return 0 on success
{
    if(relid< 0 || relid>= MAXOPEN)
    {
              return E_OUTOFBOUND;
    }
    if(strcmp(OpenRelTable[relid],"NULL")==0)
    {
        return E_NOTOPEN;
    }   
    strcpy(OpenRelTable[relid],"NULL");
    return 0;
}

