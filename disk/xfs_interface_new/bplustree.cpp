
int bplus_create(relId relid, char AttrName[ATTR_SIZE])
{
	int flag;
	
	//getting attribute catalog entry 
	union Attribute attrcatentry[6];
	flag=getAttrCatEntry(relid, AttrName, attrcatentry);

	// in case attribute does not exist
	if(flag!=SUCCESS)
		return flag;
	
	//Checking already whether an index exists for the atrribute or not
	if(attrcatentry[4].ival != -1)
	{
	 	return SUCCESS; // already index exists for the attribute
	}

	union Attribute relcatentry[6];
	getRelCatEntry(relid, relcatentry);
	
	int data_block, num_attrs;
	data_block = relcatentry[3].ival; //first record block for the relation
	num_attrs = relcatentry[1].ival; //num of attributes for the relation

	 
	// getting the free leaf index as root block
	int root_block=getFreeBlock(IND_LEAF);
	if(root_block==FAILURE)	//Problem in getting free leaf index block
		return E_DISKFULL;
	 	

	attrcatentry[4].ival= = root_block;
	setAttrCatEntry(relid, AttrName, attrcatentry);

	int attroffset, attrtype;
	attroffset = attrcatentry[6].ival;
	attrtype = attrcatentry[1].ival;;
	
	union Attribute record[num_attrs];	//
	
	//inserting index entries for each record in bplus tree
	while(data_block != -1)
	{
		/*int rec_block = Buffer::getRecBlock(data_block); 
		
		if(rec_buffer == NULL){	//problem in getting record buffer corresponding to relation
			//call bplus_destroy
			bplus_destroy(relid, AttrName);
			return FAILURE;
		}*/
	
		struct HeadInfo header; //getting header/preablem of the record block
		header = getheader(data_block);
		
		int num_slots;	//getting slotmap of the record block
		num_slots = header.numSlots;
		unsigned char slotmap[num_slots];
		getSlotmap(slotmap,data_block);
		
		int iter;
		for(iter = 0; iter < num_slots; iter++)
		{
			if(slotmap[iter] == '0')
				continue;
			
			getRecord(record, data_block,iter);	//getting iter th number record from data block
			union Attribute attrval;
			attrval = record[attroffset];	//getting attribute value corresponding to attribute name
			
			recId recid;
			recid.block = data_block;
			recid.slot = iter;
			//call bplus_insert
			bplus_insert(relid, AttrName, attrval, recid);
		}
		//delete rec_buffer;
		data_block = header.rblock; //next data block for the relation
	}
			
	return SUCCESS;
}	

int getInternalEntry(struct InternalEntry *entry,int indexNum)

int bplus_insert(int relid,char attrname[ATTR_SIZE],union Attribute val,recId rec_id)
{
	//get the AttrCatEntry of target attribute
	union Attribute attrcatentry[6];
	getAttrCatEntry(relid,attrname,attrcatentry);
	int root_block=attrcatentry[4].ival;
	struct Index indexval;
    indexval.attrval=val;
    indexval.block=rec_id.block;
    indexval.slot=rec_id.slot;
	//if root_block_num of target attribute is -1, return E_NOINDEX
	if(root_block==-1)
		return E_NOINDEX;

	int attrtype=attrcatentry[2].ival;
	int head=root_block;
	int head_type=getBlockType(head);
	while(header.block_type==IND_INTERNAL)
	{
        int nEntries=header.numEntries;
        int child_head;

        struct InternalEntry elem;
        int getter;
        getter=getInternalEntry(&elem,0);
        
        
        int iter;
        for(iter=0;iter<nEntries;iter++){
            getter=head->getEntry((void *)(&elem),iter);
            int compare_val1=compareAttr(indexval.attrval,elem.attrval,type_attr);
            if(compare_val1<=1){
                child_head=elem.lchild;
                break;
            }

        }
        if(iter==nEntries){
            child_head=elem.rchild;
        }
        
        delete head;
        root_block=child_head;
        class IndBuffer *head=Buffer::getIndBlock(root_block);
        header=head->getheader();
    }


}