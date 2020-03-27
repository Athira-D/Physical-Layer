//E_RELEXIST =0
int createRel(char relname[],int nAttrs, char attrs[][ATTR_SIZE],int attrtype[])
{
    //int relcat_entry;
    //struct RelCatEntry relcat;
    //OpenRelTable::getRelCatEntry(0,&relcat);
    //recId targetrelid;
    union Attribute attrval;
    strcpy(attrval.sval,relname);
    union Attribute relcatrec[6];//relname,#attrs,#records,firstblk,slotsperblk
    int flag=ba_search(0,relcatrec,"RelName", attrval, EQ);

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
    relcatrec[2].ival=-1;//first block=-1 ,earlier it was 0
    relcatrec[3].ival=-1;
    relcatrec[4].ival=(2016/(16*nAttrs+1));

    flag=ba_insert(0,relcatrec);
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
        if(flag!=SUCCESS){
            ba_delete(relname);
            return flag;
        }
    }
    return SUCCESS;	
}




int openrel(char RelName[16]);
{
      union Attribute *rec;
      for(int i=0;i<20;i++)
      {
        getRecord(rec,4,i);
          if(strcmp(rec.rel_name,RelName)==0)
        {
                       break;
        }
      }
       if(i==20)
      {
    cout<<"E_RELNOTEXIST";
          return FAILURE;
      }
       for(i=0;i<12;i++)
     if(strcmp(RelName,OpenRelTable[i])==0)
     {       
        return i;
     }
       for(i=0;i<12;i++)
       {
      if(OpenRelTable[i]==NULL)
      {
        strcpy(OpenRelTable[i],RelName);
        return i;   
      }
       }
       if(i==13)
       {        cout<<"E_CACHEFULL\n";
        //or E_CACHEFULL?
        return -1; 
       }     
}

int closerel(int relid)
{
    if(rel_id< 0 || rel_id>= MAXOPEN)
    {
         cout<<"E_OUTOFBOUND\n";
                     return E_OUTOFBOUND;
    }
    if(OpenRelTable[i]==NULL)
    {
        cout<<"E_NOTOPEN\n";
        return E_NOTOPEN;
    }   
          OpenRelTable[i]=NULL;
}
