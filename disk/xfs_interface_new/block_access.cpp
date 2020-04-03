//ToDo : write getBlockType()
//review getRecord
//open rel table null is long int. replace with '\0'?
//memset in getFreeslot 
//why are we not setting pblock and rblock after getFreeRecBlock in getFreeSlot
//in getRelcat do we need the & before relcat_entry or just relcat_entry
//ToDo: After getFreeslot we should update block allocation map if new block is allocated...needed in ba_insert anyway.

/*#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<bits/stdc++.h>*/
#include"disksimulator.cpp"

int getBlockType(int blocknum)
{
	FILE *disk=fopen("disk","rb");
	fseek(disk,0,SEEK_SET);
	unsigned char blockAllocationMap[4*BLOCK_SIZE];
	fread(blockAllocationMap,4*BLOCK_SIZE,1,disk);
	fclose(disk);
	return (int32_t)(blockAllocationMap[blocknum]);
	
}

struct HeadInfo getheader(int blockNum)
{
	//int BlockType=getBlockType(blockNum);
	struct HeadInfo header;
	FILE *disk=fopen("disk","rb");
	fseek(disk,blockNum*BLOCK_SIZE,SEEK_SET);
	fread(&header,32,1,disk);
	fclose(disk);
	return header;

}

int deleteBlock(int curr_block)
{
	FILE *disk;
	disk=fopen("disk","rb+");
	fseek(disk,BLOCK_SIZE*curr_block,SEEK_SET);
	unsigned char ch= 0;
	fwrite(&ch,BLOCK_SIZE,1,disk);
	fclose(disk);
}

void setheader(struct HeadInfo *header,int blockNum)
{
	FILE *disk=fopen("disk","rb+");
	fseek(disk,blockNum*BLOCK_SIZE,SEEK_SET);
	fwrite(header,32,1,disk);
	fclose(disk);
}



void getSlotmap(unsigned char * SlotMap,int blockNum)
{
	FILE *disk=fopen("disk","rb");
	fseek(disk,blockNum*2048,SEEK_SET);
	struct recBlock R;
	fread(&R,2048,1,disk);
	int numSlots=R.numSlots;
	memcpy(SlotMap,R.slotMap_Records,numSlots);
	fclose(disk);
}

void setSlotmap(unsigned char * SlotMap,int no_of_slots,int blockNum)
{
	FILE *disk=fopen("disk","rb+");
	fseek(disk,blockNum*2048+32,SEEK_SET);
	fwrite(SlotMap,no_of_slots,1,disk);
	fclose(disk);
}

int getFreeRecBlock()
{
	FILE *disk=fopen("disk","rb");
	fseek(disk,0,SEEK_SET);
	unsigned char blockAllocationMap[4*BLOCK_SIZE];
	fread(blockAllocationMap,4*BLOCK_SIZE,1,disk);
	fclose(disk);
	int iter;
	for(iter=0;iter<4*BLOCK_SIZE;iter++)
	{
		if((int32_t)(blockAllocationMap[iter])==UNUSED_BLK)
				return iter;
	}
	return FAILURE;

}

recId getFreeSlot(int block_num)
{
	recId recid = {-1, -1};
	int prev_block_num, next_block_num;
	//RecBuffer *rec_buffer;
	//struct HeadInfo header;
	int num_slots;
	int iter,num_attrs;
	struct HeadInfo header;
	// finding free slot
	while(block_num != -1)
	{
		//rec_buffer = Buffer::getRecBuffer(block_num);
		//header = rec_buffer->getheader();
		header=getheader(block_num);
		num_slots = header.numSlots;
		next_block_num = header.rblock; 
		num_attrs=header.numAttrs;
		// next block
		//getting slotmap entries
		unsigned char slotmap[num_slots];
		getSlotmap(slotmap,block_num);
		//searching for free slot in block (block_num)
		int iter;
		for(iter = 0; iter< num_slots; iter++)
		{
			if(slotmap[iter] =='0')
			{
				break;
			}
		}
		if(iter < num_slots)
		{
			recid = {block_num, iter};
			return recid;
		}
		prev_block_num = block_num;
		block_num = next_block_num;
	}
	
	//no free slot is found in existing records if block_num = -1
	block_num =getFreeRecBlock();
	if(block_num ==-1)
	{
		// no blocks are available in disk
		return recid; // no free slot can be found
	}
	//block_num = rec_buffer->getBlockNum();
	//setting header values
	header = getheader(block_num);
	header.lblock = prev_block_num;
	header.numSlots = num_slots;
	//was not there before: Athira
	header.numAttrs=num_attrs;
	setheader(&header,block_num);
	//setting slotmap
	unsigned char slotmap[num_slots];
	getSlotmap(slotmap,block_num);
	memset(slotmap,'0', sizeof(slotmap)); //all slots are free
	setSlotmap(slotmap,num_slots,block_num);
	//delete rec_buffer;
	//recid
	recid = {block_num, 0};
	//setting prev_block_num rblock to new block
	header=getheader(prev_block_num);
	header.rblock = block_num;
	setheader(&header,block_num);
	return recid;
}

int getRecord(union Attribute *rec,int blockNum,int slotNum)
{ 
	struct HeadInfo Header;
	Header=getheader(blockNum);
	int numOfSlots=Header.numSlots;
	if(slotNum < 0 || slotNum > (numOfSlots - 1))
		return E_OUTOFBOUND;
	FILE *disk=fopen("disk","rb+");
	int BlockType=getBlockType(blockNum);
	if(BlockType==REC)
	{
		struct recBlock R;
		fseek(disk,blockNum*BLOCK_SIZE,SEEK_SET);
		fread(&R,BLOCK_SIZE,1,disk);
		int numSlots=R.numSlots;
		unsigned char slotMap[numSlots];

		if(*((int32_t*)(R.slotMap_Records+ slotNum)) == 0)
	    	return E_FREESLOT;
		int numAttrs=R.numAttrs;
		// it should be R.slotmprecords+32+...???
		memcpy(rec,(R.slotMap_Records+numSlots+(slotNum*numAttrs*ATTR_SIZE)), numAttrs*16);
		fclose(disk);
		return SUCCESS;

	}
	else if(BlockType==IND_INTERNAL)
	{
		//to be done

	}	
	else if(BlockType==IND_LEAF)
	{
		//to be done
	}
	else
	{
		fclose(disk);
		return FAILURE;
	}
	/*unsigned char* bufferPtr = getBufferPtr();
	int numOfAttrib=((int32_t) (bufferPtr + 5*4));
	int numOfSlots=((int32_t) (bufferPtr + 6*4));
	if(slotNum < 0 || slotNum > numOfSlots - 1)
		return E_OUTOFBOUND;  
	unsigned char *slotMap = new unsigned char;
	getSlotMap(slotMap);
	if((int32_t*)(slotMap + slotNum) == 0)
	    return E_FREESLOT;
	memcpy((void*)(bufferPtr + 32 + numOfSlots + (slotNum*numOfAttrib)ATTR_SIZE), (void)rec, numOfAttrib*ATTR_SIZE);
	return SUCCESS;*/
}

int setRecord(union Attribute *rec,int blockNum,int slotNum)
{ 
	cout<<"setrrrr\n";
	struct HeadInfo header=getheader(blockNum);
	int numOfSlots=header.numSlots;
	int numAttrs=header.numAttrs;
	cout<<numOfSlots;

	if(slotNum < 0 || slotNum > numOfSlots - 1)
		return E_OUTOFBOUND;
	cout<<"hereee!!!\n";
	FILE *disk=fopen("disk","rb+");
	int BlockType=getBlockType(blockNum);
	cout<<BlockType<<endl;
	if(BlockType==REC)
	{
		cout<<"entering setrecord"<<endl;
		/*struct recBlock R;
		fseek(disk,blockNum*BLOCK_SIZE,SEEK_SET);
		fread(&R,BLOCK_SIZE,1,disk);
		int numAttrs=R.numAttrs;
		int numSlots=R.numSlots;*/

		fseek(disk,blockNum*BLOCK_SIZE+32+numOfSlots+slotNum*numAttrs*ATTR_SIZE,SEEK_SET);
		fwrite(rec,numAttrs*ATTR_SIZE,1,disk);
		fclose(disk);
		return SUCCESS;

	}
	else if(BlockType==IND_INTERNAL)
	{
		//to be done

	}	
	else if(BlockType==IND_LEAF)
	{
		//to be done
	}
	else
	{
		fclose(disk);
		return FAILURE;
	}
	/*unsigned char* bufferPtr = getBufferPtr();
	int numOfAttrib=((int32_t) (bufferPtr + 5*4));
	int numOfSlots=((int32_t) (bufferPtr + 6*4));
	if(slotNum < 0 || slotNum > numOfSlots - 1)
		return E_OUTOFBOUND;  
	unsigned char *slotMap = new unsigned char;
	getSlotMap(slotMap);
	if((int32_t*)(slotMap + slotNum) == 0)
	    return E_FREESLOT;
	memcpy((void*)(bufferPtr + 32 + numOfSlots + (slotNum*numOfAttrib)ATTR_SIZE), (void)rec, numOfAttrib*ATTR_SIZE);
	return SUCCESS;*/
}




int getRelCatEntry(int rel_id, union Attribute *relcat_entry)
{
	if(rel_id< 0 || rel_id>= MAXOPEN)
		return E_OUTOFBOUND;
	if(OpenRelTable[rel_id]==NULL)
		return E_NOTOPEN;
    char rel_name[16];
	strcpy(rel_name, OpenRelTable[rel_id]);
	for(int i=0;i<20;i++)
	{
		getRecord(relcat_entry,4,i);
		if(strcmp(relcat_entry[0].sval,rel_name)==0)
			return SUCCESS;
			
	}
	return FAILURE;
	
}

 int setRelCatEntry(int rel_id,union Attribute * relcat_entry)
{
	if(rel_id< 0 || rel_id>= MAXOPEN)
		return E_OUTOFBOUND;
	if(OpenRelTable[rel_id]==NULL)
		return E_NOTOPEN;
	char rel_name[16];
	strcpy(rel_name, OpenRelTable[rel_id]);
	for(int i=0;i<20;i++)
	{
		getRecord(relcat_entry,4,i);
		if(strcmp(relcat_entry[0].sval,rel_name)==0)
		{
			setRecord(relcat_entry,4,i);
			return SUCCESS;
		}	
			
	}
	
}

int getAttrCatEntry(int rel_id, char attrname[16],union Attribute *attrcat_entry)
{

	if(rel_id< 0 || rel_id>= MAXOPEN)
		return E_OUTOFBOUND;
	if(OpenRelTable[rel_id]==NULL)
		return E_NOTOPEN;
    char rel_name[16];
	strcpy(rel_name, OpenRelTable[rel_id]);
	int curr_block=5;
	int next_block=-1;
	while(curr_block!=-1)
	{
		struct HeadInfo header;
		header= getheader(curr_block);
		next_block=header.rblock;
		for(int i=0;i<20;i++)
		{
			getRecord(attrcat_entry,curr_block,i);
			if(strcmp(attrcat_entry[0].sval,rel_name)==0)
			{
				if(strcmp(attrcat_entry[1].sval,attrname)==0)
					return SUCCESS;
			}
		}
		curr_block=next_block;
	}
	
	return E_ATTRNOTEXIST;
	
}




int ba_insert(int relid, union Attribute *rec)
{
	union Attribute relcat_entry[6];
	union Attribute attrcat_entry[6];
	/*int first_block;
	int num_attrs;
	int num_slots;
	int root_block;
	recId recid;
	int iter;*/
	//RecBuffer *rec_buffer;
	struct HeadInfo header;
	
	getRelCatEntry(relid, relcat_entry);
	int first_block = relcat_entry[3].ival;
	int num_slots = relcat_entry[5].ival;
	int num_attrs = relcat_entry[1].ival;
	
	unsigned char slotmap[num_slots];
	//getting free slot
	recId recid = getFreeSlot(first_block);
	if(recid.block == -1 && recid.slot == -1)
	{ //free slot can not be found
		return FAILURE;
	}
	//rec_buffer = Buffer::getRecBuffer(recid.block);
	//header = rec_buffer->getheader();
	//inserting record in the free slot given by getFreSlot
	//rec_buffer->setRecord(rec, recid.slot);
	setRecord(rec,recid.block, recid.slot);
	//since record is inserted number of entries is increased by 1
	header= getheader(recid.block);  //arg
	header.numEntries = header.numEntries + 1; // increased number of entires in block
	setheader(&header,recid.block); //arg
	//rec_buffer->setheader();
	//settting corresponding slotmap entry as used
	getSlotmap(slotmap,recid.block);
	slotmap[recid.slot] = '1';
	setSlotmap(slotmap,num_slots,recid.block);
	//delete rec_buffer;
	//increasing number of entries in relation catalog entry
	relcat_entry[2].ival = relcat_entry[2].ival + 1;
	setRelCatEntry(relid, relcat_entry);
	
	//inserting entries into index block if exits for attributes
	/*for(iter = 0; iter < num_attrs; iter++){
		OpenRelTable::getAttrCatEntry(relid, iter, &attrcat_entry);
		if(attrcat_entry.root_block != -1){ //if index presents for the attribute
			bplus_insert(relid, attrcat_entry.attr_name, rec[iter], recid); //inserting bplus tree
			/* WRITE FAILURE CONDITION */

	
	return SUCCESS;
}

int compare(union Attribute attr1, union Attribute attr2, int attrType)
{
    if (attrType == STRING)
       return strcmp(attr1.sval, attr2.sval); 

    /* else if attrType == INT -1, 0, 1 depending on whether the value attr1.ival
       is less than, equal to or greater than the value attr2.ival */ 
    
    /* else if attrType==FLOAT return -1, 0, 1 depending on whether the value attr1.fval
       is less than, equal to or greater than the value attr2.fval */ 

    if (attrType == INT)
    {
    	if(attr1.ival<attr2.ival)
    		return -1;
    	else if(attr1.ival==attr2.ival)
    		return 0;
    	else
    		return 1;
    }

    if(attrType==FLOAT)
    {
    	if(attr1.fval<attr2.fval)
    		return -1;
    	else if(attr1.fval==attr2.fval)
    		return 0;
    	else
    		return 1;
    }
    
}

recId linear_search(relId relid, char attrName[ATTR_SIZE], union Attribute attrval, int op, recId * prev_recid)
{
	//get the record corresponding to the relation name
	union Attribute relcat_entry[6];
	getRelCatEntry(relid,relcat_entry);
	
	//get the record itself in relcat_entry array of attributes
	int curr_block,curr_slot,next_block=-1;
	int no_of_attributes=relcat_entry[1].ival;
	int no_of_slots=relcat_entry[5].ival;
	union Attribute attrcat_entry[6];
	getAttrCatEntry(relid,attrName,attrcat_entry);
	int offset= attrcat_entry[5].ival;
	int attr_type=attrcat_entry[2].ival;
	recId ret_recid;

	if((prev_recid->block==-1)&& prev_recid->slot==-1)
	{
		curr_block = relcat_entry[3].ival;
		curr_slot = 0;
	}
	
	else
	{ 
		//if the linear search knows the  hit from previous search
		 curr_block = prev_recid->block;
		 curr_slot = prev_recid->slot;
	}

	while(curr_block!=-1)
	{
		int i=curr_slot;
		struct HeadInfo header;
		header= getheader(curr_block);
		next_block=header.rblock;
		for(;i<no_of_slots;i++)
		{
			union Attribute record[no_of_attributes];
			int retval=getRecord(record,curr_block,i);
			if(retval==E_FREESLOT)
			{
				continue;
			}
			int flag=compare(attrval,record[offset],attr_type);
			bool cond=false;
			switch(op)
			{
				case NE: 
							if(flag != 0)
								cond=true;
							break;
				case LT: 
							if(flag < 0)
								cond=true;
							break;
				case LE: 
							if(flag <= 0)
								cond=true;
							break;
				case EQ: 
							if(flag == 0)
								cond=true;
							break;
				case GT: 
							if(flag > 0)
								cond=true;
							break;
				case GE: 
							if(flag >= 0)
								cond=true;
			}
			if(cond==true)
			{
				//record id of the record that satisfies the given condition	
				ret_recid = {curr_block, i}; 
				//set the prev_recid 		
				*prev_recid= ret_recid;
				return ret_recid;
			}
		}
		curr_block=next_block;
		curr_slot=0;
	}
	
	return {-1, -1}; //i.e., no record in the relation with Id relid satisfies the given condition
}		

int ba_renamerel(char oldName[ATTR_SIZE], char newName[ATTR_SIZE])
{
	//search for the relation with name newName in relation catalog 
	recId prev_recid={-1,-1};
	recId relcat_recid, attrcat_recid;
	union Attribute temp;
	strcpy(temp.sval,newName);
	relcat_recid = linear_search(RELCAT_RELID, "RelName",temp, EQ,&prev_recid);
	//If relation with name newName already exits
	if(!((relcat_recid.block==-1)&&(relcat_recid.slot==-1)))
		return E_RELEXIST;
	
	
	//search for the relation with name oldName in relation catalog 
	prev_recid={-1,-1};
	strcpy(temp.sval,oldName);
	relcat_recid = linear_search(RELCAT_RELID, "RelName",temp, EQ, &prev_recid);
	//If relation with name relName does not exits
	if((relcat_recid.block==-1)&&(relcat_recid.slot==-1))
		return E_RELNOTEXIST;
	
	//get the relation catalog record from the relation catalog (recid of the relation catalog record = relcat_recid)
	union Attribute relcat_record[6];
	getRecord(relcat_record,relcat_recid.block,relcat_recid.slot);
	strcpy(relcat_record[0].sval,newName);
	
	//update the relation catalog record in the relation catalog with relation name newName
	setRecord(relcat_record, relcat_recid.block,relcat_recid.slot);

	/*update all the attribute catalog entries in the attribute catalog corresponding to the 
	  relation with relation name oldName to the relation name newName*/
	prev_recid={-1,-1};
	while(1)
	{
		attrcat_recid = linear_search(ATTRCAT_RELID, "RelName", temp, EQ,&prev_recid);
		if(!((attrcat_recid.block==-1)&&(attrcat_recid.slot==-1)))
		{
			union Attribute attrcat_record[6];
			getRecord(attrcat_record,attrcat_recid.block,attrcat_recid.slot);
			if(strcmp(attrcat_record[0].sval,oldName)==0)
			{
				strcpy(attrcat_record[0].sval,newName);
				setRecord(attrcat_record,attrcat_recid.block,attrcat_recid.slot);
			}
		}
		else
			break;
	}
	
	return SUCCESS;
}

int ba_renameattr(char relName[ATTR_SIZE], char oldName[ATTR_SIZE], char newName[ATTR_SIZE])
{
	//search for the relation with name relName in relation catalog 
	recId prev_recid={-1,-1};
	recId relcat_recid,attrcat_recid;
	union Attribute attrcat_record[6];
	union Attribute temp;
	strcpy(temp.sval,relName);
	relcat_recid = linear_search(RELCAT_RELID, "RelName",temp, EQ,&prev_recid);
	
	if((relcat_recid.block==-1) && (relcat_recid.slot==-1))
	{ //If relation with name relName does not exits
		return E_RELNOTEXIST;
	}
	
	//Iterate over all the attributes corresponding to the relation{
		//search for the attributes with relation name relName in attribute catalog 
	prev_recid={-1,-1};
	 while(1)
	{
	 	attrcat_recid = linear_search(ATTRCAT_RELID, "RelName",temp, EQ,&prev_recid);
	 	if(!((attrcat_recid.block==-1) && (attrcat_recid.slot==-1)))
	 	{
	 		getRecord(attrcat_record,attrcat_recid.block,attrcat_recid.slot);
	   		if(strcmp(attrcat_record[1].sval,newName)==0)
	  			return E_ATTREXIST;
	 	}
	   	else
	   		break;

    }
	
	//Iterate over all the attributes corresponding to the relation{
	//search for the attributes with relation name relName in attribute catalog 
     
    prev_recid={-1,-1};
	 while(1)
	{
	 	attrcat_recid = linear_search(ATTRCAT_RELID, "RelName",temp, EQ, &prev_recid);
	 	if(!((attrcat_recid.block==-1) && (attrcat_recid.slot==-1)))
	 	{
	 		getRecord(attrcat_record,attrcat_recid.block,attrcat_recid.slot);
	   		if(strcmp(attrcat_record[1].sval,oldName)==0)
	  		{
	  			strcpy(attrcat_record[1].sval,newName);
	  			setRecord(attrcat_record,attrcat_recid.block,attrcat_recid.slot);
	  			return SUCCESS;
	  		}

	 	}
	   	else
	  		return E_ATTRNOTEXIST;

    }
}	


//Todo: updating last block of attribute catalog
int ba_delete(char relName[ATTR_SIZE])
{
	/* search for relation with name relName in relation catalog */
	recId relcat_recid,prev_recid={-1,-1};
	union Attribute temp;
	strcpy(temp.sval,relName);
	relcat_recid = linear_search(RELCAT_RELID, "RelName",temp, EQ,&prev_recid);
	
	if((relcat_recid.block==-1) && (relcat_recid.slot==-1))
	{ //If relation with relName does not exits
		return E_RELNOTEXIST;
	}
	FILE *disk;
	
	union Attribute relcat_rec[6];
	/*getting the relation catalog entry corresponding to relation with relName*/
	getRecord(relcat_rec, relcat_recid.block,relcat_recid.slot);

	//get the first record block of the relation (first_block)
	int curr_block= relcat_rec[3].ival;
	int no_of_slots=relcat_rec[5].ival;
	int no_of_attrs=relcat_rec[1].ival;
	int next_block=-1;

	//get the number of attributes corresponding to the relation (num_attrs)
	int no_of_attributes = relcat_rec[1].ival;

    //Delete all the record blocks of the relation by getting the next record blocks (rblock) from header and by calling
	while(curr_block!=-1)
	{
		struct HeadInfo header=getheader(curr_block);
		next_block=header.rblock;
		deleteBlock(curr_block);
		curr_block=next_block;
	}
	union Attribute attrcat_rec[6]; recId attrcat_recid;
	prev_recid={-1,-1};unsigned char slotmap[no_of_slots];int root_block;
	for(int i=0;i<no_of_attrs;i++)
	{
		
		attrcat_recid= linear_search(ATTRCAT_RELID, "RelName",temp, EQ,&prev_recid);
		disk=fopen("disk","rb+");
		fseek(disk,(attrcat_recid.block)*BLOCK_SIZE+32+no_of_slots+attrcat_recid.slot*no_of_attrs*ATTR_SIZE,SEEK_SET);
		unsigned char ch=0;
		fwrite(&ch,16*no_of_attrs,1,disk);
		fclose(disk);
		struct HeadInfo header=getheader(attrcat_recid.block);
		getSlotmap(slotmap,attrcat_recid.block);
		slotmap[attrcat_recid.slot]='0';
		setSlotmap(slotmap,no_of_slots,attrcat_recid.block);
		header.numEntries=header.numEntries-1;
		
		root_block=attrcat_rec[4].ival;
		if(root_block != -1)//index exists for the attribute
			//bplus_destroy(root_block); //delete the index blocks corresponding to the attribute	
		if(header.numEntries==0)
		{
			struct HeadInfo prev_header=getheader(header.lblock);
			prev_header.rblock=-1;
		}


		setheader(&header,attrcat_recid.block);	
	}
	
	
	//delete the relation catalog entry corresponding to the relation from relation catalog
	//Adjusting the number of entries in the block (decrease by 1) corresponding to the relation catalog entry
	
	//update relation catalog (i.e number of records in relation catalog is decreased by 1)
	//update attribute catalog (i.e number of records in attribute catalog is decreased by num_attrs)
	
	unsigned char relcat_slotmap[20];
	getSlotmap(relcat_slotmap,RELCAT_RELID);
	relcat_slotmap[relcat_recid.slot]='0';
	setSlotmap(relcat_slotmap,20,relcat_recid.block);
	struct HeadInfo relcat_header=getheader(RELCAT_RELID);
	relcat_header.numEntries=relcat_header.numEntries-1;
	setheader(&relcat_header,4);

	getRecord(relcat_rec,RELCAT_RELID,0);
	relcat_rec[2].ival=relcat_rec[2].ival-1;
	setRecord(relcat_rec,RELCAT_RELID,0);
	getRecord(relcat_rec,RELCAT_RELID,1);
	relcat_rec[1].ival=relcat_rec[1].ival-no_of_attrs;
	setRecord(relcat_rec,RELCAT_RELID,1);
	disk=fopen("disk","rb+");
	fseek(disk,(attrcat_recid.block)*BLOCK_SIZE+32+no_of_slots+attrcat_recid.slot*no_of_attrs*ATTR_SIZE,SEEK_SET);
	unsigned char ch=0;
	fwrite(&ch,16*no_of_attrs,1,disk);
	fclose(disk);
	return SUCCESS;
}	 


int ba_search(relId relid, union Attribute *record, char attrName[ATTR_SIZE], union Attribute attrval, int op)
{
	
	/*get the attribute catalog entry from the attribute cache corresponding 
	  to the relation with Id=relid and with attribute_name=attrName using
	  OpenRelTable::getAttrCatEntry(relid, attrName, &attrcat_entry); of cache layer */
	//get root_block from the attribute catalog entry (attrcat_entry)
	union Attribute attrcat_entry[6];
	getAttrCatEntry(relid,attrName,attrcat_entry);
	int root_block=attrcat_entry[4].ival;
	recId recid;
	static recId prev_recid={-1,-1};
	if(root_block == -1)
	{ 	//if Index does not exist for the attribute
		//search for the record id (recid) correspoding to the attribute with attribute name attrName and with value attrval  
		recid = linear_search(relid, attrName, attrval, op,&prev_recid);
	}
	else
	{ //if Index exists for the attribute
	  //search for the record id (recid) correspoding to the attribute with attribute name attrName and with value attrval
		//recid = bplus_search(relid, attrName, attrval, op,&prev_recid);
	}
	
	if((recid.block==-1) && (recid.slot==-1))
	{ //if it fails to find a record satisfying the given condition
		return FAILURE;
	}
	
	 //recid.block is the block that contains record
	getRecord(record, recid.block,recid.slot); //recid.slot is the slot that contains record

	return SUCCESS;
}


void meta()
{

	cout<<"Entered meta"<<endl;
	union Attribute rec[6];
	struct HeadInfo *H=(struct HeadInfo*)malloc(sizeof(struct HeadInfo));
	//unsigned char *slot_map=(unsigned char*)malloc(sizeof(slot_map));
    
	H->blockType=REC;
	H->pblock=-1;
	H->lblock=-1;
	H->rblock=-1;
	H->numEntries=2;
	H->numAttrs=6; 
	H->numSlots=20;
	setheader(H,4);
	cout<<"wrote header\n";
	struct HeadInfo h= getheader(4);
	cout<<h.numSlots<<endl;

	unsigned char slot_map[20];
	for(int i=0;i<20;i++)
	{
		if(i==0||i==1)
			slot_map[i]='1';
		else
			slot_map[i]='0';
	}
	setSlotmap(slot_map,20,4);
	cout<<"wrote slotmap\n";
	unsigned char temp[20];
	/*FILE * disk=fopen("disk","rb+");
	fseek(disk,4*2048+32,SEEK_SET);
	fread(temp,20,1,disk);
	for(int i=0;i<20;i++)
		if(temp[i]=='1')
			cout<<"hi!!";
	cout<<endl;*/
	getSlotmap(temp,4);
	for(int i=0;i<20;i++)
		cout<<temp[i];
	cout<<endl;
	h= getheader(4);
	cout<<h.numSlots<<endl;
	/*fseek(disk,4*2048,SEEK_SET);
	fread(&h,32,1,disk);
	cout<<h.numSlots<<endl;*/

	strcpy(rec[0].sval,"RELATIONCAT");
          rec[1].ival=6;
	rec[2].ival=2;
	rec[3].ival=4;
	rec[4].ival=4;
	rec[5].ival=20;
	setRecord(rec,4,0);
	h= getheader(4);
	cout<<h.numSlots<<endl;
	
	strcpy(rec[0].sval,"ATTRIBUTECAT");
          rec[1].ival=6;
	rec[2].ival=12;
	rec[3].ival=5;
	rec[4].ival=5;
	rec[5].ival=20;
	setRecord(rec,4,1);
	//cout<<"setrecord done\n";
	H->blockType=REC;
	H->pblock=-1;
	H->lblock=-1;
	H->rblock=-1;
	H->numEntries=12;
	H->numAttrs=6; 
	H->numSlots=20;
	setheader(H,5);
	//cout<<"set header\n";
	

	for(int i=0;i<20;i++)
	{
		if(i>=0&&i<=11)
			slot_map[i]='1';
		else
			slot_map[i]='0';
	}
	setSlotmap(slot_map,20,5);
	//cout<<"wrote slotmap\n";
	
    strcpy(rec[0].sval,"RELATIONCAT");
    strcpy(rec[1].sval,"RelName");
	strcpy(rec[2].sval,"STRING");
	rec[3].ival=-1;
	rec[4].ival=-1;
	rec[5].ival=0;
	setRecord(rec,5,0);
	//cout<<"set record done\n";
         // getRecord(rec1,5,0);
         // cout<<rec1[0].sval;     

	strcpy(rec[0].sval,"RELATIONCAT");
          strcpy(rec[1].sval,"#Attributes");
	rec[2].ival=INT;
	rec[3].ival=-1;
	rec[4].ival=-1;
	rec[5].ival=1;
	setRecord(rec,5,1);
	//cout<<"set record done\n";

	strcpy(rec[0].sval,"RELATIONCAT");
          strcpy(rec[1].sval,"#Records");
	rec[2].ival=INT;
	rec[3].ival=-1;
	rec[4].ival=-1;
	rec[5].ival=2;
	setRecord(rec,5,2);
	//cout<<"set record done\n";
	
	strcpy(rec[0].sval,"RELATIONCAT");
          strcpy(rec[1].sval,"FirstBlock");
	rec[2].ival=INT;
	rec[3].ival=-1;
	rec[4].ival=-1;
	rec[5].ival=3;
	setRecord(rec,5,3);
	//cout<<"set record done\n";
	
	strcpy(rec[0].sval,"RELATIONCAT");
          strcpy(rec[1].sval,"LastBlock");
	rec[2].ival=INT;
	rec[3].ival=-1;
	rec[4].ival=-1;
	rec[5].ival=4;
	setRecord(rec,5,4);
	//cout<<"set record done\n";
	
          strcpy(rec[0].sval,"RELATIONCAT");
          strcpy(rec[1].sval,"#Slots");
	rec[2].ival=INT;
	rec[3].ival=-1;
	rec[4].ival=-1;
	rec[5].ival=5;
	setRecord(rec,5,5);
	//cout<<"set record done\n";
	
	strcpy(rec[0].sval,"ATTRIBUTECAT");
          strcpy(rec[1].sval,"RelName");
	strcpy(rec[2].sval,"STRING");
	rec[3].ival=-1;
	rec[4].ival=-1;
	rec[5].ival=0;
	setRecord(rec,5,6);
	//cout<<"set record done\n";
	
	strcpy(rec[0].sval,"ATTRIBUTECAT");
          strcpy(rec[1].sval,"AttributeName");
	strcpy(rec[2].sval,"STRING");
	rec[3].ival=-1;
	rec[4].ival=-1;
	rec[5].ival=1;
	setRecord(rec,5,7);
	//cout<<"set record done\n";

	strcpy(rec[0].sval,"ATTRIBUTECAT");
          strcpy(rec[1].sval,"AttributeType");
	rec[2].ival=INT;
	rec[3].ival=-1;
	rec[4].ival=-1;
	rec[5].ival=2;
	setRecord(rec,5,8);
	//cout<<"set record done\n";


	strcpy(rec[0].sval,"ATTRIBUTECAT");
          strcpy(rec[1].sval,"PrimaryFlag");
	rec[2].ival=INT;
	rec[3].ival=-1;
	rec[4].ival=-1;
	rec[5].ival=3;
	setRecord(rec,5,9);
	//cout<<"set record done\n";
	
	strcpy(rec[0].sval,"ATTRIBUTECAT");
          strcpy(rec[1].sval,"RootBlock");
	rec[2].ival=INT;
	rec[3].ival=-1;
	rec[4].ival=-1;
	rec[5].ival=4;
	setRecord(rec,5,10);
	//cout<<"set record done\n";

	strcpy(rec[0].sval,"ATTRIBUTECAT");
          strcpy(rec[1].sval,"Offset");
	rec[2].ival=INT;
	rec[3].ival=-1;
	rec[4].ival=-1;
	rec[5].ival=5;
	setRecord(rec,5,11);
	//cout<<"set record done\n
	union Attribute temprec[6];
	getRecord(temprec,4,0);
	cout<<"jhhj";
	cout<<temprec[0].sval<<endl;
	
	
}
/*int main
{
	return 0;
}*/
