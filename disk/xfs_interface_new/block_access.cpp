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

 int setRelCatEntry(int rel_id, RelCatEntry *relcat_entry)
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
		}	
	}*/
	
	return SUCCESS;
}
