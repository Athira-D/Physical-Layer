struct Headinfo getheader(int blockNum)
{
	int BlockType=getBlockType(blockNum);
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
	disk=fopen("disk","wb+");
	fseek(disk,BLOCK_SIZE*curr_block,SEEK_SET);
	fwrite(0,BLOCK_SIZE,1,disk);
	fclose(disk);
}

void setheader(struct Headinfo header,int blockNum)
{
	FILE *disk=fopen("disk","wb+");
	fseek(disk,blockNum*BLOCK_SIZE,SEEK_SET);
	fwrite(&header,32,1,disk);
	fclose(disk);
}



void getSlotmap(unsigned char * SlotMap,int blockNum)
{
	FILE *disk=fopen("disk","rb");
	fseek(disk,blockNum*2048,SEEK_SET);
	struct recBlock R;
	fread(&R,2048,1,disk);
	int numSlots=R.numSlots;
	memcpy(SlotMap,R.Slots_Records,numSlots);
	fclose(disk);
}

void setSlotmap(unsigned char * SlotMap,int no_of_slots,int blockNum)
{
	FILE *disk=fopen("disk","wb+");
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
	int iter;
	
	// finding free slot
	while(block_num != -1){
		//rec_buffer = Buffer::getRecBuffer(block_num);
		//header = rec_buffer->getheader();
		struct HeadInfo header=getheader(block_num);
		num_slots = header.num_slots;
		next_block_num = header.rblock; // next block
		//getting slotmap entries
		unsigned char slotmap[num_slots];
		getSlotmap(slotmap,block_num);
		//searching for free slot in block (block_num)
		for(iter = 0; iter <  num_slots; iter++){
			if(slotmap[iter] == 0){
				break;
			}
		}
		if(iter < num_slots){
			recid = {block_num, iter};
			return recid;
		}
		prev_block_num = block_num;
		block_num = next_block_num;
	}
	
	//no free slot is found in existing records if block_num = -1
	int block_num =getFreeRecBlock();
	if(block_num ==-1){
		// no blocks are available in disk
		return recid; // no free slot can be found
	}
	//block_num = rec_buffer->getBlockNum();
	//setting header values
	header = getheader(block_num);
	header.lblock = prev_block_num;
	header.num_slots = num_slots;
	setheader(header,block_num);
	//setting slotmap
	unsigned char slotmap[num_slots];
	getSlotmap(slotmap,block_num);
	memset(slotmap, 0, sizeof(slotmap)); //all slots are free
	setSlotmap(slotmap,block_num);
	delete rec_buffer;
	//recid
	recid = {block_num, 0};
	//setting prev_block_num rblock to new block
	getheader(prev_block_num);
	header.rblock = block_num;
	setheader(block_num,header);
	return recid;
}

int getRelCatEntry(int rel_id, RelCatEntry *relcat_entry)
{
	if(rel_id< 0 || rel_id>= MAXOPEN)
		return E_OUTOFBOUND;
	if(OpenRelTable[rel_id]==NULL)
		return E_NOTOPEN;
    char rel_name[16];
	strcpy(rel_name, rel_table[rel_id]);
	int (i=0;i<20;i++)
	{
		getRecord(relcat_entry,4,i);
		if(strcmp(relcat_entry.rel_name,rel_name)==0)
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
	strcpy(rel_name, rel_table[rel_id]);
	int (i=0;i<20;i++)
	{
		getRecord(relcat_entry,4,i);
		if(strcmp(relcat_entry.rel_name,rel_name)==0)
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
		getHeader(curr_block,&header);
		next_block=header.r_block;
		int (i=0;i<20;i++)
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



int getRecord(union Attribute *rec,int blockNum,int slotNum)
{ 
	if(slotNum < 0 || slotNum > numOfSlots - 1)
		return E_OUTOFBOUND;
	FILE *disk=fopen("disk","rb");
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
		memcpy((void)rec,(void*)(R.slotMap_Records+numSlots+(slotNum*numOfAttrs*ATTR_SIZE)), numOfAttrs*16);
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
	if(slotNum < 0 || slotNum > numOfSlots - 1)
		return E_OUTOFBOUND;
	FILE *disk=fopen("disk","wb+");
	int BlockType=getBlockType(blockNum);
	if(BlockType==REC)
	{
		struct recBlock R;
		fseek(disk,blockNum*BLOCK_SIZE,SEEK_SET);
		fread(&R,BLOCK_SIZE,1,disk);
		int numAttrs=R.numAttrs;
		int numSlots=R.numSlots;
		fseek(disk,blockNum*BLOCK_SIZE+slotNum*numAttrs,SEEK_SET);
		fwrite(&rec,numAttrs*ATTR_SIZE,1,disk);
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




int ba_insert(int relid, union Attribute *rec){
	RelCatEntry relcat_entry;
	AttrCatEntry attrcat_entry;
	int first_block;
	int num_attrs;
	int num_slots;
	int root_block;
	recId recid;
	int iter;
	//RecBuffer *rec_buffer;
	struct HeadInfo header;
	
	getRelCatEntry(relid, &relcat_entry);
	first_block = relcat_entry.first_blk;
	num_slots = relcat_entry.num_slots_blk;
	num_attrs = relcat_entry.num_attr;
	
	unsigned char slotmap[num_slots];
	//getting free slot
	recid = getFreeSlot(first_block);
	if(recid.block == -1 && recid.slot == -1){ //free slot can not be found
		return FAILURE;
	}
	//rec_buffer = Buffer::getRecBuffer(recid.block);
	//header = rec_buffer->getheader();
	//inserting record in the free slot given by getFreSlot
	//rec_buffer->setRecord(rec, recid.slot);
	setRecord(rec,recid.block, recid.slot);
	//since record is inserted number of entries is increased by 1
	getheader();  //arg
	header.num_entries = header.num_entries + 1; // increased number of entires in block
	setheader(); //arg
	//rec_buffer->setheader();
	//settting corresponding slotmap entry as used
	getSlotmap(slotmap);
	slotmap[recid.slot] = '1';
	setSlotmap(slotmap);
	//delete rec_buffer;
	//increasing number of entries in relation catalog entry
	relcat_entry.num_entries = relcat_entry.num_entries + 1;
	setRelCatEntry(relid, &relcat_entry);
	
	//inserting entries into index block if exits for attributes
	/*for(iter = 0; iter < num_attrs; iter++){
		OpenRelTable::getAttrCatEntry(relid, iter, &attrcat_entry);
		if(attrcat_entry.root_block != -1){ //if index presents for the attribute
			bplus_insert(relid, attrcat_entry.attr_name, rec[iter], recid); //inserting bplus tree
			/* WRITE FAILURE CONDITION */

	
	return SUCCESS;
}


recId linear_search(relId relid, char attrName[ATTR_SIZE], union Attribute attrval, int op, recId * prev_recid)
{
	//get the record corresponding to the relation name
	union Attribute relcat_entry[6];
	getRelCatEntry(RELCAT_RELID,relcat_entry);
	
	//get the record itself in relcat_entry array of attributes
	int curr_block,curr_slot,next_block=-1;
	int no_of_attributes=relcat_entry[1].ival;
	int no_of_slots=relcat_entry[5].ival;
	union Attribute attrcat_entry[6];
	getAttrCatEntry(ATTRCAT_RELID,attrName,attrcat_entry);
	int offset= attrcat_entry[5].ival;
	int attr_type=attrcat_entry[2].ival;
	recId ret_recid;

	if(prev_recid == {-1, -1})
	{
		curr_block = relcat_entry[3].ival;
		curr_slot = 0;
	}
	
	else
	{ 
		//if the linear search knows the  hit from previous search
		 curr_block = prev_recid.block;
		 curr_slot = prev_recid.slot;
	}

	while(curr_block!=-1)
	{
		int i=curr_slot;
		struct HeadInfo header;
		getHeader(curr_block,&header);
		next_block=header.r_block;
		for(;i<no_of_slots;i++)
		{
			union Attribute record[no_of_attributes];
			int retval=getRecord(record,curr_block,i);
			if(retval==E_FREESLOT)
			{
				curr_slot++;
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
	relcat_recid = linear_search(RELCAT_RELID, "RelName", newName, EQ,&prev_recid);
	//If relation with name newName already exits
	if(relcat_recid != {-1,-1})
		return E_RELEXIST;
	
	
	//search for the relation with name oldName in relation catalog 
	prev_recid={-1,-1};
	relcat_recid = linear_search(RELCAT_RELID, "RelName", oldName, EQ, &prev_recid);
	//If relation with name relName does not exits
	if(relcat_recid == {-1,-1})
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
		attrcat_recid = linear_search(ATTRCAT_RELID, "RelName", oldName, EQ,&prev_recid);
		if(attrcat_recid!={-1,-1})
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
	union Attribute relcat_recid,attrcat_recid;
	relcat_recid = linear_search(RELCAT_RELID, "RelName", relName, EQ,&prev_recid);
	
	if(relcat_recid == {-1,-1}){ //If relation with name relName does not exits
		return E_RELNOTEXIST;
	}
	
	//Iterate over all the attributes corresponding to the relation{
		//search for the attributes with relation name relName in attribute catalog 
	prev_recid={-1,-1};
	 while(1)
	{
	 	attrcat_recid = linear_search(ATTRCAT_RELID, "RelName", relName, EQ,&prev_recid);
	 	if(attrcat_recid!={-1,-1})
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
	 	attrcat_recid = linear_search(ATTRCAT_RELID, "RelName", relName, EQ, &prev_recid);
	 	if(attrcat_recid!={-1,-1})
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
	relcat_recid = linear_search(RELCAT_RELID, "RelName", relName, EQ,&prev_recid);
	
	if(relcat_recid == {-1,-1})
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
		struct HeadInfo header=getHeader(curr_block);
		next_block=header.rblock;
		deleteBlock(curr_block);
		curr_block=next_block;
	}
	union Attribute attrcat_rec[6]; recId attrcat_recid;
	prev_recid={-1,-1};unsigned char slotmap[no_of_slots];int root_block;
	for(int i=0;i<num_of_attrs;i++)
	{
		attrcat_recid= linear_search(ATTRCAT_RELID, "RelName", relName, EQ,&prev_recid);
		disk=fopen("disk","wb+");
		fseek(disk,attrcat_recid*BLOCK_SIZE+32+no_of_slots+attrcat_recid.slot*no_of_attrs*ATTR_SIZE,SEEK_SET);
		fwrite(0,16*no_of_attrs,1,disk);
		fclose(disk);
		struct HeadInfo header=getHeader(attrcat_recid.block);
		getSlotmap(slotmap,attrcat_recid.block);
		slotmap[attrcat_recid.slot]='0';
		setSlotmap(slotmap,attrcat_recid.block);
		header.numEntries=header.numEntries-1;
		
		root_block=attrcat_rec[4].ival;
		if(root_block != -1)//index exists for the attribute
			bplus_destroy(root_block); //delete the index blocks corresponding to the attribute	
		if(header.numEntries==0)
		{
			struct HeadInfo prev_header=getHeader(header.lblock);
			prev_header.rblock=-1;
		}


		setHeader(attrcat_recid.block,&header);	
	}
	
	
	//delete the relation catalog entry corresponding to the relation from relation catalog
	//Adjusting the number of entries in the block (decrease by 1) corresponding to the relation catalog entry
	
	//update relation catalog (i.e number of records in relation catalog is decreased by 1)
	//update attribute catalog (i.e number of records in attribute catalog is decreased by num_attrs)
	
	unsigned char relcat_slotmap[20];
	getSlotmap(relcat_slotmap,RELCAT_RELID);
	relcat_slotmap[relcat_recid.slot]='0';
	setSlotmap(relcat_slotmap,relcat_recid.block);
	struct HeadInfo relcat_header=getHeader(RELCAT_RELID);
	relcat_header.numEntries=relcat_header.numEntries-1;
	setHeader(RELCAT_RELID,&relcat_header);

	getRecord(relcat_rec,RELCAT_RELID,0);
	relcat_rec[2]=relcat_rec[2]-1;
	setRecord(relcat_rec,RELCAT_RELID,0);
	getRecord(relcat_rec,RELCAT_RELID,1);
	relcat_rec[1]=relcat_rec[1]-no_of_attrs;
	setRecord(relcat_rec,RELCAT_RELID,1);
	disk=fopen("disk","wb+");
	fseek(disk,attrcat_recid*BLOCK_SIZE+32+no_of_slots+attrcat_recid.slot*no_of_attrs*ATTR_SIZE,SEEK_SET);
	fwrite(0,16*no_of_attrs,1,disk);
	fclose(disk);
	return SUCCESS;
}	 