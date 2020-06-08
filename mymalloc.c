/*
    My Memory Allocation
    Emirhan Aktürk
    19120205058
    
    External fragmentation en fazla worst fit-te olur.Çunku her seferinde en buyuk blok bulunur ve eger istenilen alana esit degilse split edilir.(Genelde esit olmaz)
    ikinci olarak first fit-te olur.Ilk uygun blok aranir.Genelde aranan ile bulunan blokların size farki worst fit e kiyasla daha az olur.
    En az best fit-te olur.Cunku istenen blok size dan buyuk en kucuk blok aranır.Boylece aranan blok ile bulunan blok size farki minimuma inmis olur.
*/
#include"mymalloc.h"
#include <stdlib.h>
#include <time.h>

Block* BestFit(const unsigned int size){
    Block *p=free_list;
    while(p!=NULL && p->info.size<size){//smallestHole ilklendirmek için ilk uygun olan block aranır
        p=(Block*)(p->next);}
    int minSizeDifference;
    if(p!=NULL && p->info.size>=size){//ilk uygun blokla ilklendir ve size degerine daha yakın var mı diye bak
        Block *smallestHole=p;
        minSizeDifference=(smallestHole->info.size - size);
        int sizeDifference;
        while(p!=NULL && minSizeDifference!=0){//Sizelar esit olmadıgı surece en az size farkı olanı ara
            if(p->info.size>=size){
            sizeDifference=(p->info.size - size);}
            
            if(sizeDifference<minSizeDifference){
                minSizeDifference=sizeDifference;
                smallestHole=p;}
            p=(Block*)(p->next);
        }
        return smallestHole;
    }
    else return NULL;
}

Block* WorstFit(const unsigned int size){
    if(free_list==NULL)
        return NULL;

    Block *p=free_list;
    Block *maxBlock=p;//free listteki en büyük blok
    while(p!=NULL){
        if(maxBlock->info.size<p->info.size)
        {   maxBlock=p;}
        p=(Block*)(p->next);
    }
    if(maxBlock->info.size>=size)//bulunan alan yeterliyse return et
        return maxBlock;
    else return NULL; //yetersizse NULL return et
}

Block* FirstFit(const unsigned int size){
    Block *p=free_list;//free_list içinde uygun alanı ara
    while((p!=NULL) &&(p->info.size<size)){ //firstFit
    //heap sonuna gelmedigin sürece,Block bossa veya size istenenden kucukse ilerle
        p=(Block*)(p->next);
    }
    return p;
}

void printFreeList(){
    if(free_list==NULL)
        {printf("no free block\n");
        return;}
    Block*itr=free_list;
    printf("free list:\n");
    while (itr!=NULL)
    {   
        printf("free Block:size:%d,isfree:%d,adress:%p\n",itr->info.size,itr->info.isfree,itr);
        itr=(Block*)(itr->next);
    }
}

int test(int fitStrategy){
    strategy=fitStrategy;
    srand(time(NULL));
    int* arr[5];
    int i,index;
    for(i=0;i<5;i++)
    {arr[i]=NULL;}
    int size;
    for(i=0;i<10;i++){
        index=rand()%5;
        if(arr[index]==NULL){//if Pointer is null, allocate memory
            size=rand()%1024+1;//Generates random values ​​between 1 and 1024
            arr[index]=mymalloc(size);
            if(arr[index]==NULL){
                printf("memory allacote failed\n");
            }
        }
        else//If the pointer shows a block, free it
        {   arr[index]=myfree(arr[index]);  }
    }
    printFreeList();

    for(i=0;i<5;i++){//free all blocks when the test ends
       if(arr[i]!=NULL)
        {arr[i]=myfree(arr[i]);}
    }
    printf("heap is free\n");
    printFreeList();
    return 1;
}

void*mymalloc(size_t size){
    static int firstcall=1;
    if(firstcall){
        heap_start=sbrk(1024+2*sizeof(Block));//heap_start+heap_end+1024byte
        heap_start->info.size=1024;
        heap_start->info.isfree=1;
        heap_end=(void*)heap_start+sizeof(Block)+heap_start->info.size;
        heap_end->info.isfree=1;
        heap_end->info.size=1024;
        free_list=heap_start;
        free_list->next=NULL;
        heap_end->next=NULL;
        firstcall=0;
        /*available free space is 1024*/ 
        printf("heap start:%p-heap end:%p\n", heap_start, heap_end);
    }
    if(size%16 != 0){//istenilen size 16nın katı değilse 16nın katına yuvarla
        size=size+16-(size%16);
    }
    printf("memory allocate size:%zu\n",size);
    Block *p;
    switch (strategy)
    {
        case bestFit:p=BestFit(size);
            break;
        case worstFit:p=WorstFit(size);
            break;
        case firstFit:p=FirstFit(size);
            break;
        default:p=BestFit(size);
            break;
    }

    if(p==NULL)//kullanılabilecek alan yok
    {   fprintf(stderr,"no available space\n");
        return NULL;}
    else if(size<=p->info.size){
            if(size==p->info.size || (p->info.size-size)<48)//uygun block bulundu
            {   p->info.isfree=0;   //kalan blok 16byte tutamayacak kadar küçükse istenen alana ekle
                if(p==free_list){
                    Block *next=(Block*)(free_list->next);
                    free_list=(Block*)(free_list->next);
                    if(next!=NULL)
                        {((Block*)((void*)next+next->info.size+sizeof(Block)))->next=NULL;}//free_list->next->prev=NULL
                    return (void*)p+sizeof(Block);}

                Block *pBtag=((void*)p+p->info.size+sizeof(Block));
                pBtag->info.isfree=0;
                ((Block*)(pBtag->next))->next=p->next;//p->prev->next=p->next
                Block *next=(Block*)(p->next);
                if(next!=NULL){
                    Block *nBtag=((void*)next+next->info.size+sizeof(Block));
                    nBtag->next=pBtag->next;//p->next->prev=p->prev
                }
            }
            else{//Bulunan Block>size
                if(p==free_list){
                    free_list=split(p,size);
                    return (void*)p+sizeof(Block);}
                
                Block *pBtag=((void*)p+p->info.size+sizeof(Block));
                Block *prev=(Block*)(pBtag->next);
                prev->next=(struct Block*)(split(p,size));//blok split edildikten sonra boş kısım listeye eklenir
            }
    }
    return (void*)p+sizeof(Block);
}
    
Block *split(Block *p,size_t newSize){
    size_t oldSize=p->info.size;
    p->info.size=newSize;
    p->info.isfree=0;
    Block *q=((void*)p+sizeof(Block)+newSize);//pointer aritmetiğinden dolayı void yaptık
    Block *pBtag=q;//block sonuna Btag eklendi
    pBtag->info.isfree=0;
    pBtag->info.size=newSize;
    q=(q+1);//yeni blok baslangıcı
    q->info.isfree=1;
    q->info.size=oldSize-newSize-2*sizeof(Block);
    Block *qBtag=(void*)q+q->info.size+sizeof(Block);
    qBtag->info.isfree=1;
    qBtag->info.size=q->info.size;
    q->next=p->next;
    Block *next=(Block*)(p->next);
    if(next!=NULL)
                {((Block*)((void*)next+next->info.size+sizeof(Block)))->next=(struct Block*)(q);}//p->next->prev=q
    return q;
}

void* myfree(void*p){
    Block *b=(p-sizeof(Block));//Block header
    b->info.isfree=1;
    Block *bBtag=(void*)b+b->info.size+sizeof(Block);
    bBtag->info.isfree=1;
    printf("Memory free size:%d\n",b->info.size);
    int addedToList=0;
    if(b!=heap_start){//Eğer heap başında değilse önceki boş mu diye bak
        Block *pBtag=b-1;
        if(pBtag->info.isfree==1){//önceki boşsa birleştir
            Block *prev=((void*)pBtag-pBtag->info.size-sizeof(Block));//prev:head of the previous block
            prev->info.size+=b->info.size+2*sizeof(Block);
            bBtag->info.size=prev->info.size;
            bBtag->next=pBtag->next;//b->prev=prev->prev
            b=prev;//head:prev,Btag:b
            addedToList=1;//önceki boş bloğa eklediği için free liste eklenmiş olur        
            }
        }
    if(bBtag!=heap_end){//heap sonunda değilse sonraki bos mu diye bak
        Block *next=bBtag+1;//next:head of the next block
        //printf("myfree:b adress %p\n",b);
        if(next->info.isfree==1){//sonraki boşsa birleştir
            b->info.size+=2*sizeof(Block)+next->info.size;
            Block *nBtag=((void*)next+next->info.size+sizeof(Block));
            nBtag->info.size=b->info.size;
            if((Block*)(nBtag->next)==b)//Eger free listte kendinden oncekiyle birlesecekse,prev->prev goster
                {nBtag->next=bBtag->next;}
            if(nBtag->next!=NULL)
            {((Block*)(nBtag->next))->next=(struct Block*)(b);}//next block prev->next=b
            if((Block*)(next->next)!=b)
            {b->next=next->next;}
            else{next->next=b->next;}
            Block* nNext=(Block*)(next->next);
            if(nNext!=NULL)
            {((Block*)((void*)nNext+nNext->info.size+sizeof(Block)))->next=(struct Block*)(b);}//next->next->prev=b
            bBtag=nBtag;//head:b,Btag:next
            addedToList=1;//Listedeki elemana eklendi
            if(next==free_list)//free_list ile birlestirildiyse free_list baslangıcını guncelle
            {   b->next=free_list->next;
                Block *next=(Block*)(free_list->next);
                if(next!=NULL)
                {((Block*)((void*)next+next->info.size+sizeof(Block)))->next=(struct Block*)(b);}//free_list->next->prev=b
                free_list=b;}
            }
    }
    
    if(addedToList==0){//free listteki bir blokla birleştirilmediyse listeye ekle
        if(free_list==NULL)
        {   free_list=b;
            free_list->next=NULL;}
        else{
            Block *itr=free_list;
            while(itr->next!=NULL)
            {itr=(Block*)(itr->next);}
            itr->next=(struct Block*)(b);
            b->next=NULL;
            bBtag->next=(struct Block*)(itr);
        }
    }
    return NULL;//free edilen pointer null isaret eder
}

int main(){
    printf("\nBest fit:\n");
    test(0);
    printf("\nWorst fit:\n");
    test(1);
    printf("\nFirst fit:\n");
    test(2);
    return 0;
}