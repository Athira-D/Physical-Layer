//Todo :  how to check if conversion fails
/*#include "schema.cpp"
#include <bits/stdc++.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

using namespace std;*/

int select(char srcrel[ATTR_SIZE],char targetrel[ATTR_SIZE], char attr[ATTR_SIZE], int op, char strval[ATTR_SIZE])
{

    // get the srcrel's open relation id(let it be srcrelid), using getRelId() method of cache layer
    // if srcrel is not open in open relation table, return E_RELNOTOPEN

    int src_relid=getRelid(srcrel);
    if(src_relid==FAILURE)
        return E_RELNOTOPEN;
    union Attribute attrcat_entry[6];

    // get the attribute catalog entry for attr, using getAttrcatEntry() method of cache layer(let it be attrcatentry).
    // if getAttrcatEntry() call fails return E_ATTRNOTEXIST
    int flag= getAttrCatEntry(src_relid,attr,attrcat_entry);
    if(flag!=SUCCESS)
        return flag;
    

    //convert strval into union Attribute (let it be val) as shown in the following code:
     //if convert fails, return E_ATTRTYPEMISMATCH
    int type=attrcat_entry[1].ival;
    union Attribute val;
    if(type==INT) 
        //The input contains a string representation of the integer attribute value.
        val.ival=stoi(strval);
        //if conversion fails(i.e string can not be converted to integer) return E_ATTRTYPEMISMATCH. 
    else if(type==FLOAT)
    {
        //do accordingly to float
        val.fval=stof(strval);

    }
    else if(type==STRING)
    {
        //No type conversion needed here
        strcpy(val.sval,strval);
    }
    else
        return E_NATTRMISMATCH;
   

    //Next task is to create the destination relation.
    //Prepare arguments for createRel() in the following way:
    //get RelcatEntry of srcrel from cache using getRelCatEntry() method of cache layer.
    //get the no. of attributes present in src relation, from RelcatEntry.(let it be nAttrs)
    union Attribute relcat_entry[6];
    getRelCatEntry(src_relid,relcat_entry);
    int nAttrs=relcat_entry[1].ival;

    // let attr_name[nAttrs][ATTR_SIZE] be a 2D array of type char(attribute names of rel).
    // let attr_types[nAttrs] be an array of type int

    char attr_name[nAttrs][ATTR_SIZE];
    int attr_types[nAttrs];

    /*iterate through 0 to nAttrs-1 :
        get the i'th attribute's AttrCatEntry (using getAttrcatEntry() of cache layer )
        fill attr_name, attr_types of corresponding attributes using Attribute catalog found.
    */
    recId prev_recid={-1,-1};
    recId recid;
    for(int i=0;i<nAttrs;i++)
    {
        union Attribute record[6];
        union Attribute a;
        strcpy(a.sval,OpenRelTable[src_relid]);
        recid=linear_search(ATTRCAT_RELID,"RelName",a,EQ,&prev_recid);
        if(!((recid.block==-1)&&(recid.slot==-1)))
        {
            getRecord(record,recid.block,recid.slot);
            strcpy(attr_name[i],record[1].sval);
            attr_types[i]=record[2].ival;  
        }
        if((recid.block==-1)&&(recid.slot==-1))
        	return E_ATTRNOTEXIST;
        

    }

    /* let retval= createRel(targetrel,no_of_attrs_srcrel,attr_name,attr_type)
       where createrel is a function in schema layer
       if create fails return retval */
    int retval=createRel(targetrel,nAttrs,attr_name,attr_types);
    if(retval!=SUCCESS)
        return retval;

    //int targetrelid = openRel(targetrel) 
    //where openRel is a function in schema layer
    /* if open fails
        delete target relation by calling deleteRel(targetrel) of schema layer
        return retval
    */
    int target_relid=openRel(targetrel);
    if(target_relid==-1)
    {	
    	retval=ba_delete(targetrel);
        return -1;
    }

    
    //Note: Before calling the search function, reset the search to start from the first hit
    // by calling ba_search of block access layer with op=RST.
    /*
    while (1):
        var: union Attribute record[no_of_attrs_srcrel];
        if ba_search(srcrelid,record,attr,val,op) returns SUCCESS:
            retval = ba_insert(targetrelid,record);
            if(insert fails):
                close the targetrel(by calling closeRel(targetrel) method of schema layer)
                delete targetrel (by calling deleteRel(targetrel) of schema layer)
                return retval

        else: break
    */

    while(1)
    {
        union Attribute record[nAttrs];
        retval=ba_search(src_relid,record,attr,val,op);
        if(retval==SUCCESS)
        {
            int ret= ba_insert(target_relid,record);
            if(ret!=SUCCESS)
            {
                closeRel(target_relid);
                int flag= ba_delete(targetrel);
                return ret;

            }
        }
        else
            break;
    }

    //Close the target relation using closeRel() method of schema layer
    // return SUCCESS;

    closeRel(target_relid);
    return SUCCESS;
    
    }