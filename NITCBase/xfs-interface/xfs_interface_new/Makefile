CC = g++
CFLAGS = -g -w

default: xfs-interface

xfs-interface: *.cpp *.h
	$(CC) $(CFLAGS) imp-exp.cpp -o xfs-interface 

clean:
	$(RM) xfs-interface *.o disk block_allocation_map attribute_catalog relation_catalog
 

