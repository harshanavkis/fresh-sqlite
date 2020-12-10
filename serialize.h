#ifndef SERIALIZE_H
#define SERIALIZE_H

int buf_len;

typedef struct
{
	int size;
	char *record;
} mem_serial;

int serialize(char **dest, Mem *ssd_record);
int deserialize(Mem **dest, char *record);

#endif /*SERIALIZE_H*/
