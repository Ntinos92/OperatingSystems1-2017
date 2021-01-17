typedef struct arrayElement* feederElement;

typedef struct arrayElement {
	int arrayElem;
	clock_t timestamp;
} arrayElement;


int sem_id=0;
int shm_id=0;
feederElement arrEl=NULL;
int Get_Semaphores(key_t);
void Sem_UP(int , int );
void Sem_DOWN(int , int );
void Init_Semaphores(int );
void Init_and_Attach_Shared_Memory();
void Destroy_Semaphores_Shared_Memory();
